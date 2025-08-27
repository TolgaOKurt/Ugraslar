#include <stdio.h>
#include <math.h>
#include <time.h>
#include <stdlib.h>
#include <stdbool.h>



//Her Array -1 ile bitsin.
//Her sonu� pozitif olmali.

//YAPILACAKLAR; MATEMAT�KSEL OLARAK OLMAYACAK D�Z�LER� �NCEDEN BEL�RLEY�P PAS GE�MES�N� SA�LA. B�YLEL�KLE PROGRAMIN HIZI ARTSIN, DAHA NOKTA ATI�I SONU�LAR BULSUN..

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
            printf("Bilinmeyen t�r\n");
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
            printf("Bilinmeyen t�r\n");
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


// burada eski ve daha yava� �al��an max bulma b�l�m� vard�(t�m sonu�lar� bulup sonra i�inde ar�yordu ama g�r�n��e g�re ararken hesaplamak daha hizli.). RIP arrtomax.


int yenicalc(int* dizi, int si) {//!!!!!!!!!!!! ben dizi i�inde s�ralama ile bulma y�ntemini yapt���m� hat�rl�yom eskicalc oydu niye buna ge�tim ki. p k���kken bu daha h�zl� herhalde neyse �imdi bana si>7 i�in dizili hesap laz�m test edelim bakal�m !!!!!!!!!!
	int max = 0;//zaten yukarda qs de duruyor �uanda s�ralancak bi�i  de yok. // yav yok dizilerle yap�nca(x<=9 i�in) daha da yava�� offffff neyi ka��r�yorum. NEYSE BU D�Z�L� ARAMA YONTEM� EKSTRA B�R LOGN EKL�YOR SANIRIM BU YUZDEN VAZGECT�MM. EN �Y� YONTEM BEN�M YEN�CALCIM HUUUUAAAAAHHH BOYL
    int toplam=0;               
    int fark=0;
    while (true) {
        bool bulundu = false;
        // Dizide do�rudan max + 1 var m�?
        for (int i = 0; i <= si; i++) {
            if (dizi[i] == max + 1) {
                max++;
                bulundu = true;
                break;
            }
        }
        if (bulundu) continue;
        // Toplam ve fark kombinasyonlar�n� kontrol et
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




int artir(int* diz,int* low,int* up,int artan,int si){//bir sonraki sayi gelsin. gelirse 0 gelmezse 5 d�ner.
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


void yis(int p){//ana is b�l�m�
	int si=p-1;
	time_t start,end;//kronometre ayar�
	int i=0;
	//int degisim=9; // D�GER YONTEME GECME �S� BEN�M YEN�CALC YONTEM�M COK DAHA HIZLI BU D�Z�L� ARAMADAN !!!! AAAAAAAAAAAAA
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
	
	//Upper reformu, eski upper hesaplama y�ntemine veda.
	upperart(upper,si);//�LK SAY�N�N P'DEN b�y�k oldu�u g�r�lmemi�tir. �K�NC� SAY�N�N 2*P DEN B�Y�K OLDU�U G�R�LMEM��T�R. //Ayni gidisat hespine uygulanmistir.  !!Kesinlik yok ancak potansiyel var.
	
	int sonucmiktari=0;
	
	printArray(upper,si,'i');//UPPER
	
	if(p==3) max=9;//�nceden hesaplanm�� ve maksimumun minimum ka� oldu�unu bilinen aramalarda h�z i�in. max=10 oldu�undan aramaya 9 ile ba�la diyoruz.
	if(p==4) max=15;
	if(p==5){//aramay� limitlendiriyoruz ��nk� en k���k ba�l� sonucun zaten 4 ile ba�lad���n� biliyoruz. kodda yap�lan de�i�iklikler i�leri bozdu mu test ederken bildi�imiz sonu�lara bakar�z.
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
	// ilk bulunan sonu� 2,4,6,7,25,26,41,42'dir. bir di�er sonu� muhtemelen 6,12,18,21,22,23,25,26'dir. maximum da 9 ile ba�layan sonu� var ise onu bulabililir ve sonra biter.
	memcpy(test, bu, (si + 1) * sizeof(int));
	//Yani bu uzun arama s�recinde 2 sonu� belli. e�er max=52 ise ki deneysel sonu�lar hep �ift sayidir, muhtemelen artmaz daha.
	max=51;
	
	printf("Baslangic:\n");
	printArray(test,si,'i');
	}
	if(p==9) max=60;// max de�er p artt�k�a artar. o y�zden p>k iken sonuc(p)>sonuc(k), buradan sonra deneysel sonu�lar� bulmak �ok uzun s�rer, ama deneysel sonu�lar�n hangi sayilardan fazla oldu�u bulmak kolay.
	if(p==10) max=72;
	if(p==11) max=84;
	if(p==12) max=96;	
	if(p==13) max=108;
	if(p==14) max=120;
	if(p>14) max=(p*p+p)/2;
//	if(p=15) max=120; veri almak �ok uzun s�r�yor.
	
	/* 
	//BETTER LOWER KISMI !!! WIP!!!
	printarr(lower,si);
	printarr(upper,si);
	printf("\n h \n");
	*/
	start=clock();// zaman ba�lad�.
	int rkrkrkrk=8;    //burasi bulunmayan sonu�lar�n bulunmasi i�in bilgisayar �al���rken daha �ok bilgilendirilme yapilmasi i�in koyulmu� bir de�i�ikliktir.
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
			printf("\nPass=%u,hesaplanan=%u,max=%d,Sure=%dsn\np6=%d",pas,hesap,max,(end-start)/1000,p6);	//gidi�at s�yleyici
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
		
		
	/*	if(2*test[p-1]<max) {//pass0   deneysel sonu�/max(son) sayi k���k e�ittir 2'ye. e�er deneysel sonu� > 2*max ise. deneysel sonu� nas�l elde edilecek? bir dizide en y�ksek sonu� max+max=2*maxtir.
			p0++;				
			continue;
		}*/
		
			
		if(2*test[(p-1)/2]>max){//pass1 dizideki ortanca sayi, tek sayilarda a�a�� yuvarlanacak �ekilde, sonucun yarisindan k���k olmali incelendi�inde b�yle bir sonuca var�lm��t�r.!! Kesinlik yok ancak potansiyel var.
			p1++;
			continue;	
		}		
		

	/*	if(test[1]>=test[0]*p){//pass2 ikinci sayinin en k���k sayinin p katindan fazla oldu�u g�r�lmemi�tir. !!Kesinlik yok ancak potansiyel var.
			p2++;
			continue;	
		}
		*/
		
		
	/*	if(p>6 && test[0]+test[1]+test[3]>p*(p-1)){//ilk 3 sayinin toplami b�y�d�k�e maxdan k���kle�iyor. max da p*p-1den k���kle�iyor.
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
//	printarr(maxarr); //istersen en son bi daha s�ylesin sana
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
    printf("T�m i�lemler tamamland�.\n");
    return 0;
	
	//AMA�: P M�KTARINDA SAY�, B�RB�RLER�YLE TOPLAMA �IKARMA VEYA KEND�S� VEYA KEND�S�*2 OLMAK �ZERE, SIRALI B�R �EK�LDE MAKS�MUM HANG� SAYIYA KADAR T�M SAYILARI ELDE EDEB�L�R�Z.
	// �RNEK 2 VE 3 �LE 1 2 3 4 5 6 ELDE ED�LEB�L�R 
	// 3-2 2 3 2+2 3+2 3+3	
	//p=say� miktari olmak �zere maksimum p*p+p sonucu elde edilebilir. buna teorik sonu� diyorum.
	//deneysel sonu� maksimum sayidan b�y�k-e�it ve teorik sonu�tan k���k olacak.
	//teorik sonucun max say�n�n 3 kat�ndan fazla oldu�u g�r�lmedi ama bu oran giderek art�yor p=7de bu oran 2,8e kadar ��kabiliyor.

}

