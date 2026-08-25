/*
 * PSPP Moduler Hizli Arama Motoru (fast_modular_solver.c)
 *
 * Bu program, Aile 1 (Siki Moduler Zincir) yapisini kullanarak istenen herhangi
 * bir P degeri icin en yuksek skorlu dizileri saniyeler icinde bulur ve
 * dogrudan ekrana basar.
 *
 * Derleme:
 *   gcc -O3 -finput-charset=UTF-8 -fexec-charset=UTF-8 fast_modular_solver.c -o
 * fast_modular_solver.exe
 *
 * Calistirma:
 *   .\fast_modular_solver.exe 17
 */

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define MAX_P 64

int P = 12;
int best_score = 0;
int delta[MAX_P];
long long total_tested = 0;
clock_t start_time;

#define BITMASK_WORDS 32 // 2048 Bit Kapasite (M=2048'e kadar)

// Donanimsal 64-Bit CTZ Skor Hesaplayici
static inline int calculate_score(const int *d, int n) {
  int p[MAX_P];
  int sum = 0;
  for (int i = 0; i < n; i++) {
    sum += d[i];
    p[i] = sum;
  }

  uint64_t seen[BITMASK_WORDS] = {0};

  for (int i = 0; i < n; i++) {
    int val = p[i];
    if (val > 0 && val < 2048)
      seen[val >> 6] |= (1ULL << (val & 63));
  }

  for (int i = 0; i < n; i++) {
    for (int j = i; j < n; j++) {
      int s = p[i] + p[j];
      if (s < 2048)
        seen[s >> 6] |= (1ULL << (s & 63));

      int diff = p[j] - p[i];
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

// Rekursif Budamali Arama
void search_modular(int depth, int current_sum, int target_sum, int base_step) {
  int remaining = P - depth;

  // Ileriye donuk toplam aralik budamasi
  if (current_sum + remaining * 1 > target_sum + 5)
    return;
  if (current_sum + remaining * base_step < target_sum)
    return;

  if (depth == P - 1) {
    // Son eleman kurali: Kesinlikle {1, 2}
    for (int last_d = 2; last_d >= 1; last_d--) {
      int final_sum = current_sum + last_d;
      if (final_sum >= target_sum && final_sum <= target_sum + 5) {
        delta[depth] = last_d;
        total_tested++;

        // Parite Kurali (En az 2 tek sayi)
        int odd_count = 0;
        for (int i = 0; i < P; i++) {
          if (delta[i] % 2 != 0)
            odd_count++;
        }
        if (odd_count < 2)
          continue;

        int score = calculate_score(delta, P);

        if (score > best_score) {
          best_score = score;
          clock_t cur_time = clock();
          double elapsed = ((double)(cur_time - start_time)) / CLOCKS_PER_SEC;
          printf("  >>> [YENI REKOR: %d] Sure: %.3fs | Delta: [", score,
                 elapsed);
          for (int i = 0; i < P; i++)
            printf("%d%s", delta[i], i == P - 1 ? "]\n" : ", ");

          // Normal diziyi de hesapla ve yaz
          printf("      Prefix Dizi: [");
          int s = 0;
          for (int i = 0; i < P; i++) {
            s += delta[i];
            printf("%d%s", s, i == P - 1 ? "]\n" : ", ");
          }
          printf("      Dizi Toplami: %d | M / Toplam Orani: %.2f\n\n",
                 final_sum, (double)score / final_sum);
          fflush(stdout);
        } else if (score == best_score && score > 0) {
          printf("  -> [AYNI REKORDA ALTERNATIF: %d] Delta: [", score);
          for (int i = 0; i < P; i++)
            printf("%d%s", delta[i], i == P - 1 ? "]\n" : ", ");
          fflush(stdout);
        }
      }
    }
    return;
  }

  // Bolgesel Budama Kurali:
  // Kuyruk boyutu: Kucuk P'lerde son 4-5 adim, buyuk P'lerde son 6-7 adimdir
  int tail_len = (P <= 6) ? 3 : ((P <= 8) ? 5 : ((P <= 12) ? 6 : 7));
  int max_tail_val = (base_step <= 8) ? 4 : (base_step / 2);

  // Govde Bolgesi (depth < P - tail_len): d <= base_step
  // Kuyruk Bolgesi (depth >= P - tail_len): d <= max_tail_val
  int max_d = (depth >= P - tail_len) ? max_tail_val : base_step;

  for (int d = max_d; d >= 1; d--) {
    delta[depth] = d;
    search_modular(depth + 1, current_sum + d, target_sum, base_step);
  }
}

int main(int argc, char *argv[]) {
  if (argc > 1)
    P = atoi(argv[1]);

  // Dinamik Taban Adimi Hesabi (P olceklendirmesi)
  int optimal_base =
      (P <= 6) ? 4 : ((P <= 10) ? 6 : ((P <= 20) ? 8 : ((((P / 2) / 2) * 2))));
  int min_base = (optimal_base > 6) ? 6 : 4;

  // Dinamik Hedef Toplam Formulu: Expected P_son = M/2 ~ P*(P+1)/3
  int expected_target = (P <= 6) ? 16 : ((P * (P + 1)) / 3);

  printf("====================================================================="
         "=\n");
  printf("         PSPP TAM DINAMIK MODULER ARAMA MOTORU                       "
         " \n");
  printf("====================================================================="
         "=\n");
  printf("Parametreler : P = %d\n", P);
  printf("Dinamik Kural: Hedef Taban = %d (Aralik: %d .. %d)\n", optimal_base,
         min_base, optimal_base);
  printf("               Hedef Toplam = ~%d [P*(P+1)/3]\n", expected_target);
  printf("====================================================================="
         "=\n\n");

  start_time = clock();

  // Dinamik hedef toplam araliginda tarama
  int sweep_range = (P <= 10) ? 4 : (P <= 20 ? 6 : (P / 3));

  for (int t_offset = -sweep_range; t_offset <= sweep_range; t_offset += 2) {
    int target = expected_target + t_offset;
    if (target <= 0)
      continue;

    for (int d0 = optimal_base; d0 >= min_base; d0 -= 2) {
      delta[0] = d0;
      search_modular(1, d0, target, d0);
    }
  }

  double total_time = ((double)(clock() - start_time)) / CLOCKS_PER_SEC;
  printf("\n==================================================================="
         "===\n");
  printf("ARAMA TAMAMLANDI!\n");
  printf("En Yuksek Skor : %d\n", best_score);
  printf("Toplam Sure    : %.4f saniye\n", total_time);
  printf("Test Edilen    : %lld kombinasyon\n", total_tested);
  printf("====================================================================="
         "=\n");

  return 0;
}
