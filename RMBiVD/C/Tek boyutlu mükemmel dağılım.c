#include<stdio.h>
#include<stdlib.h>
#include<math.h>



int main(){
	int tektarafgenislik=12;
	int us=4;
	int eksena[tektarafgenislik*2+1];//eksen a ve eksen b her sesferinde diğerini sonraki adıma taşıyacak.
	int eksenb[tektarafgenislik*2+1];
	int i=0;
	int j=0;
	
	
	
	for(i=0;i<=tektarafgenislik*2;i++){//sıfırlamak
		eksena[i]=0;
		eksenb[i]=0;
	}
	
	
	
	eksena[tektarafgenislik]=pow(10,us);//baslangic top miktarı
	
	

	for(i=0;i<tektarafgenislik*2+1;i++){
		printf("%5.d",eksena[i]);
	}

	while(j<tektarafgenislik/2){
		printf("\n");
		
		
		for(i=0;i<tektarafgenislik*2;i++){
			eksenb[i]+=eksena[i+1]/2;//soldan gidiş
			eksenb[tektarafgenislik*2-i]+=eksena[i+1]/2;//sağdan gidiş
		}
		for(i=0;i<=tektarafgenislik*2;i++){//sıfırlamak
			eksena[i]=0;
		}
		for(i=0;i<tektarafgenislik*2+1;i++){//çıktı
			printf("%5.d",eksenb[i]);
		}
		
		
		
		printf("\n");
		
		
		
		
		
		for(i=0;i<tektarafgenislik*2;i++){
			eksena[i]+=eksenb[i+1]/2;
			eksena[tektarafgenislik*2-i]+=eksenb[i+1]/2;
		}
		for(i=0;i<=tektarafgenislik*2;i++){//sıfırlamak
			eksenb[i]=0;
		}
		for(i=0;i<=tektarafgenislik*2;i++){
			printf("%5.d",eksena[i]);
		}
		
		
		j++;
	}




}
