#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>

#define BITSET_WORDS 512  // 512 * 64 = 32,768 bits (supports scores up to 32,768!)

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

int score_addition_only(const int *dizi, int n) {
    Bitset b;
    bitset_clear(&b);
    for (int i = 0; i < n; i++) {
        bitset_set(&b, dizi[i]);
    }
    for (int i = 0; i < n; i++) {
        int di = dizi[i];
        for (int j = i; j < n; j++) {
            bitset_set(&b, dizi[j] + di);
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
    printf("=====================================================================================\n");
    printf("   PSPP vs KLASIK 2-POSTA PULU KARŞILAŞTIRMASI VE GELISMIS TEMEL DIZILER (P=1..100)\n");
    printf("=====================================================================================\n\n");

    for (int p = 10; p <= 100; p += 5) {
        int classical_bound = (p * p + 6 * p + 1) / 4;
        
        // 1. Klasik 2-Posta Pulu Dizisi (Hofmeister / Rohrbach):
        // r = (p+1)/2, s = p - r
        int r = (p + 1) / 2;
        int s = p - r;
        int step = r + 1;
        int c_dizi[128];
        for (int i = 0; i < r; i++) c_dizi[i] = i + 1;
        for (int j = 0; j < s; j++) c_dizi[r + j] = (j + 1) * step;

        int score_pspp_classical = score_pspp_dizi(c_dizi, p);

        // 2. Simetrik Fark Genişletmeli 2-Posta Pulu Dizisi:
        // Fark operasyonu sayesinde step = 2r + 1 yapabiliriz!
        int step_diff = 2 * r + 1;
        int diff_dizi[128];
        for (int i = 0; i < r; i++) diff_dizi[i] = i + 1;
        for (int j = 0; j < s; j++) diff_dizi[r + j] = (j + 1) * step_diff;

        int score_pspp_diff = score_pspp_dizi(diff_dizi, p);

        printf("P = %3d | Klasik Formul: %4d | Klasik Dizi (PSPP ile): %4d | Fark-Tabanli Dizi (PSPP): %4d (Kazanc: %4d)\n",
               p, classical_bound, score_pspp_classical, score_pspp_diff, score_pspp_diff - classical_bound);

        // Eğer veritabanındaki skordan büyükse DB'ye gönder
        if (p >= 80) {
            trigger_db(diff_dizi, p);
        }
    }

    return 0;
}
