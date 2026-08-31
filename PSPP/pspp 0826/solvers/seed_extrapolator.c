/*
 * ==============================================================================
 * DOSYA ADI : solvers/seed_extrapolator.c
 * PROJE     : PSPP (Postage Stamp with Subtraction)
 * TANIM     : Tohum Tabanli Tahmin ve Mutasyon Motoru (Seed Extrapolator)
 * ==============================================================================
 * 
 * BU MOTORUN AMACI VE ÇALIŞMA PRENSİBİ:
 * -------------------------------------
 * Verilen P boyutundaki tohum dizilerini (orn: P=6) girdi olarak alip;
 *   1. Moduler Govde Genislemesi (Aile 1: Periyot tekrarlari ve kuyruk kaliplari)
 *   2. Bipartite Kopru Olcekleme (Aile 2: delta_bridge = 4P - 14)
 *   3. Asimetrik Uc Sicramasi (Aile 3: delta_son = M/2, prefix sum = M/2)
 *   4. Komsuluk Mutasyonu & Tepe Tirmanisi (Hill Climbing & Suffix Tuning)
 * operatorlerini kullanarak bir sonraki boyutun (P+1, orn: P=7) optimum cozulerini
 * deterministik olarak tahmin eder ve ekranda detayli olarak raporlar.
 * 
 * Bulunan cozumleri:
 *   - Bilinen optimumlarla eslesenler ([✔ BASARILI TAHMIN])
 *   - Yeni alternatif cozumler ([⭐ YENI ALTERNATIF])
 *   - Yeni zirve rekorlar ([🏆 YENI REKOR])
 * olarak siniflandirir ve yeni cozumleri "db_manager.py" araciligiyla test edip kaydeder.
 * 
 * ==============================================================================
 */

#if __has_include("pspp_kb.h")
#include "pspp_kb.h"
#else
#include "../pspp_kb.h"
#endif

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>
#include <time.h>

#define BITMASK_WORDS 32 // 2048-bit donanimsal kapasite (M=2048'e kadar)

// Donanimsal 2048-Bit CTZ Skor Hesaplayici
static inline int score_delta_c(const int *delta_arr, int n) {
    int p_arr[MAX_P];
    int cur = 0;
    for (int i = 0; i < n; i++) {
        cur += delta_arr[i];
        p_arr[i] = cur;
    }

    uint64_t seen[BITMASK_WORDS] = {0};

    for (int i = 0; i < n; i++) {
        int pi = p_arr[i];
        if (pi > 0 && pi < 2048) {
            seen[pi >> 6] |= (1ULL << (pi & 63));
        }
        for (int j = i; j < n; j++) {
            int pj = p_arr[j];
            int sum = pj + pi;
            int diff = pj - pi;

            if (sum < 2048) {
                seen[sum >> 6] |= (1ULL << (sum & 63));
            }
            if (diff > 0 && diff < 2048) {
                seen[diff >> 6] |= (1ULL << (diff & 63));
            }
        }
    }

    int m = 0;
    for (int w = 0; w < BITMASK_WORDS; w++) {
        uint64_t val = ~seen[w];
        if (w == 0) val &= ~1ULL;
        if (val == 0) {
            m += 64;
        } else {
            m += __builtin_ctzll(val);
            break;
        }
    }
    return m - 1;
}

// Global Istatistikler
PSPP_KnowledgeBase g_kb;
long long g_total_candidates_tested = 0;
int g_records_found = 0;
int g_alternatives_found = 0;
int g_matched_known_count = 0;

// Oturum Boyunca Tespit Edilen / Tahmin Edilen Cozumler Havuzu
typedef struct {
    int p;
    int score;
    int type; // 1: YENI REKOR, 2: YENI ALTERNATIF, 3: BASARILI TAHMIN (BILINEN OPTIMUM)
    char source[128];
    int delta[MAX_P];
    int dizi[MAX_P];
} PredictedResult;

#define MAX_PREDICTED 512
PredictedResult g_predicted_results[MAX_PREDICTED];
int g_predicted_count = 0;

// Database Manager (db_manager.py) ile Dogrulama ve Kayit Fonksiyonu
static void trigger_db_manager(const int *delta, int n) {
    char script_path[256] = "db_manager.py";
    FILE *test_f = fopen("db_manager.py", "r");
    if (test_f) {
        fclose(test_f);
    } else {
        FILE *test_parent = fopen("../db_manager.py", "r");
        if (test_parent) {
            fclose(test_parent);
            strcpy(script_path, "../db_manager.py");
        } else {
            strcpy(script_path, "scripts/db_manager.py");
        }
    }

    char cmd[2048];
    int offset = sprintf(cmd, "python \"%s\" --delta ", script_path);
    for (int i = 0; i < n; i++) {
        offset += sprintf(cmd + offset, "%d%s", delta[i], (i == n - 1) ? "" : ",");
    }
    system(cmd);
}

// Yaygin Kuyruk Sablonlari Havuzu (Aile 1 Suffix Library)
static const int TAIL_TEMPLATES[][8] = {
    {2, 1, 2, 0, 0, 0, 0, 0},             // len 3: 4'lu bloklar icin
    {3, 1, 1, 2, 1, 0, 0, 0},             // len 5: 6'li bloklar icin (P=7,8,9,10)
    {2, 1, 1, 2, 0, 0, 0, 0},             // len 4: 5'li bloklar icin
    {4, 2, 3, 1, 1, 2, 2, 0},             // len 7: 8'li bloklar icin (P=11,12)
    {6, 2, 3, 4, 2, 1, 2, 1},             // len 8: 8'li alternatif (P=11,12)
    {1, 4, 3, 3, 1, 1, 2, 1},             // len 8: 9'lu bloklar icin (P=13,14)
    {2, 3, 1, 5, 2, 1, 3, 1},             // len 8: 10'lu bloklar icin (P=15)
    {1, 2, 1, 0, 0, 0, 0, 0},             // len 3
    {1, 1, 2, 1, 0, 0, 0, 0},             // len 4
    {2, 2, 1, 1, 0, 0, 0, 0},             // len 4
    {3, 2, 1, 1, 2, 0, 0, 0}              // len 5
};
static const int TAIL_LENGTHS[] = {3, 5, 4, 7, 8, 8, 8, 3, 4, 4, 5};
static const int NUM_TAIL_TEMPLATES = sizeof(TAIL_LENGTHS) / sizeof(TAIL_LENGTHS[0]);

// Dahili Yardimci: Bu oturumda zaten bu delta dizisini raporladik mi?
static bool is_already_reported_in_session(const int *delta_arr, int p) {
    for (int i = 0; i < g_predicted_count; i++) {
        if (g_predicted_results[i].p == p && memcmp(g_predicted_results[i].delta, delta_arr, p * sizeof(int)) == 0) {
            return true;
        }
    }
    return false;
}

// Bir adayi test etme, konsola net ve anlasilir basma, gerekirse db_manager ile kaydetme
static void evaluate_and_record(const int *delta_arr, int p, const char *source_desc, bool is_explicit_prediction) {
    g_total_candidates_tested++;
    int score = score_delta_c(delta_arr, p);
    if (score <= 0) return;

    int current_best = kb_get_target_max(&g_kb, p);
    
    // 1. Durum: Yeni Zirve Rekor
    if (score > current_best) {
        g_records_found++;
        
        int p_arr[MAX_P];
        int cur = 0;
        for (int i = 0; i < p; i++) {
            cur += delta_arr[i];
            p_arr[i] = cur;
        }

        if (g_predicted_count < MAX_PREDICTED && !is_already_reported_in_session(delta_arr, p)) {
            PredictedResult *d = &g_predicted_results[g_predicted_count++];
            d->p = p;
            d->score = score;
            d->type = 1;
            strncpy(d->source, source_desc, sizeof(d->source) - 1);
            memcpy(d->delta, delta_arr, p * sizeof(int));
            memcpy(d->dizi, p_arr, p * sizeof(int));
        }

        printf("\n================================================================\n");
        printf("[🏆 YENI REKOR KESFEDILDI!] P = %d | Eski: %d -> YENI SKOR M = %d\n", p, current_best, score);
        printf("Kaynak / Operator : %s\n", source_desc);
        printf("Delta Dizisi      : [");
        for (int i = 0; i < p; i++) printf("%d%s", delta_arr[i], i == p - 1 ? "" : ", ");
        printf("]\n");
        printf("Kumulatif Dizi    : [");
        for (int i = 0; i < p; i++) printf("%d%s", p_arr[i], i == p - 1 ? "" : ", ");
        printf("]\n");
        printf(">> Database Manager (db_manager.py) ile dogrulaniyor ve kaydediliyor...\n");
        printf("================================================================\n");

        kb_update(&g_kb, p, score, delta_arr);
        trigger_db_manager(delta_arr, p);

    } else if (score == current_best && current_best > 0) {
        int idx = kb_find_solution_index(&g_kb.table[p], delta_arr, p);
        
        int p_arr[MAX_P];
        int cur = 0;
        for (int i = 0; i < p; i++) {
            cur += delta_arr[i];
            p_arr[i] = cur;
        }

        // 2. Durum: Veritabaninda henuz olmayan yeni alternatif cozum
        if (idx == -1) {
            g_alternatives_found++;

            if (g_predicted_count < MAX_PREDICTED && !is_already_reported_in_session(delta_arr, p)) {
                PredictedResult *d = &g_predicted_results[g_predicted_count++];
                d->p = p;
                d->score = score;
                d->type = 2;
                strncpy(d->source, source_desc, sizeof(d->source) - 1);
                memcpy(d->delta, delta_arr, p * sizeof(int));
                memcpy(d->dizi, p_arr, p * sizeof(int));
            }

            printf("\n----------------------------------------------------------------\n");
            printf("[⭐ YENI ALTERNATIF OPTIMUM!] P = %d | Zirve Skor: M = %d\n", p, score);
            printf("Kaynak / Operator : %s\n", source_desc);
            printf("Delta Dizisi      : [");
            for (int i = 0; i < p; i++) printf("%d%s", delta_arr[i], i == p - 1 ? "" : ", ");
            printf("]\n");
            printf("Kumulatif Dizi    : [");
            for (int i = 0; i < p; i++) printf("%d%s", p_arr[i], i == p - 1 ? "" : ", ");
            printf("]\n");
            printf(">> Database Manager (db_manager.py) ile veritabanina ekleniyor...\n");
            printf("----------------------------------------------------------------\n");

            kb_update(&g_kb, p, score, delta_arr);
            trigger_db_manager(delta_arr, p);

        } else {
            // 3. Durum: Bilinen Optimum Cozumun Basariyla Tahmin Edilmesi
            if (!is_already_reported_in_session(delta_arr, p)) {
                g_matched_known_count++;

                if (g_predicted_count < MAX_PREDICTED) {
                    PredictedResult *d = &g_predicted_results[g_predicted_count++];
                    d->p = p;
                    d->score = score;
                    d->type = 3;
                    strncpy(d->source, source_desc, sizeof(d->source) - 1);
                    memcpy(d->delta, delta_arr, p * sizeof(int));
                    memcpy(d->dizi, p_arr, p * sizeof(int));
                }

                printf("[✔ BASARILI TAHMIN] P = %d | M = %d (Optimum Hedefe Ulasildi)\n", p, score);
                printf("   Kaynak / Operator : %s\n", source_desc);
                printf("   Tahmin Delta      : [");
                for (int i = 0; i < p; i++) printf("%d%s", delta_arr[i], i == p - 1 ? "" : ", ");
                printf("]\n");
                printf("   Kumulatif Dizi    : [");
                for (int i = 0; i < p; i++) printf("%d%s", p_arr[i], i == p - 1 ? "" : ", ");
                printf("]\n\n");
            }
        }
    }
}

// -----------------------------------------------------------------------------
// OPERATOR 1: Moduler Govde Genislemesi (Aile 1)
// -----------------------------------------------------------------------------
void extrapolate_modular(const int *seed_delta, int seed_p, int target_p) {
    if (target_p <= seed_p) return;
    int extra_steps = target_p - seed_p;
    int base_step = seed_delta[0];

    int cand[MAX_P];
    char desc[128];

    // Yontem 1.1: Basa base_step ekleme
    for (int i = 0; i < extra_steps; i++) cand[i] = base_step;
    for (int i = 0; i < seed_p; i++) cand[extra_steps + i] = seed_delta[i];
    sprintf(desc, "Op 1.1 (Govde Genislemesi: Tohum P=%d basina %dx %d ekleme)", seed_p, extra_steps, base_step);
    evaluate_and_record(cand, target_p, desc, true);

    // Yontem 1.2: Govde tekrari + Hazir Kuyruk Sablonlari Kombinasyonu
    for (int step = base_step; step <= base_step + 4; step += 2) {
        for (int t = 0; t < NUM_TAIL_TEMPLATES; t++) {
            int t_len = TAIL_LENGTHS[t];
            if (t_len >= target_p) continue;
            int body_len = target_p - t_len;

            for (int i = 0; i < body_len; i++) cand[i] = step;
            for (int i = 0; i < t_len; i++) cand[body_len + i] = TAIL_TEMPLATES[t][i];

            sprintf(desc, "Op 1.2 (Moduler %dx%d Govde + Kuyruk Sablonu)", step, body_len);
            evaluate_and_record(cand, target_p, desc, true);

            // Kuyruk son adimi 1 veya 2 varyasyonu
            int old_last = cand[target_p - 1];
            cand[target_p - 1] = 1;
            sprintf(desc, "Op 1.2 (Moduler %dx%d + Kuyruk d_son=1)", step, body_len);
            evaluate_and_record(cand, target_p, desc, true);
            
            cand[target_p - 1] = 2;
            sprintf(desc, "Op 1.2 (Moduler %dx%d + Kuyruk d_son=2)", step, body_len);
            evaluate_and_record(cand, target_p, desc, true);
            
            cand[target_p - 1] = old_last;
        }
    }
}

// -----------------------------------------------------------------------------
// OPERATOR 2: Bipartite Cift Kumeli Kopru Olcekleme (Aile 2)
// -----------------------------------------------------------------------------
void extrapolate_bipartite(int target_p) {
    if (target_p < 4) return;
    int bridge = 4 * target_p - 14;
    int cand[MAX_P];

    int lower_len = (target_p - 1) / 2;
    int upper_len = target_p - 1 - lower_len;

    // Alt kumeyi olustur: [2, 2, ..., 1]
    for (int i = 0; i < lower_len - 1; i++) cand[i] = 2;
    cand[lower_len - 1] = 1;

    // Kopru
    cand[lower_len] = bridge;

    // Ust kume araligi (11'den baslayarak her P artisinda 4 buyur)
    int upper_gap = 11 + 4 * (target_p - 7);
    if (upper_gap < 1) upper_gap = 5;

    int u_idx = lower_len + 1;
    cand[u_idx] = 1;
    for (int i = 1; i < upper_len - 1; i++) cand[u_idx + i] = 1;
    cand[u_idx + upper_len - 2] = upper_gap;
    cand[target_p - 1] = 1;

    char desc[128];
    sprintf(desc, "Op 2 (Bipartite Kopru Formulu: bridge=%d, upper_gap=%d)", bridge, upper_gap);
    evaluate_and_record(cand, target_p, desc, true);
}

// -----------------------------------------------------------------------------
// OPERATOR 3: Asimetrik Uc Sicramasi (Aile 3)
// -----------------------------------------------------------------------------
void extrapolate_leapfrog(const int *seed_delta, int seed_p, int target_p) {
    if (target_p <= seed_p) return;
    int cand[MAX_P];

    for (int i = 0; i < seed_p; i++) cand[i] = seed_delta[i];
    
    int cur_sum = 0;
    for (int i = 0; i < seed_p; i++) cur_sum += seed_delta[i];

    for (int i = seed_p; i < target_p - 1; i++) {
        cand[i] = (i % 2 == 0) ? 2 : 1;
        cur_sum += cand[i];
    }
    // Son adim: Uc sicramasi (d_son = prefix_sum)
    cand[target_p - 1] = cur_sum;

    char desc[128];
    sprintf(desc, "Op 3 (Uc Sicramasi: Tohum P=%d, d_son=%d)", seed_p, cur_sum);
    evaluate_and_record(cand, target_p, desc, true);
}

// -----------------------------------------------------------------------------
// OPERATOR 4: Komsuluk Mutasyonu & Tepe Tirmanisi (Hill Climbing)
// -----------------------------------------------------------------------------
void mutate_and_hill_climb(int *base_delta, int p, int iterations) {
    int best_d[MAX_P];
    memcpy(best_d, base_delta, p * sizeof(int));
    int current_score = score_delta_c(best_d, p);

    evaluate_and_record(best_d, p, "Op 4 (Hill Climbing Baslangic Tohumu)", false);

    int cand[MAX_P];
    for (int iter = 0; iter < iterations; iter++) {
        memcpy(cand, best_d, p * sizeof(int));

        int mut_count = 1 + rand() % 2;
        for (int m = 0; m < mut_count; m++) {
            int pos = rand() % p;
            int delta_change = (rand() % 2 == 0 ? 1 : -1) * (1 + rand() % 2);
            if (cand[pos] + delta_change >= 1 && cand[pos] + delta_change <= 35) {
                cand[pos] += delta_change;
            }
        }

        if (rand() % 3 == 0 && p >= 2) {
            int pos = rand() % (p - 1);
            int tmp = cand[pos];
            cand[pos] = cand[pos + 1];
            cand[pos + 1] = tmp;
        }

        int sc = score_delta_c(cand, p);
        evaluate_and_record(cand, p, "Op 4: Komsu Mutasyon & Hill Climbing", false);

        if (sc > current_score) {
            current_score = sc;
            memcpy(best_d, cand, p * sizeof(int));
        }
    }
}

// -----------------------------------------------------------------------------
// P -> P+1 GECIS TAHMIN MOTORU (EXPERIMENT RUNNER)
// -----------------------------------------------------------------------------
void run_transition_prediction(int source_p, int target_p) {
    printf("\n================================================================\n");
    printf(">> P = %d TOHUMLARI KULLANILARAK P = %d DIZI TAHMINI YAPILIYOR\n", source_p, target_p);
    printf("================================================================\n\n");

    int target_best = kb_get_target_max(&g_kb, target_p);
    printf("[BILGI] Hedef P = %d Icin Beklenen/Hedef Zirve Skor: M = %d\n\n", target_p, target_best);

    // 1. Kaynak P boyutu tohumlarini veritabanindan al
    SolutionRecord *src_rec = &g_kb.table[source_p];
    if (src_rec->stored_solutions_count > 0) {
        printf("[KAYNAK] P = %d Boyutunda Veritabaninda %d Adet Optimum Tohum Mevcut:\n", 
               source_p, src_rec->stored_solutions_count);
        for (int s = 0; s < src_rec->stored_solutions_count; s++) {
            printf("  Tohum #%d: [", s + 1);
            for (int i = 0; i < source_p; i++) {
                printf("%d%s", src_rec->solutions[s].delta[i], i == source_p - 1 ? "" : ", ");
            }
            printf("] (M=%d)\n", src_rec->score);
        }
        printf("\n>> Tahmin Operatorleri Calistiriliyor...\n\n");

        for (int s = 0; s < src_rec->stored_solutions_count; s++) {
            int *seed = src_rec->solutions[s].delta;
            extrapolate_modular(seed, source_p, target_p);
            extrapolate_leapfrog(seed, source_p, target_p);

            // Suffix pertürbasyonu: Tohum sonuna {1, 2, 4} ekleme
            int cand[MAX_P];
            for (int i = 0; i < source_p; i++) cand[i] = seed[i];
            for (int add_val = 1; add_val <= 4; add_val++) {
                cand[source_p] = add_val;
                char desc[128];
                sprintf(desc, "Op 4.1 (Suffix Perturbasyonu: Tohum #%d + %d)", s + 1, add_val);
                evaluate_and_record(cand, target_p, desc, true);
            }
        }
    } else {
        printf("[UYARI] P = %d icin veritabaninda tohum bulunamadi! Genel modeller kullaniliyor.\n", source_p);
    }

    // Ek tohumlar: Safe Area'daki diğer küçük boyut tohumlarını da gövde genişletmesi olarak dene
    for (int sp = 1; sp < source_p; sp++) {
        SolutionRecord *rec = &g_kb.table[sp];
        for (int s = 0; s < rec->stored_solutions_count; s++) {
            extrapolate_modular(rec->solutions[s].delta, sp, target_p);
            extrapolate_leapfrog(rec->solutions[s].delta, sp, target_p);
        }
    }

    // 2. Bipartite Köprü Formülü
    extrapolate_bipartite(target_p);

    // 3. Bulunan en iyi adaylar üzerinde Hill Climbing Mutasyonu
    for (int i = 0; i < g_predicted_count; i++) {
        if (g_predicted_results[i].p == target_p && g_predicted_results[i].score >= target_best - 2) {
            int base[MAX_P];
            memcpy(base, g_predicted_results[i].delta, target_p * sizeof(int));
            mutate_and_hill_climb(base, target_p, 400);
        }
    }
}

// -----------------------------------------------------------------------------
// INTERAKTIF KULLANICI MENUSU
// -----------------------------------------------------------------------------
void interactive_menu() {
    printf("\n================================================================\n");
    printf("   PSPP TOHUM TABANLI TAHMIN VE MUTASYON MOTORU (SEED EXTRAPOLATOR)\n");
    printf("================================================================\n");
    printf(" [1] P -> P+1 Dizi Tahmini Yap (Orn: P=6 gir -> P=7 cozumlerini uret)\n");
    printf(" [2] Ozel bir Tohum Delta dizisi girerek tahmin yap (Orn: 6,6,6,3,1,1,2,1)\n");
    printf(" [3] Tek bir hedef P boyutu icin tahmin yap (Orn: 16)\n");
    printf(" [4] Safe Area ve derin boyutlarin (P=7..16) tamamini otomatik tara\n");
    printf("----------------------------------------------------------------\n");
    printf(" Seciminiz (1-4, Varsayilan: 1): ");
    
    char line[512] = {0};
    if (!fgets(line, sizeof(line), stdin)) return;
    
    int choice = 1;
    if (line[0] >= '1' && line[0] <= '4') {
        choice = line[0] - '0';
    }

    if (choice == 1) {
        printf("\n Kaynak P boyutunu girin (Ornek: 6): ");
        int src_p = 6;
        if (fgets(line, sizeof(line), stdin)) {
            int val = atoi(line);
            if (val > 0) src_p = val;
        }
        int target_p = src_p + 1;
        printf(">> Hedef P = %d boyutu tahmin edilecek.\n", target_p);
        run_transition_prediction(src_p, target_p);

    } else if (choice == 2) {
        char seed_buf[512] = {0};
        printf("\n Tohum Delta dizisini girin (Virgul veya bosluk ile, Orn: 6,6,6,3,1,1,2,1): ");
        if (fgets(seed_buf, sizeof(seed_buf), stdin)) {
            printf(" Hedef P boyutunu girin (Ornek: 9 veya 11): ");
            int target_p = 9;
            if (fgets(line, sizeof(line), stdin)) target_p = atoi(line);

            int custom_seed[MAX_P];
            int seed_len = 0;
            char *tok = strtok(seed_buf, " ,\t\r\n");
            while (tok && seed_len < MAX_P) {
                if (isdigit(tok[0])) {
                    custom_seed[seed_len++] = atoi(tok);
                }
                tok = strtok(NULL, " ,\t\r\n");
            }

            if (seed_len > 0 && target_p > seed_len) {
                printf("\n[OZEL TOHUM]: [");
                for (int i = 0; i < seed_len; i++) printf("%d%s", custom_seed[i], i == seed_len - 1 ? "" : ", ");
                printf("] (P=%d) -> Hedef P = %d\n\n", seed_len, target_p);

                extrapolate_modular(custom_seed, seed_len, target_p);
                extrapolate_leapfrog(custom_seed, seed_len, target_p);

                int base_for_mut[MAX_P];
                int extra = target_p - seed_len;
                for (int i = 0; i < extra; i++) base_for_mut[i] = custom_seed[0];
                for (int i = 0; i < seed_len; i++) base_for_mut[extra + i] = custom_seed[i];
                mutate_and_hill_climb(base_for_mut, target_p, 1000);
            } else {
                printf("[UYARI] Hedef P, tohum boyutundan buyuk olmalidir!\n");
            }
        }
    } else if (choice == 3) {
        printf("\n Hedef P boyutunu girin (Ornek: 16): ");
        if (fgets(line, sizeof(line), stdin)) {
            int target_p = atoi(line);
            if (target_p > 1) {
                run_transition_prediction(target_p - 1, target_p);
            }
        }
    } else {
        printf("\n>> Safe Area ve derin boyutlar (P=7..16) taraniyor...\n");
        for (int p = 7; p <= 16; p++) {
            run_transition_prediction(p - 1, p);
        }
    }
}

// -----------------------------------------------------------------------------
// ANA FONKSIYON
// -----------------------------------------------------------------------------
int main(int argc, char *argv[]) {
    srand((unsigned int)time(NULL));
    kb_init(&g_kb);

    int target_p = 0;
    int from_p = 0;
    int to_p = 0;
    char *custom_seed_str = NULL;

    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--target") == 0 && i + 1 < argc) {
            target_p = atoi(argv[++i]);
        } else if (strcmp(argv[i], "--from") == 0 && i + 1 < argc) {
            from_p = atoi(argv[++i]);
        } else if (strcmp(argv[i], "--to") == 0 && i + 1 < argc) {
            to_p = atoi(argv[++i]);
        } else if (strcmp(argv[i], "--seed") == 0 && i + 1 < argc) {
            custom_seed_str = argv[++i];
        } else if (strcmp(argv[i], "--help") == 0 || strcmp(argv[i], "-h") == 0) {
            printf("Kullanim:\n");
            printf("  .\\seed_extrapolator.exe                      (Interaktif Mod)\n");
            printf("  .\\seed_extrapolator.exe --from 6 --target 7  (P=6'dan P=7 Tahmini)\n");
            printf("  .\\seed_extrapolator.exe --target 16          (Hedef P boyutu)\n");
            printf("  .\\seed_extrapolator.exe --seed 6,6,6,3,1,1,2,1 --target 9 (Ozel tohum)\n");
            return 0;
        }
    }

    clock_t t0 = clock();

    if (argc == 1) {
        interactive_menu();
    } else {
        printf("================================================================\n");
        printf("   PSPP TOHUM TABANLI TAHMIN VE MUTASYON MOTORU (SEED EXTRAPOLATOR)\n");
        printf("================================================================\n");

        if (custom_seed_str != NULL && target_p > 0) {
            int custom_seed[MAX_P];
            int seed_len = 0;
            char *token = strtok(custom_seed_str, ", ");
            while (token && seed_len < MAX_P) {
                custom_seed[seed_len++] = atoi(token);
                token = strtok(NULL, ", ");
            }
            printf("[OZEL TOHUM]: [");
            for (int i = 0; i < seed_len; i++) printf("%d%s", custom_seed[i], i == seed_len - 1 ? "" : ", ");
            printf("] (P=%d) -> Hedef P = %d\n\n", seed_len, target_p);

            extrapolate_modular(custom_seed, seed_len, target_p);
            extrapolate_leapfrog(custom_seed, seed_len, target_p);

            int base_for_mut[MAX_P];
            int extra = target_p - seed_len;
            if (extra >= 0) {
                for (int i = 0; i < extra; i++) base_for_mut[i] = custom_seed[0];
                for (int i = 0; i < seed_len; i++) base_for_mut[extra + i] = custom_seed[i];
                mutate_and_hill_climb(base_for_mut, target_p, 1000);
            }
        } else if (from_p > 0 && target_p > 0) {
            run_transition_prediction(from_p, target_p);
        } else if (from_p > 0 && to_p >= from_p) {
            for (int p = from_p; p <= to_p; p++) {
                run_transition_prediction(p - 1, p);
            }
        } else if (target_p > 0) {
            run_transition_prediction(target_p - 1, target_p);
        }
    }

    double elapsed_sec = (double)(clock() - t0) / CLOCKS_PER_SEC;

    printf("\n================================================================\n");
    printf("TAHMIN MOTORU SONUC VE DOGRULAMA RAPORU:\n");
    printf("----------------------------------------------------------------\n");
    printf("Test Edilen Toplam Aday       : %lld adet\n", g_total_candidates_tested);
    printf("Tahmin Edilen Bilinen Optimum : %d adet\n", g_matched_known_count);
    printf("Kesfedilen Yeni Rekor         : %d adet\n", g_records_found);
    printf("Kesfedilen Yeni Alternatif    : %d adet\n", g_alternatives_found);
    printf("Toplam Calisma Suresi         : %.4f saniye\n", elapsed_sec);
    printf("----------------------------------------------------------------\n");

    if (g_predicted_count > 0) {
        printf("\nTAHMIN EDILEN VE KESFEDILEN TUM COZUMLER (%d ADET):\n", g_predicted_count);
        for (int i = 0; i < g_predicted_count; i++) {
            PredictedResult *d = &g_predicted_results[i];
            const char *type_str = (d->type == 1) ? "[🏆 YENI REKOR]" : 
                                   (d->type == 2) ? "[⭐ YENI ALTERNATIF]" : "[✔ BASARILI TAHMIN]";
            printf(" #%d %s P=%d | M=%d | %s\n", i + 1, type_str, d->p, d->score, d->source);
            printf("     Delta: [");
            for (int j = 0; j < d->p; j++) printf("%d%s", d->delta[j], j == d->p - 1 ? "" : ", ");
            printf("]\n");
            printf("     Dizi : [");
            for (int j = 0; j < d->p; j++) printf("%d%s", d->dizi[j], j == d->p - 1 ? "" : ", ");
            printf("]\n\n");
        }
    }
    printf("================================================================\n");

    return 0;
}
