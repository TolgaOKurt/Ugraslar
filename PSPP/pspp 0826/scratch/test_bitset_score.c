#include <stdio.h>
#include <stdint.h>

#define BITSET_WORDS 256

typedef struct {
    uint64_t w[BITSET_WORDS];
} Bitset;

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

int main() {
    Bitset b;
    for (int i = 0; i < BITSET_WORDS; i++) b.w[i] = ~0ULL;
    // clear bit 0 (value 0 is not needed)
    b.w[0] &= ~1ULL;
    // clear bit 505 (value 505)
    b.w[505 >> 6] &= ~(1ULL << (505 & 63));
    
    int sc = bitset_score(&b);
    printf("Expected 504, Got: %d\n", sc);
    return 0;
}
