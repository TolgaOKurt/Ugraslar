#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>

#define BITSET_WORDS 32

typedef struct {
    uint64_t w[BITSET_WORDS];
} Bitset;

static inline void bitset_clear(Bitset *b) {
    memset(b->w, 0, sizeof(b->w));
}

static inline void bitset_set(Bitset *b, int v) {
    if (v >= 0 && v < 2048) {
        b->w[v >> 6] |= (1ULL << (v & 63));
    }
}

static inline int bitset_score(const Bitset *b) {
    int m = 0;
    for (int w = 0; w < BITSET_WORDS; w++) {
        uint64_t val = b->w[w];
        if (w == 0) {
            val >>= 1;
            if (val == 0) return 0;
            if (~val == 0ULL) {
                m += 63;
                continue;
            }
            int tz = __builtin_ctzll(~val);
            return m + tz;
        } else {
            if (~val == 0ULL) {
                m += 64;
                continue;
            }
            int tz = __builtin_ctzll(~val);
            return m + tz;
        }
    }
    return m;
}

int evaluate_delta(const int *delta, int n) {
    static int p_arr[64];
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
    char cmd[512];
    char delta_str[256] = {0};
    for (int i = 0; i < n; i++) {
        char buf[16];
        sprintf(buf, "%d%s", delta[i], (i == n - 1) ? "" : ",");
        strcat(delta_str, buf);
    }
    sprintf(cmd, "python db_manager.py --delta %s", delta_str);
    system(cmd);
}

// Xorshift RNG
static uint64_t rng_state = 88172645463325252ULL;
static inline uint64_t xorshift64() {
    uint64_t x = rng_state;
    x ^= x << 13;
    x ^= x >> 7;
    x ^= x << 17;
    return rng_state = x;
}

static inline int rand_range(int min_v, int max_v) {
    if (min_v >= max_v) return min_v;
    return min_v + (int)(xorshift64() % (uint64_t)(max_v - min_v + 1));
}

int main(int argc, char **argv) {
    printf("=================================================================\n");
    printf("   PSPP YUKSEK TABAN KOK ATA VE KUYRUK MOTORU (BASE 13..18)\n");
    printf("=================================================================\n\n");

    int targets[] = {22, 23, 24, 25, 26, 27, 28, 29, 30};
    int num_targets = sizeof(targets) / sizeof(targets[0]);

    for (int t = 0; t < num_targets; t++) {
        int p = targets[t];
        printf("\n>>> HEDEF P = %d ICIN YUKSEK TABANLAR (BASE 13..18) TARANIYOR <<<\n", p);

        int best_score = 0;
        int best_delta[64];

        // Taban 13, 14, 15, 16, 17, 18 dene
        for (int base = 13; base <= 18; base++) {
            int tail_len = base - 1;
            int body_len = p - tail_len;
            if (body_len < 3) continue;

            // Template tail
            int tail[32];
            int cand[64];

            // 1. Initial heuristic template based on Base-12
            // [2, base/2 - 1, 3, base/2, 1, 1, 2, 1, 2, 2, 2, ...]
            for (int i = 0; i < tail_len; i++) {
                tail[i] = 1;
            }
            tail[0] = 2;
            tail[1] = base / 2 - 1;
            tail[2] = 3;
            tail[3] = base / 2;
            tail[4] = 1;
            tail[5] = 1;
            tail[6] = 2;
            tail[7] = 1;
            tail[8] = 2;
            tail[9] = 2;
            for (int i = 10; i < tail_len; i++) tail[i] = (i % 2 == 0) ? 2 : 1;

            // Simulated Annealing / Local Search on Tail (2,000,000 iterations per base)
            int current_tail[32];
            memcpy(current_tail, tail, sizeof(tail));

            for (int i = 0; i < body_len; i++) cand[i] = base;
            for (int i = 0; i < tail_len; i++) cand[body_len + i] = current_tail[i];

            int cur_sc = evaluate_delta(cand, p);
            if (cur_sc > best_score) {
                best_score = cur_sc;
                memcpy(best_delta, cand, p * sizeof(int));
            }

            for (int iter = 0; iter < 1500000; iter++) {
                int test_tail[32];
                memcpy(test_tail, current_tail, sizeof(test_tail));

                // Mutate 1 to 2 positions in tail
                int mut_pos = rand_range(0, tail_len - 1);
                int chg = rand_range(-2, 2);
                if (chg == 0) chg = (rand_range(0, 1) == 0) ? 1 : -1;

                if (test_tail[mut_pos] + chg >= 1 && test_tail[mut_pos] + chg <= base) {
                    test_tail[mut_pos] += chg;
                }

                if (rand_range(0, 100) < 30) {
                    int p1 = rand_range(0, tail_len - 2);
                    int tmp = test_tail[p1];
                    test_tail[p1] = test_tail[p1 + 1];
                    test_tail[p1 + 1] = tmp;
                }

                for (int i = 0; i < tail_len; i++) cand[body_len + i] = test_tail[i];
                int sc = evaluate_delta(cand, p);

                if (sc >= cur_sc) {
                    cur_sc = sc;
                    memcpy(current_tail, test_tail, sizeof(test_tail));
                    if (sc > best_score) {
                        best_score = sc;
                        memcpy(best_delta, cand, p * sizeof(int));
                        printf("  [YENI ZIRVE] P=%d | Taban=%d | M=%d | Delta: [", p, base, sc);
                        for (int i = 0; i < p; i++) printf("%d%s", cand[i], (i == p-1) ? "" : ", ");
                        printf("]\n");
                    }
                }
            }
        }

        printf("  -> P = %d En Iyi Skor: M = %d\n", p, best_score);
        if (best_score > 0) {
            trigger_db(best_delta, p);
        }
    }

    return 0;
}
