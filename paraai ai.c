#include <stdio.h>
#include <stdlib.h>
#include <math.h>
//��E YARAMAZ A� KODU. �NCELENMED�. SADECE CHATGPT.
//HATALI SONU� D�ND�R�YOR 5 4 6 7 ���N 18 D�ND�R�YOR ANCAK 15 ELDE ED�LEMEZ.



// Kar��la�t�rma fonksiyonu (qsort i�in)
int compare(const int *a, const int *b) {
    return (*a - *b);
}

// Kombinasyonlar i�in yard�mc� bir fonksiyon
void generate_combinations(int *array, int n, int r, int index, int *data, int i, int *results, int *count) {
    if (index == r) {
        int sum = 0;
        for (int j = 0; j < r; j++) {
            sum += data[j];
        }
        results[(*count)++] = sum;
        return;
    }
    if (i >= n) {
        return;
    }
    data[index] = array[i];
    generate_combinations(array, n, r, index + 1, data, i + 1, results, count);
    generate_combinations(array, n, r, index, data, i + 1, results, count);
}

// Mutlak farklar� ekleyen fonksiyon
void add_absolute_differences(int *array, int p, int *results, int *count) {
    for (int i = 0; i < p; i++) {
        for (int j = i + 1; j < p; j++) {
            results[(*count)++] = abs(array[i] - array[j]);
        }
    }
}

// Elemanlar�n kendilerini ve 2 katlar�n� ekleyen fonksiyon
void add_elements_and_doubles(int *array, int p, int *results, int *count) {
    for (int i = 0; i < p; i++) {
        results[(*count)++] = array[i];
        results[(*count)++] = 2 * array[i];
    }
}

// Maksimum ard���k tam say� dizisini hesaplayan ana fonksiyon
int maximize_n_for_p_elements(int p, int max_value, int *best_array) {
    int max_n = 0;

    // T�m olas� kombinasyonlar� olu�turmak
    for (int i = 1; i <= max_value - p + 1; i++) {
        int array[p];
        for (int j = 0; j < p; j++) {
            array[j] = i + j;
        }

        int results[1000] = {0}; // Sonu�lar� saklamak i�in bir dizi
        int count = 0;

        // T�m kombinasyonlar� ekle
        for (int k = 1; k <= p; k++) {
            int data[k];
            generate_combinations(array, p, k, 0, data, 0, results, &count);
        }

        // Farklar� ekle
        add_absolute_differences(array, p, results, &count);

        // Elemanlar�n kendilerini ve 2 katlar�n� ekle
        add_elements_and_doubles(array, p, results, &count);

        // Sonu�lar� s�ralamak ve tekrarl� olanlar� ��karmak
        qsort(results, count, sizeof(int), (int (*)(const void *, const void *))compare);
        int unique_results[1000];
        int unique_count = 0;
        unique_results[unique_count++] = results[0];
        for (int k = 1; k < count; k++) {
            if (results[k] != results[k - 1]) {
                unique_results[unique_count++] = results[k];
            }
        }

        // 1'den itibaren ard���k tam say�lar� kontrol et
        int n = 1;
        for (int k = 0; k < unique_count; k++) {
            if (unique_results[k] == n) {
                n++;
            } else if (unique_results[k] > n) {
                break;
            }
        }

        // E�er n daha b�y�kse g�ncelle
        if (n - 1 > max_n) {
            max_n = n - 1;
            for (int j = 0; j < p; j++) {
                best_array[j] = array[j];
            }
        }
    }
    return max_n;
}

// Ana program
int main() {
    int p = 4;               // Dizi boyutu
    int max_value = 15;      // Elemanlar�n maksimum de�eri
    int best_array[p];       // En iyi dizi

    int max_n = maximize_n_for_p_elements(p, max_value, best_array);

    // Sonu�lar� yazd�r
    printf("En iyi %d-boyutlu dizi: ", p);
    for (int i = 0; i < p; i++) {
        printf("%d ", best_array[i]);
    }
    printf("\nMaksimum n: %d\n", max_n);

    return 0;
}

