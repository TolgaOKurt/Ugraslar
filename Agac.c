#include <stdio.h>
#include <math.h>
#include <time.h>
#include <stdlib.h>

//node i�inde pointer arrayi olu�turduktan sonra bunlar aras�nda gezinmeyi beceremedi�im i�in de�i�ikli�e gidiyorum ve 2 cocuklu sisteme d�n�yorum.


typedef struct Nokta{
	int name; // nokta ismi
	struct Nokta *c1;//analar pointer listesi
	struct Nokta *c2; //cocuklar pointer listesi
}Nokta;

//WIN NAME = 99
//LOSE NAME = -1
//MAX HAMLE SAYISI BELIRLE SONSUZA KADAR LOOP G�RMES�N.




// nokta sayisi boyutuyla array yap gidi� geli� verilerini oraya koy



int main(){

	srand(time(NULL));

	int noktasayisi=2; // win + lose ile beraber minimum 3 olmali
	printf("win icin -2 isimli nokta son sirada,lose i�in -1 isimli nokta sondan bir �nceki noktadir.\n");
	printf("(kod girmek i�in 0 olmak �zere)Nokta sayisi giriniz:"); // !!!!!!!! 0 girilince kod �zerinden a�ac� olu�turan program� yaz.. !!!!!!!!!!!!!
	scanf("%d",&noktasayisi);
	
	
	if(noktasayisi=0){ // DAHA HAZIR DE��L BEL�RTEC�
		printf("WORK �N PROGRESS");
	}
	
	
	if(noktasayisi<3){
		printf("Anlamsiz!!");
		exit(-1);
	}
	
	if(noktasayisi>15){//MAX NOKTA SAY�S�
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
	
	//W�NRATE KO HESAPLIYOR
	
	Nokta* mevcut=&(liste[0]);
		
	for(int j=0;j<1000;j++){//KA� KERE BA�TAN GEZS�N.
		mevcut=&(liste[0]);
		for(int i=0;i<noktasayisi*5;i++){//KA� NOKTADAN SONRA SALSIN.
		//	printf("\n Mevcut %d",mevcut->name);
		
			//BURAS� �LERLEME �NCES�
			if(rand()%2==0) mevcut=(mevcut->c1);
			else mevcut=(mevcut->c2);
		
			//�LERLED�K W�N M� LOSE MU CHECK
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
				
			// D��ER NOKTAYA GE��LD�.
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
	//bu d�zenek sayesinde eri�im sa�lamak �ok kolay yey.
}
