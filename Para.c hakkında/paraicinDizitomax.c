
#include <stdio.h>
#include <math.h>
#include <time.h>
#include <stdlib.h>
#include <stdbool.h>
#define MAX_VALUE 200

void QS(int arr[], int low, int high) { // internetten çalýnmýþ quicksort
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



int yenicalc(int* dizi, int si) {// abooo ben malým ya  bu nasý kod böyle bi dk bunun dizi bazlý olanýný yapmam lazým// degilmisim dizili kotu calýsýyor daha yavas anlamsýz
    int max = 0;
    while (true) {
        bool bulundu = false;

        // Dizide doðrudan max + 1 var mý?
        for (int i = 0; i <= si; i++) {
            if (dizi[i] == max + 1) {
                max++;
                bulundu = true;
                break;
            }
        }

        if (bulundu) continue;

        // Toplam ve fark kombinasyonlarýný kontrol et
        for (int i = 0; i <= si; i++) {
            for (int j = i; j <= si; j++) {
                int toplam = dizi[i] + dizi[j];
                int fark = abs(dizi[i] - dizi[j]);

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

/*
int dizilicalc(int* dizi, int si) {
    int max = 0;
    int p=si+1;
    int toplamboyut=(p*(p+1))/2;
    int farkboyut=(p*(p-1))/2;
    int fullboyut=toplamboyut+farkboyut+p;

    int dizsay=0,toplamsay=0,farksay=0;
    int i=0;
    int sira=0;
    int fulldizi[p+toplamboyut+farkboyut];

	for(int i=0;i<=si;i++){
		fulldizi[sira]=dizi[i];
		sira++;
	}
	
 	for(int i=0;i<=si;i++){
 		for(int j=i;j<=si;j++){
	 		fulldizi[sira]=dizi[i]+dizi[j];
	 		sira++;
		}
 		
	}
	for(int i=0;i<=si;i++){
 		for(int j=i+1;j<=si;j++){
	 		fulldizi[sira]=abs(dizi[i]-dizi[j]);
	 		sira++;
		}
 		
	}

	QS(fulldizi,0,fullboyut-1);
	
	
	int l=0;
	for(l=0;l<fullboyut;l++){
	//	printf("\n l=%d yani %d suan",l,fulldizi[l]);
		if(fulldizi[l]<max+1){
		//printf(" pass");
			continue;
		} 
		else if(fulldizi[l]==max+1){
			//printf(" max artar");
			max++;
		}
		else break;
		
	}
	return max;
	
}

*/


int dizilicalc(int* dizi, int si) {
    int max = 0;
    int p = si + 1;
    int toplamboyut = (p * (p + 1)) / 2;
    int farkboyut = (p * (p - 1)) / 2;
    int fullboyut = toplamboyut + farkboyut + p;

    int* fulldizi = (int*)malloc(fullboyut * sizeof(int));
    if (!fulldizi) {
        printf("Bellek tahsisi basarisiz!\n");
        return -1;
    }

    int sira = 0;

    // Dizinin elemanlarýný ekle
    for (int i = 0; i <= si; i++) {
        fulldizi[sira++] = dizi[i];
    }

    // Toplam kombinasyonlarý ekle
    for (int i = 0; i <= si; i++) {
        for (int j = i; j <= si; j++) {
            fulldizi[sira++] = dizi[i] + dizi[j];
        }
    }

    // Fark kombinasyonlarýný ekle
    for (int i = 0; i <= si; i++) {
        for (int j = i + 1; j <= si; j++) {
            fulldizi[sira++] = abs(dizi[i] - dizi[j]);
        }
    }

    // Sýralama (qsort kullanýmý)
    qsort(fulldizi, fullboyut, sizeof(int), compare);

    // max hesaplama
    for (int l = 0; l < fullboyut; l++) {
        if (l > 0 && fulldizi[l] == fulldizi[l - 1]) continue; // Tekrarlarý atla

        if (fulldizi[l] == max + 1) {
            max++;
        } else if (fulldizi[l] > max + 1) {
            break;
        }
    }

    free(fulldizi);
    return max;
}



int main(){
	int p=8;//!!!!!
	int si=p-1;
	time_t start,end;
	int dizi[]={6,12,18,21,22,23,25,26,-1};
	start=clock();
	int f=dizilicalc(dizi,si);
	end=clock();
	printf("\n %d , sure %d\n",f,end-start);
	start=clock();
	f=yenicalc(dizi,si);
	end=clock();
	printf("\n %d , sure %d\n",f,end-start);
}


