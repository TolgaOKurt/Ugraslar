#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define MAX_VALUE 200

int compare(const void *a, const void *b) { return (*(int *)a - *(int *)b); }

int yenicalc(int *dizi,
             int si) { // abooo ben mal�m ya  bu nas� kod b�yle bi dk bunun dizi
                       // bazl� olan�n� yapmam laz�m// degilmisim dizili kotu
                       // cal�s�yor daha yavas anlams�z
  int max = 0;
  while (true) {
    bool bulundu = false;

    // Dizide doğrudan max + 1 var mı?
    for (int i = 0; i <= si; i++) {
      if (dizi[i] == max + 1) {
        max++;
        bulundu = true;
        break;
      }
    }

    if (bulundu)
      continue;

    // Toplam ve fark kombinasyonlar�n� kontrol et
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
      if (bulundu)
        break;
    }

    if (!bulundu)
      break;
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

int dizilicalc(int *dizi, int si) {
  int max = 0;
  int p = si + 1;
  int toplamboyut = (p * (p + 1)) / 2;
  int farkboyut = (p * (p - 1)) / 2;
  int fullboyut = toplamboyut + farkboyut + p;

  int *fulldizi = (int *)malloc(fullboyut * sizeof(int));
  if (!fulldizi) {
    printf("Bellek tahsisi basarisiz!\n");
    return -1;
  }

  int sira = 0;

  // Dizinin elemanlar�n� ekle
  for (int i = 0; i <= si; i++) {
    fulldizi[sira++] = dizi[i];
  }

  // Toplam kombinasyonlar� ekle
  for (int i = 0; i <= si; i++) {
    for (int j = i; j <= si; j++) {
      fulldizi[sira++] = dizi[i] + dizi[j];
    }
  }

  // Fark kombinasyonlar�n� ekle
  for (int i = 0; i <= si; i++) {
    for (int j = i + 1; j <= si; j++) {
      fulldizi[sira++] = abs(dizi[i] - dizi[j]);
    }
  }

  // S�ralama (qsort kullan�m�)
  qsort(fulldizi, fullboyut, sizeof(int), compare);

  // max hesaplama
  for (int l = 0; l < fullboyut; l++) {
    if (l > 0 && fulldizi[l] == fulldizi[l - 1])
      continue; // Tekrarlar� atla

    if (fulldizi[l] == max + 1) {
      max++;
    } else if (fulldizi[l] > max + 1) {
      break;
    }
  }

  free(fulldizi);
  return max;
}

int main() {
  int p = 8; //!!!!!
  int si = p - 1;
  time_t start, end;
  int dizi[] = {6, 12, 18, 21, 22, 23, 25, 26, -1};
  start = clock();
  int f = dizilicalc(dizi, si);
  end = clock();
  printf("\n %d , sure %d\n", f, end - start);
  start = clock();
  f = yenicalc(dizi, si);
  end = clock();
  printf("\n %d , sure %d\n", f, end - start);
}
