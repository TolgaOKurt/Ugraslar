#include <stdio.h>
#include "pspp_kb.h"

int main() {
    PSPP_KnowledgeBase kb;
    kb_init(&kb);
    printf("C Parser Test:\n");
    printf("P=20 Skoru: %d (Cozum Sayisi: %d)\n", kb.table[20].score, kb.table[20].stored_solutions_count);
    printf("P=33 Skoru: %d (Cozum Sayisi: %d)\n", kb.table[33].score, kb.table[33].stored_solutions_count);
    if (kb.table[20].score == 264 && kb.table[33].score == 531) {
        printf("SONUC: C JSON PARSER %%100 BASARILI!\n");
        return 0;
    }
    printf("SONUC: Beklenmeyen skor!\n");
    return 1;
}
