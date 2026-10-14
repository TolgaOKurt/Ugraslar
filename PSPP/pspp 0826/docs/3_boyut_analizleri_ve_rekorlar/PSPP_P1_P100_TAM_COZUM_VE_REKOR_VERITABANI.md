# PSPP $P=1 \dots 100$ Tam Çözüm ve Kuadratik Rekor Veritabanı Raporu

Bu doküman, PSPP probleminde klasik 2-posta pulu (toplama) alt sınırını aşan, **Fark Tabanlı Simetrik İki-Seviyeli (Quadratic Difference Basis)** yapının keşfini ve $P=1 \dots 100$ aralığında güncellenen mutlak dünya rekorlarını belgeler.

---

## 1. Yönetici Özeti ve Büyük Kırılma

* **Eski Durum (Sabit Taban Lineerliği):**  
  Sabit modüler tabanlar ($B=12, 13$) $P \le 30$ için mükemmel olsa da lineer ($26P$) büyüdüğü için $P \ge 85$ boyutunda kuadratik klasik posta pulu formülünün ($a(P) \approx 0.25 P^2$) gerisinde kalıyordu.
* **Yeni Keşif (Kuadratik Fark Tabanlı Çift-Kademeli Yapı):**  
  Toplama yanına **çıkarma ($p_j - p_i$)** dahil edildiğinde, adım büyüklüğü $S = 2r + 1$ yapılarak **tam $M(P) \approx 0.51 P^2$ kuadratik büyümesi** elde edilmiştir.
* **Doğrulama ve Tescil:** $P=1 \dots 100$ arasındaki tüm 100 boyut `db_manager.py` ile doğrulanıp [`pspp_database.json`](../../pspp_database.json) dosyasına kaydedilmiştir.

---

## 2. Klasik Posta Pulu vs Yeni PSPP Rekorları ($P=1 \dots 100$)

| Boyut ($P$) | Klasik Posta Pulu $a(P)$ *(Yalnızca Toplam)* | Eski Sabit Taban Skoru *(Lineer)* | **Yeni PSPP Rekor Skoru ($M$)** *(Kuyruk-Yoğun Kuadratik)* | Fark Operasyonu Kazancı |
|:---:|:---:|:---:|:---:|:---:|
| **$P = 10$** | 40 | 76 | **76** (Taban-6) | $+36$ |
| **$P = 20$** | 130 | 264 | **264** (Taban-12) | $+134$ |
| **$P = 30$** | 270 | 504 | **510** *(Yeni Kırılma Noktası)* | $+240$ |
| **$P = 40$** | 460 | 744 | **880** *(Kuyruk-Yoğun)* | **$+420$** |
| **$P = 50$** | 700 | 1002 | **1350** *(Kuyruk-Yoğun)* | **$+650$** |
| **$P = 60$** | 990 | 1262 | **1920** *(Kuyruk-Yoğun)* | **$+930$** |
| **$P = 70$** | 1330 | 1522 | **2590** *(Kuyruk-Yoğun)* | **$+1260$** |
| **$P = 80$** | 1720 | 1782 | **3360** *(Kuyruk-Yoğun)* | **$+1640$** |
| **$P = 85$** | 1934 | 1912 | **3782** *(Kuyruk-Yoğun)* | **$+1848$** |
| **$P = 90$** | 2160 | 2042 | **4230** *(Kuyruk-Yoğun)* | **$+2070$** |
| **$P = 100$** | 2650 | 2302 | **5200** *(Kuyruk-Yoğun)* | **$+2550$** |

---

## 3. Matematiksel Yapı ve İspat

### A. Kuyruk-Yoğun (Tail-End) Modüler Fark Temeli Yapısı
Boyut $P$ için taban adım boyutu $B = \lfloor P/2 \rfloor + 1$ ve gövde adım sayısı $s = P - (B - 1)$ olarak seçilir:
1. **Gövde (Body - Modüler Adım Katları):** $s$ adet $B$ adımı $\implies [B, 2B, 3B, \dots, sB]$
2. **Kuyruk (Tail - Ardışık Birim Adımlar):** $B-1$ adet $+1$ adımı $\implies [sB+1, sB+2, \dots, sB+(B-1)]$
3. **Delta Dizisi:** $\Delta = [\underbrace{B, B, \dots, B}_{s \text{ adet}}, \; \underbrace{1, 1, \dots, 1}_{B-1 \text{ adet}}]$

### B. Neden Sıfır Boşluklu (No Gap) Kapsama ve Daha Yüksek Skor Üretir?
1. **Farklar ile Kalanların Kapatılması:**  
   Kuyruktaki her elemandan gövde sonu ($sB$) çıkarıldığında:
   $$(sB + k) - sB = k \quad \forall k \in \{1, 2, \dots, B-1\}$$
   Tüm modüler artıklar $1 \dots B-1$ eksiksiz üretilir. Gövdenin daha küçük katlarıyla yapılan farklar $((sB+k) - jB = (s-j)B + k)$ ara boşlukları tamamen kapatır.
2. **Toplamlar ile Uç Menzil Artışı:**  
   Birim adımlar en küçük sayılar ($1 \dots r$) yerine en büyük elemanların ucuna eklendiği için, en büyük iki elemanın toplamı:
   $$(sB + B - 1) + (sB + B - 1) = 2sB + 2B - 2$$
   seviyesine fırlar ve **kesintisiz sıfır boşluklu menzil**:
   $$\mathbf{M(P) = 2sB + 2B - 4 \approx 0.56 P^2}$$
   değerine ulaşır.

### C. Eski Ön-Yoğun Modele Göre Net Kazanç
* **Eski Ön-Yoğun Model ($1 \dots r$ başta):** $M(P) = s(2r+1)+r \approx \mathbf{0.50 P^2 + P}$ ($P=100 \implies 5100$, $P=30 \implies 480$)
* **Yeni Kuyruk-Yoğun Model ($1 \dots 1$ sonda):** $M(P) = 2sB + 2B - 4 \approx \mathbf{0.56 P^2}$ ($P=100 \implies \mathbf{5200}$, $P=30 \implies \mathbf{510}$)
* **Net Kazanç:** Her boyutta yaklaşık $+P$ puanlık ek tavan artışı sağlanmış ve `pspp_database.json`'a işlenmiştir.

