
#include <stdio.h>
#include <math.h>
#include <time.h>
#include <stdlib.h>
#include <stdbool.h>






int yenicalc(int* dizi, int si) {
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

int main(){
	int p=9;
	int si=p-1;
	int dizi[]={2,3,4,6,7,8,9,0,0,-1};
	int max=yenicalc(dizi,si);
	printf("%d",max);
}


