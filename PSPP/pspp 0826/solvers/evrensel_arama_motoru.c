#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MAX_WORDS 32 // 2048 bit kapasite (M=2048'e kadar destekler)
#define MAX_P 64

static int g_target_p = 15;
static int g_base_val = 10;
static int g_min_base_count = 6;
static int g_auto_db = 1;

static int g_best_m = 0;
static int g_best_delta[MAX_P];
static int g_best_p[MAX_P];
static uint64_t g_total_evaluated = 0;
static clock_t g_start_time = 0;
static clock_t g_last_log_time = 0;

// Hizli bitmask skor hesaplayici (__builtin_ctzll ile O(1) kopuş tespiti)
static inline int get_score(const int *p, int n) {
  uint64_t seen[MAX_WORDS] = {0};

  // Tekil elemanlar
  for (int i = 0; i < n; i++) {
    int v = p[i];
    if (v > 0 && v < 2048) {
      seen[v >> 6] |= (1ULL << (v & 63));
    }
  }

  // Ikili toplamlar ve farklar
  for (int i = 0; i < n; i++) {
    for (int j = i; j < n; j++) {
      int s = p[i] + p[j];
      int d = p[j] - p[i];
      if (s < 2048)
        seen[s >> 6] |= (1ULL << (s & 63));
      if (d > 0 && d < 2048)
        seen[d >> 6] |= (1ULL << (d & 63));
    }
  }

  // Kesintisiz 1..M araligini CTZ ile aninda bul
  int m = 0;
  for (int w = 0; w < MAX_WORDS; w++) {
    uint64_t val = ~seen[w];
    if (w == 0)
      val &= ~1ULL; // 0. biti yoksay (1'den basliyoruz)
    if (val == 0) {
      m += 64;
    } else {
      m += __builtin_ctzll(val);
      break;
    }
  }
  return m - 1;
}

// Otomatik veritabani kaydi icin db_manager.py cagrisi
void trigger_db_manager(const int *delta, int n) {
  char script_path[128] = "db_manager.py";
  FILE *test_f = fopen("db_manager.py", "r");
  if (test_f) {
    fclose(test_f);
  } else {
    strcpy(script_path, "../db_manager.py");
  }

  char cmd[1024];
  int offset = sprintf(cmd, "python \"%s\" --delta ", script_path);
  for (int i = 0; i < n; i++) {
    offset += sprintf(cmd + offset, "%d%s", delta[i], (i == n - 1) ? "" : ",");
  }
  system(cmd);
}

// Rekursif Akilli Kuyruk Aramasi
void dfs_tail(int depth, int target_depth, int *delta, int *p,
              int max_allowed_step) {
  if (depth == target_depth) {
    g_total_evaluated++;

    // Her ~500k testte bir kontrol et: 5 saniye dolmussa kesinlikle anlik
    // durumu bas
    if ((g_total_evaluated & 0x7FFFF) == 0) {
      clock_t now = clock();
      double sec_since_last = (double)(now - g_last_log_time) / CLOCKS_PER_SEC;
      if (sec_since_last >= 5.0) {
        g_last_log_time = now;
        double total_elapsed = (double)(now - g_start_time) / CLOCKS_PER_SEC;
        double speed =
            (total_elapsed > 0) ? (g_total_evaluated / total_elapsed / 1e6) : 0;
        printf(
            "  >> [Ilerleme: %lluM test | %.1fs] Hiz: %.2fM/s | Aktif Dal: [",
            (unsigned long long)(g_total_evaluated / 1000000), total_elapsed,
            speed);
        for (int i = 0; i < g_target_p; i++) {
          printf("%d%s", delta[i], (i == g_target_p - 1) ? "" : ", ");
        }
        printf("] | Anlik Zirve: M = %d\n", g_best_m);
        fflush(stdout);
      }
    }

    int m = get_score(p, g_target_p);
    if (m > g_best_m) {
      g_best_m = m;
      memcpy(g_best_delta, delta, sizeof(int) * g_target_p);
      memcpy(g_best_p, p, sizeof(int) * g_target_p);

      printf("\n>>> [YENI ZIRVE BULUNDU] M = %d | P_son = %d | 2xP_son = %d\n",
             m, p[g_target_p - 1], 2 * p[g_target_p - 1]);
      printf("    Delta: [");
      for (int i = 0; i < g_target_p; i++)
        printf("%d%s", delta[i], i == g_target_p - 1 ? "]\n" : ", ");
      fflush(stdout);
    }
    return;
  }

  // Kalan adimlarda olasi adim boyutlari
  int current_idx = depth;
  int limit = max_allowed_step;
  if (limit > g_base_val)
    limit = g_base_val;

  // Kuyruk sonuna yaklastikca adimlar kuculur (moduler aciklari kapatmak icin)
  int remaining = target_depth - depth;
  if (remaining <= 2 && limit > 3)
    limit = 3;
  else if (remaining <= 4 && limit > 5)
    limit = 5;
  else if (remaining <= 6 && limit > 7)
    limit = 7;

  for (int step = limit; step >= 1; step--) {
    delta[current_idx] = step;
    p[current_idx] = (current_idx == 0) ? step : (p[current_idx - 1] + step);

    // Hizli budama: Son eleman * 2, su ana kadarki en iyi skordan kucukse devam
    // etme
    int max_possible = 2 * (p[current_idx] + (remaining - 1) * step);
    if (max_possible < g_best_m && g_best_m > 0) {
      continue;
    }

    int next_limit = (step + 1 <= g_base_val) ? (step + 1) : g_base_val;
    dfs_tail(depth + 1, target_depth, delta, p, next_limit);
  }
}

void run_universal_search() {
  printf("============================================================\n");
  printf("   PSPP EVRENSEL ARAMA MOTORU (UNIVERSAL SOLVER)\n");
  printf("============================================================\n");
  printf("Hedef Boyut (P)       : %d\n", g_target_p);
  printf("Moduler Taban         : %d\n", g_base_val);
  printf("Min Govde Sayisi      : %d\n", g_min_base_count);
  printf("Otomatik DB Kaydi     : %s\n", g_auto_db ? "AKTIF" : "PASIF");
  printf("------------------------------------------------------------\n");

  g_start_time = clock();
  g_last_log_time = g_start_time;
  g_best_m = 0;
  g_total_evaluated = 0;

  int delta[MAX_P] = {0};
  int p[MAX_P] = {0};

  // Farkli govde uzunluklarini dene (min_base_count'tan baslayarak)
  for (int num_base = g_min_base_count; num_base <= g_target_p - 6;
       num_base++) {
    int tail_len = g_target_p - num_base;
    printf("\n[*] Test Ediliyor: %d Adet %d-Govdesi + %d Elemanli Kuyruk...\n",
           num_base, g_base_val, tail_len);

    for (int i = 0; i < num_base; i++) {
      delta[i] = g_base_val;
      p[i] = (i == 0) ? g_base_val : (p[i - 1] + g_base_val);
    }

    // Kuyruk aramasini baslat
    dfs_tail(num_base, g_target_p, delta, p, g_base_val);
  }

  clock_t end_time = clock();
  double elapsed_sec = (double)(end_time - g_start_time) / CLOCKS_PER_SEC;

  printf("\n============================================================\n");
  printf("ARAMA TAMAMLANDI!\n");
  printf("============================================================\n");
  printf("Toplam Test Edilen Dizi : %llu\n",
         (unsigned long long)g_total_evaluated);
  printf("Gecen Sure              : %.2f saniye\n", elapsed_sec);
  printf("Arama Hizi              : %.2f Milyon Dizi/sn\n",
         (elapsed_sec > 0) ? (g_total_evaluated / elapsed_sec / 1e6) : 0);
  printf("EN YUKSEK SKOR          : M = %d\n", g_best_m);
  if (g_best_m > 0) {
    printf("Optimal Delta           : [");
    for (int i = 0; i < g_target_p; i++)
      printf("%d%s", g_best_delta[i], i == g_target_p - 1 ? "]\n" : ", ");
    printf("Kumulatif Dizi          : [");
    for (int i = 0; i < g_target_p; i++)
      printf("%d%s", g_best_p[i], i == g_target_p - 1 ? "]\n" : ", ");

    if (g_auto_db) {
      printf("\n[*] Bulunan en iyi cozum veritabanina gonderiliyor...\n");
      trigger_db_manager(g_best_delta, g_target_p);
    }
  }
  printf("============================================================\n\n");
}

void interactive_mode() {
  char buf[64];
  printf("============================================================\n");
  printf("         PSPP EVRENSEL ARAMA MOTORU (INTERAKTIF MOD)\n");
  printf("============================================================\n\n");

  printf("Hedef Boyut P (Eleman Sayisi) [Varsayilan: 12]: ");
  if (fgets(buf, sizeof(buf), stdin) && buf[0] != '\n') {
    int val = atoi(buf);
    if (val >= 4 && val <= MAX_P)
      g_target_p = val;
  } else {
    g_target_p = 12;
  }

  printf("Moduler Taban Degeri (Govdedeki Sayi) [Varsayilan: 11]: ");
  if (fgets(buf, sizeof(buf), stdin) && buf[0] != '\n') {
    int val = atoi(buf);
    if (val >= 2)
      g_base_val = val;
  } else {
    g_base_val = 11;
  }

  int default_govde = (g_target_p >= 10) ? (g_target_p - 10) : (g_target_p / 2);
  if (default_govde < 1)
    default_govde = 1;

  printf("Minimum Govde Sayisi (Kac adet taban sayisi ile baslasin) "
         "[Varsayilan: %d]: ",
         default_govde);
  if (fgets(buf, sizeof(buf), stdin) && buf[0] != '\n') {
    int val = atoi(buf);
    if (val >= 1 && val <= g_target_p - 4)
      g_min_base_count = val;
    else
      g_min_base_count = default_govde;
  } else {
    g_min_base_count = default_govde;
  }

  printf("Bulunan en iyi sonuc pspp_database.json'a kaydedilsin mi? (E/h) "
         "[Varsayilan: E]: ");
  if (fgets(buf, sizeof(buf), stdin) &&
      (buf[0] == 'h' || buf[0] == 'H' || buf[0] == 'n' || buf[0] == 'N')) {
    g_auto_db = 0;
  } else {
    g_auto_db = 1;
  }

  printf("\n");
  run_universal_search();

  printf("\nCikis yapmak icin Enter tusuna basin...");
  getchar();
}

int main(int argc, char *argv[]) {
  // Eger hic arguman verilmemisse kullaniciya sor (Interaktif Mod)
  if (argc == 1) {
    interactive_mode();
    return 0;
  }

  // Komut satiri parametrelerini ayrıştır
  for (int i = 1; i < argc; i++) {
    if (strcmp(argv[i], "--p") == 0 && i + 1 < argc) {
      g_target_p = atoi(argv[++i]);
    } else if (strcmp(argv[i], "--taban") == 0 && i + 1 < argc) {
      g_base_val = atoi(argv[++i]);
    } else if (strcmp(argv[i], "--govde") == 0 && i + 1 < argc) {
      g_min_base_count = atoi(argv[++i]);
    } else if (strcmp(argv[i], "--no-db") == 0) {
      g_auto_db = 0;
    } else if (strcmp(argv[i], "--help") == 0 || strcmp(argv[i], "-h") == 0) {
      printf("Kullanim: %s --p <N> --taban <B> --govde <K> [--no-db]\n",
             argv[0]);
      printf("Ornek   : %s --p 21 --taban 12 --govde 8\n", argv[0]);
      return 0;
    }
  }

  if (g_target_p < 4 || g_target_p > MAX_P) {
    printf("[HATA] P degeri 4 ile %d arasinda olmalidir!\n", MAX_P);
    return 1;
  }

  run_universal_search();
  return 0;
}
