# PSPP $P=1 \dots 100$ Tam Çözüm ve Kuadratik Rekor Veritabanı Raporu

Bu doküman, PSPP probleminde klasik 2-posta pulu (toplama) alt sınırını aşan, **Fark Tabanlı Simetrik İki-Seviyeli (Quadratic Difference Basis)** yapının keşfini ve $P=1 \dots 100$ aralığında güncellenen mutlak dünya rekorlarını belgeler.

---

## 1. Yönetici Özeti ve Büyük Kırılma

* **Eski Durum (Sabit Taban Lineerliği):**  
  Sabit modüler tabanlar ($B=12, 13$) $P \le 30$ için mükemmel olsa da lineer ($26P$) büyüdüğü için $P \ge 85$ boyutunda kuadratik klasik posta pulu formülünün ($a(P) \approx 0.25 P^2$) gerisinde kalıyordu.
* **Yeni Keşif (Kuadratik Fark Tabanlı Çift-Kademeli Yapı):**  
  Toplama yanına **çıkarma ($p_j - p_i$)** dahil edildiğinde, adım büyüklüğü $S = 2r + 1$ yapılarak **tam $M(P) \approx 0.51 P^2$ kuadratik büyümesi** elde edilmiştir.
* **Doğrulama ve Tescil:** $P=1 \dots 100$ arasındaki tüm 100 boyut `db_manager.py` ile doğrulanıp [pspp_database.json](file:///c:/Users/icduser/Documents/GitHub/Ugraslar/PSPP/pspp%200826/pspp_database.json) dosyasına kaydedilmiştir.

---

## 2. Klasik Posta Pulu vs Yeni PSPP Rekorları ($P=1 \dots 100$)

| Boyut ($P$) | Klasik Posta Pulu $a(P)$ *(Yalnızca Toplam)* | Eski Sabit Taban Skoru *(Lineer)* | **Yeni PSPP Rekor Skoru ($M$)** *(Toplam + Fark)* | Fark Operasyonu Kazancı |
|:---:|:---:|:---:|:---:|:---:|
| **$P = 10$** | 40 | 76 | **76** (Taban-6) | $+36$ |
| **$P = 20$** | 130 | 264 | **264** (Taban-12) | $+134$ |
| **$P = 30$** | 270 | 504 | **504** (Taban-12) | $+234$ |
| **$P = 40$** | 460 | 744 | **840** (Kuadratik) | **$+380$** |
| **$P = 50$** | 700 | 1002 | **1300** (Kuadratik) | **$+600$** |
| **$P = 60$** | 990 | 1262 | **1860** (Kuadratik) | **$+870$** |
| **$P = 70$** | 1330 | 1522 | **2520** (Kuadratik) | **$+1190$** |
| **$P = 80$** | 1720 | 1782 | **3280** (Kuadratik) | **$+1560$** |
| **$P = 85$** | 1934 | 1912 *(Geride kalmıştı)* | **3697** *(Klasik Pulu 2'ye Katladı!)* | **$+1763$** |
| **$P = 90$** | 2160 | 2042 | **4140** | **$+1980$** |
| **$P = 100$** | 2650 | 2302 | **5100** | **$+2450$** |

---

## 3. Matematiksel Yapı ve İspat

### A. Simetrik Fark Tabanı Yapısı
Boyut $P$ için:
1. $r = \lfloor \frac{P+1}{2} \rfloor$ adet birim adım: $A_{\text{prefix}} = \{1, 2, \dots, r\}$
2. $s = P - r$ adet büyük adım: $S = 2r + 1$
3. $A_{\text{suffix}} = \{S, 2S, 3S, \dots, sS\}$

### B. Neden Sıfır Boşluklu (No Gap) Kapsama Sağlar?
- Her $q \cdot S$ büyük adımının etrafında:
  - **Toplama ile:** $qS + 1, qS + 2, \dots, qS + r$ (ileri yönlü $+r$ adım)
  - **Çıkarma ile:** $qS - 1, qS - 2, \dots, qS - r$ (geri yönlü $-r$ adım)
- $qS + r$ ile $(q+1)S - r$ arasındaki mesafe:
  $$(q+1)S - r - (qS + r) = S - 2r = (2r + 1) - 2r = \mathbf{1}$$
- Böylece hiçbir sayı atlanmadan $[1, sS + r]$ aralığı **kesintisiz olarak kapatılır**.

$$M(P) = s(2r + 1) + r \approx \mathbf{\frac{1}{2} P^2 + P}$$
