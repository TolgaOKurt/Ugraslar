#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <math.h>
#include <time.h>
#include <stdbool.h>
#define MAX_VALUE 200



#ifdef _WIN32
  #include <io.h>
  #define TRUNCATE_FN(fd, size) _chsize((fd), (size))
#else
  #include <unistd.h>
  #define TRUNCATE_FN(fd, size) ftruncate((fd), (size))
#endif

void QS(int arr[], int low, int high) { // internetten �al�nm�� quicksort
    if (low < high) {
        int pivot = arr[(low + high) / 2]; // Selecting the middle element as the pivot
        int i = low;
        int j = high;
        int temp;

        while (i <= j) {
            while (arr[i] < pivot) i++; // Moving elements smaller than pivot to the left
            while (arr[j] > pivot) j--; // Moving elements greater than pivot to the right

            if (i <= j) {
                temp = arr[i];         // Swapping elements
                arr[i] = arr[j];
                arr[j] = temp;
                i++;
                j--;
            }
        }

        // Recursively sort the two partitions
        if (low < j) QS(arr, low, j);
        if (i < high) QS(arr, i, high);
    }
    return;
}

int compare(const void *a, const void *b) {

    return (*(int *)a - *(int *)b);
}




// Dosyadan SON sat�r� siler. Ba�ar� -> 0, hata -> -1 (errno set edilir).
int removeLastLine(const char *filename) {
    FILE *fp = fopen(filename, "rb+"); // ikili mod: CRLF �evirilerini �nlemek i�in
    if (!fp) return -1;

    if (fseek(fp, 0, SEEK_END) != 0) {
        fclose(fp);
        return -1;
    }

    long pos = ftell(fp);
    if (pos <= 0) { // bo� dosya
        fclose(fp);
        return 0;
    }

    long i = pos - 1;

    // 1) Sondaki newline/carriage-return karakterlerini atla (\n ve \r)
    while (i >= 0) {
        if (fseek(fp, i, SEEK_SET) != 0) {
            fclose(fp);
            return -1;
        }
        int c = fgetc(fp);
        if (c != '\n' && c != '\r') break;
        i--;
    }

    // E�er t�m dosya bo� veya sadece newline karakterlerinden ibaretse -> truncate 0
    if (i < 0) {
        int fd = fileno(fp);
        int rc = TRUNCATE_FN(fd, 0);
        fclose(fp);
        if (rc != 0) return -1;
        return 0;
    }

    // 2) �nceki newline'� ara (yani silmek istedi�imiz sat�r�n *ba��ndan �nceki* '\n')
    long j;
    for (j = i; j >= 0; j--) {
        if (fseek(fp, j, SEEK_SET) != 0) {
            fclose(fp);
            return -1;
        }
        int c = fgetc(fp);
        if (c == '\n') {
            // Yeni boyutu j+1 olarak ayarla (yeni son '\n' dahil edilir)
            break;
        }
    }

    long newsize = (j < 0) ? 0 : (j + 1);
    int fd = fileno(fp);
    int rc = TRUNCATE_FN(fd, newsize);
    fclose(fp);

    if (rc != 0) return -1;
    return 0;
}





// Son sat�rdaki array ve sonucu okur
// arr -> okunan input array (�nceden yeterince b�y�k olmal�)
// size -> okunan eleman say�s�n� doldurur
// result -> output de�eri doldurur
// return 0 ba�ar�, -1 hata
int readLastArray(const char *filename, int arr[], int *size, int *result) {
    FILE *fp = fopen(filename, "r");
    if (!fp) return -1;

    char line[1024];
    char lastLine[1024] = {0};

    // Sat�r sat�r oku, en son sat�r� tut
    while (fgets(line, sizeof(line), fp)) {
        strcpy(lastLine, line);
    }
    fclose(fp);

    if (strlen(lastLine) == 0) return -1; // dosya bo�

    // Sat�rdaki newline�� temizle
    lastLine[strcspn(lastLine, "\r\n")] = 0;

    // Virg�llere g�re par�ala
    char *token = strtok(lastLine, ",");
    int count = 0;
    int values[256];

    while (token) {
        values[count++] = atoi(token);
        token = strtok(NULL, ",");
    }

    if (count < 1) return -1;

    // Son de�er output
    *result = values[count - 1];

    // Geri kalanlar input array
    for (int i = 0; i < count - 1; i++) {
        arr[i] = values[i];
    }
    *size = count - 1;

    return 0;
}



// Sonu�lar� CSV'ye yazan yard�mc� fonksiyon
void logToCSV(FILE *fp, int arr[], int size, int result) {
    if (fp == NULL) return;

    // Girdileri yaz (virg�lle ayr�lm��)
    for (int i = 0; i < size; i++) {
        fprintf(fp, "%d", arr[i]);
        if (i < size - 1) fprintf(fp, ",");
    }

    // ��kt�y� da sona ekle
    fprintf(fp, ",%d\n", result);
}



int myFunction(int arr[], int size) {
    int sum = 0;
    for (int i = 0; i < size; i++)
        sum += arr[i];
    return sum;
}


int main() {
    FILE *fp = fopen("log.csv", "w");  // dosyay� a� (�zerine yazar)
    if (fp == NULL) {
        perror("Dosya a��lamad�");
        return 1;
    }
	
    // CSV ba�l���
    fprintf(fp, "input...,output\n");

    // �rnek veriler
    int arr1[] = {2, 3, 4, 20};//array �rnekleri
    int arr2[] = {5, 11, 15, 50};

    int res1 = myFunction(arr1, 4);//sonuc=
    logToCSV(fp, arr1, 4, res1);

    int res2 = myFunction(arr2, 3);
    logToCSV(fp, arr2, 4, res2);
	
    
    
    fclose(fp);  // dosyay� kapat
    
    
    int size=0,result=0;
    int arr[10];
    
    
    
    
    // Son sat�r� sil
    if (removeLastLine("log.csv") != 0) {
        perror("removeLastLine failed");
        return 1;
    } else {
        puts("Son satir silindi.");
    }
    
    
    
    
    
    if(readLastArray("log.csv", arr, &size, &result) == 0) {
    	printf("Okunan array: ");
        for (int i = 0; i < size; i++) {
            printf("%d ", arr[i]);
        }
        printf("\nSonuc: %d Boyut: %d\n", result,size);
    } else {
        printf("Dosya okunamad�!\n");
    }
    
    
    
    return 0;
}

