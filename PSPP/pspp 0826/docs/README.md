# PSPP Proje Dokümantasyon Rehberi ve Dizin İndeksi

Bu dizin, **Postage Stamp Problem with Subtraction (PSPP / OEIS A398332)** projesine ait matematiksel kanunları, algoritma motorlarını, soy ağaçlarını, rekor analizlerini ve tarihi arşiv belgelerini düzenli alt klasörlerde barındırır.

Tüm belgeler **[`pspp_database.json`](../pspp_database.json)** merkezi veritabanı ile senkronize tutulmaktadır.

---

## 🌟 Ana ve Güncel Belgeler (Kök Dizin)

* **[`KESIF_TARIHCESI_VE_SURUM_HARITASI.md`](./KESIF_TARIHCESI_VE_SURUM_HARITASI.md):**  
  Projenin faz bazlı araştırma günlüğü ve belge geçerlilik haritası. Hangi tarihte hangi rekorun kırıldığını, hangi hipotezin hangi fazda aşıldığını ve belgelerin geçerlilik durumunu kronolojik olarak listeler.
* **[`oeis wiki page.md`](./oeis%20wiki%20page.md):**  
  Resmi OEIS Wiki profili için hazırlanmış ana referans dokümanı. Bütünsel, bağımsız, LaTeX/kısaltma karmaşasından arındırılmış, $P=1 \dots 100$ arasındaki en son doğrulanmış rekorları ($a(30)=510, a(100)=5200$) içeren yayın metni.
* **[`claude_opus_4_6_incelemesi.md`](./claude_opus_4_6_incelemesi.md):**  
  14 Ekim 2026 tarihli Claude Opus 4.6 (Thinking) sistem analizi, mevcut solver envanteri değerlendirmesi ve ileriye dönük yeni arama stratejileri önerileri.

---

## 📁 1. Temel Teori ve Kurallar (`1_temel_teori_ve_kurallar/`)
PSPP dizilerinin evrensel matematiksel tanımı, budama kanunları ve işlem uzayı kuralları.

* **[`DIZI_KURALLARI.md`](./1_temel_teori_ve_kurallar/DIZI_KURALLARI.md):**  
  PSPP probleminde geçerli kesin matematiksel kanunlar (Kanun 1: Tepe Kapanış $M \le 2P_{\text{son}}$, Kanun 2: 1'in Üretimi ve Parite, Kanun 2b: 2'nin Üretimi, Kanun 2c: $p_0-1$ Kuralı, Kanun 2d: 3'ün Üretimi, Kanun 3: Simetri İkizliği). Kesin arama yapan tüm çözücüler bu kanunları uygular.

---

## 📁 2. Algoritmalar ve Motorlar (`2_algoritmalar_ve_motorlar/`)
Kombinatoryel arama motorları, kök büyütme modelleri ve tohum tahmin algoritmaları.

* **[`SOLVER_ALGORITMASI.md`](./2_algoritmalar_ve_motorlar/SOLVER_ALGORITMASI.md):**  
  C tabanlı evrensel Ters DFS (Reverse Branch & Bound) ve 2048-bit donanımsal bitmask arama motoru mimarisi (`solver.c`).
* **[`KOK_BUYUTME_CALISMA_PRENSIBI.md`](./2_algoritmalar_ve_motorlar/KOK_BUYUTME_CALISMA_PRENSIBI.md):**  
  Kök büyütme motorunun (`root_grower.py`) çalışma prensibi, aday arama aralıkları ($min\_cand, max\_cand$), seçim sıralaması ve Delta analizi.
* **[`SAFE_AREA_DIZI_TAHMINI.md`](./2_algoritmalar_ve_motorlar/SAFE_AREA_DIZI_TAHMINI.md):**  
  $P < 9$ güvenli bölge özellikleri ve deterministik geçiş operatörleriyle sonraki boyutların (özellikle $P=6 \to P=7$) tahmin edilmesi.
* **[`SEED_BOYUTU_VE_VITES_STRATEJISI.md`](./2_algoritmalar_ve_motorlar/SEED_BOYUTU_VE_VITES_STRATEJISI.md):**  
  Küçük tohumlar ($P=3, 4$) ile büyük DB köklerinin ($P \ge 15$) karşılaştırması ve matematiksel "vites" prensibi.

---

## 📁 3. Boyut Analizleri, Soy Ağaçları ve Rekorlar (`3_boyut_analizleri_ve_rekorlar/`)
Belirli $P$ boyutlarına ait detaylı dizi analizleri, soy ağaçları ve tüm zamanların doğrulanmış rekorları.

* **[`P7_ANALIZI.md`](./3_boyut_analizleri_ve_rekorlar/P7_ANALIZI.md):**  
  $P=7$ ($M=40$) boyutu özel analizi, 8 optimum çözümün sınıflandırılması ve C vs Python performans karşılaştırması.
* **[`PSPP_DIZI_ANALIZI_P1_P8.md`](./3_boyut_analizleri_ve_rekorlar/PSPP_DIZI_ANALIZI_P1_P8.md):**  
  $P=1 \dots 8$ arasındaki kanıtlanmış tüm 21 küresel optimumun morfolojik aile analizi (Modüler, Bipartite, Uç Sıçraması).
* **[`PSPP_DIZI_SOY_AGACI_P1_P10.md`](./3_boyut_analizleri_ve_rekorlar/PSPP_DIZI_SOY_AGACI_P1_P10.md):**  
  İlk 10 boyutun kök ata ve türemiş çocuk ilişkilerini gösteren tam genetik soy ağacı.
* **[`PSPP_P10_SONRASI_SOY_AGACI_VE_YENI_KESIFLER.md`](./3_boyut_analizleri_ve_rekorlar/PSPP_P10_SONRASI_SOY_AGACI_VE_YENI_KESIFLER.md):**  
  $P=11 \dots 20$ arasında modüler tabanların ($B=8 \to 9 \to 10 \to 11 \to 12$) bayrak devri ve yeni keşifler.
* **[`PSPP_P33_P34_KIRILMA_NOKTASI_VE_KUADRATIK_DONEM.md`](./3_boyut_analizleri_ve_rekorlar/PSPP_P33_P34_KIRILMA_NOKTASI_VE_KUADRATIK_DONEM.md):**  
  Gerçek kırılma noktasının $P=30$'a çekilişi ($510 > 504$), Taban-12'den Kuyruk-Yoğun modele geçiş ve $P=30 \dots 36$ rekorları.
* **[`PSPP_P1_P100_TAM_COZUM_VE_REKOR_VERITABANI.md`](./3_boyut_analizleri_ve_rekorlar/PSPP_P1_P100_TAM_COZUM_VE_REKOR_VERITABANI.md):**  
  $P=1$'den $P=100$'e kadar bilinen en iyi çözümlerin tam tablosu ve Kuyruk-Yoğun kapalı formülünün ($M = 2sB + 2B - 4$) matematiksel ispatı ($a(100)=5200$).

---

## 📁 4. Arşiv ve Tarihi Belgeler (`4_arsiv_ve_tarihi_belgeler/`)
Projenin önceki fazlarında üretilmiş, yeni keşiflerle aşılmış veya güncelliğini yitirmiş tarihi belgeler.

* **[`4_arsiv_ve_tarihi_belgeler/README.md`](./4_arsiv_ve_tarihi_belgeler/README.md):** Arşiv koleksiyonu indeksi ve belgelerin neden arşivlendiğine dair açıklamalar.
* **`OEIS_SUBMISSION_GUIDE_AND_DRAFT.md`:** OEIS kabulü öncesi hazırlanan ve eski ön-yoğun verileri ($a(30)=504, a(33)=577$) içeren ilk başvuru taslağı.
* **`BAYRAK_DEVRI_TAHMIN_MODELI.md`:** $P \ge 20$ sonrasında da doğrusal tabanların ($B=12 \to 13 \to 14$) süreceğini varsayan eski doğrusal hipotez ($P=30$ Kuyruk-Yoğun kuadratik kırılması ile aşılmıştır).
* **`PSPP_P10_P30_DERIN_ARASTIRMA_VE_REKORLAR.md`:** Taban-12 ailesinin $P=30$'da $M=504$ rekoru kırdığı döneme ait ara rapor.
* **`PSPP_OEIS_PROFILE_DATABASE.txt` & `PSPP_OEIS_PROFIL_VERI_BANKASI.txt`:** Eski $a(100)=5100$ ve $a(33)=577$ verilerini içeren eski düz metin veri dökümleri.
