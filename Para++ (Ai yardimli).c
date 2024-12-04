#include <stdio.h>
#include <math.h>
#include <time.h>
#include <stdlib.h>
#include <stdbool.h>




//Her Array -1 ile bitsin.
//Her sonuç pozitif olmali.


//YAPILACAKLAR; MATEMATÝKSEL OLARAK OLMAYACAK DÝZÝLERÝ ÖNCEDEN BELÝRLEYÝP PAS GEÇMESÝNÝ SAÐLA. BÖYLELÝKLE PROGRAMIN HIZI ARTSIN, DAHA NOKTA ATIÞI SONUÇLAR BULSUN..




void QS(int arr[], int low, int high) { // internetten çalýnmýþ quicksort
    if (low < high) {
        int pivot = arr[(low + high) / 2]; // Selecting the middle element as the pivot
        int i = low;
        int j = high;
        int temp;

        while (i <= j) {
            while (arr[i] < pivot) i++; // Moving elements smaller than pivot to the left
            while (arr[j] > pivot) j--; // Moving elements greater than pivot to the right

            if (i <= j) {
                temp = arr[i];         // Swapping elements
                arr[i] = arr[j];
                arr[j] = temp;
                i++;
                j--;
            }
        }

        // Recursively sort the two partitions
        if (low < j) QS(arr, low, j);
        if (i < high) QS(arr, i, high);
    }
    return;
}

void sifir(int* diz,int s){//dizinin ilk "s" birimini 0 yapýyor.
	int i=0;
	for (i=0;i<s;i++){
		diz[i]=0;
	}
}



void upperart(int* diz,int si){
					// p=si+1;
	int i=0;
	for (i=0;i<=si;i++){
		diz[i]=(si+1)*(1+i)+1;
	}
	return;
}




void printarr(int* diz,int si){ // Array printliyor
	int i=0;
	for(i=0;i<=si;i++){
		printf("%d -> ",i);
		printf("%d\n",diz[i]);
	}
	printf("\n");
	return;
}

// burada sifir fonksiyonun ikizi vardý. gereksizlikten silindi. RIP.

// burada eski ve daha yavaþ çalýþan max bulma bölümü vardý(tüm sonuçlarý bulup sonra içinde arýyordu ama görünüþe göre ararken hesaplamak daha hizli.). RIP arrtomax.



int yenicalc(int* dizi, int si,int eskimax) {
    int max = 0;
    int better=0;
	for (int i = 0; i <= si; i++) {
            if (dizi[i] == eskimax) {
                better=1;
                break;
            }
        }
        if (better==0){
	    	for (int i = 0; i <= si; i++) {
	            for (int j = i; j <= si; j++) {
	                int toplam = dizi[i] + dizi[j];
	                int fark = abs(dizi[i] - dizi[j]);
	
	                if (toplam == eskimax || fark == eskimax) {
	                    better=1;
	                    break;
	                }
	            }
	            if (better==1) break;
	        }
		}
	
	if(better==0) return eskimax-1;
	
	
    while (true) {
        bool bulundu = false;

        // Dizide doðrudan max + 1 var mý?
        for (int i = 0; i <= si; i++) {
            if (dizi[i] == max + 1) {
                max++;
                bulundu = true;
                break;
            }
        }

        if (bulundu) continue;

        // Toplam ve fark kombinasyonlarýný kontrol et
        for (int i = 0; i <= si; i++) {
            for (int j = i; j <= si; j++) {
                int toplam = dizi[i] + dizi[j];
                int fark = abs(dizi[i] - dizi[j]);

                if (toplam == max + 1 || fark == max + 1) {
                    max++;
                    bulundu = true;
                    break;
                }
            }
            if (bulundu) break;
        }
        if (!bulundu) break;
    }

    return max;
}






void  copy(int* ilk, int* ikinc,int si){//array kopyalama.
	int i=0;
	for (i=0;i<=si;i++){
		ikinc[i]=ilk[i];
	}
}

void  lscopy(int* ilk, int* ikinc,int l,int si){//array da bi yere kadar kopyalanýr yani
	int i=0;
	for (i=0;i<l;i++){
		ikinc[si-i]=ilk[si-i];
	}
}


void skalerart(int* diz,int s,int si){ // diziyi sabit sayý ile arttýrýr.
	int i=0;
	for (i=0;i<=si;i++){
		diz[i]=diz[i]+s;
	}
}

void lskalerart(int* diz,int s,int d,int si){// array da bi yere kadar artar yani
	int i=0;

	for (i=0;i<d;i++){
		diz[si-i]+=s;
	}
}

void siradoldur(int* diz){ // 1,2,3,4 þeklinde dizer.
	int i=0;
	for(i=0;diz[i]!=-1;i++){
		diz[i]=i+1;
	}
}

int aynimi(int* bir,int* iki,int si){ // 2 array ayný mý? ayný ise 1 döner.
	int i=0;
	for(i=0;i<=si;i++){
		if(bir[i]-iki[i]!=0){
			return -1;
		}
	}
	return 1;
}

int artir(int* diz,int* low,int* up,int artan,int si){//bir sonraki sayi gelsin. gelirse 0 gelmezse 5 döner.
	int i=0;
	


	if(diz[artan]<up[artan]){
		diz[artan]++;
		return 0;
	}
	
	else if(aynimi(up,diz,si)==1){
		return 5;
	}
	
	if(diz[artan]==up[artan]){
		lscopy(low,diz,si-artan+1,si);
		lskalerart(diz,diz[artan-1]-low[artan-1]+1,si-artan+1,si);
		return artir(diz,low,up,artan-1,si);
		
	}
	
	
}


//Burada arrtomax(RIP) kullanan is bölümü vardi. arrtomax gidince is de kederden gitti. RIP is.
void gidisat(int* diz,int si){ // Array printliyor
	int i=0;
	printf("\n");
	for(i=0;i<=si;i++){
		printf("%d,",diz[i]);
	}
	printf("-1");
	printf("\n");
	return;
}


void yis(int p){//ana is bölümü
	int si=p-1;
	time_t start,end;//kronometre ayarý
	int i=0;
	int upper[p+1];
	upper[p]=-1;
	sifir(upper,p);
	int lower[p+1];
	lower[p]=-1;
	sifir(lower,p);
	int teomax=p*p+p;
	int max=-5,maxtest2=-99;
	int temp=-6;
	int test[p+1];
	int maxarr[p+1];
	maxarr[p]=-1;
	sifir(maxarr,p);
	test[p]=-1;
	sifir(test,p);
	siradoldur(test);
	max=yenicalc(test,si,1);
	copy(test,maxarr,si);
	copy(test,lower,si);
	

	
	
	//Upper reformu, eski upper hesaplama yöntemine veda.
	upperart(upper,si);//ÝLK SAYÝNÝN P'DEN büyük olduðu görülmemiþtir. ÝKÝNCÝ SAYÝNÝN 2*P DEN BÜYÜK OLDUÐU GÖRÜLMEMÝÞTÝR. //Ayni gidisat hespine uygulanmistir.  !!Kesinlik yok ancak potansiyel var.
	
	
	int sonucmiktari=0;
	
	printarr(upper,si);
	
	if(p==3) max=9;//önceden hesaplanmýþ ve maksimumun minimum kaç olduðunu bilinen aramalarda hýz için. max=10 olduðundan aramaya 9 ile baþla diyoruz.
	if(p==4) max=15;
	if(p==5){
	skalerart(lower,3,si);//aramayý limitlendiriyoruz çünkü en küçük baþlý sonucun zaten 4 ile baþladýðýný biliyoruz. kodda yapýlan deðiþiklikler iþleri bozdu mu test ederken bildiðimiz sonuçlara bakarýz.
	skalerart(test,3,si);
	max=23;
	}
	if(p==6){
	skalerart(lower,3,si);
	skalerart(test,3,si);
	max=31;
	}
	if(p==7){
	skalerart(lower,1,si);
	skalerart(test,1,si);
	max=39;
	}
	if(p==8)
	{
	int bu[9]={2,4,6,7,25,26,40,41,-1}; // ilk bulunan sonuç 2,4,6,7,25,26,41,42'dir. bir diðer sonuç muhtemelen 6,12,18,21,22,23,25,26'dir. maximum da 9 ile baþlayan sonuç var ise onu bulabililir ve sonra biter.
	copy(bu,test,si);//Yani bu uzun arama sürecinde 2 sonuç belli. eðer max=52 ise ki deneysel sonuçlar hep çift sayidir, muhtemelen artmaz daha.
	max=51;
	printf("Baslangic:\n");
	printarr(test,si);
	}
	
	if(p==9) max=60;// max deðer p arttýkça artar. o yüzden p>k iken sonuc(p)>sonuc(k), buradan sonra deneysel sonuçlarý bulmak çok uzun sürer, ama deneysel sonuçlarýn hangi sayilardan fazla olduðu bulmak kolay.
	if(p==10) max=72;
	if(p==11) max=79;
	if(p==12) max=89;
	
	if(p==13) max=98;
	if(p==14) max=108;
	if(p>14) max=(p*p+p)/2;
//	if(p=15) max=120; veri almak çok uzun sürüyor.
	
	
	
	
	

	/* 
	//BETTER LOWER KISMI !!! WIP!!!
	printarr(lower,si);
	printarr(upper,si);
	printf("\n h \n");
	*/
	
	
	
	start=clock();// zaman baþladý.
	int rkrkrkrk=8;    //burasi bulunmayan sonuçlarýn bulunmasi için bilgisayar çalýþýrken daha çok bilgilendirilme yapilmasi için koyulmuþ bir deðiþikliktir.
	//if(p>=8) rkrkrkrk=9;
	unsigned int pas=-1,say2=0,hesap=0;
	int p1=0,p2=0,p0=0,p3=0,p4=0;
	int x=0;
	int sif=0,sif2=0,sif3=0;
	for(i=0;1;i++){
		say2++;
		if(abs(say2)%(int)(pow(10,rkrkrkrk))*p==0){
			printf("\n\ntoplam=%u, pass=%u, hesaplanan=%u, max=%d\n\n",say2,pas,hesap,max);	//gidiþat söyleyici
			if(p>=8) gidisat(test,si);//gidisat
			printf("\n");			
		}
		x=artir(test,lower,upper,p-1,si);

		
		
		if(x==5){//bitti ise
	 		break;
		}
		

		
		if(2*test[p-1]<max) {//pass0   deneysel sonuç/max(son) sayi küçük eþittir 2'ye. eðer deneysel sonuç > 2*max ise. deneysel sonuç nasýl elde edilecek? bir dizide en yüksek sonuç max+max=2*maxtir.
			p0++;				
			continue;
		}
		
			
		if(2*test[(p-1)/2]>max){//pass1 dizideki ortanca sayi, tek sayilarda aþaðý yuvarlanacak þekilde, sonucun yarisindan küçük olmali incelendiðinde böyle bir sonuca varýlmýþtýr.!! Kesinlik yok ancak potansiyel var.
			p1++;
			continue;	
		}		
		

		if(test[1]>=test[0]*p){//pass2 ikinci sayinin en küçük sayinin p katindan fazla olduðu görülmemiþtir. !!Kesinlik yok ancak potansiyel var.
			p2++;
			continue;	
		}
		
		
		
		if(p>6 && test[0]+test[1]+test[3]>p*(p-1)){//ilk 3 sayinin toplami büyüdükçe maxdan küçükleþiyor. max da p*p-1den küçükleþiyor.
			p3++;
			continue;
		}
	
		if(test[0]+test[p-1]>max+p){//
			p4++;
			continue;
		}
		
		//Burada pass3 yatýyor RIP
		
		//Burada pass4 yatýyor RIP
		
		hesap++;
		/*if(hesap==0){
			sif3++;
			printf("\n Hesap sayaci %d kere sifirlandi \n",sif3);
		} */
		temp=yenicalc(test,si,max);

		if(temp==max&&temp>teomax/2){
			sonucmiktari++;
			printf("\n%d icin %d. sonuc:\n",temp,sonucmiktari);
			printarr(test,si);

		}
		if(temp>max){
			max=temp;
			end=clock();
			sonucmiktari=1;
			printf("\n%d bulundu(sure=%d):\n",temp,end-start);
			printarr(test,si);
			copy(test,maxarr,si);
		}
		

	}
		
	
	
	printf("\n%d sonuc bulundu.\n",sonucmiktari);
	printf("\nBitti toplam=%d, pass=%d, max=%d, hesaplanan=%d\n",say2,p0+p1+p2+p3+p4,max,hesap);
//	printarr(maxarr); //istersen en son bi daha söylesin sana
	printf("\np0=%d p1=%d p2=%d p3=%d p4=%d\n",p0,p1,p2,p3,p4);
	return;
	
}


int main(){	
	printf(" Ai calismasi\n");
	printf("Program basladi. Birazdan ilerleme bilgilendirmesi yapilir.\n");
	int p=6;
	
	yis(p);

	//AMAÇ: P MÝKTARINDA SAYÝ, BÝRBÝRLERÝYLE TOPLAMA ÇIKARMA VEYA KENDÝSÝ VEYA KENDÝSÝ*2 OLMAK ÜZERE, SIRALI BÝR ÞEKÝLDE MAKSÝMUM HANGÝ SAYIYA KADAR TÜM SAYILARI ELDE EDEBÝLÝRÝZ.
	// ÖRNEK 2 VE 3 ÝLE 1 2 3 4 5 6 ELDE EDÝLEBÝLÝR 
	// 3-2 2 3 2+2 3+2 3+3
	

	
	
	//p=sayý miktari olmak üzere maksimum p*p+p sonucu elde edilebilir. buna teorik sonuç diyorum.
	//deneysel sonuç maksimum sayidan büyük-eþit ve teorik sonuçtan küçük olacak.
	//teorik sonucun max sayýnýn 3 katýndan fazla olduðu görülmedi ama bu oran giderek artýyor p=7de bu oran 2,8e kadar çýkabiliyor.
	
	printf("\nProgram calismayi durdurdu. Kapatabilirsiniz.\n");
//	scanf("%d");*/
	




}






