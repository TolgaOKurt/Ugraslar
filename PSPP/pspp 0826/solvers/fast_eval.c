#include <stdint.h>
#include <string.h>

#define BITSET_WORDS 1024  // 1024 * 64 = 65536 bits

typedef struct {
    uint64_t w[BITSET_WORDS];
} Bitset;

__declspec(dllexport) int evaluate_dizi(const int *dizi, int n) {
    if (n <= 0) return 0;
    Bitset b;
    memset(b.w, 0, sizeof(b.w));
    
    for (int i = 0; i < n; i++) {
        int v = dizi[i];
        if (v >= 0 && v < 65536) {
            b.w[v >> 6] |= (1ULL << (v & 63));
        }
    }
    
    for (int i = 0; i < n; i++) {
        int di = dizi[i];
        for (int j = i; j < n; j++) {
            int dj = dizi[j];
            int sum = dj + di;
            int diff = dj - di;
            if (sum < 65536) {
                b.w[sum >> 6] |= (1ULL << (sum & 63));
            }
            if (diff >= 0 && diff < 65536) {
                b.w[diff >> 6] |= (1ULL << (diff & 63));
            }
        }
    }
    
    int m = 0;
    uint64_t w0 = b.w[0] >> 1; // 0 bitini atla
    if ((w0 & 0x7FFFFFFFFFFFFFFFULL) == 0x7FFFFFFFFFFFFFFFULL) {
        m = 63;
        for (int w = 1; w < BITSET_WORDS; w++) {
            uint64_t val = b.w[w];
            if (val == ~0ULL) {
                m += 64;
            } else {
                return m + __builtin_ctzll(~val);
            }
        }
        return m;
    } else {
        return __builtin_ctzll(~w0);
    }
}
