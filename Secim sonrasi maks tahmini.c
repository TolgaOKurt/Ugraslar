#include <stdio.h>
#include <math.h>
#include <time.h>
#include <stdlib.h>



//Bu kod https://youtu.be/WLCwMRJBhuI?feature=shared&t=253 videosunu test etmek ve deneyimlemek icin yazilmistir.



//Hesaplarima gore ortalama %5lik hata yapiyor.

int main(){
	srand(time(NULL));//Daha rastgele bir seed olsa daha iyi olurdu.
	
	
	
	int istegebagliminimum=100;
	int istegebaglimaksimum=1000;
	int tekrar=15;
	
	
	
	
	
	int gerceksayi=rand()%(istegebaglimaksimum-istegebagliminimum)+istegebagliminimum;	
	int bilgi=0;
	int max=1;
	int tahmin=max;
	int yenigelen=0;
	float tophata=0;
	float hata=0;
	for (int klkl=0;klkl<100;klkl++){
		for(int i=0;i<tekrar;i++){
			yenigelen=(rand()%(gerceksayi-1))+1;//eger burada -1 +1 koymasaydim sonuc [0,sayi-1] arasinda olacakti. ilk -1 ile aralik [0,sayi-2] iken +1 ile aralik [1,sayi-1] arasinda oldu.
			//printf("yeni gelen:%d\n",yenigelen);
			if(yenigelen>max) max=yenigelen;
			bilgi++;
			tahmin=1+max+((max-bilgi)/bilgi);
			//printf("Gercek=%d Tahmin=%d max=%d sayi miktari=%d hata=%d oran=%%%.1f\n",gerceksayi,tahmin,max,bilgi,abs(gerceksayi-tahmin),100*(float)abs(gerceksayi-tahmin)/(float)gerceksayi);
		}
		hata=100*(float)abs(gerceksayi-tahmin)/(float)gerceksayi;
		tophata+=hata;
		printf("Gercek=%d Tahmin=%d max=%d sayi miktari=%d hata=%d oran=%%%.1f\n",gerceksayi,tahmin,max,bilgi,abs(gerceksayi-tahmin),hata);
		gerceksayi=rand()%(istegebaglimaksimum-istegebagliminimum)+istegebagliminimum;
		bilgi=0;
		max=1;
		tahmin=max;
		yenigelen=0;
		printf("\nortalama hata=%%%.1f\n",tophata/(klkl+1));
	}
	

	

}
