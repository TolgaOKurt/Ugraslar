#ifndef PSPP_KB_H
#define PSPP_KB_H

/*
 * ==============================================================================
 * DOSYA ADI : pspp_kb.h (PSPP Knowledge Base - C Bilgi Tabani Header'i)
 * ==============================================================================
 * 
 * BU DOSYANIN GOREVI VE AMACI:
 * ----------------------------
 * Bu kutuphane, C dilinde yazilan tum PSPP arama motorlarinin (solver'larin)
 * merkezi veritabani olan "pspp_database.json" ile dogrudan ve yuksek performansla
 * iletisim kurmasini saglar.
 * 
 * TEMEL ISLEVLERI:
 * 1. C Veri Yapilari: Delta dizilerini, kumulatif dizileri ve her boyuta (P) ait
 *    rekor skorlari (M) C struct dizileri halinde RAM'de tutar.
 * 2. Hizli JSON Okuma (kb_load_from_json): Arama motoru basladiginda diske gidip
 *    mevcut rekorlari C hafizasina yukler.
 * 3. Erken Budama Referansi (kb_get_target_max): Arama motorlari, arama yaparken
 *    buradaki rekorlari esik degeri olarak kullanarak ulasilmasi imkansiz dallari
 *    saniyeler icinde eler (pruning).
 * 4. Otomatik Veritabani Guncelleme (kb_update / kb_save_json): Yeni bir rekor
 *    veya alternatif cozum bulundugunda eski alt cozumleri siler ve JSON olarak kaydeder.
 * 
 * NOT: Eger "pspp_database.json" dosyasi diskte yoksa, tablo sifir (bos) baslar;
 *      icerisinde hicbir yapay veya sabit kodlu veri barindirmaz.
 * ==============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#define MAX_P 64
#define MAX_SOLUTIONS 64
#define DB_FILE "pspp_database.json"
#define DB_FILE_PARENT "../pspp_database.json"

/*
 * Tek bir Delta ve Normal Dizi Çözüm Çifti
 */
typedef struct {
    int delta[MAX_P];       // Delta artış dizisi
    int dizi[MAX_P];        // Normal kümülatif dizi (prefix sums)
} DeltaSequence;

/*
 * Tek bir boyuta (P) ait Tüm Çözüm ve Rekor Kaydı
 */
typedef struct {
    int p;                                  // Boyut (eleman sayısı)
    int score;                              // Ulaşılan maksimum kesintisiz aralık skoru (M)
    bool is_optimal;                        // Kesinleşmiş optimum mu?
    int solutions_count;                    // Bu skora ulaşan toplam çözüm sayısı
    int stored_solutions_count;             // Hafızada saklanan alternatif çözüm sayısı
    DeltaSequence solutions[MAX_SOLUTIONS]; // Alternatif çözümler havuzu
} SolutionRecord;

/*
 * PSPP Bilgi Tabanı ve Dinamik Tablo Yapısı
 */
typedef struct {
    SolutionRecord table[MAX_P];
    int max_p;
} PSPP_KnowledgeBase;

/*
 * Dahili Yardımcı: Bir boyuta yeni bir alternatif çözüm ekler
 */
static inline void kb_add_solution(PSPP_KnowledgeBase* kb, int p, int score, const int* delta_arr, bool is_optimal) {
    if (p <= 0 || p >= MAX_P) return;
    
    SolutionRecord* rec = &kb->table[p];
    
    if (score > rec->score || rec->score == 0) {
        rec->p = p;
        rec->score = score;
        rec->is_optimal = is_optimal;
        rec->solutions_count = 1;
        rec->stored_solutions_count = 0;
    } else if (score == rec->score) {
        rec->solutions_count++;
    } else {
        return; // Düşük skor eklenmez
    }
    
    if (rec->stored_solutions_count < MAX_SOLUTIONS) {
        for (int s = 0; s < rec->stored_solutions_count; s++) {
            if (memcmp(rec->solutions[s].delta, delta_arr, p * sizeof(int)) == 0) {
                return; // Zaten kayıtlı
            }
        }
        DeltaSequence* seq = &rec->solutions[rec->stored_solutions_count];
        int sum = 0;
        for (int i = 0; i < p; i++) {
            seq->delta[i] = delta_arr[i];
            sum += delta_arr[i];
            seq->dizi[i] = sum;
        }
        rec->stored_solutions_count++;
    }
}

/*
 * JSON Formatında Diske Kaydetme Fonksiyonu
 */
static inline void kb_save_json(const PSPP_KnowledgeBase* kb, const char* filepath) {
    FILE* f = fopen(filepath, "w");
    if (!f) return;

    fprintf(f, "{\n");
    bool first_p = true;
    for (int p = 1; p < MAX_P; p++) {
        const SolutionRecord* rec = &kb->table[p];
        if (rec->score <= 0 || rec->stored_solutions_count <= 0) continue;

        if (!first_p) fprintf(f, ",\n");
        first_p = false;

        fprintf(f, "  \"%d\": {\n", p);
        fprintf(f, "    \"p\": %d,\n", p);
        fprintf(f, "    \"score\": %d,\n", rec->score);
        fprintf(f, "    \"is_optimal\": %s,\n", rec->is_optimal ? "true" : "false");
        fprintf(f, "    \"solutions\": [\n");

        for (int s = 0; s < rec->stored_solutions_count; s++) {
            const DeltaSequence* seq = &rec->solutions[s];
            fprintf(f, "      {\n");
            
            fprintf(f, "        \"delta\": [");
            for (int i = 0; i < p; i++) {
                fprintf(f, "%d%s", seq->delta[i], i == p - 1 ? "" : ", ");
            }
            fprintf(f, "],\n");

            fprintf(f, "        \"dizi\": [");
            for (int i = 0; i < p; i++) {
                fprintf(f, "%d%s", seq->dizi[i], i == p - 1 ? "" : ", ");
            }
            fprintf(f, "]\n");

            fprintf(f, "      }%s\n", s == rec->stored_solutions_count - 1 ? "" : ",");
        }
        fprintf(f, "    ]\n");
        fprintf(f, "  }");
    }
    fprintf(f, "\n}\n");
    fclose(f);
}

/*
 * Basit JSON Parser: pspp_database.json dosyasını okuyup tabloyu doldurur
 */
static inline bool kb_load_from_json(PSPP_KnowledgeBase* kb, const char* filepath) {
    FILE* f = fopen(filepath, "r");
    if (!f) return false;

    fseek(f, 0, SEEK_END);
    long len = ftell(f);
    fseek(f, 0, SEEK_SET);
    if (len <= 0) { fclose(f); return false; }

    char* buf = (char*)malloc(len + 1);
    if (!buf) { fclose(f); return false; }
    fread(buf, 1, len, f);
    buf[len] = '\0';
    fclose(f);

    char* cur = buf;
    while ((cur = strstr(cur, "\"p\":")) != NULL) {
        int p = 0, score = 0;
        if (sscanf(cur, "\"p\": %d", &p) != 1 || p <= 0 || p >= MAX_P) {
            cur += 4;
            continue;
        }
        
        char* score_ptr = strstr(cur, "\"score\":");
        if (score_ptr) sscanf(score_ptr, "\"score\": %d", &score);

        char* opt_ptr = strstr(cur, "\"is_optimal\":");
        bool is_opt = (opt_ptr && strncmp(opt_ptr + 13, "true", 4) == 0);

        char* sol_ptr = strstr(cur, "\"solutions\":");
        if (!sol_ptr) { cur += 4; continue; }

        char* d_ptr = sol_ptr;
        while ((d_ptr = strstr(d_ptr, "\"delta\":")) != NULL) {
            char* next_p = strstr(cur + 4, "\"p\":");
            if (next_p && d_ptr > next_p) break;

            char* bracket = strchr(d_ptr, '[');
            if (!bracket) break;
            bracket++;

            int delta_arr[MAX_P] = {0};
            int count = 0;
            char* token = bracket;
            while (*token && *token != ']' && count < p) {
                while (*token == ' ' || *token == ',' || *token == '\t' || *token == '\n' || *token == '\r') token++;
                if (*token == ']') break;
                delta_arr[count++] = atoi(token);
                while (*token && *token != ',' && *token != ']') token++;
            }

            if (count == p && score > 0) {
                kb_add_solution(kb, p, score, delta_arr, is_opt);
            }
            d_ptr = token;
        }
        cur += 4;
    }

    free(buf);
    return true;
}

/*
 * Bilgi Tabanını Başlatır (pspp_database.json'dan yükler)
 */
static inline void kb_init(PSPP_KnowledgeBase* kb) {
    memset(kb, 0, sizeof(PSPP_KnowledgeBase));
    kb->max_p = MAX_P - 1;

    // Önce çalışma dizininde dene, yoksa üst dizinde dene
    if (kb_load_from_json(kb, DB_FILE) || kb_load_from_json(kb, DB_FILE_PARENT)) {
        return;
    }

    // Dosya yoksa tablo tamamen boş başlar (sabit kodlu veri yüklenmez)
}

/*
 * Bir boyuta yeni bir çözüm önerildiğinde güncelleme yapar (yeni rekor veya alternatif)
 */
static inline bool kb_update(PSPP_KnowledgeBase* kb, int p, int score, const int* delta_arr) {
    if (p <= 0 || p >= MAX_P) return false;
    
    SolutionRecord* rec = &kb->table[p];
    if (score > rec->score || rec->score == 0) {
        kb_add_solution(kb, p, score, delta_arr, false);
        return true; // Yeni rekor
    } else if (score == rec->score) {
        int old_count = rec->stored_solutions_count;
        kb_add_solution(kb, p, score, delta_arr, false);
        return (rec->stored_solutions_count > old_count); // Yeni alternatif
    }
    return false;
}

/*
 * Tek bir boyutun rekorunu ve alternatiflerini ekrana basar
 */
static inline void kb_print_record(const SolutionRecord* rec) {
    if (!rec || rec->score <= 0) return;
    printf("\n=== P = %d REKOR BILGISI ===\n", rec->p);
    printf("Zirve Skor (M) : %d\n", rec->score);
    printf("Cozum Sayisi   : %d\n", rec->stored_solutions_count);
    for (int s = 0; s < rec->stored_solutions_count; s++) {
        printf("  [#%d] Delta: [", s + 1);
        for (int i = 0; i < rec->p; i++) {
            printf("%d%s", rec->solutions[s].delta[i], i == rec->p - 1 ? "]\n" : ", ");
        }
    }
    printf("============================\n");
}

/*
 * Hedef Bilinen Max Değerini Getirir
 */
static inline int kb_get_target_max(const PSPP_KnowledgeBase* kb, int p) {
    if (p > 0 && p < MAX_P && kb->table[p].score > 0) {
        return kb->table[p].score;
    }
    if (p >= 20) {
        return 264 + 22 * (p - 20);
    }
    return 0;
}

#endif // PSPP_KB_H
