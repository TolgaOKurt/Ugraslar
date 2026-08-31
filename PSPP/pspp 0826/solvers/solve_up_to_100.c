#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>

#define BITSET_WORDS 256  // 256 * 64 = 16384 bits (scores up to 16,384!)

typedef struct {
    uint64_t w[BITSET_WORDS];
} Bitset;

static inline void bitset_clear(Bitset *b) {
    memset(b->w, 0, sizeof(b->w));
}

static inline void bitset_set(Bitset *b, int v) {
    if (v >= 0 && v < 16384) {
        b->w[v >> 6] |= (1ULL << (v & 63));
    }
}

static inline int bitset_score(const Bitset *b) {
    int m = 0;
    uint64_t w0 = b->w[0] >> 1;
    if ((w0 & 0x7FFFFFFFFFFFFFFFULL) == 0x7FFFFFFFFFFFFFFFULL) {
        m = 63;
        for (int w = 1; w < BITSET_WORDS; w++) {
            uint64_t val = b->w[w];
            if (val == ~0ULL) {
                m += 64;
            } else {
                int tz = __builtin_ctzll(~val);
                return m + tz;
            }
        }
        return m;
    } else {
        return __builtin_ctzll(~w0);
    }
}

int evaluate_delta(const int *delta, int n) {
    static int p_arr[128];
    int sum = 0;
    for (int i = 0; i < n; i++) {
        sum += delta[i];
        p_arr[i] = sum;
    }
    Bitset b;
    bitset_clear(&b);
    for (int i = 0; i < n; i++) {
        bitset_set(&b, p_arr[i]);
    }
    for (int i = 0; i < n; i++) {
        int pi = p_arr[i];
        for (int j = i; j < n; j++) {
            int pj = p_arr[j];
            bitset_set(&b, pj + pi);
            bitset_set(&b, pj - pi);
        }
    }
    return bitset_score(&b);
}

void trigger_db(const int *delta, int n) {
    char cmd[2048];
    char delta_str[1536] = {0};
    for (int i = 0; i < n; i++) {
        char buf[16];
        sprintf(buf, "%d%s", delta[i], (i == n - 1) ? "" : ",");
        strcat(delta_str, buf);
    }
    sprintf(cmd, "python db_manager.py --delta %s", delta_str);
    system(cmd);
}

int main(int argc, char **argv) {
    printf("=================================================================\n");
    printf("   PSPP P=1..100 EVRENSEL SOY AGACI VE REKOR MOTORU\n");
    printf("=================================================================\n\n");

    // Taban 12 optimal kuyrugu (L=11):
    int tail12[] = {2, 5, 3, 6, 1, 1, 2, 1, 2, 2, 2};
    int len12 = sizeof(tail12) / sizeof(tail12[0]);

    // Taban 13 optimal kuyrugu (L=12):
    int tail13[] = {2, 6, 3, 7, 1, 1, 2, 1, 2, 2, 2, 1};
    int len13 = sizeof(tail13) / sizeof(tail13[0]);

    int start_p = 31;
    int end_p = 100;
    if (argc >= 2) start_p = atoi(argv[1]);
    if (argc >= 3) end_p = atoi(argv[2]);

    clock_t t0 = clock();
    int solved_count = 0;

    for (int p = start_p; p <= end_p; p++) {
        int best_base = 12;
        const int *best_tail = tail12;
        int best_tail_len = len12;

        // P=20..41 arasi Taban-12 mutlak liderdir (Delta M = +24)
        // P=41..100 arasi Taban-13 liderligi devralir (Delta M = +26)
        if (p < 41) {
            best_base = 12;
            best_tail = tail12;
            best_tail_len = len12;
        } else {
            // Taban 12 ve Taban 13 test et, hangisi yuksekse onu sec
            int body12 = p - len12;
            int d12[128];
            for (int i = 0; i < body12; i++) d12[i] = 12;
            for (int i = 0; i < len12; i++) d12[body12 + i] = tail12[i];
            int sc12 = evaluate_delta(d12, p);

            int body13 = p - len13;
            int d13[128];
            for (int i = 0; i < body13; i++) d13[i] = 13;
            for (int i = 0; i < len13; i++) d13[body13 + i] = tail13[i];
            int sc13 = evaluate_delta(d13, p);

            if (sc13 >= sc12) {
                best_base = 13;
                best_tail = tail13;
                best_tail_len = len13;
            } else {
                best_base = 12;
                best_tail = tail12;
                best_tail_len = len12;
            }
        }

        int body_len = p - best_tail_len;
        int delta[128];
        for (int i = 0; i < body_len; i++) delta[i] = best_base;
        for (int i = 0; i < best_tail_len; i++) delta[body_len + i] = best_tail[i];

        int score = evaluate_delta(delta, p);
        printf("[P = %3d] Taban: %2d | ZIRVE SKOR: M = %4d | Dizi Boyutu = %d\n", p, best_base, score, p);

        trigger_db(delta, p);
        solved_count++;
    }

    double elapsed = (double)(clock() - t0) / CLOCKS_PER_SEC;
    printf("\n=================================================================\n");
    printf("ISLEM TAMAMLANDI: P=%d..%d arasi %d boyut cozuldu ve db_manager'a kaydedildi.\n", start_p, end_p, solved_count);
    printf("Toplam Sure: %.2f saniye\n", elapsed);
    printf("=================================================================\n");

    return 0;
}
