#include <stdio.h>
#include <stdlib.h> // abs, rand, srand, qsort, malloc, free için
#include <time.h>   // time için
#include <stdbool.h> // bool turu için
#include <math.h>   // abs için

// --- GA Parametreleri (Sub-optimalden kaçmak için ayarlandi) ---
#define POPULATION_SIZE 10000     // *** DEgIsIKLIK *** (Artirildi: 100 -> 300) Daha fazla çesitlilik
#define CHROMOSOME_LENGTH 7    // Her bir dizinin uzunlugu (n)
#define MAX_GENE_VALUE 50       // Dizideki sayilarin alabilecegi maksimum deger (1'den baslar)
#define MIN_GENE_VALUE 1        // Dizideki sayilarin alabilecegi minimum deger
#define MUTATION_RATE 0.15      // *** DEgIsIKLIK *** (Artirildi: 0.05 -> 0.10) Yerel optimumdan kaçis için
#define CROSSOVER_RATE 0.8      // *** DEgIsIKLIK *** (Artirildi: 0.7 -> 0.8)
#define MAX_GENERATIONS 10000    // *** DEgIsIKLIK *** (Artirildi: 500 -> 1000) Daha fazla zaman
#define TOURNAMENT_SIZE 2       // *** DEgIsIKLIK *** (Azaltildi: 3 -> 2) Seçilim baskisini azaltir
#define ELITISM_COUNT 1         // *** DEgIsIKLIK *** (Azaltildi: 2 -> 1) En iyi 1 birey korunsun yeter

// --- Birey Yapisi ---
typedef struct {
    int* genes;     // Bireyin genleri (sirali int dizisi)
    int fitness;  // objective_function'dan donen skor
} Individual;

// ===============================================
// === KULLANICININ HEDEF FONKSIYONU (Fitness) ===
// ===============================================
// (Degisiklik yok)
int objective_function(int* array, int n) {
    int max = 0;
    while (true) {
        bool bulundu = false;
        // Dizide dogrudan max + 1 var mi?
        for (int i = 0; i < n; i++) {
            if (array[i] == max + 1) {
                max++;
                bulundu = true;
                break;
            }
        }
        if (bulundu) continue;

        // Toplam ve fark kombinasyonlarini kontrol et
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
// ===============================================

// --- qsort için karsilastirma fonksiyonlari ---
int compare_integers(const void* a, const void* b) {
    return (*(int*)a - *(int*)b);
}

int compare_individuals(const void* a, const void* b) {
    Individual* indA = (Individual*)a;
    Individual* indB = (Individual*)b;
    return (indB->fitness - indA->fitness);
}

// --- GA Fonksiyonlari ---

void create_individual(Individual* individual) {
    individual->genes = (int*)malloc(CHROMOSOME_LENGTH * sizeof(int));
    for (int j = 0; j < CHROMOSOME_LENGTH; j++) {
        individual->genes[j] = (rand() % (MAX_GENE_VALUE - MIN_GENE_VALUE + 1)) + MIN_GENE_VALUE;
    }
    qsort(individual->genes, CHROMOSOME_LENGTH, sizeof(int), compare_integers);
    individual->fitness = 0;
}

void initialize_population(Individual* population) {
    for (int i = 0; i < POPULATION_SIZE; i++) {
        create_individual(&population[i]);
    }
}

void calculate_fitness(Individual* population) {
    for (int i = 0; i < POPULATION_SIZE; i++) {
        population[i].fitness = objective_function(population[i].genes, CHROMOSOME_LENGTH);
    }
}

Individual* select_parent(Individual* population) {
    Individual* best = NULL;
    for (int i = 0; i < TOURNAMENT_SIZE; i++) {
        int idx = rand() % POPULATION_SIZE;
        if (best == NULL || population[idx].fitness > best->fitness) {
            best = &population[idx];
        }
    }
    return best;
}

/**
 * @brief *** DEgIsIKLIK ***
 * Duzgun Çaprazlama (Uniform Crossover) uygulandi.
 * Her gen, %50 sansla ebeveyn 1 veya 2'den alinir.
 * Bu, genetik materyalin daha iyi karismasini saglar.
 */
void crossover(Individual* parent1, Individual* parent2, Individual* offspring) {
    offspring->genes = (int*)malloc(CHROMOSOME_LENGTH * sizeof(int));
    
    if ((rand() / (double)RAND_MAX) < CROSSOVER_RATE) {
        // Uniform Crossover
        for (int i = 0; i < CHROMOSOME_LENGTH; i++) {
            if ((rand() / (double)RAND_MAX) < 0.5) {
                offspring->genes[i] = parent1->genes[i];
            } else {
                offspring->genes[i] = parent2->genes[i];
            }
        }
    } else {
        // Çaprazlama olmazsa, ebeveyn 1'i kopyala
        for (int i = 0; i < CHROMOSOME_LENGTH; i++) {
            offspring->genes[i] = parent1->genes[i];
        }
    }

    // *** KURAL: Dizi her zaman sirali olmali ***
    qsort(offspring->genes, CHROMOSOME_LENGTH, sizeof(int), compare_integers);
    offspring->fitness = 0;
}

/**
 * @brief *** DEgIsIKLIK ***
 * "Creep Mutation" (Kaydirmali/Surunen Mutasyon) uygulandi.
 * Geni tamamen rastgele bir degerle degistirmek yerine,
 * mevcut degere kuçuk bir rastgele sayi ekler/çikarir.
 * Bu, ince ayar (fine-tuning) için daha iyidir.
 */
void mutate(Individual* individual) {
    bool mutated = false;
    for (int i = 0; i < CHROMOSOME_LENGTH; i++) {
        if ((rand() / (double)RAND_MAX) < MUTATION_RATE) {
            // Kuçuk bir miktar ekle/çikar (orn: -4 ile +5 arasi)
            int creep_value = (rand() % 10) - 4; 
            individual->genes[i] += creep_value;

            // Genin sinirlar içinde kaldigindan emin ol
            if (individual->genes[i] > MAX_GENE_VALUE) {
                individual->genes[i] = MAX_GENE_VALUE;
            }
            if (individual->genes[i] < MIN_GENE_VALUE) {
                individual->genes[i] = MIN_GENE_VALUE;
            }
            mutated = true;
        }
    }

    // *** KURAL: Dizi her zaman sirali olmali ***
    if (mutated) {
        qsort(individual->genes, CHROMOSOME_LENGTH, sizeof(int), compare_integers);
    }
}


void free_population(Individual* population) {
    for (int i = 0; i < POPULATION_SIZE; i++) {
        free(population[i].genes);
    }
    free(population);
}

// --- Ana Fonksiyon ---
int main() {
    srand(time(NULL));

    Individual* population = (Individual*)malloc(POPULATION_SIZE * sizeof(Individual));
    initialize_population(population);

    printf("Genetik Algoritma Baslatildi (Iyilestirilmis Surum)...\n");
    printf("Parametreler: Populasyon=%d, Dizi Uzunlugu=%d, Max Deger=%d, Nesil=%d\n",
           POPULATION_SIZE, CHROMOSOME_LENGTH, MAX_GENE_VALUE, MAX_GENERATIONS);
    printf("Mutasyon Orani: %.2f, Elitizm: %d, Turnuva: %d\n",
           MUTATION_RATE, ELITISM_COUNT, TOURNAMENT_SIZE);
    printf("--------------------------------------------------\n");

    int best_fitness_so_far = 0;
    int generations_no_improvement = 0;

    for (int gen = 0; gen < MAX_GENERATIONS; gen++) {
        calculate_fitness(population);
        qsort(population, POPULATION_SIZE, sizeof(Individual), compare_individuals);

        // Ilerleme Raporu
        if (population[0].fitness > best_fitness_so_far) {
            best_fitness_so_far = population[0].fitness;
            generations_no_improvement = 0;
            
            printf("Nesil %4d | YENI EN IYI: %d | Dizi: [", gen, best_fitness_so_far);
            for (int i = 0; i < CHROMOSOME_LENGTH; i++) {
                printf("%d%s", population[0].genes[i], (i == CHROMOSOME_LENGTH - 1) ? "" : ", ");
            }
            printf("]\n");

        } else if (gen % 100 == 0) { // Her 100 nesilde bir durum raporu
             printf("Nesil %4d | Mevcut En Iyi: %d (En son %d nesil once bulundu)\n", gen, best_fitness_so_far, generations_no_improvement);
        }

        generations_no_improvement++;

        // *** DEgIsIKLIK *** (Opsiyonel)
        // Eger 200 nesildir hiçbir gelisme olmadiysa,
        // algoritmanin takildigini varsayip durdurabiliriz.
        if (generations_no_improvement > 200) {
            printf("200 nesildir gelisme yok. Yerel optimuma takilmis olabilir. Durduruluyor.\n");
            break;
        }


        Individual* new_population = (Individual*)malloc(POPULATION_SIZE * sizeof(Individual));

        // Elitizm
        for (int i = 0; i < ELITISM_COUNT; i++) {
            new_population[i].genes = (int*)malloc(CHROMOSOME_LENGTH * sizeof(int));
            for (int j = 0; j < CHROMOSOME_LENGTH; j++) {
                new_population[i].genes[j] = population[i].genes[j];
            }
            new_population[i].fitness = population[i].fitness;
        }

        // Geri kalan populasyon
        for (int i = ELITISM_COUNT; i < POPULATION_SIZE; i++) {
            Individual* parent1 = select_parent(population);
            Individual* parent2 = select_parent(population);
            crossover(parent1, parent2, &new_population[i]);
            mutate(&new_population[i]);
        }

        free_population(population);
        population = new_population;
    }

    // --- Sonuç ---
    calculate_fitness(population);
    qsort(population, POPULATION_SIZE, sizeof(Individual), compare_individuals);

    printf("--------------------------------------------------\n");
    printf("Genetik Algoritma Tamamlandi!\n");
    printf("En Yuksek Fitness: %d\n", population[0].fitness);
    printf("Bulunan En Iyi Dizi (Sirali): [");
    for (int i = 0; i < CHROMOSOME_LENGTH; i++) {
        printf("%d%s", population[0].genes[i], (i == CHROMOSOME_LENGTH - 1) ? "" : ", ");
    }
    printf("]\n");

    free_population(population);

    return 0;
}
