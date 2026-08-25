#if __has_include("pspp_kb.h")
#include "pspp_kb.h"
#else
#include "../pspp_kb.h"
#endif
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

/*
 * PSPP Morfolojik 3-Aile Bazli Akilli Arama Motoru (family_solver.c)
 *
 * 3 Temel Matematiksel Arketip:
 *   [AILE 1] Siki Moduler Zincir : P_son = M/2, Tekrarlayan Adimlar (4,6,8),
 * d_son in {1,2} [AILE 2] Cift Kumeli Kopru   : Tam 1 adet buyuk kopru
 * (d_bridge ~ 4P-14), d_son in {1,2} [AILE 3] Asimetrik Uc Sicrama: d_son =
 * M/2, P_son = M, Genis Fark Kapsama
 *
 * Matematiksel Budama Kurallari:
 *   1. Ilk Eleman Siniri: 2 <= d0 <= P-1
 *   2. Son Eleman Catallanmasi: d_son in {1, 2} VEYA d_son = M/2
 *   3. Parite Dengesi: En az 2 tek adim (odd delta) zorunlulugu
 *   4. Tek Kopru Butcesi: delta > P olan adim sayisi <= 1
 */

int P = 12;
int bilinen_max = 0;
int hedef_esik = 0;
int best_score = 0;
int delta[MAX_P];
bool ENABLE_FAMILY_2 =
    false; // Aile 2 (Cift Kumeli Kopru) - Istege bagli (varsayilan: KAPALI)
bool ENABLE_FAMILY_3 = false; // Aile 3 (Asimetrik Uc Sicramasi) - Istege bagli
                              // (varsayilan: KAPALI)
PSPP_KnowledgeBase g_kb;
clock_t g_start_time;

// Telemetri ve Istatistikler
typedef struct {
  long long total_nodes;
  long long tested_leaves;
  long long prune_parity;       // Parite (tek sayi eksikligi) ile elenen
  long long prune_sum_bounds;   // Dizi toplami hedef disi olan
  long long prune_bridge_limit; // Birden fazla buyuk sicrama ile elenen
  long long prune_tail_filter;  // Gecersiz son eleman ile elenen
} FamilyStats;

FamilyStats stats_f1, stats_f2, stats_f3, stats_total;

#define BITMASK_WORDS 32 // 2048 Bit Kapasite (M=2048'e kadar)

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

void process_leaf(int family_id) {
  stats_total.tested_leaves++;

  // 1. Parite Kontrolu (En az 2 tek sayi zorunlulugu)
  int odd_count = 0;
  for (int i = 0; i < P; i++) {
    if (delta[i] % 2 != 0)
      odd_count++;
  }
  if (odd_count < 2) {
    stats_total.prune_parity++;
    return;
  }

  if (stats_total.tested_leaves % 20000000 == 0) {
    clock_t cur_time = clock();
    double elapsed = ((double)(cur_time - g_start_time)) / CLOCKS_PER_SEC;
    double speed =
        (elapsed > 0) ? (stats_total.tested_leaves / 1000000.0) / elapsed : 0.0;
    printf("  >> [Aile %d | %lldM test] Sure: %.1fs | Hiz: %.2fM/s | Aktif: [",
           family_id, stats_total.tested_leaves / 1000000, elapsed, speed);
    for (int i = 0; i < P; i++)
      printf("%d%s", delta[i], i == P - 1 ? "]" : ", ");
    printf(" | En Iyi: %d\n", best_score);
    fflush(stdout);
  }

  int score = deltatomax_c(delta, P);

  // Eger yeni bir en yuksek skor bulunursa veya mevcut rekor ile ayni skorda
  // alternatif bulunursa
  if (score > best_score || (score == best_score && score > 0)) {
    if (score > best_score) {
      best_score = score;
      bool is_new = kb_update(&g_kb, P, score, delta);
      printf("  >>> [AILE %d - %s: %d] Delta: [", family_id,
             is_new ? "YENI REKOR" : "EN IYI", score);
      for (int i = 0; i < P; i++)
        printf("%d%s", delta[i], i == P - 1 ? "]\n" : ", ");
      fflush(stdout);
      if (score > bilinen_max)
        bilinen_max = score;
    } else if (score == best_score && score > 0) {
      kb_update(&g_kb, P, score, delta);
      printf("  -> [Aile %d] Cozum #%d: [", family_id,
             g_kb.table[P].solutions_count);
      for (int i = 0; i < P; i++)
        printf("%d%s", delta[i], i == P - 1 ? "]\n" : ", ");
      fflush(stdout);
    }
  }
}

// ============================================================================
// AILE 1: SIKI MODULER ZINCIR ARAMASI (M / Sum ~ 2.0, P_son = M/2)
// ============================================================================
void search_family_1(int depth, int current_sum, int target_sum) {
  stats_total.total_nodes++;
  stats_f1.total_nodes++;

  // Kalan adimlar icin ileriye donuk kesin aralik kontrolu
  int remaining = P - depth;
  if (current_sum + remaining * 1 > target_sum + 3)
    return;
  if (current_sum + remaining * delta[0] < target_sum)
    return;

  if (depth == P - 1) {
    // Son eleman Aile 1'de kesinlikle {1, 2}'dir!
    for (int last_d = 1; last_d <= 2; last_d++) {
      int final_sum = current_sum + last_d;
      if (final_sum >= target_sum && final_sum <= target_sum + 3) {
        delta[depth] = last_d;
        process_leaf(1);
      }
    }
    return;
  }

  // Matematiksel Sinirlama:
  // Kuyruk Bolgesi (son 7 adim): d <= 4 (kucuk moduler artik kapaticilari)
  // Govde Bolgesi (ilk adimlar) : d <= delta[0] (moduler tavan)
  int max_d = (depth >= P - 7) ? 4 : delta[0];

  // En guclu moduler adaylari once test etmek icin buyukten kucuge tara
  for (int d = max_d; d >= 1; d--) {
    delta[depth] = d;
    search_family_1(depth + 1, current_sum + d, target_sum);
  }
}

// ============================================================================
// AILE 2: CIFT KUMELI KOPRU ARAMASI (Tam 1 adet buyuk kopru sicramasi)
// ============================================================================
void search_family_2(int depth, int current_sum, int bridge_pos, int bridge_val,
                     int target_sum) {
  stats_total.total_nodes++;
  stats_f2.total_nodes++;

  int remaining = P - depth;
  if (current_sum + remaining * 1 > target_sum + 4)
    return;
  if (current_sum + remaining * (2 * P) < target_sum - 4)
    return;

  if (depth == P - 1) {
    // Son eleman Aile 2'de kesinlikle {1, 2}'dir!
    for (int last_d = 1; last_d <= 2; last_d++) {
      delta[depth] = last_d;
      process_leaf(2);
    }
    return;
  }

  if (depth == bridge_pos) {
    delta[depth] = bridge_val;
    search_family_2(depth + 1, current_sum + bridge_val, bridge_pos, bridge_val,
                    target_sum);
    return;
  }

  // Kume elemanlari: Kucuk adimlar
  int max_step = (depth < bridge_pos) ? 4 : 2 * P;
  for (int d = 1; d <= max_step; d++) {
    if (depth == 1 && d > (P - 1) * delta[0])
      break;
    delta[depth] = d;
    search_family_2(depth + 1, current_sum + d, bridge_pos, bridge_val,
                    target_sum);
  }
}

// ============================================================================
// AILE 3: ASIMETRIK UC SICRAMALI ARAMASI (d_son = M/2, P_son = M)
// ============================================================================
void search_family_3(int depth, int current_sum, int tail_val) {
  stats_total.total_nodes++;
  stats_f3.total_nodes++;

  if (depth == P - 1) {
    // Son eleman M/2 sicramasi!
    delta[depth] = tail_val;
    process_leaf(3);
    return;
  }

  // Ara elemanlar (1 .. 2*P)
  for (int d = 1; d <= 2 * P; d++) {
    if (depth == 1 && d > (P - 1) * delta[0])
      break;
    delta[depth] = d;
    search_family_3(depth + 1, current_sum + d, tail_val);
  }
}

// ============================================================================
// ANA PROGRAM VE 3-AILE YURUTUCUSU
// ============================================================================
int main(int argc, char **argv) {
  kb_init(&g_kb);

  if (argc > 1)
    P = atoi(argv[1]);
  if (argc > 2)
    bilinen_max = atoi(argv[2]);
  else
    bilinen_max = kb_get_target_max(&g_kb, P);
  if (argc > 3)
    ENABLE_FAMILY_2 = (atoi(argv[3]) != 0);
  if (argc > 4)
    ENABLE_FAMILY_3 = (atoi(argv[4]) != 0);
  hedef_esik = bilinen_max;

  printf("====================================================================="
         "=\n");
  printf("         PSPP MORFOLOJIK 3-AILE BAZLI AKILLI ARAMA MOTORU            "
         "\n");
  printf("====================================================================="
         "=\n");
  printf("Parametreler : P=%d, Hedef Esik >= %d\n", P, bilinen_max);
  printf("Arama Modu   : Aile 1 [AKTIF] | Aile 2 [%s] | Aile 3 [%s]\n",
         ENABLE_FAMILY_2 ? "AKTIF" : "KAPALI",
         ENABLE_FAMILY_3 ? "AKTIF" : "KAPALI");
  printf("Veritabani   : %s (Kalici Otomatik Depolama)\n", DB_FILE);
  printf("---------------------------------------------------------------------"
         "-\n");
  if (g_kb.table[P].score > 0) {
    printf("Bilgi Tabanindaki Mevcut Kayit:\n");
    kb_print_record(&g_kb.table[P]);
  }
  printf("====================================================================="
         "=\n\n");

  clock_t start_all = clock();
  g_start_time = start_all;

  // --------------------------------------------------------------------------
  // ASAMA 1: AILE 1 (SIKI MODULER ZINCIR) ARAMASI (DEFAULT AKTIF)
  // --------------------------------------------------------------------------
  printf(">>> [FAZ 1/3]: Aile 1 (Siki Moduler Aritmetik) Aramasi "
         "Baslatiliyor...\n");
  clock_t f1_start = clock();
  int min_target_sum = (hedef_esik > 0) ? ((hedef_esik + 1) / 2) : 1;
  int max_f1_d0 = (P > 12) ? 12 : P;

  // Hedef toplami alt esikten baslatip yukari dogru tara
  for (int cur_target = min_target_sum; cur_target <= min_target_sum + 16;
       cur_target += 2) {
    if (cur_target <= 0)
      continue;
    for (int d0 = max_f1_d0; d0 >= 2; d0 -= (d0 >= 6 ? 2 : 1)) {
      delta[0] = d0;
      search_family_1(1, d0, cur_target);
    }
  }
  double f1_time = ((double)(clock() - f1_start)) / CLOCKS_PER_SEC;
  printf(
      "    [Faz 1 Tamamlandi]: Sure: %.4fs | Dugum: %lld | En Iyi Skor: %d\n\n",
      f1_time, stats_f1.total_nodes, best_score);

  // --------------------------------------------------------------------------
  // ASAMA 2: AILE 2 (CIFT KUMELI KOPRU) ARAMASI (OPSIYONEL)
  // --------------------------------------------------------------------------
  if (ENABLE_FAMILY_2) {
    printf(">>> [FAZ 2/3]: Aile 2 (Cift Kumeli Kopru Sicramasi) Aramasi "
           "Baslatiliyor...\n");
    clock_t f2_start = clock();
    int min_bridge = 2 * P - 4;
    int max_bridge = 4 * P - 10;
    if (min_bridge < 4)
      min_bridge = 4;

    for (int bridge_pos = (P / 2) - 1; bridge_pos <= (P / 2) + 1;
         bridge_pos++) {
      for (int bridge_val = min_bridge; bridge_val <= max_bridge;
           bridge_val++) {
        for (int d0 = 1; d0 <= 4; d0++) {
          delta[0] = d0;
          search_family_2(1, d0, bridge_pos, bridge_val, min_target_sum);
        }
      }
    }
    double f2_time = ((double)(clock() - f2_start)) / CLOCKS_PER_SEC;
    printf("    [Faz 2 Tamamlandi]: Sure: %.4fs | Dugum: %lld\n\n", f2_time,
           stats_f2.total_nodes);
  } else {
    printf(">>> [FAZ 2/3]: Aile 2 (Cift Kumeli Kopru Sicramasi) - ATLANDI "
           "(Kapali)\n\n");
  }

  // --------------------------------------------------------------------------
  // ASAMA 3: AILE 3 (ASIMETRIK UC SICRAMASI) ARAMASI (OPSIYONEL)
  // --------------------------------------------------------------------------
  if (ENABLE_FAMILY_3) {
    printf(">>> [FAZ 3/3]: Aile 3 (Asimetrik Uc Sicramasi) Aramasi "
           "Baslatiliyor...\n");
    clock_t f3_start = clock();
    for (int tail_val = min_target_sum; tail_val <= min_target_sum + 20;
         tail_val += 2) {
      for (int d0 = 2; d0 <= P; d0++) {
        delta[0] = d0;
        search_family_3(1, d0, tail_val);
      }
    }
    double f3_time = ((double)(clock() - f3_start)) / CLOCKS_PER_SEC;
    printf("    [Faz 3 Tamamlandi]: Sure: %.4fs | Dugum: %lld\n\n", f3_time,
           stats_f3.total_nodes);
  } else {
    printf(">>> [FAZ 3/3]: Aile 3 (Asimetrik Uc Sicramasi) - ATLANDI "
           "(Kapali)\n\n");
  }

  double total_time = ((double)(clock() - start_all)) / CLOCKS_PER_SEC;

  // --------------------------------------------------------------------------
  // RAPORLAMA VE ISTATISTIKLER
  // --------------------------------------------------------------------------
  printf("====================================================================="
         "=\n");
  printf("              3-AILE MORFOLOJIK ARAMA RAPORU                         "
         " \n");
  printf("====================================================================="
         "=\n");
  printf("Toplam Dugum Ziyareti (DFS Calls) : %lld\n", stats_total.total_nodes);
  printf("Test Edilen Yaprak (Kombinasyon)  : %lld\n",
         stats_total.tested_leaves);
  printf("En Yuksek Skor                    : %d\n", best_score);
  printf("Bulunan Cozum Sayisi              : %d\n",
         g_kb.table[P].solutions_count);
  printf("Gecen Toplam Sure                 : %.4f saniye\n", total_time);
  if (total_time > 0) {
    printf("Ortalama Arama Hizi               : %.2f Milyon test/sn\n",
           (stats_total.tested_leaves / 1000000.0) / total_time);
  }
  printf("---------------------------------------------------------------------"
         "-\n");
  printf("[BUDAMA ETKINLIK RAPORU]\n");
  printf(" - Parite Kurali (Tek sayi < 2)   : %lld yaprak elendi\n",
         stats_total.prune_parity);
  printf("====================================================================="
         "=\n");

  printf("\n[Guncellenmis DP Tablo Kaydi - pspp_database.json Kaydedildi]:\n");
  kb_print_record(&g_kb.table[P]);
  printf("====================================================================="
         "=\n");

  return 0;
}
