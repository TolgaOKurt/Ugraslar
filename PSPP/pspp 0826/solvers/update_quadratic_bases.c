#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#define BITSET_WORDS 512

typedef struct {
    uint64_t w[BITSET_WORDS];
} Bitset;

static inline void bitset_clear(Bitset *b) {
    memset(b->w, 0, sizeof(b->w));
}

static inline void bitset_set(Bitset *b, int v) {
    if (v >= 0 && v < 32768) {
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

int score_pspp_dizi(const int *dizi, int n) {
    Bitset b;
    bitset_clear(&b);
    for (int i = 0; i < n; i++) {
        bitset_set(&b, dizi[i]);
    }
    for (int i = 0; i < n; i++) {
        int di = dizi[i];
        for (int j = i; j < n; j++) {
            int dj = dizi[j];
            bitset_set(&b, dj + di);
            bitset_set(&b, dj - di);
        }
    }
    return bitset_score(&b);
}

void trigger_db(const int *dizi, int n) {
    char cmd[4096];
    char dizi_str[3072] = {0};
    for (int i = 0; i < n; i++) {
        char buf[16];
        sprintf(buf, "%d%s", dizi[i], (i == n - 1) ? "" : ",");
        strcat(dizi_str, buf);
    }
    sprintf(cmd, "python db_manager.py --dizi %s", dizi_str);
    system(cmd);
}

int main() {
    printf("=================================================================\n");
    printf("   PSPP P=30..100 QUADRATIC DIFFERENCE BASIS GUNCELLEMESI\n");
    printf("=================================================================\n\n");

    for (int p = 30; p <= 100; p++) {
        // En iyi r'yi bul (r eleman 1..r, geri kalan s eleman (2r+1) katları)
        int best_m = 0;
        int best_dizi[128];

        for (int r = 1; r < p; r++) {
            int s = p - r;
            int step = 2 * r + 1;
            int cand[128];
            for (int i = 0; i < r; i++) cand[i] = i + 1;
            for (int j = 0; j < s; j++) cand[r + j] = (j + 1) * step;

            int sc = score_pspp_dizi(cand, p);
            if (sc > best_m) {
                best_m = sc;
                memcpy(best_dizi, cand, p * sizeof(int));
            }
        }

        printf("[P = %3d] Kuadratik Fark Dizisi Zirvesi: M = %4d\n", p, best_m);
        trigger_db(best_dizi, p);
    }

    return 0;
}
