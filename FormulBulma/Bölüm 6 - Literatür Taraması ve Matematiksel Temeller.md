# Bölüm 6: Literatür Taraması ve Matematiksel Temeller (Akademik Karşılıklar)

> [!IMPORTANT]
> **Genel Bakış:**
> Bu çalışmada (Bölüm 1'den 5'e kadar) geliştirdiğimiz "Hat Başı Analizi", "Sonlu Farklar Piramidi" ve "Dizi İçinde Dizi Mimarisi" kavramları; matematikte **Sonlu Kalkülüs (Finite Calculus)**, **Binom Dönüşümü (Binomial Transform)**, **Umbral Kalkülüs (Umbral Calculus)** ve **Kombinatorik Dizi Analizi (Analytic Combinatorics)** literatürünün temel taşlarını oluşturmaktadır.
> Bu belgede kullandığımız formüllerin, operatörlerin ve teoremlerin evrensel akademik literatürdeki karşılıkları, kaynakları ve ilişkili teorileri sunulmaktadır.

---

## 1. Terminoloji Eşleme Tablosu (Bizim Modelimiz $\longleftrightarrow$ Literatür)

| Geliştirdiğimiz Kavram / Formül | Matematiksel Literatürdeki Adı | Uluslararası / İngilizce Literatür |
| :--- | :--- | :--- |
| **Sonlu Farklar Piramidi** | Fark Tablosu / Fark Üçgeni | *Finite Difference Table / Difference Triangle* |
| **Hat Başı Dizisi ($H$)** | Öncü Farklar / Binom Dönüşümü | *Leading Differences / Binomial Transform* |
| **$a_n = \sum \binom{n-1}{k} h_k$** | Gregory-Newton İleri Fark İnterpolasyonu | *Gregory-Newton Forward Difference Formula* |
| **$h_k = k! \cdot S(m,k)$ (Hat Başı)** | Stirling Taban Dönüşümü | *Stirling Numbers of the 2nd Kind / Monomial Basis* |
| **$a \cdot (a-1)^k$ (Üstel Hat Başı)** | Üstel Fonksiyonun Fark Türevi | *Forward Difference of Exponential Functions* |
| **$- + - +$ Fibonacci Dizilimi** | Negatif İndisli Fibonacci | *Negafibonacci Sequence ($F_{-n}$)* |
| **Dizi İçinde Dizi Mimarisi** | Düşen Faktöriyel Tabanı / Umbral Seri | *Falling Factorial Basis / Binomial Sheffer Sequence* |

---

## 2. Temel Operatör Teorisi (Finite Difference Calculus)

Matematiksel literatürde (özellikle **George Boole, 1860** ve **Donald Knuth, 1994**) bu sistem üç temel lineer operatör ile ifade edilir:

1. **Birim Operatör ($I$):** $I(a_n) = a_n$
2. **Öteleme (Shift) Operatörü ($E$):** $E(a_n) = a_{n+1}$
3. **İleri Fark (Forward Difference) Operatörü ($\Delta$):** 
   $$\Delta = E - I \implies \Delta a_n = a_{n+1} - a_n$$

### Binom Teoremi ile Operatör İlişkisi:
$k$. mertebeden fark operatörü doğrudan binom açılımı ile verilir:
$$\Delta^k = (E - I)^k = \sum_{j=0}^k (-1)^{k-j} \binom{k}{j} E^j$$

Bu operatör bir dizinin ilk elemanına ($a_1$) uygulandığında, bizim **"Hat Başı Formülü"** olarak adlandırdığımız eşitlik elde edilir:
$$\mathbf{h_k = \Delta^k a_1 = \sum_{j=0}^k (-1)^{k-j} \binom{k}{j} a_{j+1}}$$

Literatürde bu işleme bir dizinin **Euler / Binom Dönüşümü (Binomial Transform)** adı verilir.

---

## 3. Gregory-Newton İleri Fark İnterpolasyon Formülü

Literatürde **James Gregory (1670)** ve **Isaac Newton (1687)** tarafından keşfedilen bu formül, ayrık kalkülüsün Taylor serisi karşılığıdır:

$$f(x) = f(1) + \frac{\Delta f(1)}{1!} (x-1) + \frac{\Delta^2 f(1)}{2!} (x-1)(x-2) + \dots + \frac{\Delta^k f(1)}{k!} (x-1)_k$$

Burada $(x-1)_k = (x-1)(x-2)\dots(x-k)$ **Düşen Faktöriyel (Falling Factorial)** fonksiyonudur.

Kombinasyon tanımı $\binom{n-1}{k} = \frac{(n-1)_k}{k!}$ kullanıldığında formül tam olarak Bölüm 5'te kurduğumuz formüle dönüşür:
$$\mathbf{a_n = \sum_{k=0}^\infty \binom{n - 1}{k} h_k}$$

---

## 4. Stirling Sayıları ve Taban Dönüşüm Teoremi

Polinomların hat başlarında ortaya çıkan $(1, 1)$, $(1, 3, 2)$, $(1, 7, 12, 6)$, $(1, 15, 50, 60, 24)$ dizilimleri literatürde **Monom Tabanından Düşen Faktöriyel Tabanına Geçiş** teoremidir:

$$x^m = \sum_{k=0}^m \left\{ \begin{matrix} m \\ k \end{matrix} \right\} (x)_k = \sum_{k=0}^m \left\{ \begin{matrix} m \\ k \end{matrix} \right\} k! \binom{x}{k}$$

- Burada $\left\{ \begin{matrix} m \\ k \end{matrix} \right\}$ veya $S(m,k)$, **2. Tür Stirling Sayılarıdır (Stirling Numbers of the Second Kind)**.
- Bu sayılar, $m$ elemanlı bir kümeyi $k$ adet boş olmayan alt kümeye bölme sayısını ifade eder.
- `FormulBulma/Stirling Hesaplayıcı/Stirling Hesaplayıcı.c` programınızın $S(a,b) = S(a-1,b-1) + b \cdot S(a-1,b)$ reküransı ile hesapladığı değerler tam olarak bu taban matrisinin katsayılarıdır.

---

## 5. Özel Dizi Ailelerinin Literatürdeki Karşılıkları

### 5.1. Üstel Dizilerin Fark Kalkülüsü
Literatürde üstel fonksiyonların fark türevi:
$$\Delta (a^x) = a^{x+1} - a^x = a^x (a - 1)$$
Sürekli matematikteki $e^x$'in türevinin kendisine eşit olması gibi ($\frac{d}{dx} 2^x = \ln 2 \cdot 2^x$), ayrık matematikte de **$2^x$ fonksiyonunun fark türevi doğrudan kendisine eşittir**:
$$\Delta (2^x) = 2^x (2 - 1) = 2^x$$
Bu nedenle $2^x$ dizisinin hat başları literatürde **$\Delta$-sabit (invariable under forward difference)** olarak bilinir: $(2, 2, 2, 2, \dots)$.

---

### 5.2. Negafibonacci ve Negalucas Sayıları
Fibonacci dizisinin fark tablosunun sol kenarında ortaya çıkan $0, 1, -1, 2, -3, 5, -8, 13, \dots$ dizilimi literatürde **Donald Knuth (1968)** ve **Edouard Lucas (1878)** tarafından incelenen **Negafibonacci ($F_{-n}$)** teoremi ile tanımlanır:

$$F_{-n} = (-1)^{n+1} F_n$$
$$L_{-n} = (-1)^n L_n$$

Fark operatörü $\Delta F_n = F_{n-1}$ olduğundan, ileri fark almak Fibonacci dizisinde indisi 1 eksiltmeye eşdeğerdir. Bu sebeple fark piramidinin sol kenarı Fibonacci dizisinin **zaman içinde geriye doğru uzantısını (negatif zaman / negatif indis)** çizer.

---

### 5.3. Harmonik Dizi ve Alternans Euler İnversiyonu
Harmonik dizinin ($1/n$) hat başlarında elde ettiğimiz $\frac{(-1)^k}{k+1}$ katsayıları, literatürde **Mercator Serisi** ve **Euler-Mascheroni Sabiti ($\gamma$)** türetimlerinde kullanılan binom inversiyonudur:
$$\sum_{j=0}^k (-1)^{k-j} \binom{k}{j} \frac{1}{j+1} = \frac{(-1)^k}{k+1} \implies \ln(1+x) \text{ katsayıları}$$

---

### 5.4. Faktöriyel Dizisi ve Subfaktöriyeller (Derangements)
$n!$ dizisinin hat başları $(1, 1, 3, 11, 53, 309, 2119, \dots)$, kombinatorik literatürde **Euler Faktöriyel Farkları** ve **Düzensizlik Sayıları (Subfactorials / Derangements - $!n$)** ile ilişkilidir (OEIS A000166 / A000522).

---

## 6. OEIS (On-Line Encyclopedia of Integer Sequences) Eşleşmeleri

Bu çalışmada incelediğimiz dizilerin ve hat başı dizilerinin uluslararası **OEIS** kayıtları:

| Dizi / Hat Başı Dizisi | OEIS Kodu | Tanım / Literatürdeki Adı |
| :--- | :---: | :--- |
| **Fibonacci Dizisi** | [A000045](https://oeis.org/A000045) | Fibonacci Numbers: $F(n) = F(n-1) + F(n-2)$ |
| **Lucas Dizisi** | [A000032](https://oeis.org/A000032) | Lucas Numbers: $L(n) = L(n-1) + L(n-2)$ |
| **Negafibonacci** | [A039834](https://oeis.org/A039834) | Extended Fibonacci sequence to negative indices |
| **2. Tür Stirling Sayıları** | [A008277](https://oeis.org/A008277) | Stirling numbers of 2nd kind $S(n,k)$ |
| **Faktöriyel Farkları** | [A000522](https://oeis.org/A000522) | Total number of arrangements: $a(n) = n \cdot a(n-1) + 1$ |
| **Recamán Dizisi** | [A005132](https://oeis.org/A005132) | Recamán's sequence: $a(n) = a(n-1) \pm n$ |
| **Tam Kareler Hat Başı** | $(1, 3, 2)$ | $\Delta^k(1^2)$ Monomial differences |
| **Küp Sayılar Hat Başı** | $(1, 7, 12, 6)$ | $\Delta^k(1^3)$ Monomial differences |

---

## 7. Önerilen Akademik Kaynaklar ve İleri Okuma

Bu konularda derinleşmek isteyenler için temel akademik başvuru kaynakları:

1. **Ronald L. Graham, Donald E. Knuth, Oren Patashnik (1994):**
   * *Concrete Mathematics: A Foundation for Computer Science* (Addison-Wesley).
   * Özellikle **Bölüm 2 (Sums & Finite Calculus)** ve **Bölüm 6 (Special Numbers - Stirling & Fibonacci)**.
2. **George Boole (1860):**
   * *A Treatise on the Calculus of Finite Differences* (Macmillan and Co.).
   * Sonlu farklar kalkülüsünün klasik başyapıtı.
3. **Steven Roman (1984):**
   * *The Umbral Calculus* (Academic Press).
   * Polinom taban dönüşümleri ve Binom Şablonları teorisi.
4. **Philippe Flajolet, Robert Sedgewick (2009):**
   * *Analytic Combinatorics* (Cambridge University Press).
   * Üreteç fonksiyonları ve dizi dönüşümleri.
5. **Eric W. Weisstein:**
   * *Binomial Transform*, *Newton's Forward Difference Formula*, *Stirling Transform* (Wolfram MathWorld).
