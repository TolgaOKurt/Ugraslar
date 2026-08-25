#include <stdio.h>
#include <stdlib.h> // abs(), rand(), srand(), malloc(), free(), qsort()
#include <stdbool.h> // bool, true, false
#include <math.h>     // exp()
#include <time.h>     // time()
#include <string.h>   // memcpy()

// --- Optimizasyon Parametreleri (Burayý degiþtirebilirsiniz) ---
#define N 10
#define MIN_VAL 1
#define MAX_VAL 65

// --- Benzetimli Tavlama Parametreleri ---
#define INITIAL_TEMPERATURE 1000.0
#define FINAL_TEMPERATURE 0.001
#define COOLING_RATE 0.995
#define ITERATIONS_PER_TEMP 20000

// --- Sizin Sagladýgýnýz Amaç Fonksiyonu ---
// (Degiþiklik yok, fonksiyon sýralý veya sýrasýz dizilerle çalýþabilir)
int objective_function(int* array, int n) {
    int max = 0;
    while (true) {
        bool bulundu = false;
        // 1. Dizide dogrudan max + 1 var mý?
        for (int i = 0; i < n; i++) {
            if (array[i] == max + 1) {
                max++;
                bulundu = true;
                break;
            }
        }
        if (bulundu) continue;

        // 2. Toplam ve fark kombinasyonlarýný kontrol et
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

// --- Yardýmcý Fonksiyonlar ---

/**
 * @brief qsort() fonksiyonu için karþýlaþtýrma fonksiyonu.
 */
int compare_ints(const void *a, const void *b) {
    int arg1 = *(const int*)a;
    int arg2 = *(const int*)b;
    return (arg1 > arg2) - (arg1 < arg2); // Kýsaca (arg1 - arg2)
}

/**
 * @brief Belirtilen aralýkta [min, max] rastgele bir tam sayý üretir.
 */
int random_int(int min, int max) {
    return min + rand() % (max - min + 1);
}

/**
 * @brief [0.0, 1.0] aralýgýnda rastgele bir double üretir.
 */
double random_double() {
    return (double)rand() / RAND_MAX;
}

/**
 * @brief Rastgele sayýlardan oluþan bir baþlangýç çözümü (dizisi) üretir.
 * YENÝ: Üretilen dizi küçükten büyüge sýralanýr.
 */
void generate_initial_solution(int* array, int n) {
    for (int i = 0; i < n; i++) {
        array[i] = random_int(MIN_VAL, MAX_VAL);
    }
    
    // YENÝ: Baþlangýç çözümünün sýralý olmasýný garantile
    qsort(array, n, sizeof(int), compare_ints);
}

/**
 * @brief Mevcut çözümden küçük bir degiþiklik (mutasyon) yaparak 
 * yeni bir "komþu" çözüm üretir.
 * YENÝ: Üretilen komþu çözüm de sýralanýr.
 */
void get_neighbor(int* current, int* neighbor, int n) {
    // Önce mevcut çözümü komþuya kopyala
    memcpy(neighbor, current, n * sizeof(int));

    // Degiþtirilecek rastgele bir indeks seç
    int index_to_change = random_int(0, n - 1);
    
    // O indekse yeni rastgele bir deger ata
    neighbor[index_to_change] = random_int(MIN_VAL, MAX_VAL);

    // YENÝ: Dizinin sýralý kalmasýný saglamak için tekrar sýrala
    // Bu, "komþu" tanýmýmýzý "sýralý diziler uzayýnda" tutar.
    qsort(neighbor, n, sizeof(int), compare_ints);
}

// --- Ana Benzetimli Tavlama Fonksiyonu ---
// (Degiþiklik yok)
void simulated_annealing() {
    // Rastgele sayý üretecini baþlat
    srand(time(NULL));

    // Çözüm dizileri için bellek ayýr
    int* current_solution = (int*)malloc(N * sizeof(int));
    int* neighbor_solution = (int*)malloc(N * sizeof(int));
    int* best_solution = (int*)malloc(N * sizeof(int));

    if (!current_solution || !neighbor_solution || !best_solution) {
        fprintf(stderr, "Bellek ayrýlamadý!\n");
        exit(1);
    }

    // 1. Baþlangýç çözümü üret ve skorunu hesapla
    generate_initial_solution(current_solution, N); // Artýk sýralý üretecek
    int current_score = objective_function(current_solution, N);

    // 2. En iyi çözümü baþlangýç çözümü olarak ayarla
    memcpy(best_solution, current_solution, N * sizeof(int));
    int best_score = current_score;

    // 3. Sýcaklýgý baþlat
    double temperature = INITIAL_TEMPERATURE;
    long long iteration = 0;

    printf("Benzetimli Tavlama basliyor (Sirali Diziler Kisitlamasiyla)...\n");
    printf("Parametreler: N=%d, Range=[%d, %d], T_init=%.1f, Cool=%.3f\n\n", 
           N, MIN_VAL, MAX_VAL, INITIAL_TEMPERATURE, COOLING_RATE);

    // 4. Ana Döngü
    while (temperature > FINAL_TEMPERATURE) {
        
        for (int i = 0; i < ITERATIONS_PER_TEMP; i++) {
            
            // 6. Bir komþu çözüm üret (artýk sýralý komþu üretecek)
            get_neighbor(current_solution, neighbor_solution, N);

            // 7. Komþunun skorunu hesapla
            int neighbor_score = objective_function(neighbor_solution, N);

            // 8. Skor farkýný (Delta) hesapla
            int score_diff = neighbor_score - current_score;

            // 9. Karar verme (Metropolis kriteri)
            if (score_diff > 0) {
                // Yeni çözüm daha iyi
                memcpy(current_solution, neighbor_solution, N * sizeof(int));
                current_score = neighbor_score;
            } else {
                // Yeni çözüm daha kötü, olasýlýkla kabul et
                double acceptance_prob = exp((double)score_diff / temperature);
                if (random_double() < acceptance_prob) {
                    memcpy(current_solution, neighbor_solution, N * sizeof(int));
                    current_score = neighbor_score;
                }
            }

            // 10. Þimdiye kadarki en iyi çözümü güncelle
            if (current_score > best_score) {
                memcpy(best_solution, current_solution, N * sizeof(int));
                best_score = current_score;
            }
            
            iteration++;
        }

        // 11. Sistemi sogut
        temperature *= COOLING_RATE;
        
        if (iteration % (ITERATIONS_PER_TEMP * 100) == 0) {
            printf("[Iter %lld] Sicaklik: %.4f, Skor: %d ", iteration, temperature, best_score);
		    printf("(N=%d): [ ", N);
		    for (int i = 0; i < N; i++) {
		        printf("%d ", best_solution[i]);
		    }
		    printf("]\n");
        }
    }

    // 12. Sonuçlarý yazdýr
    printf("\n--- Benzetimli Tavlama Tamamlandi ---\n");
    printf("Toplam Iterasyon: %lld\n", iteration);
    printf("En Yuksek Skor (Max Deger): %d\n", best_score);
    printf("Bu Skoru Veren Sirali Dizi (N=%d): [ ", N);
    for (int i = 0; i < N; i++) {
        printf("%d ", best_solution[i]);
    }
    printf("]\n");

    // 13. Ayrýlan bellegi serbest býrak
    free(current_solution);
    free(neighbor_solution);
    free(best_solution);
}

// --- Programýn Baþlangýç Noktasý ---

int main() {
    simulated_annealing();
    return 0;
}
