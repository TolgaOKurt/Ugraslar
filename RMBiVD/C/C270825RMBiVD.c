//27.08.25 saat 15:15

#include<stdio.h>
#include<stdlib.h>
#include<time.h>

int main(){
	srand(time(0));
	
	printf("Slm Dnya\n");
	
	int boyut=1,topmiktari=10;
	float TopKonumlari[boyut][topmiktari];
	float Yon[boyut][topmiktari];
	
	int i=0,j=0,tur=0,maxtur=100;
	
	for(i=0;i<topmiktari;i++){ //0 dolu matrix
		for(j=0;j<boyut;j++){
			TopKonumlari[j][i]=0;
		} 
	}
	
	

	
	printf("Baslangic:  ");
	for(j=0;j<boyut;j++){ //printer
		for(i=0;i<topmiktari;i++){
			printf("%.2f  ",TopKonumlari[j][i]);
		} 
	}
	printf("\n");
	while(tur<maxtur){
		printf("Tur:%d      ",tur);
		for(i=0;i<topmiktari;i++){ //randomluk
			for(j=0;j<boyut;j++){
				Yon[j][i]=-1+2*(rand()%2);
			} 
		}
		
		for(i=0;i<topmiktari;i++){ //randomluk
			for(j=0;j<boyut;j++){
				TopKonumlari[j][i]+=Yon[j][i];
			} 
		}
		
		
		for(j=0;j<boyut;j++){ //printer2
			for(i=0;i<topmiktari;i++){
				printf("%.2f  ",TopKonumlari[j][i]);
			} 
			printf("\n");
		}
		
		tur++;
	}	
	
	
	
	
	
}
