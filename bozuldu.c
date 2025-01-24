#include <stdio.h>
#include <math.h>
#include <time.h>
#include <stdlib.h>
#include <stdbool.h>



//Her Array -1 ile bitsin.
//Her sonuç pozitif olmali.

//YAPILACAKLAR; MATEMATÝKSEL OLARAK OLMAYACAK DÝZÝLERÝ ÖNCEDEN BELÝRLEYÝP PAS GEÇMESÝNÝ SAÐLA. BÖYLELÝKLE PROGRAMIN HIZI ARTSIN, DAHA NOKTA ATIÞI SONUÇLAR BULSUN..

int compare(const void* a, const void* b) {
	printf("Checkpoint 2");
    return (*(int*)a - *(int*)b);
}

void printArray(void* array, int size, char type) {
	printf("Checkpoint 3");
    for (int i = 0; i <= size; i++) {
        printf("%d -> ", i);
        if (type == 'i') {
            printf("%d\n", ((int*)array)[i]);
        } else if (type == 'd') {
            printf("%f\n", ((double*)array)[i]);
        } else {
            printf("Bilinmeyen tür\n");
            return;
        }
    }
    printf("\n");
}

void zeroArray(void* array, int size, char type) {

	printf("Checkpoint 4");
    for (int i = 0; i <= size; i++) {
        if (type == 'i') {
            ((int*)array)[i] = 0;
        } else if (type == 'd') {
            ((double*)array)[i] = 0.0;
        } else {
            printf("Bilinmeyen tür\n");
            return;
        }
    }
}

void artirDizi(int* diz, int s, int start, int end) {
	printf("Checkpoint 6");
    for (int i = start; i <= end; i++) {
        diz[i] += s;
    }
}

void siradoldur(int* diz, int size) {
	printf("Checkpoint 5");
    for (int i = 0; i < size; i++) {
        diz[i] = i + 1;
    }
}





int fsskm(int* dizi,int boyut,int max){ // fsskm=farklar standart sapma kucuk mu
	int buyuk=0;
	double farktoplami=0;
	double farkdizisi[boyut-1];
	double farktt=0;
	double temp=0;
	zeroArray(farkdizisi,boyut-1,'d');
	for(int i=0;i<boyut-1;i++){
		farkdizisi[i]=(double)dizi[i+1]-(double)dizi[i];
		farktoplami=farktoplami+farkdizisi[i];
	}
	
	double ort=farktoplami/(boyut-1);
	for(int i=0;i<boyut-1;i++){
		temp=(farkdizisi[i]-ort);
		farkdizisi[i]=temp*temp;
	}
	for(int i=0;i<boyut-1;i++){
		farktt=farktt+farkdizisi[i];
	}
	farktt=farktt/(boyut-1);
	farktt = sqrt(farktt);
	
	
	float fiyakalisism=(boyut*boyut+boyut)/4;
	if(farktt>fiyakalisism){
	return -1;
	}
	else return 0;
}


void upperart(int* diz,int si){
					// p=si+1;
	int i=0;
	for (i=0;i<=si;i++){
		diz[i]=(si+1)*(1+i)+1;
	}
	return;
}


int dizilicalc(int* dizi, int si) {
    int max = 0;
    int p=si+1;
    int toplamboyut=(p*(p+1))/2;
    int farkboyut=(p*(p-1))/2;
    int fullboyut=toplamboyut+farkboyut+p;

    int dizsay=0,toplamsay=0,farksay=0;
    int i=0;
    int sira=0;
    int fulldizi[p+toplamboyut+farkboyut];

	for(int i=0;i<=si;i++){
		fulldizi[sira]=dizi[i];
		sira++;
	}
	
 	for(int i=0;i<=si;i++){
 		for(int j=i;j<=si;j++){
	 		fulldizi[sira]=dizi[i]+dizi[j];
	 		sira++;
		}
 		
	}
	for(int i=0;i<=si;i++){
 		for(int j=i+1;j<=si;j++){
	 		fulldizi[sira]=abs(dizi[i]-dizi[j]);
	 		sira++;
		}
 		
	}

	qsort(fulldizi, fullboyut, sizeof(int), compare);
	
	int l=0;
	for(l=0;l<fullboyut;l++){
		if(fulldizi[l]<max+1){
			continue;
		} 
		else if(fulldizi[l]==max+1){
			max++;
		}
		else break;
	}
	return max;
}


// burada eski ve daha yavaþ çalýþan max bulma bölümü vardý(tüm sonuçlarý bulup sonra içinde arýyordu ama görünüþe göre ararken hesaplamak daha hizli.). RIP arrtomax.


int yenicalc(int* dizi, int si) {//!!!!!!!!!!!! ben dizi içinde sýralama ile bulma yöntemini yaptýðýmý hatýrlýyom eskicalc oydu niye buna geçtim ki. p küçükken bu daha hýzlý herhalde neyse þimdi bana si>7 için dizili hesap lazým test edelim bakalým !!!!!!!!!!
	int max = 0;//zaten yukarda qs de duruyor þuanda sýralancak biþi  de yok. // yav yok dizilerle yapýnca(x<=9 için) daha da yavaþþ offffff neyi kaçýrýyorum. NEYSE BU DÝZÝLÝ ARAMA YONTEMÝ EKSTRA BÝR LOGN EKLÝYOR SANIRIM BU YUZDEN VAZGECTÝMM. EN ÝYÝ YONTEM BENÝM YENÝCALCIM HUUUUAAAAAHHH BOYL
    int toplam=0;               
    int fark=0;
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
                toplam = dizi[i] + dizi[j];
                fark = abs(dizi[i] - dizi[j]);

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




int artir(int* diz,int* low,int* up,int artan,int si){//bir sonraki sayi gelsin. gelirse 0 gelmezse 5 döner.
	int i=0;
	if(diz[artan]<up[artan]){
		diz[artan]++;
		return 0;
	}	
	else if (memcmp(up, diz, (si + 1) * sizeof(int)) == 0) return 5;
	if(diz[artan]==up[artan]){
		memmove(diz + (si - (si - artan + 1)), low + (si - (si - artan + 1)), (si - artan + 1) * sizeof(int));
		artirDizi(diz, diz[artan - 1] - low[artan - 1] + 1, artan - 1, si);
		return artir(diz,low,up,artan-1,si);
	}
	return -6;
}


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
	//int degisim=9; // DÝGER YONTEME GECME ÝSÝ BENÝM YENÝCALC YONTEMÝM COK DAHA HIZLI BU DÝZÝLÝ ARAMADAN !!!! AAAAAAAAAAAAA
	int upper[p+1];
	upper[p]=-1;
	zeroArray(upper,p,'i');
	int lower[p+1];
	lower[p]=-1;
	zeroArray(lower,p,'i');
	int teomax=p*p+p;
	int max=-5,maxtest2=-99;
	int temp=-6;
	int test[p+1];
	int maxarr[p+1];
	maxarr[p]=-1;
	zeroArray(maxarr,p,'i');
	test[p]=-1;
	zeroArray(test,p,'i');
	siradoldur(test, p+1);
	/*if(p>=degisim){
		max=dizilicalc(test,si);
	}
	else */
	max=yenicalc(test,si);
	memcpy(maxarr, test, (si + 1) * sizeof(int));
	memcpy(lower, test, (si + 1) * sizeof(int));
	
	//Upper reformu, eski upper hesaplama yöntemine veda.
	upperart(upper,si);//ÝLK SAYÝNÝN P'DEN büyük olduðu görülmemiþtir. ÝKÝNCÝ SAYÝNÝN 2*P DEN BÜYÜK OLDUÐU GÖRÜLMEMÝÞTÝR. //Ayni gidisat hespine uygulanmistir.  !!Kesinlik yok ancak potansiyel var.
	
	int sonucmiktari=0;
	
	printArray(upper,si,'i');//UPPER
	
	if(p==3) max=9;//önceden hesaplanmýþ ve maksimumun minimum kaç olduðunu bilinen aramalarda hýz için. max=10 olduðundan aramaya 9 ile baþla diyoruz.
	if(p==4) max=15;
	if(p==5){//aramayý limitlendiriyoruz çünkü en küçük baþlý sonucun zaten 4 ile baþladýðýný biliyoruz. kodda yapýlan deðiþiklikler iþleri bozdu mu test ederken bildiðimiz sonuçlara bakarýz.
	artirDizi(lower, 3, 0, si);
	artirDizi(test, 3, 0, si);
	max=23;
	}
	if(p==6){
	artirDizi(lower, 3, 0, si);
	artirDizi(test, 3, 0, si);
	max=31;
	}
	if(p==7){
	artirDizi(lower, 1, 0, si);
	artirDizi(test, 1, 0, si);
	max=39;
	}
	if(p==8)
	{
	int bu[9]={2,4,6,7,25,26,40,41,-1}; 
	// ilk bulunan sonuç 2,4,6,7,25,26,41,42'dir. bir diðer sonuç muhtemelen 6,12,18,21,22,23,25,26'dir. maximum da 9 ile baþlayan sonuç var ise onu bulabililir ve sonra biter.
	memcpy(test, bu, (si + 1) * sizeof(int));
	//Yani bu uzun arama sürecinde 2 sonuç belli. eðer max=52 ise ki deneysel sonuçlar hep çift sayidir, muhtemelen artmaz daha.
	max=51;
	
	printf("Baslangic:\n");
	printArray(test,si,'i');
	}
	if(p==9) max=60;// max deðer p arttýkça artar. o yüzden p>k iken sonuc(p)>sonuc(k), buradan sonra deneysel sonuçlarý bulmak çok uzun sürer, ama deneysel sonuçlarýn hangi sayilardan fazla olduðu bulmak kolay.
	if(p==10) max=72;
	if(p==11) max=84;
	if(p==12) max=96;	
	if(p==13) max=108;
	if(p==14) max=120;
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
	int p1=0,p2=0,p0=0,p3=0,p4=0,p5=0,p6=0;
	int x=0;
	int sif=0,sif2=0,sif3=0;
	int toplamd=0,better=0;;
	for(i=0;1;i++){
		say2++;
		if(abs(say2)%(int)(pow(10,rkrkrkrk))*p==0){
			end=clock();
			printf("\nPass=%u,hesaplanan=%u,max=%d,Sure=%dsn\np6=%d",pas,hesap,max,(end-start)/1000,p6);	//gidiþat söyleyici
			gidisat(test,si);//gidisat
			printf("\n");			
		}
		x=artir(test,lower,upper,p-1,si);
		if(x==-6){
			printf("HATA");
			exit(-6);
		}
		if(x==5){//bitti ise
	 		break;
		}
		
		
	/*	if(2*test[p-1]<max) {//pass0   deneysel sonuç/max(son) sayi küçük eþittir 2'ye. eðer deneysel sonuç > 2*max ise. deneysel sonuç nasýl elde edilecek? bir dizide en yüksek sonuç max+max=2*maxtir.
			p0++;				
			continue;
		}*/
		
			
		if(2*test[(p-1)/2]>max){//pass1 dizideki ortanca sayi, tek sayilarda aþaðý yuvarlanacak þekilde, sonucun yarisindan küçük olmali incelendiðinde böyle bir sonuca varýlmýþtýr.!! Kesinlik yok ancak potansiyel var.
			p1++;
			continue;	
		}		
		

	/*	if(test[1]>=test[0]*p){//pass2 ikinci sayinin en küçük sayinin p katindan fazla olduðu görülmemiþtir. !!Kesinlik yok ancak potansiyel var.
			p2++;
			continue;	
		}
		*/
		
		
	/*	if(p>6 && test[0]+test[1]+test[3]>p*(p-1)){//ilk 3 sayinin toplami büyüdükçe maxdan küçükleþiyor. max da p*p-1den küçükleþiyor.
			p3++;
			continue;
		}*/
		if(test[0]+test[p-1]>max+p){//
			p4++;
			continue;
		}
		toplamd=0;
		better=0;
		for (int i = 0; i <= si; i++) {
            if (test[i] == max) {
                better=1;
                break;
            }
        }
        if (better==0){
	    	for (int i = 0; i <= si; i++) {
	            for (int j = i; j <= si; j++) {
	                toplamd = test[i] + test[j];
	                if (toplamd == max) {
	                    better=1;
	                    break;
	                }
	            }
	            if (better==1) break;
	        }
		}
		if(better==0){
			p5++;
			continue;
		}
		//FARK STANDART SATMA ORTALAMASINA GORE MUHTEMEL ELEMELER // fsskm=farklar standart sapma kucuk mu
		/*if(-1==fsskm(test,p,max)){
			p6++;
			continue;
		}*/
		hesap++;
		/*if(hesap==0){
			sif3++;
			printf("\n Hesap sayaci %d kere sifirlandi \n",sif3);
		} */
		/*if(p>=degisim){
			temp=dizilicalc(test,si);
		}
		else */
		temp=yenicalc(test,si);
		if(temp==max&&temp>teomax/2){
			sonucmiktari++;
			end=clock();
			printf("\n(%dsn)%d icin %d. sonuc:\n",(end-start)/1000,temp,sonucmiktari);
			printArray(test,si,'i');
			
			
		}
		if(temp>max){
			max=temp;
			end=clock();
			sonucmiktari=1;
			printf("\n(%dsn)%d bulundu:\n",(end-start)/1000,temp);
			printArray(test,si,'i');
			memcpy(maxarr, test, (si + 1) * sizeof(int));
		}
		

	}
		
	float tpp=p0+p1+p2+p3+p4+p5+p6;
	hesap=hesap;
	printf("\n%d sonuc bulundu.\n",sonucmiktari);
	printf("\nBitti toplam=%d, pass=%.0f, max=%d, hesaplanan=%d\n",say2,tpp,max,hesap);
//	printarr(maxarr); //istersen en son bi daha söylesin sana
//	printf("\np0=%d p1=%d p2=%d p3=%d p4=%d p5=%d \n",p0,p1,p2,p3,p4,p5);
	printf("\nYuzdelikler pass=%.2f hesap=%.2f",100*(float)tpp/say2,100*(float)hesap/say2);
	printf("\np1=%.2f p4=%.2f p5=%.2f p6=%.2f",100*(float)p1/tpp,100*(float)p4/tpp,100*(float)p5/tpp,100*(float)p6/tpp);
	end=clock();
	printf("\nSure=%dsn\n",(end-start)/1000);
	return;
}


int main(){	
	printf(" Ai calismasi\n");
	printf("Program basladi. Birazdan ilerleme bilgilendirmesi yapilir.\n");
	yis(3);
    printf("Tüm iþlemler tamamlandý.\n");
    return 0;
	
	//AMAÇ: P MÝKTARINDA SAYÝ, BÝRBÝRLERÝYLE TOPLAMA ÇIKARMA VEYA KENDÝSÝ VEYA KENDÝSÝ*2 OLMAK ÜZERE, SIRALI BÝR ÞEKÝLDE MAKSÝMUM HANGÝ SAYIYA KADAR TÜM SAYILARI ELDE EDEBÝLÝRÝZ.
	// ÖRNEK 2 VE 3 ÝLE 1 2 3 4 5 6 ELDE EDÝLEBÝLÝR 
	// 3-2 2 3 2+2 3+2 3+3	
	//p=sayý miktari olmak üzere maksimum p*p+p sonucu elde edilebilir. buna teorik sonuç diyorum.
	//deneysel sonuç maksimum sayidan büyük-eþit ve teorik sonuçtan küçük olacak.
	//teorik sonucun max sayýnýn 3 katýndan fazla olduðu görülmedi ama bu oran giderek artýyor p=7de bu oran 2,8e kadar çýkabiliyor.

}

