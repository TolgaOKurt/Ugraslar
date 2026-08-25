#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// Her Array -1 ile bitsin.
// Her sonu� pozitif olmali.

// YAPILACAKLAR; MATEMAT�KSEL OLARAK OLMAYACAK D�Z�LER� �NCEDEN BEL�RLEY�P PAS
// GE�MES�N� SA�LA. B�YLEL�KLE PROGRAMIN HIZI ARTSIN, DAHA NOKTA ATI�I SONU�LAR
// BULSUN..

void QS(int arr[], int low, int high) { // internetten �al�nm�� quicksort
  if (low < high) {
    int pivot =
        arr[(low + high) / 2]; // Selecting the middle element as the pivot
    int i = low;
    int j = high;
    int temp;

    while (i <= j) {
      while (arr[i] < pivot)
        i++; // Moving elements smaller than pivot to the left
      while (arr[j] > pivot)
        j--; // Moving elements greater than pivot to the right

      if (i <= j) {
        temp = arr[i]; // Swapping elements
        arr[i] = arr[j];
        arr[j] = temp;
        i++;
        j--;
      }
    }

    // Recursively sort the two partitions
    if (low < j)
      QS(arr, low, j);
    if (i < high)
      QS(arr, i, high);
  }
  return;
}

void sifir(int *diz, int s) { // dizinin ilk "s" birimini 0 yap�yor.
  int i = 0;
  for (i = 0; i < s; i++) {
    diz[i] = 0;
  }
}

void upperart(int *diz, int si) {
  // p=si+1;
  int i = 0;
  for (i = 0; i <= si; i++) {
    diz[i] = (si + 1) * (1 + i) + 1;
  }
  return;
}

void printarr(int *diz, int si) { // Array printliyor
  int i = 0;
  for (i = 0; i <= si; i++) {
    printf("%d -> ", i);
    printf("%d\n", diz[i]);
  }
  printf("\n");
  return;
}

// burada sifir fonksiyonun ikizi vard�. gereksizlikten silindi. RIP.

// burada eski ve daha yava� �al��an max bulma b�l�m� vard�(t�m sonu�lar� bulup
// sonra i�inde ar�yordu ama g�r�n��e g�re ararken hesaplamak daha hizli.). RIP
// arrtomax.

int yenicalc(int *diz, int si) { // Max ka�a gitti�ini buluyor
  int max = 0;
  int kontroller = 1;
  int buturmu = 0; // eger bu tur sayi bulamad�ysa bitmi�tir.
  int i = 0, j = 0;
  for (; 1;) {

    buturmu = 0;

    for (i = 0; i <= si; i++) { // sayi var m� bak��
      if (diz[i] == max + 1) {
        max++;
        kontroller = 1;
        buturmu = 1;
        break;
      }
      kontroller = 0;
    }
    for (i = 0; i <= si; i++) { // toplamda veya farkda sayi var m� bak��
      for (j = 0; j <= si; j++) { // toplamda veya farkda sayi var m� bak��
        if (diz[i] + diz[j] == max + 1) {
          max++;
          kontroller = 1;
          buturmu = 1;
          break;
        } else if (abs(diz[i] - diz[j]) == max + 1) {
          max++;
          kontroller = 1;
          buturmu = 1;
          break;
        }
        kontroller = 0;
      }
    }
    if (kontroller == 0 && buturmu == 0)
      return max;
  }

  return max;
}

void copy(int *ilk, int *ikinc, int si) { // array kopyalama.
  int i = 0;
  for (i = 0; i <= si; i++) {
    ikinc[i] = ilk[i];
  }
}

void lscopy(int *ilk, int *ikinc, int l,
            int si) { // array da bi yere kadar kopyalan�r yani
  int i = 0;
  for (i = 0; i < l; i++) {
    ikinc[si - i] = ilk[si - i];
  }
}

void skalerart(int *diz, int s, int si) { // diziyi sabit say� ile artt�r�r.
  int i = 0;
  for (i = 0; i <= si; i++) {
    diz[i] = diz[i] + s;
  }
}

void lskalerart(int *diz, int s, int d,
                int si) { // array da bi yere kadar artar yani
  int i = 0;

  for (i = 0; i < d; i++) {
    diz[si - i] += s;
  }
}

void siradoldur(int *diz) { // 1,2,3,4 �eklinde dizer.
  int i = 0;
  for (i = 0; diz[i] != -1; i++) {
    diz[i] = i + 1;
  }
}

int aynimi(int *bir, int *iki, int si) { // 2 array ayn� m�? ayn� ise 1 d�ner.
  int i = 0;
  for (i = 0; i <= si; i++) {
    if (bir[i] - iki[i] != 0) {
      return -1;
    }
  }
  return 1;
}

int artir(int *diz, int *low, int *up, int artan,
          int si) { // bir sonraki sayi gelsin. gelirse 0 gelmezse 5 d�ner.
  int i = 0;

  if (diz[artan] < up[artan]) {
    diz[artan]++;
    return 0;
  }

  else if (aynimi(up, diz, si) == 1) {
    return 5;
  }

  if (diz[artan] == up[artan]) {
    lscopy(low, diz, si - artan + 1, si);
    lskalerart(diz, diz[artan - 1] - low[artan - 1] + 1, si - artan + 1, si);
    return artir(diz, low, up, artan - 1, si);
  }
}

// Burada arrtomax(RIP) kullanan is b�l�m� vardi. arrtomax gidince is de
// kederden gitti. RIP is.
void gidisat(int *diz, int si) { // Array printliyor
  int i = 0;
  printf("\n");
  for (i = 0; i <= si; i++) {
    printf("%d,", diz[i]);
  }
  printf("-1");
  printf("\n");
  return;
}

void yis(int p) { // ana is b�l�m�
  int si = p - 1;
  time_t start, end; // kronometre ayar�
  int i = 0;
  int upper[p + 1];
  upper[p] = -1;
  sifir(upper, p);
  int lower[p + 1];
  lower[p] = -1;
  sifir(lower, p);
  int teomax = p * p + p;
  int max = -5, maxtest2 = -99;
  int temp = -6;
  int test[p + 1];
  int maxarr[p + 1];
  maxarr[p] = -1;
  sifir(maxarr, p);
  test[p] = -1;
  sifir(test, p);
  siradoldur(test);
  max = yenicalc(test, si);
  copy(test, maxarr, si);
  copy(test, lower, si);
  // Upper reformu, eski upper hesaplama y�ntemine veda.
  upperart(upper,
           si); // �LK SAY�N�N P'DEN b�y�k oldu�u g�r�lmemi�tir. �K�NC� SAY�N�N
                // 2*P DEN B�Y�K OLDU�U G�R�LMEM��T�R. //Ayni gidisat hespine
                // uygulanmistir.  !!Kesinlik yok ancak potansiyel var.

  if (p == 3)
    max = 9; // �nceden hesaplanm�� ve maksimumun minimum ka� oldu�unu bilinen
             // aramalarda h�z i�in. max=10 oldu�undan aramaya 9 ile ba�la
             // diyoruz.
  if (p == 4)
    max = 15;
  if (p == 5) {
    skalerart(
        lower, 3,
        si); // aramay� limitlendiriyoruz ��nk� en k���k ba�l� sonucun zaten 4
             // ile ba�lad���n� biliyoruz. kodda yap�lan de�i�iklikler i�leri
             // bozdu mu test ederken bildi�imiz sonu�lara bakar�z.
    skalerart(test, 3, si);
    max = 23;
  }
  if (p == 6) {
    skalerart(lower, 3, si);
    skalerart(test, 3, si);
    max = 31;
  }
  if (p == 7) {
    skalerart(lower, 1, si);
    skalerart(test, 1, si);
    max = 39;
  }
  if (p == 8) {
    int bu[9] = {
        2,  4,  6,  7, 25,
        26, 40, 41, -1}; // ilk bulunan sonu� 2,4,6,7,25,26,41,42'dir. bir di�er
                         // sonu� muhtemelen 6,12,18,21,22,23,25,26'dir. maximum
                         // da 9 ile ba�layan sonu� var ise onu bulabililir ve
                         // sonra biter.
    copy(bu, test,
         si); // Yani bu uzun arama s�recinde 2 sonu� belli. e�er max=52 ise ki
              // deneysel sonu�lar hep �ift sayidir, muhtemelen artmaz daha.
    max = 51;
    printf("Baslangic:\n");
    printarr(test, si);
  }

  if (p == 9)
    max =
        60; // max de�er p artt�k�a artar. o y�zden p>k iken sonuc(p)>sonuc(k),
            // buradan sonra deneysel sonu�lar� bulmak �ok uzun s�rer, ama
            // deneysel sonu�lar�n hangi sayilardan fazla oldu�u bulmak kolay.
  if (p == 10)
    max = 68;
  if (p == 11)
    max = 78;
  if (p == 12)
    max = 88;
  if (p == 13)
    max = 98;
  if (p > 13)
    max = (p * p + p) / 2;

  /*
  //BETTER LOWER KISMI !!! WIP!!!
  printarr(lower,si);
  printarr(upper,si);
  printf("\n h \n");
  */

  start = clock(); // zaman ba�lad�.
  int rkrkrkrk =
      8; // burasi bulunmayan sonu�lar�n bulunmasi i�in bilgisayar �al���rken
         // daha �ok bilgilendirilme yapilmasi i�in koyulmu� bir de�i�ikliktir.
  if (p >= 8)
    rkrkrkrk = 7;
  unsigned int pas = -1, say2 = 0, hesap = 0;
  int p1 = 0, p2 = 0, p0 = 0;
  int x = 0;
  int sif = 0, sif2 = 0, sif3 = 0;
  for (i = 0; 1; i++) {
    say2++;
    if (abs(say2) % (int)(pow(10, rkrkrkrk)) * p == 0) {
      printf("\n\ntoplam=%u, pass=%u, hesaplanan=%u, max=%d\n\n", say2, pas,
             hesap, max); // gidi�at s�yleyici
      if (p >= 8)
        gidisat(test, si); // gidisat
      printf("\n");
    }
    x = artir(test, lower, upper, p - 1, si);
    if (x == 5) { // bitti ise
      break;
    }

    if (test[p - 1] <
        ((max / 2) -
         1)) { // pass0   deneysel sonu�/max(son) sayi k���k e�ittir 2'ye. e�er
               // deneysel sonu� > 2*max ise. deneysel sonu� nas�l elde
               // edilecek? bir dizide en y�ksek sonu� max+max=2*maxtir.
      pas++;
      p0++;
      continue;
    }
    if (test[(p - 1) / 2] >
        max / 2) { // pass1 dizideki ortanca sayi, tek sayilarda a�a��
                   // yuvarlanacak �ekilde, sonucun yarisindan k���k olmali
                   // incelendi�inde b�yle bir sonuca var�lm��t�r.!! Kesinlik
                   // yok ancak potansiyel var.
      pas++;
      p1++;
      continue; // hey 25.08.2026 dan geliyorum burda bir mantıksal hata yok mu?
                // max durmadan artan bir değişken bu durumda ortadaki sayı
                // küçük olmalı
      // dedik ama sayı arttıkça kapladığı alan artar bu bir hata gibi duruyor.
      // çünkü muhtemel sonuçları engelliyor olaiblir.
    }
    if (test[1] >=
        test[0] *
            p) { // pass2 ikinci sayinin en k���k sayinin p katindan fazla
                 // oldu�u g�r�lmemi�tir. !!Kesinlik yok ancak potansiyel var.
      pas++;
      p2++;
      continue;
    }

    // Burada pass3 yat�yor RIP

    // Burada pass4 yat�yor RIP

    hesap++;
    if (hesap == 0) {
      sif3++;
      printf("\n Hesap sayaci %d kere sifirlandi \n", sif3);
    }
    temp = yenicalc(test, si);
    if (temp == max && temp > teomax / 2) {
      printf("\n%d icin alternatif:\n", temp);
      printarr(test, si);
    }
    if (temp > max) {
      max = temp;
      end = clock();
      printf("\n%d bulundu(sure=%d):\n", temp, end - start);
      printarr(test, si);
      copy(test, maxarr, si);
    }
  }

  printf("\nBitti toplam=%d, pass=%d, max=%d, hesaplanan=%d\n", say2, pas, max,
         hesap);
  //	printarr(maxarr); //istersen en son bi daha s�ylesin sana
  printf("\np0=%d p1=%d p2=%d \n", p0, p1, p2);
  return;
}

int main() {
  printf(" benim kod calismasi\n");
  printf("Program basladi. Birazdan ilerleme bilgilendirmesi yapilir.\n");
  int p = 8;
  yis(p);

  // AMA�: P M�KTARINDA SAY�, B�RB�RLER�YLE TOPLAMA �IKARMA VEYA KEND�S� VEYA
  // KEND�S�*2 OLMAK �ZERE, SIRALI B�R �EK�LDE MAKS�MUM HANG� SAYIYA KADAR T�M
  // SAYILARI ELDE EDEB�L�R�Z.
  //  �RNEK 2 VE 3 �LE 1 2 3 4 5 6 ELDE ED�LEB�L�R
  //  3-2 2 3 2+2 3+2 3+3

  // p=say� miktari olmak �zere maksimum p*p+p sonucu elde edilebilir. buna
  // teorik sonu� diyorum. deneysel sonu� maksimum sayidan b�y�k-e�it ve teorik
  // sonu�tan k���k olacak. teorik sonucun max say�n�n 3 kat�ndan fazla oldu�u
  // g�r�lmedi ama bu oran giderek art�yor p=7de bu oran 2,8e kadar ��kabiliyor.

  printf("\nProgram calismayi durdurdu. Kapatabilirsiniz.\n");
  //	scanf("%d");*/
}
