# PSPP Araştırma Tarihçesi, Keşif Kronolojisi ve Belge Geçerlilik Haritası
## Chronological Research History, Milestone Epochs & Document Validity Map

Bu doküman, PSPP (OEIS A398332) projesinde bugüne kadar gerçekleştirilen tüm matematiksel sıçramaların, faz değişimlerinin ve rekor keşiflerinin **kronolojik tarihçesini** belgeler.

Gelecekte yeni bir rekor veya model keşfedildiğinde **eski analiz dokümanlarını silmek veya baştan yazmak yerine**, bu haritaya yeni bir **Faz (Epoch)** kaydı eklenir. Böylece hangi dokümanın hangi tarih aralığında ve hangi varsayımlarla geçerli olduğu şeffaf bir şekilde izlenebilir.

---

## 🗺️ Fazlar ve Keşif Kronolojisi Özeti

```
[Faz 1: Erken Dönem & Safe Area] ─────────► [Faz 2: Büyük Doğrusal Tabanlar] ─────────► [Faz 3: İlk Kuadratik & OEIS] ─────────► [Faz 4: Kuyruk-Yoğun Kırılma] ─────────► [Faz 5: Yeni Solverlar]
   (P=1..8 Kesin Çözümler)                     (Taban-12 Saltanatı: P=9..29)                (A398332 Tescili, a(100)=5100)               (Gerçek Kırılma P=30, a(100)=5200)           (Claude Opus & Z3 & Beam)
   Ağustos 2026 Öncesi                         Eylül 2026 Başı                              1 - 19 Eylül 2026                            24 Eylül 2026                                14 Ekim 2026
```

---

## 📜 Detaylı Faz Kayıtları ve Belge Eşleştirmesi

### 🔹 FAZ 1: Erken Dönem, Güvenli Bölge (Safe Area) ve Kesin Kanunlar
* **Dönem:** Proje Başlangıcı — Ağustos 2026
* **Kapsam:** $P = 1 \dots 8$
* **Matematiksel Atılım:**
  - $P \le 8$ için arama uzayının tamamı taranarak toplam **21 adet küresel optimum** matematiksel kesinlikle kanıtlandı.
  - 3 temel morfolojik arketip keşfedildi: **Aile 1 (Sıkı Modüler)**, **Aile 2 (Bipartite Köprü)**, **Aile 3 (Uç Sıçraması)**.
  - C tabanlı 2048-bit bitmask motoru (`solver.c`) ve 6 kesin matematiksel budama kanunu formüle edildi.
* **Geçerlilik Durumu:** ✅ **MUTLAK VE DEĞİŞMEZ (Evergreen)** — $P \le 8$ çözümleri matematiksel olarak kanıtlanmıştır, asla eskimez.
* **Bu Faza Ait Dokümanlar:**
  * [`docs/1_temel_teori_ve_kurallar/DIZI_KURALLARI.md`](./1_temel_teori_ve_kurallar/DIZI_KURALLARI.md) *(Kesin Kanunlar)*
  * [`docs/3_boyut_analizleri_ve_rekorlar/PSPP_DIZI_ANALIZI_P1_P8.md`](./3_boyut_analizleri_ve_rekorlar/PSPP_DIZI_ANALIZI_P1_P8.md) *(21 Optimum Çözüm Analizi)*
  * [`docs/3_boyut_analizleri_ve_rekorlar/P7_ANALIZI.md`](./3_boyut_analizleri_ve_rekorlar/P7_ANALIZI.md) *(P=7 Özel İncelemesi)*
  * [`docs/2_algoritmalar_ve_motorlar/SOLVER_ALGORITMASI.md`](./2_algoritmalar_ve_motorlar/SOLVER_ALGORITMASI.md) *(C Motoru Mimarisi)*
  * [`docs/2_algoritmalar_ve_motorlar/SAFE_AREA_DIZI_TAHMINI.md`](./2_algoritmalar_ve_motorlar/SAFE_AREA_DIZI_TAHMINI.md) *(P<9 Tahmin Operatörleri)*

---

### 🔹 FAZ 2: Doğrusal Modüler Tabanlar ve Bayrak Devri Hipotezi
* **Dönem:** Ağustos Sonu — Eylül 2026 Başı
* **Kapsam:** $P = 9 \dots 32$
* **Matematiksel Atılım:**
  - $P \ge 9$ sonrasında Aile 2 ve Aile 3'ün tükendiği, modüler taban zincirlerinin ($B=6 \to 8 \to 9 \to 10 \to 11 \to 12$) periyodik gövde büyümesiyle rekor kırdığı tespit edildi.
  - Taban-12 hanedanlığı ($d_i = 12$ gövde, 11 elemanlı kuyruk) ile $a(P) = 24P - 216$ formülü bulundu.
  - $P=20 \dots 29$ arasındaki tüm dünya rekorları ($M=264 \dots 480$) kırıldı; $P=30$ için tarihte ilk kez $M=504$ bulundu.
* **Eski Hipotez:** Doğrusal taban bayrak devrinin ($B=12 \to 13 \to 14 \dots$) $P=155$'e kadar devam edeceği öngörülüyordu.
* **Geçerlilik Durumu:** 
  - $P \le 29$ için: ✅ **GÜNCEL VE AKTİF** (Taban-12 hala bu aralıkta en iyisidir).
  - $P \ge 30$ için: 📦 **AŞILDI (Faz 4 ile yerini kuadratik modele bıraktı)**.
* **Bu Faza Ait Dokümanlar:**
  * [`docs/3_boyut_analizleri_ve_rekorlar/PSPP_DIZI_SOY_AGACI_P1_P10.md`](./3_boyut_analizleri_ve_rekorlar/PSPP_DIZI_SOY_AGACI_P1_P10.md) *(İlk 10 Boyut Genetik Bağı)*
  * [`docs/3_boyut_analizleri_ve_rekorlar/PSPP_P10_SONRASI_SOY_AGACI_VE_YENI_KESIFLER.md`](./3_boyut_analizleri_ve_rekorlar/PSPP_P10_SONRASI_SOY_AGACI_VE_YENI_KESIFLER.md) *(P=11..20 Modüler Geçişler)*
  * [`docs/4_arsiv_ve_tarihi_belgeler/BAYRAK_DEVRI_TAHMIN_MODELI.md`](./4_arsiv_ve_tarihi_belgeler/BAYRAK_DEVRI_TAHMIN_MODELI.md) *(Arşivlendi - Eski Doğrusal Hipotez)*
  * [`docs/4_arsiv_ve_tarihi_belgeler/PSPP_P10_P30_DERIN_ARASTIRMA_VE_REKORLAR.md`](./4_arsiv_ve_tarihi_belgeler/PSPP_P10_P30_DERIN_ARASTIRMA_VE_REKORLAR.md) *(Arşivlendi - P=30=504 Dönemi)*

---

### 🔹 FAZ 3: İlk Kuadratik Fark Tabanı ve Resmi OEIS Başvurusu
* **Dönem:** 1 Eylül 2026 — 19 Eylül 2026
* **Kapsam:** $P = 33 \dots 100$
* **Matematiksel Atılım:**
  - Rohrbach'ın klasik 2-posta pulu toplamsal sınırı ($a_{\text{add}}(n) \approx 0.25 n^2$), çıkarma dahil edilerek iki katına çıkarıldı.
  - Simetrik Ön-Yoğun Fark Tabanı ($1 \dots r$ başta, $S=2r+1$ adım boyutu) ile $a(n) \ge \lfloor (n^2 + 2n) / 2 \rfloor$ alt sınırı ispatlandı.
  - Bu modelle $P=33$'te doğrusal Taban-12 ilk kez geçildi ($577 > 576$) ve $P=100$ için $M=5100$ rekoruna ulaşıldı.
  - Dizi, Tolga Kurt tarafından OEIS'e sunuldu ve **19 Eylül 2026** tarihinde resmi olarak **OEIS A398332** adıyla onaylandı.
* **Geçerlilik Durumu:** 
  - OEIS tescili: ✅ **RESMİ VE KALICI (A398332)**.
  - $a(n)$ sayısal değerleri: 📦 **AŞILDI (Faz 4 ile $+P$ puan geliştirildi)**.
* **Bu Faza Ait Dokümanlar:**
  * [`docs/4_arsiv_ve_tarihi_belgeler/OEIS_SUBMISSION_GUIDE_AND_DRAFT.md`](./4_arsiv_ve_tarihi_belgeler/OEIS_SUBMISSION_GUIDE_AND_DRAFT.md) *(Arşivlendi - İlk Taslak Metin)*
  * [`docs/4_arsiv_ve_tarihi_belgeler/PSPP_OEIS_PROFILE_DATABASE.txt`](./4_arsiv_ve_tarihi_belgeler/PSPP_OEIS_PROFILE_DATABASE.txt) *(Arşivlendi - Eski 5100 Veri Dökümü)*

---

### 🔹 FAZ 4: Büyük Kırılma — Kuyruk-Yoğun (Tail-End) Modüler Fark Temeli
* **Dönem:** 24 Eylül 2026
* **Kapsam:** $P = 30 \dots 100+$
* **Matematiksel Atılım:**
  - Birim adımların başa ($1 \dots r$) konması yerine, adım katlarının öne, ardışık birim adımların ise **kuyruğa** eklenmesiyle **Kuyruk-Yoğun Modüler Fark Temeli** keşfedildi:
    $$B = [\underbrace{B, 2B, \dots, sB}_{s \text{ adım}}, \quad \underbrace{sB+1, sB+2, \dots, sB+(B-1)}_{B-1 \text{ birim kuyruk}}]$$
  - Gerçek faz değişiminin $P=33$ değil, **3 boyut daha erken $P=30$ ($510 > 504$)** olduğu ispatlandı.
  - $P=30 \dots 100$ arasındaki tüm 64 boyutta $+P$ puanlık net rekor artışları sağlandı:
    $$a(30) = 510, \quad a(31) = 542, \quad a(32) = 576, \quad a(33) = 610, \quad \dots \quad a(100) = \mathbf{5200}$$
  - $a(n) = 2sB + 2B - 4 \approx \mathbf{0.56 n^2}$ kuadratik hızıyla Rohrbach tavanı yeni bir seviyeye taşındı.
* **Geçerlilik Durumu:** ✅ **GÜNCEL VE AKTİF DÜNYA REKORLARI** (`pspp_database.json` ile %100 uyumlu).
* **Bu Faza Ait Dokümanlar:**
  * [`docs/3_boyut_analizleri_ve_rekorlar/PSPP_P33_P34_KIRILMA_NOKTASI_VE_KUADRATIK_DONEM.md`](./3_boyut_analizleri_ve_rekorlar/PSPP_P33_P34_KIRILMA_NOKTASI_VE_KUADRATIK_DONEM.md) *(Faz Değişimi Raporu)*
  * [`docs/3_boyut_analizleri_ve_rekorlar/PSPP_P1_P100_TAM_COZUM_VE_REKOR_VERITABANI.md`](./3_boyut_analizleri_ve_rekorlar/PSPP_P1_P100_TAM_COZUM_VE_REKOR_VERITABANI.md) *(P=1..100 Tam Rekor Tablosu)*
  * [`docs/oeis wiki page.md`](./oeis%20wiki%20page.md) *(Resmi Wiki Sayfası - Düz Metin / MediaWiki)*

---

### 🔹 FAZ 5: İleri Düzey Arama Mimarileri ve Heuristic Motorlar
* **Dönem:** 14 Ekim 2026 — Devam Ediyor
* **Kapsam:** $P = 1 \dots 100+$ genelinde arama uzayı optimizasyonları
* **Matematiksel & Algoritmik Hedefler:**
  - Claude Opus 4.6 sistem analizi ile mevcut solverların zayıf noktalarının tespiti.
  - Düzensiz modüler adımlar (non-uniform step size) ve kuyruk pertürbasyonları ile $a(100) > 5200$ rekorunun aranması.
  - SMT/Z3 kısıt programlama (`z3_solver.py`), Beam search kök büyütme (`root_grower.py`) ve basamaklı tırmanma (`step_ladder_search.py`) motorlarının entegrasyonu.
* **Geçerlilik Durumu:** ✅ **AKTİF GELİŞTİRME FAZI**
* **Bu Faza Ait Dokümanlar:**
  * [`docs/claude_opus_4_6_incelemesi.md`](./claude_opus_4_6_incelemesi.md) *(14 Ekim 2026 Sistem İncelemesi ve Yol Haritası)*
  * [`docs/2_algoritmalar_ve_motorlar/KOK_BUYUTME_CALISMA_PRENSIBI.md`](./2_algoritmalar_ve_motorlar/KOK_BUYUTME_CALISMA_PRENSIBI.md) *(Akıllı Aday Üretimi)*
  * [`docs/2_algoritmalar_ve_motorlar/SEED_BOYUTU_VE_VITES_STRATEJISI.md`](./2_algoritmalar_ve_motorlar/SEED_BOYUTU_VE_VITES_STRATEJISI.md) *(Vites Stratejisi)*

---

## 🔮 Gelecek Keşifler İçin Protokol (Faz 6 ve Sonrası)

Gelecekte yeni bir rekor kırıldığında veya yeni bir aile keşfedildiğinde izlenecek adımlar:

1. **Veritabanı Tescili:** Yeni çözüm `db_manager.submit_solution()` aracılığıyla `pspp_database.json`'a işlenir.
2. **Yeni Analiz Belgesi:** Bulunan keşif için `docs/3_boyut_analizleri_ve_rekorlar/` klasörüne sadece o keşfi anlatan yeni bir dosya açılır (örn: `P100_PERTURBASYON_REKORU.md`).
3. **Tarihçe Güncellemesi:** Bu dosyaya (`docs/KESIF_TARIHCESI_VE_SURUM_HARITASI.md`) **"FAZ 6: ..."** başlığı eklenir; hangi boyutların aşıldığı not düşülür.
4. **Eski Belgeler Korunur:** Eski analiz dokümanları silinmez veya baştan yazılmaz; sadece durumları bu haritada `Faz 6 ile Aşıldı` olarak güncellenir.
