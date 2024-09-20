#include <stdio.h>
#include <math.h>
#include <time.h>
#include <stdlib.h>



//Bu kod https://youtu.be/WLCwMRJBhuI?feature=shared&t=253 videosunu test etmek ve deneyimlemek icin yazilmistir.


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
	for(int i=0;i<tekrar;i++){
		yenigelen=(rand()%(gerceksayi-1))+1;//eger burada -1 +1 koymasaydim sonuc [0,sayi-1] arasinda olacakti. ilk -1 ile aralik [0,sayi-2] iken +1 ile aralik [1,sayi-1] arasinda oldu.
		printf("yeni gelen:%d\n",yenigelen);
		if(yenigelen>max) max=yenigelen;
		bilgi++;
		tahmin=1+max+((max-bilgi)/bilgi);
		printf("Gercek=%d Tahmin=%d max=%d sayi miktari=%d hata=%d oran=%%%.1f\n",gerceksayi,tahmin,max,bilgi,abs(gerceksayi-tahmin),100*(float)abs(gerceksayi-tahmin)/(float)gerceksayi);
	}


}
