//27.08.25 saat 15:15

#include<stdio.h>
#include<stdlib.h>
#include<time.h>
#include<math.h>

int main(){
	srand(time(0));
	
	//printf("Slm Dnya\n");
	
	int boyut=1,topmiktari=10,maxtur=100,dagilimdegiskeni=71;
	int dagilim[maxtur+1][dagilimdegiskeni];
	int i=0,j=0;
	if(boyut==1){
		for(i=0;i<=maxtur;i++){
			for(j=0;j<dagilimdegiskeni;j++){
				dagilim[i][j]=0;
			}	
		}
	}
	
	int merkezsayaci[topmiktari];
	
	float TopKonumlari[boyut][topmiktari];
	float Yon[boyut][topmiktari];
	
	//basit stat
	float ortuzaklik=0,ortkonum=0,topkonum=0,topuzaklik=0,merkezedonmeorani=0,merkezdengecmesayisi;
	int tur=0;
	
	
	

	
	for(i=0;i<topmiktari;i++){ //0 dolu matrix
		merkezsayaci[i]=0;//0 dolu
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
	if(boyut==1){
		for(i=0;i<topmiktari;i++){ //ilk dagilim
			for(j=0;j<boyut;j++){
				
				if(abs(TopKonumlari[j][i])<=(dagilimdegiskeni-1)/2){					
					dagilim[0][(int)TopKonumlari[j][i]+(dagilimdegiskeni-1)/2]+=1;
				
				}
			}	
		}
	}	
	
	printf("\n");
	while(tur<maxtur){
		printf("Tur:%d      ",tur);
		for(i=0;i<topmiktari;i++){ //random yon
			for(j=0;j<boyut;j++){
				Yon[j][i]=-1+2*(rand()%2);
			} 
		}
		
		for(i=0;i<topmiktari;i++){ //basit yeni yol
			for(j=0;j<boyut;j++){
			
			
			
			

			
			
			
			
				TopKonumlari[j][i]+=Yon[j][i];
				if(abs(TopKonumlari[j][i])<0.5){
					//printf("\n%d Degisti",merkezedondumu[i]);
					merkezsayaci[i]+=1;
					//printf(" %d oldu cunku %.2f oldu %d numara %\n",merkezedondumu[i],TopKonumlari[j][i],i+1);
					
				}
				topuzaklik+=abs(TopKonumlari[j][i]);
				topkonum+=TopKonumlari[j][i];
				
				if(boyut==1){
					if(abs(TopKonumlari[j][i])<=(dagilimdegiskeni-1)/2){					
						dagilim[tur+1][(int)TopKonumlari[j][i]+(dagilimdegiskeni-1)/2]+=1;
					}
				}
				
				
				
				
			} 
		}
		
		
		for(i=0;i<topmiktari;i++){ //printer2
			printf("(");
			for(j=0;j<boyut;j++){
				if(j!=0)printf(",");
				printf("%.2f",TopKonumlari[j][i]);
				
			} 
			printf(")  ");
		}
		printf("\n");
		tur++;
	}	
	
	
	ortuzaklik=topuzaklik/(topmiktari*maxtur);
	ortkonum=topkonum/(topmiktari*maxtur);
	printf("\nortkonum= %f ve ortuzaklik=%f\n",ortkonum,ortuzaklik);
	
	for(i=0;i<topmiktari;i++){
		merkezdengecmesayisi+=merkezsayaci[i];		
	}
	for(i=0;i<topmiktari;i++){
		if(merkezsayaci[i]>=1) merkezedonmeorani++;		
	}
	merkezdengecmesayisi/=topmiktari;
	merkezedonmeorani/=topmiktari;
	printf("Merkezden gecme sayisi %.4f donmeorani %.4f",merkezdengecmesayisi,merkezedonmeorani);
	if(boyut==1){
		printf("\n");
		for(i=0;i<=maxtur;i++){
			printf("\n");
			for(j=0;j<dagilimdegiskeni;j++){
		
				printf(" %d",dagilim[i][j]);
	
			}	
		}			
	}
	
	
	if(boyut==1){
		printf("\n\nDesen basliyor\n");
		for(i=0;i<=maxtur;i++){
			printf("\n");
			for(j=0;j<dagilimdegiskeni-6;j++){
				if(dagilim[i][j]!=0)
					printf("#");
				else printf("-");
			}	
		}			
	}
	
	
	
	
	
	
	printf("\n\nPROGRAM BITTI!");	
}
