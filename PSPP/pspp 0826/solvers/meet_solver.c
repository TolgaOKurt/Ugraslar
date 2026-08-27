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
#include <stdbool.h>

#ifdef _OPENMP
#include <omp.h>
#endif

/*
 * ==============================================================================
 * PSPP ORTADA BULUSMA ARAMA MOTORU (meet_solver.c)
 * ==============================================================================
 * 
 * BU MOTORUN MİMARİSİ (MEET-IN-THE-MIDDLE / BÖL VE FETHET):
 * ---------------------------------------------------------
 * 1. P elemanlı dizi 3 bağımsız bileşene ayrıştırılır:
 *      - Sol Blok   (L eleman) : delta[0] .. delta[L-1]
 *      - Köprü      (1 eleman) : delta[L] (d_bridge)
 *      - Sağ Blok   (R eleman) : delta[L+1] .. delta[P-1]
 * 
 * 2. Öteleme Bağımsızlığı (Shift Invariance):
 *      - Sağ blokların iç farkları köprüden ve sol adadan bağımsızdır.
 *      - Sol ve Sağ tablolar RAM'de bir kez önceden üretilir (Precomputation).
 * 
 * 3. Hızlı Eşleştirme (Cross-Term Matching):
 *      - Çapraz farklar ve toplamlar 2048-bit bitmask register'ında birleştirilir.
 *      - OpenMP ile çekirdeklere dağıtılarak milisaniyeler içinde rekor bulunur.
 * ==============================================================================
 */

#define BITMASK_WORDS 32 // 2048 Bit Donanimsal Kapasite
#define MAX_LEFT_CANDS   150000
#define MAX_RIGHT_CANDS   80000

// Sol Küme Adayı Yapısı
typedef struct {
  int delta[MAX_P];
  int p[MAX_P];
  int sum_L;
  bool has_one;
  bool has_two;
  uint64_t internal_mask[BITMASK_WORDS];
} LeftCandidate;

// Sağ Küme Adayı Yapısı
typedef struct {
  int delta[MAX_P];
  int q[MAX_P]; // Relatif pullar (0'a gore)
  int sum_R;
  bool has_one;
  uint64_t internal_diff_mask[BITMASK_WORDS];
} RightCandidate;

LeftCandidate *g_left_cands = NULL;
int g_left_count = 0;

RightCandidate *g_right_cands = NULL;
int g_right_count = 0;

int g_P = 8;
int g_L = 3;
int g_R = 4;
int g_hedef_esik = 0;
int g_bilinen_max = 0;
int g_num_threads = 8;
int g_best_score = 0;
PSPP_KnowledgeBase g_kb;

clock_t g_start_clock;
double g_start_wall;

// 64-Bit CTZ Skorlayıcı
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
    if (w == 0) val &= ~1ULL;
    if (val == 0) m += 64;
    else { m += __builtin_ctzll(val); break; }
  }
  return m - 1;
}

// Sol Blokları Üret
void generate_left_rec(int depth, int current_sum, int max_sum, int *cur_delta) {
  if (depth == g_L) {
    if (g_left_count >= MAX_LEFT_CANDS) return;
    LeftCandidate *cand = &g_left_cands[g_left_count++];
    cand->sum_L = current_sum;
    cand->has_one = false;
    cand->has_two = false;

    int cur = 0;
    for (int i = 0; i < g_L; i++) {
      cand->delta[i] = cur_delta[i];
      if (cur_delta[i] == 1) cand->has_one = true;
      if (cur_delta[i] == 2) cand->has_two = true;
      cur += cur_delta[i];
      cand->p[i] = cur;
    }

    memset(cand->internal_mask, 0, sizeof(cand->internal_mask));
    for (int i = 0; i < g_L; i++) {
      int pi = cand->p[i];
      if (pi < 2048) cand->internal_mask[pi >> 6] |= (1ULL << (pi & 63));
      for (int j = i; j < g_L; j++) {
        int pj = cand->p[j];
        int sum = pj + pi;
        int diff = pj - pi;
        if (sum < 2048) cand->internal_mask[sum >> 6] |= (1ULL << (sum & 63));
        if (diff > 0 && diff < 2048) cand->internal_mask[diff >> 6] |= (1ULL << (diff & 63));
      }
    }
    return;
  }

  int limit = (depth == 0) ? g_P : 18;
  for (int d = 1; d <= limit; d++) {
    cur_delta[depth] = d;
    generate_left_rec(depth + 1, current_sum + d, max_sum, cur_delta);
  }
}

// Sağ Blokları Üret (Öteleme Bağımsız)
void generate_right_rec(int depth, int current_sum, int max_sum, int *cur_delta) {
  if (depth == g_R) {
    if (g_right_count >= MAX_RIGHT_CANDS) return;
    RightCandidate *cand = &g_right_cands[g_right_count++];
    cand->sum_R = current_sum;
    cand->has_one = false;

    int cur = 0;
    for (int i = 0; i < g_R; i++) {
      cand->delta[i] = cur_delta[i];
      if (cur_delta[i] == 1) cand->has_one = true;
      cur += cur_delta[i];
      cand->q[i] = cur;
    }

    memset(cand->internal_diff_mask, 0, sizeof(cand->internal_diff_mask));
    for (int i = 0; i < g_R; i++) {
      int qi = cand->q[i];
      for (int j = i + 1; j < g_R; j++) {
        int qj = cand->q[j];
        int diff = qj - qi;
        if (diff > 0 && diff < 2048) cand->internal_diff_mask[diff >> 6] |= (1ULL << (diff & 63));
      }
    }
    return;
  }

  int limit = (depth == g_R - 1) ? 26 : 18;
  for (int d = 1; d <= limit; d++) {
    cur_delta[depth] = d;
    generate_right_rec(depth + 1, current_sum + d, max_sum, cur_delta);
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

static double g_last_log_wtime = 0.0;

void run_meet_solver() {
  printf("=====================================================================\n");
  printf("          PSPP ORTADA BULUSMA ARAMA MOTORU (meet_solver)             \n");
  printf("=====================================================================\n");
  printf("Boyut (P)        : %d (Sol: %d, Kopru: 1, Sag: %d)\n", g_P, g_L, g_R);
  printf("Is Parcacigi     : %d Thread (OpenMP Paralel)\n", g_num_threads);
  printf("Zemin Citasi     : Hedef Esik >= %d\n", g_hedef_esik);
  printf("Veritabani       : pspp_database.json\n");
  printf("---------------------------------------------------------------------\n");
  if (g_kb.table[g_P].score > 0) {
    printf("Bilgi Tabanindaki Mevcut Kayit:\n");
    kb_print_record(&g_kb.table[g_P]);
  }
  printf("=====================================================================\n\n");

  g_left_cands = (LeftCandidate *)malloc(sizeof(LeftCandidate) * MAX_LEFT_CANDS);
  g_right_cands = (RightCandidate *)malloc(sizeof(RightCandidate) * MAX_RIGHT_CANDS);
  g_left_count = 0;
  g_right_count = 0;

  printf(">> Sol ve Sag ada tablolari onbellege aliniyor... ");
  fflush(stdout);

  clock_t t0 = clock();
  int tmp[MAX_P];
  generate_left_rec(0, 0, 40, tmp);
  generate_right_rec(0, 0, 40, tmp);
  clock_t t1 = clock();
  double prep_time = (double)(t1 - t0) / CLOCKS_PER_SEC;

  printf("TAMAMLANDI (%.3f sn)\n", prep_time);
  printf("   - Sol Aday Sayisi : %d\n", g_left_count);
  printf("   - Sag Aday Sayisi : %d\n", g_right_count);
  printf(">> Ortada capraz eslestirme baslatiliyor...\n\n");
  fflush(stdout);

  g_start_clock = clock();
#ifdef _OPENMP
  g_start_wall = omp_get_wtime();
#else
  g_start_wall = (double)clock() / CLOCKS_PER_SEC;
#endif
  g_last_log_wtime = g_start_wall;

  int min_required_sum = (g_hedef_esik + 1) / 2;
  int teorik_tavan = g_P * g_P + g_P;
  long long total_matches_checked = 0;
  g_best_score = 0;

#pragma omp parallel num_threads(g_num_threads) reduction(+:total_matches_checked)
  {
    int local_full_delta[MAX_P];
    long long local_checked = 0;

#pragma omp for schedule(dynamic, 64)
    for (int l = 0; l < g_left_count; l++) {
      LeftCandidate *lc = &g_left_cands[l];

      int max_b = teorik_tavan - lc->sum_L - g_R;
      if (max_b > 30) max_b = 30;

      for (int d_bridge = 1; d_bridge <= max_b; d_bridge++) {
        for (int r = 0; r < g_right_count; r++) {
          RightCandidate *rc = &g_right_cands[r];

          // Kanun 1: Alt Eşik Kontrolü
          int total_sum = lc->sum_L + d_bridge + rc->sum_R;
          if (total_sum < min_required_sum || total_sum > teorik_tavan)
            continue;

          // Kanun 2: 1'in Üretilmesi Kontrolü
          if (!lc->has_one && !rc->has_one && d_bridge != 1)
            continue;

          total_matches_checked++;
          local_checked++;

          // 5 Saniyede Bir Canli Nabiz / Ilerleme Logu (Thread-Safe)
          if ((local_checked & 0x1FFFF) == 0) {
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
                  double spd = (elapsed > 0) ? (total_matches_checked / elapsed / 1e6) : 0;
                  printf("  >> [Ilerleme: %lldM esleme | %.1fs] Hiz: %.2fM/s | Aktif Sol: [",
                         total_matches_checked / 1000000, elapsed, spd);
                  for (int i = 0; i < g_L; i++)
                    printf("%d%s", lc->delta[i], i == g_L - 1 ? "" : ", ");
                  printf("] | Kopru: %d | Aktif Sag: [", d_bridge);
                  for (int i = 0; i < g_R; i++)
                    printf("%d%s", rc->delta[i], i == g_R - 1 ? "" : ", ");
                  printf("] | Anlik En Iyi: M = %d\n", g_best_score);
                  fflush(stdout);
                }
              }
            }
          }

          // Tam Delta Dizisini Oluştur
          for (int i = 0; i < g_L; i++) local_full_delta[i] = lc->delta[i];
          local_full_delta[g_L] = d_bridge;
          for (int i = 0; i < g_R; i++) local_full_delta[g_L + 1 + i] = rc->delta[i];

          int score = deltatomax_c(local_full_delta, g_P);

          if (score >= g_hedef_esik) {
#pragma omp critical
            {
              if (score > g_best_score) {
                g_best_score = score;
                if (score > g_bilinen_max) g_bilinen_max = score;
              }

              int res = kb_update(&g_kb, g_P, score, local_full_delta);
              int sol_idx = kb_find_solution_index(&g_kb.table[g_P], local_full_delta, g_P);

              if (res == KB_NEW_RECORD) {
                printf("  >>> [YENI REKOR: M = %d] Delta: [", score);
                for (int i = 0; i < g_P; i++) printf("%d%s", local_full_delta[i], i == g_P - 1 ? "]\n" : ", ");
                fflush(stdout);
              } else if (res == KB_NEW_ALTERNATIVE) {
                printf("  >>> [YENI KESIF (Alternatif #%d): M = %d] Delta: [",
                       g_kb.table[g_P].stored_solutions_count, score);
                for (int i = 0; i < g_P; i++) printf("%d%s", local_full_delta[i], i == g_P - 1 ? "]\n" : ", ");
                fflush(stdout);
              } else if (res == KB_ALREADY_EXISTS) {
                printf("  -> [DB'DE MEVCUT #%d: M = %d] Delta: [",
                       sol_idx >= 0 ? (sol_idx + 1) : 1, score);
                for (int i = 0; i < g_P; i++) printf("%d%s", local_full_delta[i], i == g_P - 1 ? "]\n" : ", ");
                fflush(stdout);
              }
            }
          }
        }
      }
    }
  }

  double total_time = 0;
#ifdef _OPENMP
  total_time = omp_get_wtime() - g_start_wall;
#else
  total_time = (double)(clock() - g_start_clock) / CLOCKS_PER_SEC;
#endif

  char buf_matches[64];
  format_num(total_matches_checked, buf_matches, sizeof(buf_matches));

  printf("\n=====================================================================\n");
  printf("                     ARAMA VE ESLESTIRME RAPORU                      \n");
  printf("=====================================================================\n");
  printf("Onbellek Uretim Suresi : %.4f saniye\n", prep_time);
  printf("Capraz Esleme Suresi   : %.4f saniye\n", total_time);
  printf("Toplam Gecen Sure      : %.4f saniye\n", prep_time + total_time);
  printf("Test Edilen Eslesme    : %s\n", buf_matches);
  printf("En Yuksek Skor         : M = %d\n", g_best_score);
  printf("Bulunan Cozum Sayisi   : %d\n", g_kb.table[g_P].solutions_count);
  if (total_time > 0) {
    printf("Esleme Hizi            : %.2f Milyon test/sn\n",
           (total_matches_checked / 1000000.0) / total_time);
  }
  printf("=====================================================================\n");

  if (g_kb.table[g_P].score > 0) {
    printf("\n[Guncellenmis DP Tablo Kaydi - Diske Yazildi]:\n");
    kb_print_record(&g_kb.table[g_P]);
    kb_save_json(&g_kb, DB_FILE);
  }

  free(g_left_cands);
  free(g_right_cands);
}

void interactive_mode() {
  char buf[64];
  printf("=====================================================================\n");
  printf("         PSPP ORTADA BULUSMA ARAMA MOTORU (INTERAKTIF MOD)           \n");
  printf("=====================================================================\n\n");

  printf("Hedef Boyut P (Eleman Sayisi) [Varsayilan: 8]: ");
  if (fgets(buf, sizeof(buf), stdin) && buf[0] != '\n') {
    int val = atoi(buf);
    if (val >= 3 && val < MAX_P) g_P = val;
  } else {
    g_P = 8;
  }

  g_L = g_P / 2;
  g_R = g_P - 1 - g_L;

  printf("Sol Blok Boyutu L [Varsayilan: %d]: ", g_L);
  if (fgets(buf, sizeof(buf), stdin) && buf[0] != '\n') {
    int val = atoi(buf);
    if (val >= 1 && val < g_P - 1) {
      g_L = val;
      g_R = g_P - 1 - g_L;
    }
  }

  printf("Calisacak Is Parcacigi (Thread) Sayisi [Varsayilan: 8]: ");
  if (fgets(buf, sizeof(buf), stdin) && buf[0] != '\n') {
    int val = atoi(buf);
    if (val >= 1) g_num_threads = val;
  } else {
    g_num_threads = 8;
  }

  g_bilinen_max = kb_get_target_max(&g_kb, g_P);
  printf("Hedef Esik / Zemin Citasi [Varsayilan: %d]: ", g_bilinen_max);
  if (fgets(buf, sizeof(buf), stdin) && buf[0] != '\n') {
    int val = atoi(buf);
    if (val >= 0) g_hedef_esik = val;
    else g_hedef_esik = g_bilinen_max;
  } else {
    g_hedef_esik = g_bilinen_max;
  }

  printf("\n");
  run_meet_solver();

  printf("\nCikis yapmak icin Enter tusuna basin...");
  getchar();
}

int main(int argc, char **argv) {
  kb_init(&g_kb);

  if (argc == 1) {
    interactive_mode();
    return 0;
  }

  if (argc > 1) g_P = atoi(argv[1]);

  g_L = g_P / 2;
  g_R = g_P - 1 - g_L;

  for (int i = 2; i < argc; i++) {
    if (strcmp(argv[i], "--threads") == 0 && i + 1 < argc) {
      g_num_threads = atoi(argv[++i]);
    } else if (strcmp(argv[i], "--esik") == 0 && i + 1 < argc) {
      g_hedef_esik = atoi(argv[++i]);
    } else if (strcmp(argv[i], "--left") == 0 && i + 1 < argc) {
      g_L = atoi(argv[++i]);
      g_R = g_P - 1 - g_L;
    } else if (atoi(argv[i]) > 0 && g_hedef_esik == 0) {
      g_hedef_esik = atoi(argv[i]);
    }
  }

  if (g_hedef_esik == 0) {
    g_bilinen_max = kb_get_target_max(&g_kb, g_P);
    g_hedef_esik = g_bilinen_max;
  }

  if (g_num_threads < 1) g_num_threads = 8;

  run_meet_solver();
  return 0;
}
