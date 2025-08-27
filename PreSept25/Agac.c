#include <stdio.h>
#include <math.h>
#include <time.h>
#include <stdlib.h>

//node içinde pointer arrayi oluþturduktan sonra bunlar arasýnda gezinmeyi beceremediðim için deðiþikliðe gidiyorum ve 2 cocuklu sisteme dönüyorum.


typedef struct Nokta{
	int name; // nokta ismi
	struct Nokta *c1;//analar pointer listesi
	struct Nokta *c2; //cocuklar pointer listesi
}Nokta;

//WIN NAME = 99
//LOSE NAME = -1




                                                        // nokta sayisi boyutuyla array yap gidiþ geliþ verilerini oraya koy



int main(){

	srand(time(NULL));

	int noktasayisi=2; // win + lose ile beraber minimum 3 olmali
	printf("win icin 99 isimli nokta son sirada,lose için -1 isimli nokta sondan bir onceki noktadir.\n");
	//printf("(kod girmek icin 0 olmak uzere)");                                                                   // !!!!!!!! 0 girilince kod üzerinden aðacý oluþturan programý yaz.. !!!!!!!!!!!!!
	printf("Nokta sayisi giriniz:"); 
	scanf("%d",&noktasayisi);
	
	
	if(noktasayisi==0){ // DAHA HAZIR DEÐÝL BELÝRTECÝ                                                           !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
		printf("WORK IN PROGRESS\n");
		printf("Kod:");
		exit(-1);
		
	}

	
	if(noktasayisi<3){
		printf("Anlamsiz!!");
		exit(-1);
	}
	
	if(noktasayisi>15){//MAX NOKTA SAYÝSÝ
		printf("YAPMA!!");
		exit(-1);
	}
	
	Nokta liste[noktasayisi];
	int kod[noktasayisi*2-3];
	kod[noktasayisi*2-4]=-999;
	
	for(int i=0;i<noktasayisi-2;i++){
		liste[i].name=i;
	}
	
	liste[noktasayisi-1].name=99;//win
	liste[noktasayisi-2].name=-1;//lose
	
	for(int i=0;i<noktasayisi;i++){
		printf("\n%d isimli %d sirali\n",liste[i].name,i);
	}
	
	int cocukn=0;
	printf("Liste numarasi giriniz! -2 ismi win -1 ismi lose noktasidir\n");
	
	for(int i=0;i<noktasayisi-2;i++){
		printf("%d numarali noktanin ilk cocugu:",liste[i].name);
		scanf("%d",&cocukn);
		kod[2*i]=cocukn;
		liste[i].c1=&(liste[cocukn]);
		cocukn=0;
		printf("%d numarali noktanin ikinci cocugu:",liste[i].name);
		scanf("%d",&cocukn);
		kod[2*i+1]=cocukn;
		liste[i].c2=&(liste[cocukn]);
		cocukn=0;
	}
		
	printf("\nKod:");
	
	for(int i=0;kod[i]!=-999;i++){
		printf("%d",kod[i]);
	}
	
	printf("\n");
	
	


	int wins=0,loses=0;
	//EKLENECEKLER: URBO ÝBO BÝK SÝBO STBÝK S2BO 2BO        BELKÝ KO/KOA
	// ÝBO * STBÝK = URBO
	// ÝBO = URB*(1-BÝK)
	// SnBO = ÝBO*(1-BÝK)*BÝK^n
	//baslangictan bagimsiz olarak KO ve BÝK aðaçla alakalidir baðýmsýzdir. URB ve ÝBO ise baþlangýç noktasina baðlýdýr.
	
	
	
	
	//BULUNMA ORANI ARRAYI YAP. URBO HESAPLA            !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	
	
	Nokta* mevcut=&(liste[0]);
	int sinir=noktasayisi*6; //5de iyi galiba
	
	for(int j=0;j<pow(10,5);j++){//KAÇ KERE BAÞTAN GEZSÝN.
	
	
		mevcut=&(liste[0]); //BURAYI DEÐÝÞTÝRÝNCE BAÞLANGIÇ NOKTASÝNÝ DEÐÝÞTÝRÝRSÝN HER NOKTANIN WRSÝNÝ HESAPLAMASI ÝÇÝN BURAYI DEÐÝÞKEN AYARLA!!       !!!!!!!!!!!!!!!!!!!!!!!!
		
		for(int i=0;i<sinir;i++){//KAÇ NOKTADAN SONRA SALSIN.
		
			if(i==sinir-4) printf("\n SINIR ARTMALI \n");
			
			//buraya urbo hesabi ayarlican
			
			
			if(rand()%2==0) mevcut=(mevcut->c1);//ilerleme
			else mevcut=(mevcut->c2);//ilerleme
		

			if(mevcut->name==99){//kazanma noktasi kontrolu

				wins++;
				break;
			}
			if(mevcut->name==-1){//kayip noktasi kontrolu

				loses++;
				break;
			}
				
		}	
		
	
	}
	
	printf("%d tekrar %d win ve %d lose, yani %.3f winrate ve %.3f loserate",wins+loses,wins,loses,(float)(wins)/(wins+loses),(float)(loses)/(wins+loses));
	

}
