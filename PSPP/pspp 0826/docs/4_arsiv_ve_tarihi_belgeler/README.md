# PSPP Arşiv ve Tarihi Belgeler Koleksiyonu (Outdated / Historical Archive)

Bu dizin, PSPP projesinin önceki araştırma fazlarında üretilmiş, ancak **yeni matematiksel keşifler (özellikle $P=30$ Kuyruk-Yoğun Faz Değişimi) ve güncel veritabanı rekorları ($a(30)=510 \dots a(100)=5200$) sonrasında güncelliğini yitirmiş tarihi belgeleri** saklar.

Buradaki dokümanlar projenin gelişim sürecini ve hipotez evrimini belgelemek amacıyla korunmaktadır; ancak **aktif referans olarak kullanılmamalıdır.**

---

## 📁 Arşivlenen Belgeler ve Arşivlenme Nedenleri

| Belge | Önceki Konumu | Tarihi Bağlamı | Neden Arşivlendi? (Güncel Durum) |
|:---|:---|:---|:---|
| **`OEIS_SUBMISSION_GUIDE_AND_DRAFT.md`** | `1_temel_teori_ve_kurallar/` | Dizi OEIS'e sunulmadan önceki ilk başvuru taslağı | Dizi resmi olarak **A398332** numarasıyla onaylanmıştır. Eski belgede $a(30)=504, a(33)=577$ yer alıyordu. Güncel resmi metin kök dizindeki [`oeis wiki page.md`](../oeis%20wiki%20page.md) dosyasıdır. |
| **`BAYRAK_DEVRI_TAHMIN_MODELI.md`** | `2_algoritmalar_ve_motorlar/` | $P=20 \dots 150$ arasında doğrusal tabanların ($B=12 \to 13 \to 14$) saltanat süreceğini varsayan analitik model | $P=30$'da Kuyruk-Yoğun kuadratik model ($M=510 > 504$) doğrusal taban zincirini kırmış ve faz değişimini başlatmıştır. Bu model doğrusal dönemin teorik hipotezi olarak tarihi niteliktedir. |
| **`PSPP_P10_P30_DERIN_ARASTIRMA_VE_REKORLAR.md`** | `3_boyut_analizleri_ve_rekorlar/` | Taban-12 ailesinin $P=30$'da $M=504$ rekorunu kırdığı ilk dönem raporu | $P=30$ için bulunan $M=504$ rekoru, Kuyruk-Yoğun model ($M=510$) ile aşılmıştır. $P=30$ sonrası kuadratik döneme geçmiştir. |
| **`PSPP_OEIS_PROFILE_DATABASE.txt`** | Kök dizin (`pspp 0826/`) | Ön-yoğun kuadratik model ile oluşturulmuş eski veri tabanı dökümü | $a(100)=5100$ ve $a(33)=577$ eski sayılarını içerir. Güncel veritabanı `pspp_database.json` ($a(100)=5200$) dosyasıdır. |
| **`PSPP_OEIS_PROFIL_VERI_BANKASI.txt`** | Kök dizin (`pspp 0826/`) | Yukarıdaki dosyanın Türkçe başlıklı birebir kopyası | Fazlalık kopya ve eski veri dökümü. |

---

## 🧭 Güncel Referanslar Nerede?

* **Resmi OEIS Metni:** [`docs/oeis wiki page.md`](../oeis%20wiki%20page.md)
* **Merkezi Doğrulanmış Veritabanı:** [`pspp_database.json`](../../pspp_database.json)
* **Kırılma Noktası Analizi:** [`docs/3_boyut_analizleri_ve_rekorlar/PSPP_P33_P34_KIRILMA_NOKTASI_VE_KUADRATIK_DONEM.md`](../3_boyut_analizleri_ve_rekorlar/PSPP_P33_P34_KIRILMA_NOKTASI_VE_KUADRATIK_DONEM.md)
* **$P=1 \dots 100$ Tam Çözüm Kataloğu:** [`docs/3_boyut_analizleri_ve_rekorlar/PSPP_P1_P100_TAM_COZUM_VE_REKOR_VERITABANI.md`](../3_boyut_analizleri_ve_rekorlar/PSPP_P1_P100_TAM_COZUM_VE_REKOR_VERITABANI.md)
