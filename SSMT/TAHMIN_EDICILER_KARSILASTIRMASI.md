# SSMT Tahmin Edicileri Kapsamlı Karşılaştırma Raporu
## Alman Tank Problemi (German Tank Problem) İstatistiksel Analizi

Bu doküman, SSMT simülasyon ortamında uygulanan **8 farklı tahmin edici algoritmanın** matematiksel temellerini, teorik arka planlarını, birbirleriyle olan performans yarışını ve *"Neden MVUE, Bayes Medyan ve Bayes Ortalama baş başa yarışıyor?"* sorusunun bilimsel açıklamasını içerir.

---

## 🏆 Zirvedeki "Kutsal Üçlü" (The Holy Trinity)

Monte Carlo deneylerinde ve tekil simülasyonlarda en düşük hata oranlarına sahip olan ve birbirleriyle baş başa yarışan üç yöntem: **MVUE**, **Bayes Medyan** ve **Bayes Ortalama**'dır.

### 1. Neden Bu Üçü Zirvede ve Birbirine Çok Yakın?

İstatistikte ayrık tekdüze dağılımın ($[1, N]$) üst sınırını tahmin ederken örneklemdeki en kritik bilgi **"gözlemlenen en büyük seri numarası" ($m$)** değeridir. 

> [!IMPORTANT]
> **Yeterli İstatistik (Sufficient Statistic) İlkesi:**
> Lehmann-Scheffé ve Rao-Blackwell teoremlerine göre, popülasyon üst sınırı $N$ hakkında örneklemdeki tüm faydalı bilgi yalnızca $m$ (maksimum) ve $k$ (gözlem sayısı) değişkenlerinde toplanmıştır. Aradaki diğer tekil sayıların büyüklükleri $N$ hakkında ekstra bilgi sağlamaz (gürültüdür).

Zirvedeki bu üç yöntem yalnızca $m$ ve $k$ değerlerini kullanarak tahmini genişletir:

```
┌─────────────────┬──────────────────────────────────────────┬─────────────────────────────┐
│ Yöntem          │ Tam Formül                               │ Yaklaşık Form (k arttıkça)  │
├─────────────────┼──────────────────────────────────────────┼─────────────────────────────┤
│ MVUE            │ m + (m / k) - 1                          │ m * (1 + 1.000 / k)         │
│ Bayes Ortalama  │ (m - 1) * (k - 1) / (k - 2)              │ m * (1 + 1.000 / (k - 2))   │
│ Bayes Medyan    │ m * 2^(1 / (k - 1))                      │ m * (1 + 0.693 / (k - 1))   │
└─────────────────┴──────────────────────────────────────────┴─────────────────────────────┘
```

$k$ sayısı biraz arttığında ($k \ge 10$), $1/k$, $1/(k-2)$ ve $0.693/(k-1)$ payları birbirine çok yaklaşır. Bu nedenle her 3 yöntem de neredeyse **aynı matematiksel sınıra yakınsar**.

---

## 🔍 Yöntemlerin Derinlemesine Analizi ve Karakteristikleri

### 1. 🥇 MVUE (Minimum Variance Unbiased Estimator)
* **Formül:** $\hat{N} = m + \frac{m}{k} - 1$
* **Felsefesi:** Frekansçı (Klasik) İstatistik.
* **Avantajı:** **Sıfır Sapma (Zero Bias).** Uzun vadede sonsuz sayıda deneme yapılsa, tahminlerin beklenen değeri tam olarak $E[\hat{N}] = N$'dir. Ne sistematik olarak fazla ne de eksik tahmin üretir.
* **Varyansı:** $\mathcal{O}(N^2 / k^2)$ mertebesindedir. $k$ arttıkça karesel hızla hatasını sıfırlar.

### 2. 🥈 Bayes Medyan (Bayesian Median)
* **Formül ($k > 1$):** $\hat{N} = m \times 2^{\frac{1}{k - 1}}$
* **Felsefesi:** Bayesci Olasılık (Düz öncül altında sonsal dağılımın %50 olasılık eşiği).
* **Avantajı:** **En Düşük Mutlak Hata (MAE).** Sonsal olasılık dağılımı sağa çarpık (asimetrik) olduğu için medyan, ortalamadan biraz daha temkinlidir ($\ln 2 \approx 0.693 < 1.0$). Bu sayede aşırı şişkin uç tahminler üretmez ve mutlak yüzde hatayı en aza indirmede sık sık MVUE'yi bile kıl payı geçer.

### 3. 🥉 Bayes Ortalama (Bayesian Mean)
* **Formül ($k > 2$):** $\hat{N} = (m - 1) \frac{k - 1}{k - 2}$
* **Felsefesi:** Bayesci Sonsal Dağılımın Kütle Merkezi (Beklenen Değeri).
* **Avantajı:** Tüm olası $N$ değerlerinin olasılık ağırlıklı ortalamasını alır. $k > 3$ olduğunda hızla MVUE ile neredeyse birebir aynı davranışı sergiler.

---

## 📉 Diğer 5 Yöntem Neden Geride Kalıyor?

| Yöntem | Formül | Hata Nedeni / Zaafı |
| :--- | :--- | :--- |
| **Simetrik Aralık** | $m + \min(X) - 1$ | $1$'e olan boşluk ile $N$'ye olan boşluğun simetrik olduğunu varsayar. Sezgisel olarak başarılıdır (ortalama ~%8-9 hata), ancak varyansı MVUE kadar kararlı değildir. |
| **Bilinen Maks (BM)** | $m$ | Gerçek $N$ her zaman $m$'den büyük veya eşit olduğu için **her zaman negatif yanlıdır** (asla gerçek sayının üstüne çıkamaz). Sürekli eksik tahmin yapar. |
| **Düzeltilmiş Moment** | $2\bar{x} - 1$ | $m$ yerine tüm örneklemin ortalamasını ($\bar{x}$) kullanır. Rastgele gelen sayılar tesadüfen küçükse tahmini aşırı küçük, büyükse aşırı büyük olur (Varyansı $\mathcal{O}(N^2 / k)$ seviyesindedir). |
| **2x Ortalama** | $2\bar{x}$ | Hem ortalama dalgalanmalarına açıktır hem de teorik olarak $+1$'lik fazlalık sapması taşır. |
| **Örneklem Medyanı** | $2 \times \text{Medyan} - 1$ | Küçük gözlem sayılarında ($k=10$) örneklemin medyanı aşırı dalgalandığı için en yüksek hataya (%20+) sahip yöntemdir. |

---

## 📊 Monte Carlo Simülasyon Verileri (100 Bağımsız Deney, $k=10$, $N \in [500, 1500]$)

Aşağıdaki tablo, SSMT motorunun 100 bağımsız rastgele evrende gerçekleştirdiği simülasyonun sonuçlarıdır:

| Sıra | Tahmin Edici | Ortalama Hata (%) | Standart Sapma | Min - Maks Hata Aralığı | Değerlendirme |
| :---: | :--- | :---: | :---: | :---: | :--- |
| 🥇 **1** | **Bayes Medyan** | **%7.44** | $\pm 8.11$ | %0.0 - %46.5 | En iyi MAE performansı |
| 🥈 **2** | **MVUE** | **%7.80** | $\pm 7.60$ | %0.0 - %45.6 | En kararlı yansız tahminci |
| 🥉 **3** | **Bayes Ortalama** | **%8.65** | $\pm 7.02$ | %0.1 - %44.4 | Düşük standart sapma |
| **4** | **Simetrik Aralık** | **%9.62** | $\pm 8.93$ | %0.0 - %46.8 | Başarılı sezgisel alternatif |
| **5** | **Bilinen Maks (BM)** | **%10.01** | $\pm 9.85$ | %0.1 - %50.5 | Güvenli alt sınır |
| **6** | **2x Ortalama** | **%14.18** | $\pm 10.23$ | %0.1 - %40.3 | Yüksek varyans |
| **7** | **Düzeltilmiş Moment** | **%14.19** | $\pm 10.24$ | %0.0 - %40.4 | Yüksek varyans |
| **8** | **Örneklem Medyanı** | **%20.62** | $\pm 14.39$ | %0.7 - %59.0 | Yetersiz kararlılık |

---

## 🧭 Hangi Senaryoda Hangi Yöntem Seçilmeli?

```
Tahmin Amacınız Nedir?
│
├── 🎯 "Uzun vadede ortalama sapmam sıfır olsun (Ne eksik ne fazla)":
│    └── ➔ MVUE (Minimum Variance Unbiased Estimator)
│
├── 📉 "Tek bir deneyde mutlak hata payımı en aza indireyim (Aşırı şişkin tahmin istemiyorum)":
│    └── ➔ Bayes Medyan (Bayesian Median)
│
├── 🛡️ "Kesin olarak en az kaç adet üretildiğini bileyim (Güvenli Taban Eşiği)":
│    └── ➔ Bilinen Maksimum Değer (Sample Maximum)
│
└── 🧩 "Minimum ve Maksimum seri numaralarını birlikte hesaba katan sezgisel yaklaşım":
     └── ➔ Simetrik Aralık (Symmetric Spacing)
```

---

## 🔬 N ile m Arasındaki Dinamik İlişki ve Yöntem Başarılarına Etkisi

Simülasyonlarda dikkat çeken en önemli olgulardan biri, **gerçek üst limit ($N$) ile gözlemlenen en büyük sayı ($m$) arasındaki farkın ($N - m$)** ve oranının ($m / N$) tahmin edicilerin performansını doğrudan belirlemesidir.

### 1. $m$'nin Beklenen Değeri ve Ortalama Yakalama Oranı

$k$ adet iadesiz gözlem yapıldığında, $m$'nin beklenen değeri teorik olarak şudur:

$$E[m] = \frac{k}{k + 1} (N + 1) \approx \frac{k}{k + 1} N$$

Bu formül, ortalama olarak $m$'nin gerçek $N$'nin ne kadarlık kısmını yakaladığını gösterir:
* **$k = 1$ için:** $E[m] \approx \frac{1}{2} N$ (%50 yakalama)
* **$k = 5$ için:** $E[m] \approx \frac{5}{6} N$ (%83.3 yakalama)
* **$k = 10$ için:** $E[m] \approx \frac{10}{11} N$ (%90.9 yakalama)
* **$k = 50$ için:** $E[m] \approx \frac{50}{51} N$ (%98.0 yakalama)

---

### 2. $m / N$ Oranına Göre 3 Farklı Davranış Senaryosu

Gerçekleşen rastgele çekilişte $m$'nin $N$'ye olan mesafesine göre tahminciler şu şekilde tepki verir:

```
                  ┌─────────────────────────────────────────────────────────┐
                  │                 m / N ORANI SPEKTRUMU                   │
                  └─────────────────────────────────────────────────────────┘
   0%                                      70%                    95%     100%
   ├────────────────────────────────────────┼──────────────────────┼───────┤
   │    SENARYO A: m << N                   │ SENARYO B: Normal    │ SENARYO C: m ≈ N
   │    (Şanssız / Düşük Maksimum)          │ (Beklenen Aralık)    │ (Erken Yakalama)
   │                                        │                      │
   │  • BM: Feci Hata (%30-50 eksik)        │ • MVUE: En İyi       │ • BM: Zirvede (%0-2 hata)
   │  • MVUE: Kurtarıcı (+m/k ile toparlar) │ • Bayes: En İyi      │ • MVUE: Aşırı Tahmin (+m/k şişer)
   │  • Bayes: Dengeli yukarı çeker         │ • BM: %5-10 Eksik    │ • Bayes Medyan: MVUE'den az şişer
```

#### Senaryo A: $m \ll N$ (Düşük Maksimum / Şanssız Örneklem — $m/N < 75\%$)
* **Ne Olur?** Çekilen $k$ adet sayı tesadüfen alt aralıkta kümelenmiştir (Örn: $N=1000$ iken $m=650$).
* **Bilinen Maks (BM):** Ağır bir yenilgi alır (%35 hata, çünkü asla $m$'nin üstüne çıkamaz).
* **MVUE ve Bayes Ortalama:** $m$'nin üzerine $m/k$ payı ekleyerek tahmini yukarı iter ($650 + 65 = 715$). Hatayı %35'ten %28'e indirerek durumu toparlar.
* **Kazanan:** **MVUE ve Bayes Grubu**.

#### Senaryo B: $m \approx \frac{k}{k+1}N$ (Normal / Beklenen Durum — $m/N \approx 85\% - 95\%$)
* **Ne Olur?** Dağılım teorik beklenen değere uygun şekilde gerçekleşmiştir.
* **MVUE ve Bayes:** Tam olarak tasarlanan matematiksel dengeyi yakalar; eklenen $+m/k$ payı $N - m$ boşluğunu neredeyse kusursuz kapatır.
* **Kazanan:** **MVUE (%1-4 hata) ve Bayes Medyan**.

#### Senaryo C: $m \approx N$ (Erken Yakalama / Şanslı Örneklem — $m/N > 97\%$)
* **Ne Olur?** İlk birkaç çekilişte şans eseri popülasyonun en tepesindeki tanklardan biri ele geçirilmiştir (Örn: $N=1000$ iken $m=992$).
* **Bilinen Maks (BM):** Neredeyse 0 hata (%0.8) ile **tüm yöntemleri ezer geçer**.
* **MVUE ve Bayes Ortalama:** $m$'nin zaten $N$'ye ulaştığını "bilmedikleri" için kural gereği üzerine $+m/k$ (örneğin $+99$ daha) eklerler. Tahmin $\hat{N} = 1091$ çıkar ve **aşırı tahmin (overestimation)** hatası yaparlar.
* **Bayes Medyan Farkı:** Bayes Medyan'ın çarpanı ($0.693/k$), MVUE'nin çarpanından ($1/k$) daha küçük olduğu için bu senaryoda MVUE kadar aşırı şişmez ve daha az hata yapar.
* **Kazanan:** **Bilinen Maksimum (BM)**.

---

### 3. Asimetri ve Yanlılık (Bias) İkilemi

| Yöntem | Hata Yönü | $m \to N$ Olduğunda Risk |
| :--- | :--- | :--- |
| **Bilinen Maks ($m$)** | **Tek Taraflı (Her zaman $\le N$)** | Sıfır risk. $m=N$ olduğunda hatası tam 0'dır. |
| **MVUE & Bayes** | **Çift Taraflı (Bazen $\le N$, bazen $\ge N$)** | $m \approx N$ olduğunda yukarı doğru aşırı sapma (overestimate) riski taşır. |

> [!TIP]
> **Önemli Çıkarım:**
> MVUE'nin uzun vadede "yansız" (unbiased) olmasının sırrı tam olarak buradadır: $m$ küçükken yaptığı yukarı doğru düzeltmeler ile, $m$ büyükken yaptığı yukarı doğru aşırı tahminler birbirini ortalamada **tam sıfırlar**.

---

## 📌 Sonuç ve Genel Özet

1. **Yeterli İstatistiğin Gücü:** $N$ tahmininde tüm cevher $m$ (maksimum) ve $k$ (gözlem sayısı) değişkenindedir. Aritmetik ortalama ($2\bar{x}$) veya medyan gibi tüm seriyi hesaba katan yöntemler gürültüden ötürü her zaman geride kalır.
2. **Kutsal Üçlü (MVUE, Bayes Medyan, Bayes Ortalama):** Matematiksel olarak teorik sınıra ulaşmış yöntemlerdir. Küçük gözlemlerde ($k \le 10$) Bayes Medyan aşırı tahmini frenlediği için hafifçe öne çıkabilirken, büyük gözlemlerde bu üçü tamamen birleşir.
3. **$k$ Büyüdükçe Tek Kazanan $m$ Olur:** $k$ arttıkça $P(m = N) = k/N$ olasılığı $1$'e gider, belirsizlik payı ($m/k$) sıfıra çöker ve tüm istatistiksel modeller **Bilinen Maksimum ($m$)** noktasında birleşir.
