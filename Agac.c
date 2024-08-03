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
//MAX HAMLE SAYISI BELIRLE SONSUZA KADAR LOOP GÝRMESÝN.




// nokta sayisi boyutuyla array yap gidiþ geliþ verilerini oraya koy



int main(){

	srand(time(NULL));

	int noktasayisi=2; // win + lose ile beraber minimum 3 olmali
	printf("win icin -2 isimli nokta son sirada,lose için -1 isimli nokta sondan bir önceki noktadir.\n");
	printf("(kod girmek için 0 olmak üzere)Nokta sayisi giriniz:"); // !!!!!!!! 0 girilince kod üzerinden aðacý oluþturan programý yaz.. !!!!!!!!!!!!!
	scanf("%d",&noktasayisi);
	
	
	if(noktasayisi=0){ // DAHA HAZIR DEÐÝL BELÝRTECÝ
		printf("WORK ÝN PROGRESS");
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
	
	//BULUNMA ORANI ARRAYI YAP. URBO HESAPLA
	
	//WÝNRATE KO HESAPLIYOR
	
	Nokta* mevcut=&(liste[0]);
		
	for(int j=0;j<1000;j++){//KAÇ KERE BAÞTAN GEZSÝN.
		mevcut=&(liste[0]);
		for(int i=0;i<noktasayisi*5;i++){//KAÇ NOKTADAN SONRA SALSIN.
		//	printf("\n Mevcut %d",mevcut->name);
		
			//BURASÝ ÝLERLEME ÖNCESÝ
			if(rand()%2==0) mevcut=(mevcut->c1);
			else mevcut=(mevcut->c2);
		
			//ÝLERLEDÝK WÝN MÝ LOSE MU CHECK
			if(mevcut->name==99){
				//printf("\nWIN");
				wins++;
				break;
			}
			if(mevcut->name==-1){
			//	printf("\nLose");
				loses++;
				break;
			}
				
			// DÝÐER NOKTAYA GEÇÝLDÝ.
		}	
		
	
	}
	
	printf("%d tekrar %d win ve %d lose, yani %.3f winrate ve %.3f loserate",wins+loses,wins,loses,(float)(wins)/(wins+loses),(float)(loses)/(wins+loses));
	


	
/*	Nokta mahmut;
	Nokta tolga;
	mahmut.name=1;
	tolga.name=900;
	
	mahmut.c1=&tolga;
	mahmut.c2=&mahmut;
	tolga.c1=&mahmut;
	tolga.c2=&tolga;
	liste[0]=mahmut;
	liste[1]=tolga;
	printf("%d\n",liste[0].name);
	printf("%d\n",liste[1].name);
	
	
	
	printf("%p\n",mahmut);
	printf("%p\n",mahmut.c1);
	printf("%p\n",tolga);
	printf("%p\n",mahmut.c2);
	Nokta* mevcut;
	mevcut=&mahmut;
	printf("\n tolga ismi %d",tolga.name);
	printf("\n mahmut isim %d",mahmut.name);
	mevcut->name+=10;
	printf("\n 0mevcut isim %d",mevcut->name);
	mevcut=(mevcut->c1);
	printf("\n 1mevcut isim %d",mevcut->name);
	mevcut->name+=10;
	mevcut=(mevcut->c2);
	printf("\n 1mevcut isim %d",mevcut->name);
	mevcut=(mevcut->c1);
	printf("\n 2mevcut isim %d",mevcut->name);
	mevcut->name+=10;
	mevcut=(mevcut->c2);
	printf("\n 3mevcut isim %d",mevcut->name);





*/
	//bu düzenek sayesinde eriþim saðlamak çok kolay yey.
}
