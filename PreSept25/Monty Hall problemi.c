#include <stdio.h>
#include <math.h>
#include <time.h>
#include <stdlib.h>



int main(){
	srand(time(NULL));
	int odul=0;
	int tercih=0;
	int ac=0;
	int tamam =0;
	float say1=0,say2=0,say3=0,say4=0;
	float k=100000;//100k da 0,33??? oluyor
	for(int i=0;i<(int)k;i++){
		odul=(rand()%3)+1;
		tercih=(rand()%3)+1;

		//bilerek acma durumu
		tamam=0;
		for (;tamam!=1;){
			ac=(rand()%3)+1;
			if (ac!=tercih && ac!=odul){ //tercih acilamaz ve odul acýlamaz
				if(tercih==odul){//tercih odul ise degistirmeme kazandýrýr
					say2++;
				}
				if(tercih!=odul){
					say3++;
				}
				tamam=1;
			}
		
		}
		//bilmeden acanlar
		tamam=0;
		for (;tamam!=1;){
			ac=(rand()%3)+1;
			if (ac!=tercih){//tercih acilamaz
				if(ac==odul){//odul actýysa kaybettin
					say1++;
				}
				if(ac!=odul && tercih!=odul){ //odul acýlmadý degistirirsen odulu alcaksýn
					say4++;
				}
				tamam=1;
			}
		
		}
		
		
		
	}

	printf(" bilerek acil+degistirme kazan=%f \n bilerek acil+degistir kazan=%f \n rastgele acil+kesin kaybettin=%f \n rastgele acil+odul acilmadi+degistir kazan=%f ",say2/k,say3/k,say1/k,say4/(k-say1));
	
	printf("\n 3 kapi. birini sec. sunucu bir kapiyi acsin.\n acilan kapi odullu degildi. kaybetmedin devam et. \n karar vermen lazim kapimi degistireyim mi? Ihtimalleri hesaplamaya basla. Sunucuya sor. \n Actigin kapinin bos oldugunu biliyor muydun? \n Cevap evetse %%66 kazanma orani icin kapini degistir.");	
	printf("\n Cevap hayirsa naparsan yap kazanma oranin %%50\n Kendi bilgilerimizle sunucuya soru sormadan ihtimalin %%66 oldugunu bilemezdik.\n acilan kapinin arkasinin bos olmasi bize ekstra bir bilgi vermedi.");
	printf("\n Ama sunucunun sonucu ayni olsa dahi yaptigi aksiyonu yaparken gelecek hakkinda ne bildigi gercegi bizim ihtimalimizi degistirdi.\n");
	printf("Yani sonuclar ayni olsa dahi neyi nasil neden yapitigin onemlidir!");
}
