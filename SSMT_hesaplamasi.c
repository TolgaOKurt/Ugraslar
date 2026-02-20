/*
 * SSMT Hesaplamasi - Alman Tank Problemi
 * (German Tank Problem Simulation)
 *
 * Varsayim: Tanklar 1'den N'e kadar ardisik seri numarasiyla numaralandirilmistir.
 * N (gercek toplam uretim adedi) bilinmemektedir.
 *
 * Tahmin Yontemleri:
 *   MVUE (Minimum Variance Unbiased Estimator) : m + (m / k) - 1
 *   2 * Ortalama Yontemi                       : 2 * (toplam / k)
 *   Bilinen Maks Deger                         : k adet gozlemde gorulen en buyuk sayi
 */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

/* ---------- Ayarlanabilir Parametreler ---------- */
#define MIN_SERI    1       /* Seri numarasi alt siniri   */
#define MAX_SERI    1000    /* Gercek maks uretim (N)     */
#define MAX_GOZLEM  20      /* Kac gozlem yapilacagi (k)  */
/* ------------------------------------------------ */

/* [MIN, MAX] araliginda rastgele tam sayi uret */
static int rastgele(int min, int max)
{
    return min + rand() % (max - min + 1);
}

int main(void)
{
    srand((unsigned int)time(NULL));

    int  gercek_N   = MAX_SERI;   /* Bilinmeyen, ama simulasyonda kullanilan gercek deger */
    int  gorulen[MAX_GOZLEM];     /* Her adimda gorulen seri numaralari                   */
    int  maks_gorulen = 0;        /* Simdiye kadar gorulen en buyuk sayi (m)              */
    long toplam       = 0;        /* Gorulen degerlerin toplami (ortalama icin)            */

    printf("-------------------------------------------------------\n");
    printf("  Alman Tank Problemi - SSMT Simulasyonu\n");
    printf("  Upper(N)=%d  k_max=%d\n", gercek_N, MAX_GOZLEM);
    printf("-------------------------------------------------------\n\n");

    /* Sutun basliklari */
    printf("%-4s %-6s %-7s %-7s %-7s %-7s  |  %-8s %-8s %-8s  Siralama\n",
           "k", "m", "Upper", "MVUE", "2xO", "BM",
           "EoMVUE", "Eo2xO", "EoBM");

    printf("%-4s %-6s %-7s %-7s %-7s %-7s  |  %-8s %-8s %-8s  --------\n",
           "--", "------", "-----", "-----", "-----", "-----",
           "------", "------", "------");

    for (int k = 1; k <= MAX_GOZLEM; k++)
    {
        /* Yeni bir gozlem: [MIN_SERI, gercek_N] arasindan rastgele sayi */
        int yeni = rastgele(MIN_SERI, gercek_N);
        gorulen[k - 1] = yeni;

        /* Maksimum guncelle */
        if (yeni > maks_gorulen)
            maks_gorulen = yeni;

        /* Toplam guncelle */
        toplam += yeni;

        /* --- Tahminler --- */
        int m = maks_gorulen;

        /* MVUE: m + floor(m / k) - 1  */
        int mvue = m + (m / k) - 1;

        /* 2 * Ortalama */
        double ort       = (double)toplam / k;
        int    iki_ort   = (int)(2.0 * ort);

        /* Bilinen maks = gorulen en buyuk */
        int bilinen_maks = m;

        /* Hatalar: |gercek_N - tahmin| / gercek_N  (yuzde olarak) */
        double hata_mvue       = fabs((double)(gercek_N - mvue))        / gercek_N * 100.0;
        double hata_iki_ort    = fabs((double)(gercek_N - iki_ort))     / gercek_N * 100.0;
        double hata_bilin_maks = fabs((double)(gercek_N - bilinen_maks))/ gercek_N * 100.0;

        /* Hata siralamasi: en dusukten en yuksege */
        /* 3 degeri bubble-sort ile sirala (isim tutucu) */
        typedef struct { double val; const char *ad; } HErr;
        HErr h[3] = {
            {hata_mvue,       "EoMVUE"},
            {hata_iki_ort,    "Eo2xO" },
            {hata_bilin_maks, "EoBM"  }
        };
        /* 3 elemanli simple sort */
        for (int i = 0; i < 2; i++)
            for (int j = i+1; j < 3; j++)
                if (h[j].val < h[i].val) { HErr tmp=h[i]; h[i]=h[j]; h[j]=tmp; }

        char siralama[32];
        snprintf(siralama, sizeof(siralama), "%s<%s<%s",
                 h[0].ad, h[1].ad, h[2].ad);

        /* Cikti */
        printf("%-4d %-6d %-7d %-7d %-7d %-7d  |  %6.1f%%  %6.1f%%  %6.1f%%  %s\n",
               k, yeni, gercek_N,
               mvue, iki_ort, bilinen_maks,
               hata_mvue, hata_iki_ort, hata_bilin_maks,
               siralama);
    }

    printf("\n-------------------------------------------------------\n");
    printf("  Tamamlandi: k=%d | maks_m=%d | Upper=%d\n",
           MAX_GOZLEM, maks_gorulen, gercek_N);
    printf("-------------------------------------------------------\n");

    return 0;
}
