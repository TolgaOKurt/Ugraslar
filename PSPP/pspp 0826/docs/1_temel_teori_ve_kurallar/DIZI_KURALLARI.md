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
> Bu ancak dizide en az bir `1` elemanı bulunmasıyla (`∃k: delta[k] = 1`) mümkündür.
> (Çünkü tüm `delta[k] >= 2` olduğunda tüm tekiller >= 2, tüm toplamlar >= 4 ve tüm farklar >= 2 olur).

- **Parite İspatı:**  
  Eğer bir dizideki tüm elemanlar çift sayı ise; iki çift sayının toplamı da farkı da daima çift sayıdır. Dolayısıyla tek bir sayı bile (en başta `1`) üretilemez ve skor `M = 0` olur.
- **Kesin Budama Kuralı:**  
  Dizinin elemanlarının tamamı çift olamaz; dizide en az bir tane `delta[k] = 1` bulunmak zorundadır.

---

### Kanun 2b: 2 Sayısının Üretilmesi Kanunu

> **Teorem:**
> 1'den M'ye kadar kesintisiz bir menzil için `2` sayısının da mutlaka üretilmesi şarttır.
> PSPP işlem uzayında (`p_i`, `p_i + p_j`, `p_j - p_i`) $2$ sayısının oluşabilmesi için aşağıdaki **3 durumdan en az birinin** sağlanması zorunludur:
> 1. `delta[0] = 1` (yani `p_0 = 1` olup `p_0 + p_0 = 1 + 1 = 2` toplamı ile),
> 2. Dizide herhangi bir yerde `delta[k] = 2` bulunması (`p_0 = 2` veya `p_k - p_{k-1} = 2` farkı ile),
> 3. Dizide yan yana iki adet `1` bulunması (`[..., 1, 1, ...]` olup `p_{k+1} - p_{k-1} = 1 + 1 = 2` farkı ile).

- **Matematiksel İspat:**  
  Eğer `delta[0] != 1`, dizide hiç `2` yok ve peş peşe `[1, 1]` yoksa:
  - Tekiller: `p_0 >= 2`, diğer `p_i >= 3` olur (`p_i = 2` olamaz).
  - Toplamlar: En küçük toplam `p_0 + p_0 >= 2 + 2 = 4 > 2` olur (`sum = 2` olamaz).
  - Farklar: Tüm tekil adımlar `delta >= 3` veya izole `1`'ler olduğundan hiçbir alt aralık toplamı $2$ yapamaz.
  - Dolayısıyla $2$ sayısı üretilemez ve skor **$M \le 1$** ile sınırlı kalır.
- **Kesin Budama Kuralı:**  
  Ters DFS sırasında `depth == 0`'a gelindiğinde dizide `2` veya `[1, 1]` yoksa, ilk eleman **yalnızca $\delta_0 = 1$ veya $\delta_0 = 2$** olabilir; $\delta_0 \ge 3$ olan tüm dallar anında budanır.

---

### Kanun 2c: p0 - 1 Sayısının Üretilmesi Kanunu (p0 >= 4 İçin)

> **Teorem:**
> Dizinin en küçük elemanı $p_0 = \delta_0 \ge 4$ olduğunda, $(p_0 - 1 \ge 3)$ sayısının üretilebilmesi için dizide **farkı $p_0 - 1$ olan en az iki eleman ($p_j - p_i = p_0 - 1$) bulunması zorunludur.**
> Başka bir deyişle, $\delta_1, \dots, \delta_{P-1}$ kuyruk elemanlarının ardışık alt toplamlarından en az biri $p_0 - 1$ değerine eşit olmak zorundadır.

- **Matematiksel İspat:**  
  - **Tekiller:** Tüm $p_i \ge p_0 > p_0 - 1$ olduğundan hiçbir tekil pul $p_0 - 1$ olamaz.
  - **Toplamlar:** En küçük toplam $p_0 + p_0 = 2p_0 > p_0 - 1$ olduğundan hiçbir toplam $p_0 - 1$ olamaz.
  - **Farklar:** Dolayısıyla $(p_0 - 1)$ sayısı YALNIZCA iki pulun farkından ($p_j - p_i = \sum_{k=i+1}^j \delta_k$) gelebilir.
  - Eğer $\delta_1 \dots \delta_{P-1}$ farkları arasında $p_0 - 1$ yoksa, $p_0 - 1$ sayısı kesinlikle üretilemez ve zincir o noktada koparak **$M \le p_0 - 2$** alır (optimum olamaz).
- **Kesin Budama Kuralı:**  
  Ters DFS'te `depth == 0`'a gelindiğinde, `delta[1]..delta[P-1]` elemanlarının ürettiği tüm farklar bir bitmask (`diff_mask`) içine alınır. `d >= 4` için eğer `(d - 1)` bu maskede yoksa, o `d` seçeneği alt ağaca inilmeden **O(1)'de anında budanır.** (`p0 - 1 = 1` ve `p0 - 1 = 2` durumları zaten Kanun 2 ve Kanun 2b tarafından denetlendiği için bu kural `p0 >= 4` için çalışır).

---

### Kanun 2d: 3 Sayısının Üretilmesi Kanunu

> **Teorem:**
> 1'den M'ye kadar kesintisiz bir menzil için `3` sayısının da mutlaka üretilmesi şarttır.
> PSPP işlem uzayında (`p_i`, `p_i + p_j`, `p_j - p_i`) 3 sayısının oluşabilmesi için aşağıdaki **4 durumdan en az birinin** sağlanması zorunludur:
> 1. Dizide herhangi bir yerde `delta[k] = 3` bulunması (`p0 = 3` veya tekil fark `3` ile),
> 2. Dizide herhangi bir yerde yan yana `[1, 2]` veya `[2, 1]` çifti bulunması (`1 + 2 = 3` farkı ile),
> 3. Dizide herhangi bir yerde peş peşe üç adet `1` bulunması (`[..., 1, 1, 1, ...]` olup `1 + 1 + 1 = 3` farkı ile),
> 4. Dizinin en başında iki adet `1` bulunması (`delta[0] = 1` ve `delta[1] = 1` olup `p0 + p1 = 1 + 2 = 3` toplamı ile).

- **Matematiksel İspat:**  
  3 sayısı pozitif tamsayılar kümesinde yalnızca `3`, `1 + 2` ve `1 + 1 + 1` olarak parçalanabilir.
  Eğer kuyrukta (`delta[1]..delta[P-1]`) `3`, `[1, 2]`, `[2, 1]` ve `[1, 1, 1]` yoksa:
  - `delta[0] = 3` seçilirse durum 1 sağlanır (geçerli).
  - `delta[0] = 2` seçilirse sadece `delta[1] = 1` ise `[2, 1]` oluşur (şartlı). `delta[1] != 1` ise 3 üretilemez!
  - `delta[0] = 1` seçilirse sadece `delta[1] = 2` (`[1, 2]`) veya `delta[1] = 1` (`1+2=3` toplamı) ise geçerlidir. `delta[1] >= 3` ise 3 üretilemez!
  - `delta[0] >= 4` ise tekil, toplam veya farktan 3 gelmesi imkansızdır.
  - Dolayısıyla bu şartları sağlamayan adaylarda 3 sayısı asla üretilemez ve skor **M <= 2** ile sınırlı kalır.
- **Kesin Budama Kuralı:**  
  Ters DFS sırasında `depth == 0`'a gelindiğinde kuyrukta 3 üretimi yoksa, `max_d = 3` sınırına çekilerek `d >= 4` olan tüm dallar ve geçersiz `d = 1, 2` durumları anında budanır.

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
| **2'nin Üretilmesi Kanunu (Kanun 2b)** | **Kesin Kanun** | EVET | **ASLA KAÇIRMAZ** | Bütün Solver'lar (Evrensel) |
| **p0 - 1 Üretimi Kanunu (Kanun 2c)** | **Kesin Kanun** | EVET | **ASLA KAÇIRMAZ** | Bütün Solver'lar (Evrensel) |
| **3'ün Üretilmesi Kanunu (Kanun 2d)** | **Kesin Kanun** | EVET | **ASLA KAÇIRMAZ** | Bütün Solver'lar (Evrensel) |
| **P_son <= P*(P+1) (Teorik Tavan)**| **Kesin Kanun** | EVET | **ASLA KAÇIRMAZ** | Bütün Solver'lar (Evrensel) |
| **Kuyruk Daralma Kuralı** | **Sezgisel Kısıt**| HAYIR | **Kaçırabilir (Aile 3'ü eler)** | Sadece Modüler Taban Motorları |
| **Gövde Tekrarı Kısıtı** | **Sezgisel Kısıt**| HAYIR | **Kaçırabilir (Asimetriyi eler)**| Sadece Taban Arama Motorları |
| **Statik bilinen_max Sınırı** | **Sezgisel Kısıt**| HAYIR | **Kaçırabilir** | Sadece Dinamik Çıta Olarak Kullanılmalı |
