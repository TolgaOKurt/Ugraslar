//27.08.25 saat 15:15

#include<stdio.h>
#include<stdlib.h>
#include<time.h>
#include<math.h>

int main(){
	srand(time(0));
	
	//printf("Slm Dnya\n");
	
	int boyut=4,topmiktari=100,maxtur=100,dagilimdegiskeni=1;//deðiþtirilebilirler
	
	
	//int dagilim[maxtur+1][dagilimdegiskeni];//1 boyut stat tracker
	int i=0,j=0;//döngü deðiþkenleri
	/*if(boyut==1){//boyut 1 ise
		for(i=0;i<=maxtur;i++){
			for(j=0;j<dagilimdegiskeni;j++){
				dagilim[i][j]=0;
			}	
		}
	}*/
	
	int merkezsayaci[topmiktari];//merkezden geçme sayilarini tutan array
	
	float TopKonumlari[boyut][topmiktari];// toplarýn konumlarini tutan array
	float Yon[boyut][topmiktari];//toplarýn sýradaki hareketlerini tutan array
	float eskilertoplami=0,frand=0;
	int uygunluksayaci=0;
	//basit stats
	float ortuzaklik=0,ortkonum[boyut],topkonum[boyut],topuzaklik=0,merkezedonmeorani=0,merkezdengecmesayisi=0;
	int tur=0;
	
	
	for(i=0;i<boyut;i++){ //0 dolu matrix
		ortkonum[i]=0;//0 dolu
		topkonum[i]=0;

	}

	
	for(i=0;i<topmiktari;i++){ //0 dolu matrix
		merkezsayaci[i]=0;//0 dolu
		for(j=0;j<boyut;j++){
			TopKonumlari[j][i]=0;
			Yon[j][i]=0;
		} 
	}
	
	

	
	//printf("Baslangic: ");
	
	
	
	/*
	for(i=0;i<topmiktari;i++){ //printer
		printf("(");
		for(j=0;j<boyut;j++){
			if(j!=0)printf(",");
			printf("%6.2f",TopKonumlari[j][i]);
		} 
		printf(")  ");
	}*/
	
	
	
	
	
	
	
	
	
	/*
	
	if(boyut==1){//boyut 1 ise ayar
		for(i=0;i<topmiktari;i++){ //ilk dagilim
			for(j=0;j<boyut;j++){
				
				if(abs(TopKonumlari[j][i])<=(dagilimdegiskeni-1)/2){					
					dagilim[0][(int)TopKonumlari[j][i]+(dagilimdegiskeni-1)/2]+=1;
				
				}
			}	
		}
	}	
	*/
	printf("\n");
	while(tur<maxtur){
		//printf("Tur:%d      ",tur);
		
		
		for(i=0;i<topmiktari;i++){ //random yön
			eskilertoplami=0;
			//printf("\n 0 olmali %f\n",eskilertoplami);
			
			
			for(j=0;j<boyut;j++){
				
				frand=rand();
				frand=2*frand/RAND_MAX-1;
				//printf("\n random sayi %f\n",frand);
				
				if(j<boyut-1){
					if(1-eskilertoplami<0.001){
						Yon[j][i]=0;
					}
					else{
						Yon[j][i]=frand*sqrt(1-eskilertoplami);
					}

					
					if (isfinite(Yon[j][i])==0) {
						printf("y yoooo %f=%f*sqrt(1-%f)\n",Yon[j][i],frand,eskilertoplami);
						exit(4);
					}
					
				}
				else{
					Yon[j][i]=(-1+2*(rand()%2))*(sqrt(1-eskilertoplami));
					
					if (isfinite(Yon[j][i])==0) {
						printf("y yoooo %f\n",Yon[j][i]);
						exit(5);
					}
				}
				
				//printf("\n yon %f\n",Yon[j][i]);
				eskilertoplami+=Yon[j][i]*Yon[j][i];
				//printf("\n toplam %f\n",eskilertoplami);
				
			}
			
			//printf("1 olmali ki %.5f",eskilertoplami);
		}
		// +-x=sqrt(1)
		// +-y=sqrt(1-xx)
		// +-z=sqrt(1-[xx+yy])
		// +-w=sqrt(1-[xx+yy+zz]
		
		
		
		
		
		eskilertoplami=0;
		for(i=0;i<topmiktari;i++){ //basit yeni yol
			
			for(j=0;j<boyut;j++){
				


				
				TopKonumlari[j][i]+=Yon[j][i];
				topuzaklik+=sqrt(TopKonumlari[j][i]*TopKonumlari[j][i]);
				//printf("%f ve j= %d\n",TopKonumlari[j][i],j);
				
				
				topkonum[j]+=TopKonumlari[j][i];

				
				/*
				if(boyut==1){
					if(abs(TopKonumlari[j][i])<=(dagilimdegiskeni-1)/2){					
						dagilim[tur+1][(int)TopKonumlari[j][i]+(dagilimdegiskeni-1)/2]+=1;
					}
				}*/
				eskilertoplami+=TopKonumlari[j][i]*TopKonumlari[j][i];
				
				
				
				
			} 
			eskilertoplami=sqrt(eskilertoplami);
			if(eskilertoplami<0.5){
				merkezsayaci[i]+=1;
				//printf("\n MERKEZ %f \n",eskilertoplami);
			} 
			
			
			eskilertoplami=0;
		}
		
		/*
		for(i=0;i<topmiktari;i++){ //printer2
			printf("(");
			for(j=0;j<boyut;j++){
				if(j!=0)printf(",");
				printf("%6.2f",TopKonumlari[j][i]);
				
			} 
			printf(")  ");
		}
		printf("\n");*/
		tur++;
	}	
	
	
	ortuzaklik=topuzaklik/(topmiktari*maxtur);//stats
	printf("\nOK=(");
	for(i=0;i<boyut;i++){
		if(i!=0)printf(",");
		//printf("\n test tk %6.2f   \n",topkonum[i]);
		ortkonum[i]=topkonum[i]/(topmiktari*maxtur);
		printf("%6.2f",ortkonum[i]);
	}
	printf(") ");
	
	printf("OU=%f",ortuzaklik);
	
	for(i=0;i<topmiktari;i++){
		merkezdengecmesayisi+=merkezsayaci[i];		
	}
	for(i=0;i<topmiktari;i++){
		if(merkezsayaci[i]>=1) merkezedonmeorani++;		
	}
	merkezdengecmesayisi/=topmiktari;
	merkezedonmeorani/=topmiktari;
	
	
	printf(" TBMGS=%.4f EA1KGO=%.4f",merkezdengecmesayisi,merkezedonmeorani);
	
	/*
	if(boyut==1){//daðýlým görseli
		printf("\n");
		for(i=0;i<=maxtur;i++){
			printf("\n");
			for(j=0;j<dagilimdegiskeni;j++){
		
				printf("%2.d",dagilim[i][j]);
	
			}	
		}			
	}*/
	
	
/*	if(boyut==1){
		printf("\n\nDesen\n");
		for(i=0;i<=maxtur;i++){
			printf("\n");
			for(j=0;j<dagilimdegiskeni-6;j++){
				if(dagilim[i][j]!=0)
					printf("#");
				else printf("-");
			}	
		}			
	}
*/	
	
	
	
	
	
	printf("\n\nPROGRAM BITTI!");	
}
