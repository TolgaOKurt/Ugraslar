#include <stdio.h>
#include <math.h>
#include <time.h>
#include <stdlib.h>
#include <stdbool.h>




//Her Array -1 ile bitsin.
//Her sonu� pozitif olmali.


//YAPILACAKLAR; MATEMAT�KSEL OLARAK OLMAYACAK D�Z�LER� �NCEDEN BEL�RLEY�P PAS GE�MES�N� SA�LA. B�YLEL�KLE PROGRAMIN HIZI ARTSIN, DAHA NOKTA ATI�I SONU�LAR BULSUN..

void sifird(double* diz,int s){//dizinin ilk "s" birimini 0 yap�yor.
	int i=0;
	for (i=0;i<s;i++){
		diz[i]=0;
	}
}

void printarrd(double* diz,int si){ // Array printliyor
	int i=0;
	for(i=0;i<=si;i++){
		printf("%d -> ",i);
		printf("%f\n",diz[i]);
	}
	printf("\n");
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

int fsskm(int* dizi,int boyut,int max){ // fsskm=farklar standart sapma kucuk mu
	int buyuk=0;
	double farktoplami=0;
	double farkdizisi[boyut-1];
	double farktt=0;
	double temp=0;
	sifird(farkdizisi,boyut-1);
	for(int i=0;i<boyut-1;i++){
		farkdizisi[i]=(double)dizi[i+1]-(double)dizi[i];
		farktoplami=farktoplami+farkdizisi[i];
	}
	
	double ort=farktoplami/(boyut-1);
//	printf("\n ort deger== %f \n",ort);
	for(int i=0;i<boyut-1;i++){
		temp=(farkdizisi[i]-ort);
	//	printf("%f = %f - %f / %f\n",temp,farkdizisi[i],ort,farktoplami);
		farkdizisi[i]=temp*temp;
	}
//	printarrd(farkdizisi,boyut-2);
	for(int i=0;i<boyut-1;i++){
		farktt=farktt+farkdizisi[i];
	}
//	printf("\n dizinin toplami== %f \n",farktt);
	farktt=farktt/(boyut-1);
//	printf("\n bolunmesi== %f \n",farktt);
	farktt=pow(farktt,0.5);
	
	
	float fiyakalisism=(boyut*boyut+boyut)/4;
//	if (max>15) fiyakalisism=(1+(double)max/4); // ne yapmali??
	//printf("\n buuuu->  %f",fiyakalisism);
	if(farktt>fiyakalisism){
//	printf("SS=%f",farktt);
	return -1;
	}
	else return 0;
}

void QS(int arr[], int low, int high) { // internetten �al�nm�� quicksort
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

void sifir(int* diz,int s){//dizinin ilk "s" birimini 0 yap�yor.
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

	QS(fulldizi,0,fullboyut-1);
	
/*	for(int l=0;l<fullboyut;l++){
		printf("%d,",fulldizi[l]);
	}*/
	int l=0;
	for(l=0;l<fullboyut;l++){
	//	printf("\n l=%d yani %d suan",l,fulldizi[l]);
		if(fulldizi[l]<max+1){
		//printf(" pass");
			continue;
		} 
		else if(fulldizi[l]==max+1){
			//printf(" max artar");
			max++;
		}
		else break;
		
	}
	return max;
	
}



// burada sifir fonksiyonun ikizi vard�. gereksizlikten silindi. RIP.

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






void  copy(int* ilk, int* ikinc,int si){//array kopyalama.
	int i=0;
	for (i=0;i<=si;i++){
		ikinc[i]=ilk[i];
	}
}

void  lscopy(int* ilk, int* ikinc,int l,int si){//array da bi yere kadar kopyalan�r yani
	int i=0;
	for (i=0;i<l;i++){
		ikinc[si-i]=ilk[si-i];
	}
}


void skalerart(int* diz,int s,int si){ // diziyi sabit say� ile artt�r�r.
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

void siradoldur(int* diz){ // 1,2,3,4 �eklinde dizer.
	int i=0;
	for(i=0;diz[i]!=-1;i++){
		diz[i]=i+1;
	}
}

int aynimi(int* bir,int* iki,int si){ // 2 array ayn� m�? ayn� ise 1 d�ner.
	int i=0;
	for(i=0;i<=si;i++){
		if(bir[i]-iki[i]!=0){
			return -1;
		}
	}
	return 1;
}

int artir(int* diz,int* low,int* up,int artan,int si){//bir sonraki sayi gelsin. gelirse 0 gelmezse 5 d�ner.
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


//Burada arrtomax(RIP) kullanan is b�l�m� vardi. arrtomax gidince is de kederden gitti. RIP is.
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
	
	/*if(p>=degisim){
		max=dizilicalc(test,si);
	}
	else */
	max=yenicalc(test,si);
	copy(test,maxarr,si);
	copy(test,lower,si);
	

	
	
	//Upper reformu, eski upper hesaplama y�ntemine veda.
	upperart(upper,si);//�LK SAY�N�N P'DEN b�y�k oldu�u g�r�lmemi�tir. �K�NC� SAY�N�N 2*P DEN B�Y�K OLDU�U G�R�LMEM��T�R. //Ayni gidisat hespine uygulanmistir.  !!Kesinlik yok ancak potansiyel var.
	
	
	int sonucmiktari=0;
	
	printarr(upper,si);//UPPER
	
	if(p==3) max=9;//�nceden hesaplanm�� ve maksimumun minimum ka� oldu�unu bilinen aramalarda h�z i�in. max=10 oldu�undan aramaya 9 ile ba�la diyoruz.
	if(p==4) max=15;
	if(p==5){
	skalerart(lower,3,si);//aramay� limitlendiriyoruz ��nk� en k���k ba�l� sonucun zaten 4 ile ba�lad���n� biliyoruz. kodda yap�lan de�i�iklikler i�leri bozdu mu test ederken bildi�imiz sonu�lara bakar�z.
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
	int bu[9]={2,4,6,7,25,26,40,41,-1}; // ilk bulunan sonu� 2,4,6,7,25,26,41,42'dir. bir di�er sonu� muhtemelen 6,12,18,21,22,23,25,26'dir. maximum da 9 ile ba�layan sonu� var ise onu bulabililir ve sonra biter.
	copy(bu,test,si);//Yani bu uzun arama s�recinde 2 sonu� belli. e�er max=52 ise ki deneysel sonu�lar hep �ift sayidir, muhtemelen artmaz daha.
	max=51;
	printf("Baslangic:\n");
	printarr(test,si);
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
	int rkrkrkrk=7;    //burasi bulunmayan sonu�lar�n bulunmasi i�in bilgisayar �al���rken daha �ok bilgilendirilme yapilmasi i�in koyulmu� bir de�i�ikliktir.
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
			if(p>=8) gidisat(test,si);//gidisat
			printf("\n");			
		}
		x=artir(test,lower,upper,p-1,si);

		
		
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
	
	
	
	
	
	
	
		//Burada pass3 yat�yor RIP
		
		//Burada pass4 yat�yor RIP
		
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
			printarr(test,si);
			
			

		}
		if(temp>max){
			max=temp;
			end=clock();
			sonucmiktari=1;
			printf("\n(%dsn)%d bulundu:\n",(end-start)/1000,temp);
			printarr(test,si);
			copy(test,maxarr,si);
			
			
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
	int p=7;
	
	yis(p);

	//AMA�: P M�KTARINDA SAY�, B�RB�RLER�YLE TOPLAMA �IKARMA VEYA KEND�S� VEYA KEND�S�*2 OLMAK �ZERE, SIRALI B�R �EK�LDE MAKS�MUM HANG� SAYIYA KADAR T�M SAYILARI ELDE EDEB�L�R�Z.
	// �RNEK 2 VE 3 �LE 1 2 3 4 5 6 ELDE ED�LEB�L�R 
	// 3-2 2 3 2+2 3+2 3+3
	

	
	
	//p=say� miktari olmak �zere maksimum p*p+p sonucu elde edilebilir. buna teorik sonu� diyorum.
	//deneysel sonu� maksimum sayidan b�y�k-e�it ve teorik sonu�tan k���k olacak.
	//teorik sonucun max say�n�n 3 kat�ndan fazla oldu�u g�r�lmedi ama bu oran giderek art�yor p=7de bu oran 2,8e kadar ��kabiliyor.
	
	printf("\nProgram calismayi durdurdu. Kapatabilirsiniz.\n");
//	scanf("%d");*/
	




}






