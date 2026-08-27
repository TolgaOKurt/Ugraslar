#if __has_include("pspp_kb.h")
#include "pspp_kb.h"
#else
#include "../pspp_kb.h"
#endif

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#ifdef _OPENMP
#include <omp.h>
#endif

/*
 * ==============================================================================
 * PSPP ULTRA HYBRID & BRANCH SOLVER (ultra_solver.c)
 * ==============================================================================
 *
 * BU MOTORUN ÖZELLİKLERİ VE KAPSAMI:
 * -----------------------------------
 * 1. KESİN MATEMATİKSEL KANUNLAR (Tüm Modlarda Aktif):
 *    - Kanun 1 : İleriye Dönük Dinamik Alt Eşik & Tepe Kapanış Budaması
 *    - Kanun 2 : 1'in Üretilmesi Kanunu (depth=0 ve depth=1 Erken Tükenme
 * Kısıtı)
 *    - Kanun 2b: 2'nin Üretilmesi Kanunu (depth=0 ve depth=1 Erken Tükenme
 * Kısıtı)
 *    - Kanun 2c: Genelleştirilmiş O(1) Öncül Bitmask (1..d-1 tüm farkların
 * zorunluluğu)
 *    - Kanun 2d: 3'ün Üretilmesi Kanunu (4 Şartlı Erken Budama)
 *    - Kanun 2e: Parite & Tek Sayı Kanunu (Hiç tek yoksa delta[0] mutlak tek
 * olmalı)
 *    - Kanun 3 : Güvercin Yuvası Mutlak Teorik Tavanı
 *
 * 2. SEZGİSEL VE MORFOLOJİK HIZLANDIRICILAR (Mode 2 & Mode 3):
 *    - Maksimum 1 Büyük Sıçrama Kuralı (delta > P sonrası döngü tavanı = P)
 *    - Son Eleman Çatallanması (delta_son in {1,2,3} veya delta_son ~ Hedef/2)
 *    - İlk Eleman Sınırı (delta_0 <= P)
 *
 * 3. DONANIM VE MİMARİ:
 *    - 2048-Bit Donanımsal CTZ Bitmask Motoru (deltatomax_c)
 *    - OpenMP Çok Çekirdekli Paralel Arama Desteği
 *    - Saf Hesaplama Hızı + Efektif Arama Gücü Ölçümü
 *    - pspp_database.json Otomatik Kayıt & Akıllı Çözüm Ayrıştırma
 * ==============================================================================
 */

#define MODE_EXACT 1 // %100 Kesin Matematiksel Kanunlar (Sıfır Kayıp Garantili)
#define MODE_HYBRID 2  // Kesin Kanunlar + Morfolojik Kısıtlar (Yüksek Hız)
#define MODE_MODULAR 3 // Aile 1 Sıkı Modüler Arayıcı (P >= 9)

#define BITMASK_WORDS 32 // 2048 Bit Kapasite

// Donanımsal 64-Bit CTZ Skor Hesaplayıcı
static inline int deltatomax_c(const int *delta_arr, int n) {
  int p_arr[MAX_P];
  int cur = 0;
  for (int i = 0; i < n; i++) {
    cur += delta_arr[i];
    p_arr[i] = cur;
  }

  uint64_t seen[BITMASK_WORDS] = {0};

  for (int i = 0; i < n; i++) {
    int pi = p_arr[i];
    if (pi > 0 && pi < 2048)
      seen[pi >> 6] |= (1ULL << (pi & 63));

    for (int j = i; j < n; j++) {
      int pj = p_arr[j];
      int sum = pj + pi;
      int diff = pj - pi;

      if (sum < 2048)
        seen[sum >> 6] |= (1ULL << (sum & 63));
      if (diff > 0 && diff < 2048)
        seen[diff >> 6] |= (1ULL << (diff & 63));
    }
  }

  int m = 0;
  for (int w = 0; w < BITMASK_WORDS; w++) {
    uint64_t val = ~seen[w];
    if (w == 0)
      val &= ~1ULL;
    if (val == 0) {
      m += 64;
    } else {
      m += __builtin_ctzll(val);
      break;
    }
  }
  return m - 1;
}

// Arama İstatistikleri Yapısı
typedef struct {
  long long total_nodes;          // Toplam DFS düğüm ziyareti
  long long tested_leaves;        // Test edilen yaprak sayısı
  long long prune_k1_sum;         // Kanun 1: Yetersiz toplam budaması
  long long prune_k2_parity;      // Kanun 2 & 2e: 1 ve parite budaması
  long long prune_k2b_two;        // Kanun 2b: 2 üretilememe budaması
  long long prune_k2c_bitmask;    // Kanun 2c: Genelleştirilmiş öncül bitmask budaması
  long long prune_k2d_three;      // Kanun 2d: 3 üretilememe budaması
  long long prune_depth1_branch;  // depth=1 Seviyesi Kalın Dal Budaması
  long long prune_heuristic_jump; // Sezgisel: 2. büyük sıçrama budaması
} UltraStats;

#define MAX_THREADS 64
static UltraStats g_thread_stats[MAX_THREADS];
static double g_last_log_wtime = 0.0;

// Global Parametreler
int g_P = 8;
int g_mode = MODE_EXACT;
int g_hedef_esik = 0;
int g_bilinen_max = 0;
int g_user_max_delta = 0;
int g_num_threads = 1;
int g_best_score = 0;
PSPP_KnowledgeBase g_kb;

clock_t g_start_clock;
double g_start_wall;

static void format_num(long long val, char *out, size_t sz) {
  if (val >= 1000000000LL) {
    snprintf(out, sz, "%.2fB (%lld)", val / 1e9, val);
  } else if (val >= 1000000LL) {
    snprintf(out, sz, "%.2fM (%lld)", val / 1e6, val);
  } else if (val >= 1000LL) {
    snprintf(out, sz, "%.2fK (%lld)", val / 1e3, val);
  } else {
    snprintf(out, sz, "%lld", val);
  }
}

// Ultra Ters DFS Arama Motoru
void dfs_ultra_worker(int depth, int current_sum, int teorik_tavan, int has_one,
                      int has_two, int has_adj_ones, int has_three,
                      int has_12_21, int has_111, int has_odd,
                      int has_large_jump, int ones_streak, int prev_val,
                      int *local_delta, UltraStats *st) {
  st->total_nodes++;

  int min_required_sum = (g_hedef_esik + 1) / 2;

  // 1. KANUN 1: İleriye Dönük Dal Budaması (Ağacın her seviyesinde)
  int remaining_steps = depth + 1;
  int max_step_limit = (g_user_max_delta > 0) ? g_user_max_delta : teorik_tavan;
  if (current_sum + remaining_steps * max_step_limit < min_required_sum) {
    st->prune_k1_sum++;
    return;
  }

  // 2. YAPRAK NOKTASI (depth < 0)
  if (depth < 0) {
    if (current_sum < min_required_sum) {
      st->prune_k1_sum++;
      return;
    }
    if (!has_one) {
      st->prune_k2_parity++;
      return;
    }
    if (!has_odd) {
      st->prune_k2_parity++;
      return;
    }
    if (!has_two && !has_adj_ones && local_delta[0] != 1) {
      st->prune_k2b_two++;
      return;
    }

    int makes_three =
        has_three || has_12_21 || has_111 || (local_delta[0] == 3) ||
        (local_delta[0] == 2 && local_delta[1] == 1) ||
        (local_delta[0] == 1 && (local_delta[1] == 2 || local_delta[1] == 1));
    if (!makes_three) {
      st->prune_k2d_three++;
      return;
    }

    st->tested_leaves++;

    // 5 Saniyede Bir Canli Nabiz / Ilerleme Logu (Thread-Safe)
    if ((st->tested_leaves & 0x1FFFF) == 0) {
      double now_w = 0;
#ifdef _OPENMP
      now_w = omp_get_wtime();
#else
      now_w = (double)clock() / CLOCKS_PER_SEC;
#endif
      if (now_w - g_last_log_wtime >= 5.0) {
#pragma omp critical
        {
          if (now_w - g_last_log_wtime >= 5.0) {
            g_last_log_wtime = now_w;
            double elapsed = now_w - g_start_wall;

            long long cur_leaves = 0;
            long long cur_pruned = 0;
            for (int t = 0; t < g_num_threads && t < MAX_THREADS; t++) {
              cur_leaves += g_thread_stats[t].tested_leaves;
              cur_pruned += (g_thread_stats[t].prune_k1_sum +
                             g_thread_stats[t].prune_k2_parity +
                             g_thread_stats[t].prune_k2b_two +
                             g_thread_stats[t].prune_k2c_bitmask +
                             g_thread_stats[t].prune_k2d_three +
                             g_thread_stats[t].prune_depth1_branch +
                             g_thread_stats[t].prune_heuristic_jump);
            }
            long long cur_eff = cur_leaves + cur_pruned;
            double spd = (elapsed > 0) ? (cur_leaves / elapsed / 1e6) : 0;
            double eff_spd = (elapsed > 0) ? (cur_eff / elapsed / 1e6) : 0;

            printf("  >> [Ilerleme: %lldM test (Efektif: %lldM) | %.1fs] Saf Hiz: %.2fM/s | Efektif Guc: %.2fM/s | Aktif Delta: [",
                   cur_leaves / 1000000, cur_eff / 1000000, elapsed, spd, eff_spd);
            for (int i = 0; i < g_P; i++) {
              printf("%d%s", local_delta[i], i == g_P - 1 ? "" : ", ");
            }
            printf("] | Anlik En Iyi: M = %d\n", g_best_score);
            fflush(stdout);
          }
        }
      }
    }

    int score = deltatomax_c(local_delta, g_P);

    if (score >= g_hedef_esik) {
#pragma omp critical
      {
        if (score > g_best_score) {
          g_best_score = score;
          if (score > g_bilinen_max)
            g_bilinen_max = score;
        }

        int res = kb_update(&g_kb, g_P, score, local_delta);
        int sol_idx =
            kb_find_solution_index(&g_kb.table[g_P], local_delta, g_P);

        if (res == KB_NEW_RECORD) {
          printf("  >>> [YENI REKOR: M = %d] Delta: [", score);
          for (int i = 0; i < g_P; i++)
            printf("%d%s", local_delta[i], i == g_P - 1 ? "]\n" : ", ");
          fflush(stdout);
        } else if (res == KB_NEW_ALTERNATIVE) {
          printf("  >>> [YENI KESIF (Alternatif #%d): M = %d] Delta: [",
                 g_kb.table[g_P].stored_solutions_count, score);
          for (int i = 0; i < g_P; i++)
            printf("%d%s", local_delta[i], i == g_P - 1 ? "]\n" : ", ");
          fflush(stdout);
        } else if (res == KB_ALREADY_EXISTS) {
          printf("  -> [DB'DE MEVCUT #%d: M = %d] Delta: [",
                 sol_idx >= 0 ? (sol_idx + 1) : 1, score);
          for (int i = 0; i < g_P; i++)
            printf("%d%s", local_delta[i], i == g_P - 1 ? "]\n" : ", ");
          fflush(stdout);
        }
      }
    }
    return;
  }

  // 3. İÇ DÜĞÜM TAVAN HESAPLAMASI
  int max_d =
      (g_user_max_delta > 0) ? g_user_max_delta : (teorik_tavan - current_sum);
  if (max_d < 1)
    max_d = 1;

  // Sezgisel Mod Kısıtı: Maksimum 1 Büyük Sıçrama (delta > P)
  if (g_mode == MODE_HYBRID && has_large_jump) {
    if (max_d > g_P) {
      st->prune_heuristic_jump += (max_d - g_P);
      max_d = g_P;
    }
  }

  // 4. KESİN DAL BUDAMASI (depth == 1): 1 ve 2'nin Erken Tükenme Kanunu
  if (depth == 1) {
    if (!has_one && !has_two && !has_adj_ones) {
      // Kalan 2 adımda hem 1 hem 2 üretilebilmesi için delta[1] <= 2 olmak
      // zorundadır!
      if (max_d > 2) {
        st->prune_depth1_branch += (max_d - 2);
        max_d = 2;
      }
    }
  }

  // 5. KESİN DAL BUDAMASI (depth == 0): Uç Eleman Erken Budaması
  int suffix_has_3 = has_three || has_12_21 || has_111;
  if (depth == 0) {
    // Sezgisel İlk Eleman Sınırı
    if (g_mode == MODE_HYBRID && max_d > g_P) {
      max_d = g_P;
    }

    if (!has_one) {
      if (max_d > 1) {
        st->prune_k2_parity += (max_d - 1);
        max_d = 1;
      }
    } else if (!has_two && !has_adj_ones) {
      if (max_d > 2) {
        st->prune_k2b_two += (max_d - 2);
        max_d = 2;
      }
    } else if (!suffix_has_3) {
      if (max_d > 3) {
        st->prune_k2d_three += (max_d - 3);
        max_d = 3;
      }
    }

    // Genelleştirilmiş O(1) Suffix Fark Bitmask'i (delta[1]..delta[P-1])
    uint64_t diff_mask[BITMASK_WORDS] = {0};
    for (int i = 1; i < g_P; i++) {
      int sum = 0;
      for (int j = i; j < g_P; j++) {
        sum += local_delta[j];
        if (sum < 2048) {
          diff_mask[sum >> 6] |= (1ULL << (sum & 63));
        }
      }
    }

    for (int d = 1; d <= max_d; d++) {
      // Kanun 2e: Parite Kontrolü (Dizide hiç tek sayı yoksa d mutlak tek sayı
      // olmalı)
      if (!has_odd && (d % 2 == 0)) {
        st->prune_k2_parity++;
        continue;
      }

      // Kanun 2d: 3 Üretilebilirlik Ek Kontrolü
      if (!suffix_has_3) {
        if (d == 2 && local_delta[1] != 1) {
          st->prune_k2d_three++;
          continue;
        }
        if (d == 1 && local_delta[1] >= 3) {
          st->prune_k2d_three++;
          continue;
        }
      }

      // Kanun 2c: Genelleştirilmiş 1..d-1 Öncül Bitmask Kontrolü
      if (d > 1 && d <= 63) {
        uint64_t req = (1ULL << d) - 2;
        if ((diff_mask[0] & req) != req) {
          st->prune_k2c_bitmask++;
          continue; // 1..d-1 arasında eksik sayı var, d çöpe atılır!
        }
      }

      local_delta[0] = d;
      dfs_ultra_worker(
          -1, current_sum + d, teorik_tavan, has_one || (d == 1),
          has_two || (d == 2), has_adj_ones || (prev_val == 1 && d == 1),
          has_three || (d == 3), has_12_21, has_111, has_odd || (d % 2 != 0),
          has_large_jump || (d > g_P), 0, d, local_delta, st);
    }
    return;
  }

  // 6. REKÜRSİF DALLANMA DÖNGÜSÜ (depth > 0)
  for (int d = 1; d <= max_d; d++) {
    // Sezgisel Son Eleman Çatallanması (depth == P-1)
    if (g_mode == MODE_HYBRID && depth == g_P - 1) {
      bool is_standard = (d <= 3);
      int half_target = min_required_sum;
      bool is_jump = (d >= half_target - 2 && d <= half_target + 4);
      if (!is_standard && !is_jump) {
        st->prune_heuristic_jump++;
        continue;
      }
    }

    local_delta[depth] = d;
    int next_adj = has_adj_ones || (prev_val == 1 && d == 1);
    int next_12_21 =
        has_12_21 || (prev_val == 1 && d == 2) || (prev_val == 2 && d == 1);
    int cur_streak = (d == 1) ? (ones_streak + 1) : 0;
    int next_111 = has_111 || (cur_streak >= 3);

    dfs_ultra_worker(depth - 1, current_sum + d, teorik_tavan,
                     has_one || (d == 1), has_two || (d == 2), next_adj,
                     has_three || (d == 3), next_12_21, next_111,
                     has_odd || (d % 2 != 0), has_large_jump || (d > g_P),
                     cur_streak, d, local_delta, st);
  }
}

void run_ultra_solver() {
  int teorik_tavan = g_P * g_P + g_P;

  printf("=====================================================================\n");
  printf("          PSPP ULTRA DAL & BUDAK ARAMA MOTORU (ultra_solver)         \n");
  printf("=====================================================================\n");
  printf("Boyut (P)        : %d\n", g_P);
  printf("Arama Modu       : %s\n",
         g_mode == MODE_EXACT    ? "1: KESIN MATEMATIKSEL (%100 Garantili)"
         : g_mode == MODE_HYBRID ? "2: HIBRIT SEZGISEL (Rekor Hizlandirmali)"
                                 : "3: SIKI MODULER");
  printf("Is Parcacigi     : %d Thread (OpenMP Paralel)\n", g_num_threads);
  printf("Delta Limiti     : %s\n", (g_user_max_delta > 0)
                                        ? "KULLANICI LIMITI AKTIF"
                                        : "SERBEST (Teorik Tavan)");
  if (g_user_max_delta > 0)
    printf("Max Delta Siniri : delta <= %d\n", g_user_max_delta);
  printf("Zemin Citasi     : Hedef Esik >= %d\n", g_hedef_esik);
  printf("Veritabani       : pspp_database.json\n");
  printf("---------------------------------------------------------------------\n");
  if (g_kb.table[g_P].score > 0) {
    printf("Bilgi Tabanindaki Mevcut Kayit:\n");
    kb_print_record(&g_kb.table[g_P]);
  }
  printf("=====================================================================\n\n");

  g_start_clock = clock();
#ifdef _OPENMP
  g_start_wall = omp_get_wtime();
#else
  g_start_wall = (double)clock() / CLOCKS_PER_SEC;
#endif
  g_last_log_wtime = g_start_wall;
  memset(g_thread_stats, 0, sizeof(g_thread_stats));

  UltraStats global_stats = {0};
  g_best_score = 0;

  int max_top_d =
      (g_user_max_delta > 0) ? g_user_max_delta : (teorik_tavan - (g_P - 1));
  if (max_top_d < 1)
    max_top_d = 1;

#pragma omp parallel num_threads(g_num_threads)
  {
    int tid = 0;
#ifdef _OPENMP
    tid = omp_get_thread_num();
#endif
    if (tid >= MAX_THREADS) tid = 0;
    UltraStats *local_st = &g_thread_stats[tid];
    memset(local_st, 0, sizeof(UltraStats));

    int local_delta[MAX_P] = {0};

#pragma omp for schedule(dynamic, 1)
    for (int top_d = 1; top_d <= max_top_d; top_d++) {
      if (g_mode == MODE_HYBRID) {
        int half_target = (g_hedef_esik + 1) / 2;
        bool is_std = (top_d <= 3);
        bool is_jump = (top_d >= half_target - 2 && top_d <= half_target + 4);
        if (!is_std && !is_jump) {
          local_st->prune_heuristic_jump++;
          continue;
        }
      }

      local_delta[g_P - 1] = top_d;
      dfs_ultra_worker(g_P - 2, top_d, teorik_tavan, (top_d == 1), (top_d == 2),
                       0, (top_d == 3), 0, 0, (top_d % 2 != 0), (top_d > g_P),
                       (top_d == 1 ? 1 : 0), top_d, local_delta, local_st);
    }

#pragma omp critical
    {
      global_stats.total_nodes += local_st->total_nodes;
      global_stats.tested_leaves += local_st->tested_leaves;
      global_stats.prune_k1_sum += local_st->prune_k1_sum;
      global_stats.prune_k2_parity += local_st->prune_k2_parity;
      global_stats.prune_k2b_two += local_st->prune_k2b_two;
      global_stats.prune_k2c_bitmask += local_st->prune_k2c_bitmask;
      global_stats.prune_k2d_three += local_st->prune_k2d_three;
      global_stats.prune_depth1_branch += local_st->prune_depth1_branch;
      global_stats.prune_heuristic_jump += local_st->prune_heuristic_jump;
    }
  }

  double total_time = 0;
#ifdef _OPENMP
  total_time = omp_get_wtime() - g_start_wall;
#else
  total_time = (double)(clock() - g_start_clock) / CLOCKS_PER_SEC;
#endif

  long long total_pruned =
      global_stats.prune_k1_sum + global_stats.prune_k2_parity +
      global_stats.prune_k2b_two + global_stats.prune_k2c_bitmask +
      global_stats.prune_k2d_three + global_stats.prune_depth1_branch +
      global_stats.prune_heuristic_jump;
  long long total_effective = global_stats.tested_leaves + total_pruned;

  char buf_nodes[64], buf_leaves[64], buf_eff[64];
  char buf_k1[64], buf_k2[64], buf_k2b[64], buf_k2c[64], buf_k2d[64],
      buf_d1[64], buf_hj[64];

  format_num(global_stats.total_nodes, buf_nodes, sizeof(buf_nodes));
  format_num(global_stats.tested_leaves, buf_leaves, sizeof(buf_leaves));
  format_num(total_effective, buf_eff, sizeof(buf_eff));
  format_num(global_stats.prune_k1_sum, buf_k1, sizeof(buf_k1));
  format_num(global_stats.prune_k2_parity, buf_k2, sizeof(buf_k2));
  format_num(global_stats.prune_k2b_two, buf_k2b, sizeof(buf_k2b));
  format_num(global_stats.prune_k2c_bitmask, buf_k2c, sizeof(buf_k2c));
  format_num(global_stats.prune_k2d_three, buf_k2d, sizeof(buf_k2d));
  format_num(global_stats.prune_depth1_branch, buf_d1, sizeof(buf_d1));
  format_num(global_stats.prune_heuristic_jump, buf_hj, sizeof(buf_hj));

  printf("\n=====================================================================\n");
  printf("                     ARAMA VE BUDAMA ISTATISTIKLERI                  \n");
  printf("=====================================================================\n");
  printf("Toplam Dugum Ziyareti (DFS Calls) : %s\n", buf_nodes);
  printf("Test Edilen Yaprak (Kombinasyon)  : %s\n", buf_leaves);
  printf("Efektif Taranan Esdeger Uzay      : %s\n", buf_eff);
  printf("En Yuksek Skor                    : M = %d\n", g_best_score);
  printf("Bulunan Cozum Sayisi              : %d\n",
         g_kb.table[g_P].solutions_count);
  printf("Gecen Toplam Sure                 : %.4f saniye\n", total_time);
  if (total_time > 0) {
    printf("Saf Hesaplama Hizi (Throughput)   : %.2f Milyon test/sn\n",
           (global_stats.tested_leaves / 1000000.0) / total_time);
    printf("Efektif Arama Gucu (Search Power) : %.2f Milyon kombinasyon/sn\n",
           (total_effective / 1000000.0) / total_time);
  }
  printf("---------------------------------------------------------------------\n");
  printf("[BUDAMA ETKINLIK RAPORU]\n");
  printf(" - Kanun 1  (Yetersiz Toplam / Alt Esik)  : %s dal elendi\n", buf_k1);
  printf(" - Kanun 2  (1'in Uretimi & Parite)       : %s dal elendi\n", buf_k2);
  printf(" - Kanun 2b (2'nin Uretimi Budamasi)     : %s dal elendi\n", buf_k2b);
  printf(" - Kanun 2c (Genel Oncul Bitmask)        : %s dal elendi\n", buf_k2c);
  printf(" - Kanun 2d (3'un Uretimi Budamasi)      : %s dal elendi\n", buf_k2d);
  printf(" - depth=1  (Kalin Dal Erken Budamasi)   : %s dal elendi\n", buf_d1);
  if (g_mode == MODE_HYBRID) {
    printf(" - Sezgisel (Maks 1 Sicrama / Uc Kisiti) : %s dal elendi\n",
           buf_hj);
  }
  printf("=====================================================================\n");

  if (g_kb.table[g_P].score > 0) {
    printf("\n[Guncellenmis DP Tablo Kaydi - Diske Yazildi]:\n");
    kb_print_record(&g_kb.table[g_P]);
    kb_save_json(&g_kb, DB_FILE);
  }
}

void interactive_mode() {
  char buf[64];
  printf("=====================================================================\n");
  printf("         PSPP ULTRA DAL & BUDAK ARAMA MOTORU (INTERAKTIF MOD)        \n");
  printf("=====================================================================\n\n");

  printf("Hedef Boyut P (Eleman Sayisi) [Varsayilan: 8]: ");
  if (fgets(buf, sizeof(buf), stdin) && buf[0] != '\n') {
    int val = atoi(buf);
    if (val >= 1 && val < MAX_P)
      g_P = val;
  } else {
    g_P = 8;
  }

  printf("Arama Modu Secin:\n");
  printf("  1) KESIN MATEMATIKSEL (%%100 Garantili - Sifir Kayip)\n");
  printf("  2) HIBRIT SEZGISEL (Rekor Hizlandirmali - Cok Hizli)\n");
  printf("Seciminiz [1/2, Varsayilan: 1]: ");
  if (fgets(buf, sizeof(buf), stdin) && buf[0] != '\n') {
    int val = atoi(buf);
    if (val == 2)
      g_mode = MODE_HYBRID;
    else
      g_mode = MODE_EXACT;
  } else {
    g_mode = MODE_EXACT;
  }

  printf("Calisacak Is Parcacigi (Thread) Sayisi [Varsayilan: 8]: ");
  if (fgets(buf, sizeof(buf), stdin) && buf[0] != '\n') {
    int val = atoi(buf);
    if (val >= 1)
      g_num_threads = val;
  } else {
    g_num_threads = 8;
  }

  g_bilinen_max = kb_get_target_max(&g_kb, g_P);
  printf("Hedef Esik / Zemin Citasi [Varsayilan: %d]: ", g_bilinen_max);
  if (fgets(buf, sizeof(buf), stdin) && buf[0] != '\n') {
    int val = atoi(buf);
    if (val >= 0)
      g_hedef_esik = val;
    else
      g_hedef_esik = g_bilinen_max;
  } else {
    g_hedef_esik = g_bilinen_max;
  }

  printf("Maksimum Delta Limiti (Tek delta tavani, bos=Serbest): ");
  if (fgets(buf, sizeof(buf), stdin) && buf[0] != '\n') {
    int val = atoi(buf);
    if (val > 0)
      g_user_max_delta = val;
    else
      g_user_max_delta = 0;
  } else {
    g_user_max_delta = 0;
  }

  printf("\n");
  run_ultra_solver();

  printf("\nCikis yapmak icin Enter tusuna basin...");
  getchar();
}

int main(int argc, char **argv) {
  kb_init(&g_kb);

  if (argc == 1) {
    interactive_mode();
    return 0;
  }

  if (argc > 1)
    g_P = atoi(argv[1]);

  for (int i = 2; i < argc; i++) {
    if (strcmp(argv[i], "--mode") == 0 && i + 1 < argc) {
      i++;
      if (strcmp(argv[i], "hybrid") == 0 || strcmp(argv[i], "2") == 0)
        g_mode = MODE_HYBRID;
      else
        g_mode = MODE_EXACT;
    } else if (strcmp(argv[i], "--threads") == 0 && i + 1 < argc) {
      g_num_threads = atoi(argv[++i]);
    } else if (strcmp(argv[i], "--max-delta") == 0 && i + 1 < argc) {
      g_user_max_delta = atoi(argv[++i]);
    } else if (strcmp(argv[i], "--esik") == 0 && i + 1 < argc) {
      g_hedef_esik = atoi(argv[++i]);
    } else if (atoi(argv[i]) > 0 && g_hedef_esik == 0) {
      g_hedef_esik = atoi(argv[i]);
    }
  }

  if (g_hedef_esik == 0) {
    g_bilinen_max = kb_get_target_max(&g_kb, g_P);
    g_hedef_esik = g_bilinen_max;
  }

  if (g_num_threads < 1)
    g_num_threads = 8;

  run_ultra_solver();
  return 0;
}
