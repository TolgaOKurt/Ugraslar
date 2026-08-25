# PSPP (Postage Stamp with Subtraction) Dizi ve Arama Kuralları

Bu doküman, PSPP probleminde kullanılan matematiksel kuralları iki temel kategoriye ayırarak açıklar:
1. **Kesin Matematiksel Kanunlar** (Evrensel, ispatlı ve asla optimal kaçırmayan kurallar).
2. **Sezgisel ve Morfolojik Arama Kısıtları** (Arama uzayını daraltan, belirli aileleri hedefleyen ancak diğer ailelerdeki çözümleri kaçırabilen kısıtlar).

---

# BÖLÜM 1: KESİN MATEMATİKSEL KANUNLAR
*(Evrensel İspatlı Kurallar — Kesin Optimum Arayan Her Algoritmada Kullanılmak Zorundadır)*

Bu kurallar matematiksel kesinlik taşır. Bu kurallara uymayan hiçbir dizi hedef skora ulaşamaz; dolayısıyla bu kurallarla budama yapmak **asla bir optimal çözümü kaçırmaz.**

---

### Kanun 1: Tepe Kapanış Kanunu ve Dinamik Alt Eşik (M <= 2 * P_son)

> **Teorem:**
> Bir pozitif tamsayı kümesinde (P) iki elemanın toplamıyla oluşturulabilecek en büyük sayı, kümenin en büyük elemanının iki katıdır:
> `M_max = 2 * max(P) = 2 * P_son`
> (Delta cinsinden: `M_max = 2 * sum(delta)`)

- **Gerekçe:**  
  Kümelerdeki en büyük iki elemanın toplamı `P_son + P_son = 2 * P_son`'dur. Farklar ise her zaman `P_son - P_0 < P_son` olacağı için tepe noktayı belirleyen tek işlem toplamadır.

- **`bilinen_max` Bir Üst Limit Değil, KESİNLİKLE BİR ALT LİMİTTİR (Taban Çıtası):**
  Arama algoritmalarında hedef skoru yukarıdan asla sınırlandırmayız; çünkü aradığımız yeni rekorlar ve küresel optimum daima daha yukarıdadır. 
  `bilinen_max` (veya anlık en yüksek skor), aramanın altına çekilen **geçilmez bir zemin (taban filtresi)** görevi görür:
  - **Aşağıdan Kısıtlama Mantığı:**  
    Eğer bir adayın veya tamamlanmamış bir dalın üretebileceği en yüksek teorik sayı olan `2 * P_son`, alt eşiğimiz olan `bilinen_max` değerine bile **ulaşamıyorsa** (`2 * P_son < bilinen_max`), o aday bu taban çıtasını aşamaz ve yetersiz olduğu için doğrudan elenir.
  - **Formül:**  
    `sum(delta) >= (bilinen_max + 1) // 2`  *(Adayın bu alt sınırı geçebilmesi için asgari toplam şartı)*

---

### Kanun 2: 1 Sayısının Üretilmesi ve Parite Kanunu

> **Teorem:**
> 1'den M'ye kadar kesintisiz bir menzil oluşması için, en küçük pozitif tam sayı olan `1`'in küme tarafından mutlaka üretilmesi şarttır.
> Bu ancak şu iki durumdan biriyle mümkündür:
> 1. `1` sayısı doğrudan kümenin içinde bulunmalıdır (`1 ∈ P`).
> 2. Kümede farkı `1` olan en az iki eleman bulunmalıdır (`p_j - p_i = 1`).

- **Parite İspatı:**  
  Eğer bir dizideki tüm elemanlar çift sayı ise; iki çift sayının toplamı da farkı da daima çift sayıdır. Dolayısıyla tek bir sayı bile (en başta `1`) üretilemez ve skor `M = 0` olur.
- **Kesin Budama Kuralı:**  
  Dizinin elemanlarının tamamı çift olamaz; en az bir tek sayı veya ardışık eleman çifti barındırmak zorundadır.

---

### Kanun 3: Güvercin Yuvası Mutlak Teorik Tavanı

> **Teorem:**
> p elemanlı bir kümenin üretebileceği toplam bağımsız işlem sayısı (çiftler toplamı ve farkları):
> `Kapasite = p * (p + 1)` adettir.
> Dolayısıyla sıfır çakışmalı bir evrende bile `M <= p * (p + 1)` sınırını aşamaz.

- **Son Eleman Tavanı:**  
  Dizinin son elemanı `P_son` asla `p * (p + 1)` değerinden büyük olamaz; aksi takdirde toplamlar hedef aralığın dışına taşar ve aralık kesintiye uğrar.

---

# BÖLÜM 2: SEZGİSEL VE MORFOLOJİK ARAMA KISITLARI
*(Heuristic / Aile Bazlı Kurallar — Arama Uzayını Küçültür, Belirli Aileleri Hızlandırır Ama Genel Optimumu Kaçırabilir)*

Bu kurallar evrensel birer matematiksel kanun **değildir**. Milyarlarca kombinasyonu saniyelere indirmek için belirli morfolojik ailelere (örneğin Aile 1: Sıkı Modüler Gövde) özel olarak uygulanır. 

> **ÖNEMLİ:**  
> Eğer bir arama motoru *"Ben tüm uzayı tarayıp kesin küresel optimumu bulacağım"* diyorsa, aşağıdaki sezgisel kısıtları **KULLANMAMALIDIR**. 
> Çünkü bu kısıtlar arama bölgesini daraltırken diğer ailelerdeki (örneğin Aile 3: Uç Sıçraması) geçerli ve rekor çözümleri kaçırabilir!

---

### Kısıt 1: Dinamik Alt Eşik Çıtası (bilinen_max Asla Bir Üst Sınır Olamaz)

- **Temel İlke:**  
  Optimum ve yeni rekorlar daima yukarıdadır; dolayısıyla arama alanı yukarıdan asla kapatılmaz. `bilinen_max`, arama motoruna sadece **aşağıdan bir zemin sınırı (alt eşik)** çeker.
- **Dinamik Yükselme Mekanizması:**  
  Arama motoru çalışırken daha yüksek bir skor bulduğunda (örneğin 130 -> 140), bu zemin çıtası anında 140'a yükseltilir. Artık 140'ın altında kalan hiçbir aday incelenmez; ancak 140'ın üstü (141, 142, ...) tamamen serbest ve açıktır.

---

### Kısıt 2: Kuyruk Daralma Kısıtı (Tail Contraction)

- **Açıklama:** Arama motorlarının kuyruk sonundaki adımları `1, 2, 3` gibi küçük sayılarla sınırlamasıdır.
- **Neden Her Yerde Geçerli Değildir?**  
  - **Aile 1 (Modüler Gövde):** Bu kural Aile 1 için mükemmel çalışır; kuyruk kapanışı `[..., 2, 1, 1]` ile biter.
  - **Aile 3 (Uç Sıçraması):** P = 7 rekorumuz olan `[5, 6, 2, 4, 2, 1, 20]` çözümünde son eleman daralmak yerine **20 gibi devasa bir sıçrama** yapmıştır.
- **Sonuç:** Kuyruk daralma kuralı evrensel değildir; yalnızca **Modüler Gövde Arayıcılarında (Aile 1)** kullanılabilir. Genel çözücüler son adımı serbest bırakmalıdır.

---

### Kısıt 3: Sabit Taban ve Gövde Uzunluğu Kısıtı

- **Açıklama:** Dizinin başında peş peşe `k` adet aynı taban sayısının (`[10, 10, 10, ...]`) zorunlu tutulmasıdır.
- **Neden Optimal Kaçırabilir?**  
  Bu kısıt, asimetrik veya çok tabanlı (örneğin `[5, 3, 11, 1, 2, 1, 6, 26]`) çözümleri arama uzayının tamamen dışına atar.
- **Kullanım Yeri:** Yalnızca hedefli modüler faz geçişi aramalarında (`evrensel_arama_motoru.c`) yüksek hız elde etmek için kullanılır.

---

### Kısıt 4: Ortanca ve İkinci Eleman Sınırları

- **Açıklama:** `dizi[1] <= p * dizi[0]` veya ortanca eleman sınırı gibi deneysel gözlemler.
- **Neden Optimal Kaçırabilir?**  
  Küçük P değerlerinde tutarlı gözükse bile, asimetrik uç sıçramalarında veya hibrit bloklarda bu sınırları aşan uç çözümler bulunabilir.

---

# 📊 Özet Karşılaştırma Tablosu

| Kural / Kısıt | Kategori | İspatlı mı? | Genel Optimumu Kaçırır mı? | Hangi Solver'da Kullanılmalı? |
| :--- | :---: | :---: | :---: | :--- |
| **M <= 2 * P_son (Tepe Kapanış)** | **Kesin Kanun** | EVET | **ASLA KAÇIRMAZ** | Bütün Solver'lar (Evrensel) |
| **1'in Üretimi & Parite Kuralı** | **Kesin Kanun** | EVET | **ASLA KAÇIRMAZ** | Bütün Solver'lar (Evrensel) |
| **P_son <= P*(P+1) (Teorik Tavan)**| **Kesin Kanun** | EVET | **ASLA KAÇIRMAZ** | Bütün Solver'lar (Evrensel) |
| **Kuyruk Daralma Kuralı** | **Sezgisel Kısıt**| HAYIR | **Kaçırabilir (Aile 3'ü eler)** | Sadece Modüler Taban Motorları |
| **Gövde Tekrarı Kısıtı** | **Sezgisel Kısıt**| HAYIR | **Kaçırabilir (Asimetriyi eler)**| Sadece Taban Arama Motorları |
| **Statik bilinen_max Sınırı** | **Sezgisel Kısıt**| HAYIR | **Kaçırabilir** | Sadece Dinamik Çıta Olarak Kullanılmalı |
