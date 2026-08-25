//Rastgele Baþlangýçlý Tepe Týrmanma (Random-Restart Hill Climbing)

#include <stdio.h>
#include <stdlib.h>
#include <time.h>    // time() fonksiyonu için
#include <limits.h>  // INT_MIN için
#include <math.h>    // abs() için
#include <stdbool.h>


// --- Ayarlar ---
const int MIN_VAL = 0;   // Array elemanlarý için minimum deðer
const int MAX_VAL = 56;   // Array elemanlarý için maksimum deðer
const int n = 8;

const int MAX_RESTARTS = 10000;           // Toplam yeniden baþlama sayýsý
const int NEIGHBORS_TO_CHECK = 100;    // Her týrmanma adýmýnda bakýlacak komþu sayýsý
// --- Ayarlar Bitti ---


void print_array(const char* label, int* array, int n) {
    printf("%s: [ ", label);
    
    // Array'in tüm elemanlarý arasýnda virgül ile dolaþ
    for (int i = 0; i < n; i++) {
        printf("%d", array[i]);
        
        // Son eleman deðilse, sonrasýna virgül ve boþluk koy
        if (i < n - 1) {
            printf(", ");
        }
    }
    
    // Satýrý bitir
    printf(" ]\n");
}

int int_compare(const void * a, const void * b) {
   return ( *(int*)a - *(int*)b );
}






/**
 * @brief Amaç fonksiyonu.
 * Skoru maksimize etmeye çalýþýyoruz.
 * Bu örnekte, tüm elemanlarýn TARGET_VAL (5) olmasýný istiyoruz.
 * Skor, hedeften toplam sapmanýn negatifidir. En iyi skor 0'dýr.
 */
int objective_function(int* array, int n) {
    int max = 0;
    while (true) {
        bool bulundu = false;

        // Dizide doðrudan max + 1 var mý?
        for (int i = 0; i < n; i++) {
            if (array[i] == max + 1) {
                max++;
                bulundu = true;
                break;
            }
        }

        if (bulundu) continue;

        // Toplam ve fark kombinasyonlarýný kontrol et
        for (int i = 0; i < n; i++) {
            for (int j = i; j < n; j++) {
                int toplam = array[i] + array[j];
                int fark = abs(array[i] - array[j]);

                if (toplam == max + 1 || fark == max + 1) {
                    max++;
                    bulundu = true;
                    break;
                }
            }
            if (bulundu) break;
        }

        if (!bulundu) break;
    }

    return max;
    
    
    
    
    

}




/**
 * @brief Bir array'i MIN_VAL ve MAX_VAL arasýnda rastgele tamsayýlarla doldurur.
 */

void generate_random_solution(int* array, int n) {
    // 1. Adým: Array'i rastgele sayýlarla doldur (önceki gibi)
    for (int i = 0; i < n; i++) {
        array[i] = (rand() % (MAX_VAL - MIN_VAL + 1)) + MIN_VAL;
    }

    // 2. Adým: Array'i sýrala
    // qsort(array, array_uzunlugu, her_elemanin_boyutu, karsilastirma_fonksiyonu)
    qsort(array, n, sizeof(int), int_compare);
}

/**
 * @brief Bir "komþu" çözüm üretir.
 * 'current_solution'ý 'neighbor_solution'a kopyalar ve sonra
 * rastgele bir elemanýn deðerini +1 veya -1 deðiþtirir.
 * Deðerlerin sýnýrlar (MIN_VAL, MAX_VAL) içinde kalmasýný saðlar.
 */
void get_random_neighbor(int* current_solution, int* neighbor_solution, int n) {
    // Önce mevcut çözümü komþuya kopyala
    for (int i = 0; i < n; i++) {
        neighbor_solution[i] = current_solution[i];
    }

    // Deðiþtirilecek rastgele bir indeks seç
    int index_to_change = rand() % n;
    
    // +1 mi -1 mi olacaðýna karar ver
    int change = (rand() % 2 == 0) ? 1 : -1;

    // Deðeri uygula
    neighbor_solution[index_to_change] += change;

    // Sýnýrlarý kontrol et
    if (neighbor_solution[index_to_change] > MAX_VAL) {
        neighbor_solution[index_to_change] = MAX_VAL;
    }
    if (neighbor_solution[index_to_change] < MIN_VAL) {
        neighbor_solution[index_to_change] = MIN_VAL;
    }
}

/**
 * @brief Yardýmcý fonksiyon: Array'i ve skorunu ekrana basar.
 */
void print_solution(const char* label, int* array, int n, int score) {
    printf("%s (Skor: %d): [ ", label, score);
    for (int i = 0; i < n; i++) {
        printf("%d ", array[i]);
    }
    printf("]\n");
}

/**
 * @brief Bir array'i diðerine kopyalar.
 */
void copy_array(int* source, int* dest, int n) {
    for (int i = 0; i < n; i++) {
        dest[i] = source[i];
    }
}


int main() {
    // Array boyutunu (n) burada tanýmlýyoruz


    // Rastgele sayý üretecini baþlat
    srand(time(NULL));

    // Çözümleri tutmak için bellekten yer ayýr
    // (malloc kullanarak n boyutunu dinamik olarak yönetiyoruz)
    int* global_best_solution = (int*)malloc(n * sizeof(int));
    int* current_solution     = (int*)malloc(n * sizeof(int));
    int* best_neighbor        = (int*)malloc(n * sizeof(int));
    int* temp_neighbor        = (int*)malloc(n * sizeof(int));

    if (global_best_solution == NULL || current_solution == NULL || best_neighbor == NULL || temp_neighbor == NULL) {
        printf("Bellek ayrilamadi!\n");
        return 1;
    }

    int global_best_score = INT_MIN; // Baþlangýçta en düþük skoru ata

    printf("Rastgele Baþlangiçli Tepe Tirmanma basliyor...\n");
    printf("Array Boyutu (n): %d\n", n);
    printf("Toplam Yeniden Baslama Sayisi: %d\n\n", MAX_RESTARTS);


    // --- 1. DIÞ DÖNGÜ: Random-Restart ---
    for (int restart = 0; restart < MAX_RESTARTS; restart++) {
        
        // Tamamen rastgele yeni bir çözümle baþla
        generate_random_solution(current_solution, n);
        int current_score = objective_function(current_solution, n);

        // --- 2. ÝÇ DÖNGÜ: Hill Climbing (Tepe Týrmanma) ---
        while (1) {
            
            // Mevcut týrmanma adýmýndaki en iyi komþuyu ara
            copy_array(current_solution, best_neighbor, n);
            int best_neighbor_score = current_score;

            for (int i = 0; i < NEIGHBORS_TO_CHECK; i++) {
                get_random_neighbor(current_solution, temp_neighbor, n);
                int temp_score = objective_function(temp_neighbor, n);

                // Daha iyi bir komþu bulursak, onu kaydet
                if (temp_score > best_neighbor_score) {
                    best_neighbor_score = temp_score;
                    copy_array(temp_neighbor, best_neighbor, n);
                }
            }

            // Döngü sonunda, bulduðumuz en iyi komþu mevcut durumdan daha mý iyi?
            if (best_neighbor_score > current_score) {
                // Evet, tepeye týrmanmaya devam et
                current_score = best_neighbor_score;
                
                copy_array(best_neighbor, current_solution, n);
            } else {
                // Hayýr, daha iyi bir komþu bulamadýk.
                // Bu bir "yerel maksimum" (local maximum).
                // Týrmanmayý bitir ve bu döngüden çýk.
                break; 
            }
        } // Tepe Týrmanma (iç döngü) bitti.

        // Bulduðumuz yerel maksimum, þimdiye kadarki genel en iyiden daha mý iyi?
        if (current_score > global_best_score) {
            global_best_score = current_score;
            copy_array(current_solution, global_best_solution, n);
            
            // Yeni en iyi çözümü raporla
            printf("--- YENI GLOBAL EN IYI COZUM (Restart %d) ---\n", restart + 1);
            print_solution("Yeni En Iyi", global_best_solution, n, global_best_score);
            printf("---------------------------------------------\n");
        }

    } // Yeniden Baþlatma (dýþ döngü) bitti.

    // --- Sonuçlarý Raporla ---
    printf("\n=== ARAMA TAMAMLANDI ===\n");
    print_solution("Bulunan En iyi cozum", global_best_solution, n, global_best_score);

    // Ayrýlan belleði serbest býrak
    free(global_best_solution);
    free(current_solution);
    free(best_neighbor);
    free(temp_neighbor);

    return 0;
}
