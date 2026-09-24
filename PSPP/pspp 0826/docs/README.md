# PSPP Proje Dokümantasyon Rehberi ve Dizin İndeksi

Bu dizin, PSPP (Postage Stamp with Subtraction) projesine ait matematiksel kanunları, algoritma motorlarını, soy ağaçlarını ve rekor analizlerini konularına göre ayrılmış alt klasörlerde barındırır.

---

## 📁 1. Temel Teori ve Kurallar (`1_temel_teori_ve_kurallar/`)
PSPP dizilerinin matematiksel tanımı, oluşturulabilir sayılar kümesi ve akademik OEIS standartları.

* [DIZI_KURALLARI.md](file:///c:/Users/TolgaLocal/Documents/Github/Ugraslar/PSPP/pspp%200826/docs/1_temel_teori_ve_kurallar/DIZI_KURALLARI.md): PSPP dizilerinin temel kuralları, toplam/fark kombinasyonları ve matematiksel kısıtlar.
* [OEIS_SUBMISSION_GUIDE_AND_DRAFT.md](file:///c:/Users/TolgaLocal/Documents/Github/Ugraslar/PSPP/pspp%200826/docs/1_temel_teori_ve_kurallar/OEIS_SUBMISSION_GUIDE_AND_DRAFT.md): OEIS veritabanı kayıt taslağı, formüller ve akademik referanslar.

---

## 📁 2. Algoritmalar ve Motorlar (`2_algoritmalar_ve_motorlar/`)
Kombinatoryel arama motorları, kök büyütme modelleri ve tohum tahmin algoritmaları.

* [KOK_BUYUTME_CALISMA_PRENSIBI.md](file:///c:/Users/TolgaLocal/Documents/Github/Ugraslar/PSPP/pspp%200826/docs/2_algoritmalar_ve_motorlar/KOK_BUYUTME_CALISMA_PRENSIBI.md): Sağlıklı kök büyütme motorunun çalışma prensibi, aday arama aralıkları ($min\_cand, max\_cand$), seçim sıralaması ve Delta analizi.
* [SEED_BOYUTU_VE_VITES_STRATEJISI.md](file:///c:/Users/TolgaLocal/Documents/Github/Ugraslar/PSPP/pspp%200826/docs/2_algoritmalar_ve_motorlar/SEED_BOYUTU_VE_VITES_STRATEJISI.md): Küçük kök ($P=3, 4$) vs. büyük DB kökü ($P \ge 15$) analizi, matematiksel "vites" mantığı ve $P=35$ deney sonuçları.
* [SOLVER_ALGORITMASI.md](file:///c:/Users/TolgaLocal/Documents/Github/Ugraslar/PSPP/pspp%200826/docs/2_algoritmalar_ve_motorlar/SOLVER_ALGORITMASI.md): C tabanlı evrensel Branch & Bound arama motoru mimarisi.
* [BAYRAK_DEVRI_TAHMIN_MODELI.md](file:///c:/Users/TolgaLocal/Documents/Github/Ugraslar/PSPP/pspp%200826/docs/2_algoritmalar_ve_motorlar/BAYRAK_DEVRI_TAHMIN_MODELI.md): Modüler bayrak devri ve periyot geçiş tahmin modelleri.
* [SAFE_AREA_DIZI_TAHMINI.md](file:///c:/Users/TolgaLocal/Documents/Github/Ugraslar/PSPP/pspp%200826/docs/2_algoritmalar_ve_motorlar/SAFE_AREA_DIZI_TAHMINI.md): $P < 9$ güvenli bölge özellikleri ve sonraki boyut tahminleri.

---

## 📁 3. Boyut Analizleri, Soy Ağaçları ve Rekorlar (`3_boyut_analizleri_ve_rekorlar/`)
Belirli $P$ boyutlarına ait detaylı dizi analizleri, soy ağaçları ve tüm zamanların rekorları.

* [P7_ANALIZI.md](file:///c:/Users/TolgaLocal/Documents/Github/Ugraslar/PSPP/pspp%200826/docs/3_boyut_analizleri_ve_rekorlar/P7_ANALIZI.md): $P=7$ boyutu özel analizi ve kırılma noktaları.
* [PSPP_DIZI_ANALIZI_P1_P8.md](file:///c:/Users/TolgaLocal/Documents/Github/Ugraslar/PSPP/pspp%200826/docs/3_boyut_analizleri_ve_rekorlar/PSPP_DIZI_ANALIZI_P1_P8.md): $P=1$ ile $P=8$ arasındaki dizilerin detaylı incelemesi.
* [PSPP_DIZI_SOY_AGACI_P1_P10.md](file:///c:/Users/TolgaLocal/Documents/Github/Ugraslar/PSPP/pspp%200826/docs/3_boyut_analizleri_ve_rekorlar/PSPP_DIZI_SOY_AGACI_P1_P10.md): İlk 10 boyutun kök ve mutasyon soy ağacı.
* [PSPP_P10_SONRASI_SOY_AGACI_VE_YENI_KESIFLER.md](file:///c:/Users/TolgaLocal/Documents/Github/Ugraslar/PSPP/pspp%200826/docs/3_boyut_analizleri_ve_rekorlar/PSPP_P10_SONRASI_SOY_AGACI_VE_YENI_KESIFLER.md): $P > 10$ sonrası soy ağaçları ve yeni keşifler.
* [PSPP_P10_P30_DERIN_ARASTIRMA_VE_REKORLAR.md](file:///c:/Users/TolgaLocal/Documents/Github/Ugraslar/PSPP/pspp%200826/docs/3_boyut_analizleri_ve_rekorlar/PSPP_P10_P30_DERIN_ARASTIRMA_VE_REKORLAR.md): Orta ve büyük boyutlarda ($P=10 \dots 30$) derin arama rekorları.
* [PSPP_P33_P34_KIRILMA_NOKTASI_VE_KUADRATIK_DONEM.md](file:///c:/Users/TolgaLocal/Documents/Github/Ugraslar/PSPP/pspp%200826/docs/3_boyut_analizleri_ve_rekorlar/PSPP_P33_P34_KIRILMA_NOKTASI_VE_KUADRATIK_DONEM.md): $P=33$ ve $P=34$ kırılma noktaları ve kuadratik rejim analizleri.
* [PSPP_P1_P100_TAM_COZUM_VE_REKOR_VERITABANI.md](file:///c:/Users/TolgaLocal/Documents/Github/Ugraslar/PSPP/pspp%200826/docs/3_boyut_analizleri_ve_rekorlar/PSPP_P1_P100_TAM_COZUM_VE_REKOR_VERITABANI.md): $P=1$'den $P=100$'e kadar bilinen en iyi çözümlerin tam kataloğu.
