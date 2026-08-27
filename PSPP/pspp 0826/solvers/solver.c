#if __has_include("pspp_kb.h")
#include "pspp_kb.h"
#else
#include "../pspp_kb.h"
#endif
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

/*
 * PSPP Ultra Hizli C Arama Motoru (solver.c) - TERS DFS & DINAMIK DELTA LIMITI
 *
 * BU MOTORUN ARAMA MANTIGI:
 * -------------------------
 * 1. TERS DFS (Ilk Eleman Once Artar):
 *    Arama sagdan sola (P-1'den 0'a) dogru derinlesir.
 *    Boylece en icteki dongu delta[0]'i dondurur:
 *    [1, 1, 1, 1] -> [2, 1, 1, 1] -> [3, 1, 1, 1] ...
 *
 * 2. KULLANICI DELTA LIMITI:
 *    Kullanici tek bir delta elemaninin cikabilecegi maksimum degeri
 * belirleyebilir. (Girilmezse mutlak teorik tavan kullanilir).
 *
 * 3. DONANIMSAL 64-BIT CTZ:
 *    Her yaprakta kesintisiz menzil O(1) donanimsal komutlarla hesaplanir.
 */

int P = 8;
int bilinen_max = 0;
int hedef_esik = 0;
int user_max_delta = 0; // 0 = Sinirsiz / Serbest Tavan
int best_score = 0;
int delta[MAX_P];
PSPP_KnowledgeBase g_kb;
clock_t g_start_time;
clock_t g_last_log_time;

// Arama ve Budama Istatistikleri
typedef struct {
  long long total_nodes;          // Toplam girilen dugum (DFS fonksiyon cagrisi)
  long long tested_leaves;        // Test edilen yaprak sayisi
  long long prune_kural1_leaf;    // Kanun 1: Yetersiz toplam nedeniyle elenenler
  long long prune_kural2_parity;  // Kanun 2: 1 uretilememe / parite nedeniyle elenenler
  long long prune_kural2b_two;    // Kanun 2b: 2 uretilememe nedeniyle elenenler
  long long prune_kural2c_p0;     // Kanun 2c: p0 - 1 uretilememe nedeniyle elenenler
  long long prune_kural2d_three;  // Kanun 2d: 3 uretilememe nedeniyle elenenler
} SearchStats;

SearchStats stats;

#define BITMASK_WORDS 32 // 2048 Bit Kapasite (M=2048'e kadar sifir overflow)

// 64-Bit CTZ Skor Hesaplayici (2048-Bit Donanimsal Menzil)
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
      val &= ~1ULL; // 0. biti yoksay
    if (val == 0) {
      m += 64;
    } else {
      m += __builtin_ctzll(val);
      break;
    }
  }
  return m - 1;
}

// TERS DFS: depth = P-1'den baslayip 0'a dogru derinlesir
void dfs_ters(int depth, int current_sum, int teorik_tavan, int has_one,
              int has_two, int has_adj_ones, int has_three, int has_12_21,
              int has_111, int ones_streak, int prev_val) {
  stats.total_nodes++;

  int min_required_sum = (hedef_esik + 1) / 2;

  // Ileriye donuk erken budama (Kanun 1): Kalan adimlar maksimumla doldurulsa
  // bile yetmiyorsa kes
  int remaining_steps = depth + 1; // 0'a kadar kalan adim sayisi
  int max_step_limit = (user_max_delta > 0) ? user_max_delta : teorik_tavan;
  if (current_sum + remaining_steps * max_step_limit < min_required_sum) {
    stats.prune_kural1_leaf++;
    return;
  }

  // Yaprak Noktasi (depth < 0): Tum delta[0]..delta[P-1] degerleri
  // yerlestirildi!
  if (depth < 0) {
    // Kanun 1 Alt Esik Kontrolu
    if (current_sum < min_required_sum) {
      stats.prune_kural1_leaf++;
      return;
    }

    // Kanun 2: 1 Sayisinin Uretimi ve Parite Kontrolu
    // Eger hicbir delta elemani 1 degilse, 1 uretilemez (M=0).
    if (!has_one) {
      stats.prune_kural2_parity++;
      return;
    }

    // Kanun 2b: 2 Sayisinin Uretimi Kontrolu
    // delta[0] != 1 VE hic 2 yok VE hic [1,1] yoksa 2 uretilemez (M <= 1).
    if (!has_two && !has_adj_ones && delta[0] != 1) {
      stats.prune_kural2b_two++;
      return;
    }

    // Kanun 2d: 3 Sayisinin Uretimi Kontrolu
    int makes_three = has_three || has_12_21 || has_111 ||
                      (delta[0] == 3) ||
                      (delta[0] == 2 && delta[1] == 1) ||
                      (delta[0] == 1 && (delta[1] == 2 || delta[1] == 1));
    if (!makes_three) {
      stats.prune_kural2d_three++;
      return;
    }

    stats.tested_leaves++;

    // 5 Saniyede Bir Canli Nabiz / Ilerleme Logu
    if ((stats.tested_leaves & 0x7FFFF) == 0) {
      clock_t now = clock();
      double sec_since_last = (double)(now - g_last_log_time) / CLOCKS_PER_SEC;
      if (sec_since_last >= 5.0) {
        g_last_log_time = now;
        double total_elapsed = (double)(now - g_start_time) / CLOCKS_PER_SEC;
        long long total_pruned = stats.prune_kural1_leaf + stats.prune_kural2_parity +
                                 stats.prune_kural2b_two + stats.prune_kural2c_p0 +
                                 stats.prune_kural2d_three;
        long long total_effective = stats.tested_leaves + total_pruned;
        double speed = (total_elapsed > 0)
                           ? (stats.tested_leaves / total_elapsed / 1e6)
                           : 0;
        double eff_speed = (total_elapsed > 0)
                               ? (total_effective / total_elapsed / 1e6)
                               : 0;
        printf(
            "  >> [Ilerleme: %lldM test (Efektif: %lldM) | %.1fs] Saf Hiz: %.2fM/s | Efektif Guc: %.2fM/s | Aktif Delta: [",
            stats.tested_leaves / 1000000, total_effective / 1000000, total_elapsed, speed, eff_speed);
        for (int i = 0; i < P; i++) {
          printf("%d%s", delta[i], i == P - 1 ? "" : ", ");
        }
        printf("] | Anlik En Iyi: M = %d\n", best_score);
        fflush(stdout);
      }
    }

    int score = deltatomax_c(delta, P);

    if (score >= hedef_esik) {
      if (score > best_score) {
        best_score = score;
        if (score > bilinen_max)
          bilinen_max = score;
      }

      int res = kb_update(&g_kb, P, score, delta);
      int sol_idx = kb_find_solution_index(&g_kb.table[P], delta, P);

      if (res == KB_NEW_RECORD) {
        printf("  >>> [YENI REKOR: M = %d] Delta: [", score);
        for (int i = 0; i < P; i++)
          printf("%d%s", delta[i], i == P - 1 ? "]\n" : ", ");
        fflush(stdout);
      } else if (res == KB_NEW_ALTERNATIVE) {
        printf("  >>> [YENI KESIF (Alternatif #%d): M = %d] Delta: [",
               g_kb.table[P].stored_solutions_count, score);
        for (int i = 0; i < P; i++)
          printf("%d%s", delta[i], i == P - 1 ? "]\n" : ", ");
        fflush(stdout);
      } else if (res == KB_ALREADY_EXISTS) {
        printf("  -> [DB'DE MEVCUT #%d: M = %d] Delta: [",
               sol_idx >= 0 ? (sol_idx + 1) : 1, score);
        for (int i = 0; i < P; i++)
          printf("%d%s", delta[i], i == P - 1 ? "]\n" : ", ");
        fflush(stdout);
      }
    }
    return;
  }

  // Bu derinlik icin alinabilecek ust sinir
  int max_d =
      (user_max_delta > 0) ? user_max_delta : (teorik_tavan - current_sum);
  if (max_d < 1)
    max_d = 1;

  // Kanun 2 & Kanun 2b & Kanun 2d Erken Dal Budamasi (depth == 0):
  int suffix_has_3 = has_three || has_12_21 || has_111;
  if (depth == 0 && !has_one) {
    // 1 sayisinin uretilebilmesi icin delta[0] MUTLAKA 1 olmak zorundadir.
    if (max_d > 1) {
      stats.prune_kural2_parity += (max_d - 1);
      max_d = 1;
    }
  } else if (depth == 0 && !has_two && !has_adj_ones) {
    // Kanun 2b: 2 sayisinin uretilebilmesi icin delta[0] MUTLAKA 1 veya 2 olmak zorundadir.
    // (d >= 3 olan dallar 2 sayisini uretemez ve M <= 1 olur).
    if (max_d > 2) {
      stats.prune_kural2b_two += (max_d - 2);
      max_d = 2;
    }
  } else if (depth == 0 && !suffix_has_3) {
    // Kanun 2d: 3 sayisinin uretilebilmesi icin delta[0] en fazla 3 olabilir (d >= 4 olan dallar 3 uretemez).
    if (max_d > 3) {
      stats.prune_kural2d_three += (max_d - 3);
      max_d = 3;
    }
  }

  // Kanun 2c: p0 - 1 Uretilme Kontrolu (d >= 4, yani p0 - 1 >= 3 icin):
  // delta[0] >= 4 secildiginde, (d - 1) sayisi tekil veya toplamlardan gelemez;
  // yalnizca delta[1]..delta[P-1] farklarindan (alt aralik toplamlarindan) gelebilir.
  uint64_t diff_mask[BITMASK_WORDS] = {0};
  if (depth == 0 && max_d >= 4) {
    for (int i = 1; i < P; i++) {
      int sum = 0;
      for (int j = i; j < P; j++) {
        sum += delta[j];
        if (sum < 2048) {
          diff_mask[sum >> 6] |= (1ULL << (sum & 63));
        }
      }
    }
  }

  for (int d = 1; d <= max_d; d++) {
    if (depth == 0 && d >= 4) {
      int target = d - 1;
      if (!(diff_mask[target >> 6] & (1ULL << (target & 63)))) {
        stats.prune_kural2c_p0++;
        continue; // Kanun 2c ile aninda budama!
      }
    }

    // Kanun 2d Ek Kontrol: depth == 0 noktasinda eger kuyrukta hic 3 uretimi yoksa:
    if (depth == 0 && !suffix_has_3) {
      if (d == 2 && delta[1] != 1) {
        stats.prune_kural2d_three++;
        continue;
      }
      if (d == 1 && delta[1] >= 3) {
        stats.prune_kural2d_three++;
        continue;
      }
    }

    delta[depth] = d;
    int next_adj = has_adj_ones || (prev_val == 1 && d == 1);
    int next_12_21 = has_12_21 || (prev_val == 1 && d == 2) || (prev_val == 2 && d == 1);
    int cur_streak = (d == 1) ? (ones_streak + 1) : 0;
    int next_111 = has_111 || (cur_streak >= 3);

    dfs_ters(depth - 1, current_sum + d, teorik_tavan, has_one || (d == 1),
             has_two || (d == 2), next_adj, has_three || (d == 3), next_12_21,
             next_111, cur_streak, d);
  }
}

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

void run_solver() {
  int teorik_tavan = P * P + P;

  printf("====================================================================="
         "=\n");
  printf("         PSPP ULTRA ARAMA MOTORU (TERS DFS: ILK ELEMAN ONCELIKLI)    "
         " \n");
  printf("====================================================================="
         "=\n");
  printf("Parametreler : P = %d\n", P);
  printf("Delta Limiti : %s\n", (user_max_delta > 0)
                                    ? "KULLANICI LIMITI AKTIF"
                                    : "SERBEST (Teorik Tavan)");
  if (user_max_delta > 0)
    printf("Max Delta    : Her bir delta <= %d\n", user_max_delta);
  printf("Zemin Citasi : Hedef Esik >= %d\n", hedef_esik);
  printf(
      "Arama Sirasi : [1,1,1] -> [2,1,1] -> [3,1,1] (delta[0] once artar)\n");
  printf("Veritabani   : pspp_database.json\n");
  printf("---------------------------------------------------------------------"
         "-\n");
  if (g_kb.table[P].score > 0) {
    printf("Bilgi Tabanindaki Mevcut Kayit:\n");
    kb_print_record(&g_kb.table[P]);
  }
  printf("====================================================================="
         "=\n\n");

  g_start_time = clock();
  g_last_log_time = g_start_time;
  stats.total_nodes = 0;
  stats.tested_leaves = 0;
  stats.prune_kural1_leaf = 0;
  stats.prune_kural2_parity = 0;
  stats.prune_kural2b_two = 0;
  stats.prune_kural2c_p0 = 0;
  stats.prune_kural2d_three = 0;
  best_score = 0;

  // Aramayi P-1. indeksten baslat
  dfs_ters(P - 1, 0, teorik_tavan, 0, 0, 0, 0, 0, 0, 0, 0);

  clock_t end = clock();
  double cpu_time = ((double)(end - g_start_time)) / CLOCKS_PER_SEC;

  long long total_pruned = stats.prune_kural1_leaf + stats.prune_kural2_parity +
                           stats.prune_kural2b_two + stats.prune_kural2c_p0 +
                           stats.prune_kural2d_three;
  long long total_effective = stats.tested_leaves + total_pruned;

  char buf_nodes[64], buf_leaves[64], buf_eff[64], buf_k1[64], buf_k2[64], buf_k2b[64], buf_k2c[64], buf_k2d[64];
  format_num(stats.total_nodes, buf_nodes, sizeof(buf_nodes));
  format_num(stats.tested_leaves, buf_leaves, sizeof(buf_leaves));
  format_num(total_effective, buf_eff, sizeof(buf_eff));
  format_num(stats.prune_kural1_leaf, buf_k1, sizeof(buf_k1));
  format_num(stats.prune_kural2_parity, buf_k2, sizeof(buf_k2));
  format_num(stats.prune_kural2b_two, buf_k2b, sizeof(buf_k2b));
  format_num(stats.prune_kural2c_p0, buf_k2c, sizeof(buf_k2c));
  format_num(stats.prune_kural2d_three, buf_k2d, sizeof(buf_k2d));

  printf("\n==================================================================="
         "===\n");
  printf("                     ARAMA VE BUDAMA ISTATISTIKLERI                  "
         " \n");
  printf("====================================================================="
         "=\n");
  printf("Toplam Dugum Ziyareti (DFS Calls) : %s\n", buf_nodes);
  printf("Test Edilen Yaprak (Kombinasyon)  : %s\n", buf_leaves);
  printf("Efektif Taranan Esdeger Uzay      : %s (Budanan dallarla)\n", buf_eff);
  printf("En Yuksek Skor                    : M = %d\n", best_score);
  printf("Bulunan Cozum Sayisi              : %d\n",
         g_kb.table[P].solutions_count);
  printf("Gecen Toplam Sure                 : %.4f saniye\n", cpu_time);
  if (cpu_time > 0) {
    printf("Saf Hesaplama Hizi (Throughput)   : %.2f Milyon test/sn\n",
           (stats.tested_leaves / 1000000.0) / cpu_time);
    printf("Efektif Arama Gucu (Search Power) : %.2f Milyon kombinasyon/sn\n",
           (total_effective / 1000000.0) / cpu_time);
  }
  printf("---------------------------------------------------------------------"
         "-\n");
  printf("[BUDAMA ETKINLIK RAPORU]\n");
  printf(" - Kanun 1  (Yetersiz Toplam / Alt Esik)  : %s dal elendi\n", buf_k1);
  printf(" - Kanun 2  (1'in Uretimi & Parite)       : %s dal elendi\n", buf_k2);
  printf(" - Kanun 2b (2'nin Uretimi Budamasi)     : %s dal elendi\n", buf_k2b);
  printf(" - Kanun 2c (p0 - 1 Uretimi Budamasi)    : %s dal elendi\n", buf_k2c);
  printf(" - Kanun 2d (3'un Uretimi Budamasi)      : %s dal elendi\n", buf_k2d);
  printf("====================================================================="
         "=\n");

  if (g_kb.table[P].score > 0) {
    printf("\n[Guncellenmis DP Tablo Kaydi - Diske Yazildi]:\n");
    kb_print_record(&g_kb.table[P]);
    kb_save_json(&g_kb, DB_FILE);
  }
}

void interactive_mode() {
  char buf[64];
  printf("====================================================================="
         "=\n");
  printf("         PSPP ULTRA ARAMA MOTORU (INTERAKTIF MOD - TERS DFS)         "
         " \n");
  printf("====================================================================="
         "=\n\n");

  printf("Hedef Boyut P (Eleman Sayisi) [Varsayilan: 8]: ");
  if (fgets(buf, sizeof(buf), stdin) && buf[0] != '\n') {
    int val = atoi(buf);
    if (val >= 1 && val < MAX_P)
      P = val;
  } else {
    P = 8;
  }

  printf("Maksimum Delta Limiti (Tek bir delta elemaninin max degeri, "
         "bos=Serbest): ");
  if (fgets(buf, sizeof(buf), stdin) && buf[0] != '\n') {
    int val = atoi(buf);
    if (val > 0)
      user_max_delta = val;
    else
      user_max_delta = 0;
  } else {
    user_max_delta = 0; // Serbest
  }

  bilinen_max = kb_get_target_max(&g_kb, P);
  printf("Hedef Esik / Zemin Citasi [Varsayilan: %d]: ", bilinen_max);
  if (fgets(buf, sizeof(buf), stdin) && buf[0] != '\n') {
    int val = atoi(buf);
    if (val >= 0)
      hedef_esik = val;
    else
      hedef_esik = bilinen_max;
  } else {
    hedef_esik = bilinen_max;
  }

  printf("\n");
  run_solver();

  printf("\nCikis yapmak icin Enter tusuna basin...");
  getchar();
}

int main(int argc, char **argv) {
  // 1. Bilgi Tabanini Baslat (pspp_database.json'dan yukler)
  kb_init(&g_kb);

  // Eger hic arguman verilmemisse Interaktif Mod ac
  if (argc == 1) {
    interactive_mode();
    return 0;
  }

  // 2. Argumanlari Ayristir
  if (argc > 1)
    P = atoi(argv[1]);

  for (int i = 2; i < argc; i++) {
    if (strcmp(argv[i], "--max-delta") == 0 && i + 1 < argc) {
      user_max_delta = atoi(argv[++i]);
    } else if (strcmp(argv[i], "--esik") == 0 && i + 1 < argc) {
      hedef_esik = atoi(argv[++i]);
    } else if (atoi(argv[i]) > 0 && hedef_esik == 0) {
      hedef_esik = atoi(argv[i]);
    }
  }

  if (hedef_esik == 0) {
    bilinen_max = kb_get_target_max(&g_kb, P);
    hedef_esik = bilinen_max;
  }

  run_solver();
  return 0;
}
