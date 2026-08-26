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
  long long total_nodes;       // Toplam girilen dugum (DFS fonksiyon cagrisi)
  long long tested_leaves;     // Test edilen yaprak sayisi
  long long prune_kural1_leaf; // Kural 1: Yetersiz toplam nedeniyle elenenler
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
void dfs_ters(int depth, int current_sum, int teorik_tavan) {
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
    if (current_sum < min_required_sum) {
      stats.prune_kural1_leaf++;
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
        double speed = (total_elapsed > 0)
                           ? (stats.tested_leaves / total_elapsed / 1e6)
                           : 0;
        printf(
            "  >> [Ilerleme: %lldM test | %.1fs] Hiz: %.2fM/s | Aktif Delta: [",
            stats.tested_leaves / 1000000, total_elapsed, speed);
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
        bool is_new = kb_update(&g_kb, P, score, delta);
        printf("  >>> [%s: M = %d] Delta: [", is_new ? "YENI REKOR" : "EN IYI",
               score);
        for (int i = 0; i < P; i++)
          printf("%d%s", delta[i], i == P - 1 ? "]\n" : ", ");
        fflush(stdout);
        if (score > bilinen_max)
          bilinen_max = score;
      } else if (score == best_score) {
        kb_update(&g_kb, P, score, delta);
        printf("  -> Cozum #%d: [", g_kb.table[P].solutions_count);
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

  for (int d = 1; d <= max_d; d++) {
    delta[depth] = d;
    dfs_ters(depth - 1, current_sum + d, teorik_tavan);
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
  best_score = 0;

  // Aramayi P-1. indeksten baslat (depth = P - 1)
  dfs_ters(P - 1, 0, teorik_tavan);

  clock_t end = clock();
  double cpu_time = ((double)(end - g_start_time)) / CLOCKS_PER_SEC;

  printf("\n==================================================================="
         "===\n");
  printf("                     ARAMA VE BUDAMA ISTATISTIKLERI                  "
         " \n");
  printf("====================================================================="
         "=\n");
  printf("Toplam Dugum Ziyareti (DFS Calls) : %lld\n", stats.total_nodes);
  printf("Test Edilen Yaprak (Kombinasyon)  : %lld\n", stats.tested_leaves);
  printf("En Yuksek Skor                    : M = %d\n", best_score);
  printf("Bulunan Cozum Sayisi              : %d\n",
         g_kb.table[P].solutions_count);
  printf("Gecen Toplam Sure                 : %.4f saniye\n", cpu_time);
  if (cpu_time > 0) {
    printf("Ortalama Arama Hizi               : %.2f Milyon test/sn\n",
           (stats.tested_leaves / 1000000.0) / cpu_time);
  }
  printf("---------------------------------------------------------------------"
         "-\n");
  printf("[BUDAMA ETKINLIK RAPORU]\n");
  printf(" - Kural 1 (Yetersiz Toplam Dal Budamasi) : %lld dal elendi\n",
         stats.prune_kural1_leaf);
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
