# SSMT - Seçim Sonrası Maksimum Tahmini (Alman Tank Problemi)

Bu proje, istatistikte **Alman Tank Problemi (German Tank Problem)** olarak bilinen ayrık tekdüze dağılımın üst sınırını ($N$) tahmin etme problemini simüle eden, karşılaştıran ve görselleştiren modüler bir Python kütüphanesi ve CLI aracıdır.

---

## Mimari Prensipler (Architecture)

1. **Modüler & DRY:** Tüm ortak matematiksel ve durum yönetimi fonksiyonları soyut sınıfta toplanmıştır.
2. **One Source of Trust (Tek Doğruluk Kaynağı):** Simülasyonun tam geçmişi, anlık durumu ve metrikleri yalnızca `Comparator` tarafından yönetilir.
3. **Standart Veri Sözleşmeleri (Contracts):** Bileşenler (`core/contracts.py`) içerisinde tanımlanmış tip güvenli modeller ve sabit anahtarlarla haberleşir.
4. **Tahmin İzolasyonu:** Tahmin ediciler gerçek $N$ değerini asla göremez; sadece kendilerine iletilen örneklem sayıları ile tahmin üretirler.

---

## Dizin Yapısı

```text
SSMT/
├── core/
│   ├── contracts.py       # Standart iletişim anahtarları ve DTO'lar
│   ├── producer.py        # Üretici (Rastgele N belirler ve örneklem üretir)
│   └── base_estimator.py  # Tahmin ediciler için soyut temel sınıf
├── estimators/
│   ├── registry.py            # Tahmin edici kayıt ve fabrika mekanizması
│   ├── mvue.py                # MVUE: m + (m / k) - 1 (Optimal Yansız)
│   ├── bayesian_mean.py       # Bayesci Ortalama: (m - 1)*(k - 1)/(k - 2)
│   ├── bayesian_median.py     # Bayesci Medyan: m * 2^(1 / (k - 1))
│   ├── symmetric_spacing.py   # Simetrik Aralık: m + min(X) - 1
│   ├── sample_max.py          # Bilinen Maksimum Değer (m)
│   ├── moments.py             # 2x Ortalama (Momentler Yöntemi)
│   ├── corrected_moments.py   # Düzeltilmiş Moment: 2*ort - 1
│   └── sample_median.py       # Örneklem Medyanı: 2*medyan - 1
├── evaluation/
│   ├── comparator.py      # Simülasyon yöneticisi & metrik hesaplayıcı
│   └── visualizer.py      # Matplotlib çok panelli grafik & ASCII Tablo
├── main.py                # Komut satırı arayüzü (CLI)
├── requirements.txt       # Gereksinimler (matplotlib)
├── TAHMIN_EDICILER_KARSILASTIRMASI.md  # 8 Tahmin edicinin derinlemesine bilimsel analizi
└── README.md
```

---

## Kurulum & Çalıştırma

### Gereksinimler
```bash
pip install -r requirements.txt
```

### 1. İnteraktif Menü (Sihirbaz Modu - Önerilen)
Hiçbir parametre vermeden doğrudan çalıştırdığınızda tüm modları adım adım soran interaktif bir menü açılır:
```bash
python main.py
```

### 2. Komut Satırı (CLI) Parametreleri ile Çalıştırma
İsterseniz doğrudan komut satırı argümanlarıyla da çalıştırabilirsiniz:

```bash
# 20 gözlem adımı ile rastgele N için simülasyon ve grafik:
python main.py --observations 20 --save-plot ssmt_simulation.png

# Sabit bir N (Örn: 1000) belirleyerek çalıştırma:
python main.py --fixed-limit 1000 --observations 25

# 100 turluk Monte Carlo başarı karşılaştırması:
python main.py --monte-carlo 100 --observations 15
```

---

## Yeni Bir Tahmin Edici Nasıl Eklenir?

Yeni bir tahmin edici eklemek için tek yapmanız gereken `BaseEstimator`'dan türeyen bir sınıf yazıp `@register_estimator` dekoratörünü eklemektir:

```python
# estimators/my_estimator.py
from core.base_estimator import BaseEstimator
from estimators.registry import register_estimator

@register_estimator
class MyEstimator(BaseEstimator):
    def __init__(self):
        super().__init__(
            name="my_custom_est",
            display_name="Özel Tahmin Edici",
            description="Benim özel formülüm"
        )

    def _calculate_estimate(self) -> float:
        # self.max_observed (m), self.observation_count (k), self.mean_observed vb. hazırdır
        return self.max_observed * 1.1
```

Bu sınıf sisteme eklendiğinde **otomatik olarak** simülasyona, grafiklere ve karşılaştırma tablolarına dahil edilir.
