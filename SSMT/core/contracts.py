"""
SSMT Core Contracts & Common Keys
Bu modül, sistem genelinde veri bütünlüğünü sağlayan ortak anahtarları,
sözleşmeleri ve veri yapılarını (Data Transfer Objects) barındırır.
"""

from dataclasses import dataclass, field
from typing import Dict, List, Any


# --- Ortak İletişim Anahtarları (Common Keys) ---
KEY_STEP = "step"                         # k: Gözlem adım numarası (1, 2, ...)
KEY_SAMPLE = "sample"                     # Yeni üretilen örneklem değeri
KEY_MAX_OBSERVED = "max_observed"         # Şu ana kadar gözlemlenen en büyük sayı (m)
KEY_TRUE_LIMIT = "true_limit"             # Gerçek üst sınır (N) - Yalnızca Karşılaştırıcı ve Üretici bilir
KEY_ESTIMATES = "estimates"               # Tahmin edici isimlerine göre tahmin değerleri {isim: float}
KEY_ABSOLUTE_ERRORS = "abs_errors"        # Tahmin edici isimlerine göre mutlak hatalar |tahmin - N|
KEY_PERCENT_ERRORS = "pct_errors"         # Tahmin edici isimlerine göre bağıl hata yüzdeleri (|tahmin - N| / N * 100)


@dataclass(frozen=True)
class StepRecord:
    """
    Tek bir gözlem adımına ait tüm simülasyon verisini temsil eder.
    'Single Source of Truth' kaydının atomik birimidir.
    """
    step: int                               # Gözlem sayısı (k)
    sample: int                             # Bu adımda gelen örneklem
    max_observed: int                       # Bu adıma kadarki en büyük örneklem (m)
    true_limit: int                         # Gerçek üst limit (N)
    estimates: Dict[str, float]             # {TahminEdiciAdi: TahminDegeri}
    abs_errors: Dict[str, float]            # {TahminEdiciAdi: MutlakHata}
    pct_errors: Dict[str, float]            # {TahminEdiciAdi: YuzdeHata}

    def to_dict(self) -> Dict[str, Any]:
        """Sözleşme anahtarlarına uygun standart sözlük formatı."""
        return {
            KEY_STEP: self.step,
            KEY_SAMPLE: self.sample,
            KEY_MAX_OBSERVED: self.max_observed,
            KEY_TRUE_LIMIT: self.true_limit,
            KEY_ESTIMATES: dict(self.estimates),
            KEY_ABSOLUTE_ERRORS: dict(self.abs_errors),
            KEY_PERCENT_ERRORS: dict(self.pct_errors),
        }


@dataclass
class EstimatorSummary:
    """Bir tahmin edicinin simülasyon genelindeki başarı metrikleri."""
    name: str
    mae: float                  # Ortalama Mutlak Hata (Mean Absolute Error)
    mse: float                  # Ortalama Hata Karesi (Mean Squared Error)
    mean_pct_error: float       # Ortalama Yüzde Hata
    final_estimate: float       # Son adım tahmini
    final_pct_error: float      # Son adım yüzde hatası


@dataclass
class SimulationRun:
    """
    Tek bir simülasyon oturumunun 'One Source of Truth' veri modeli.
    """
    true_limit: int
    total_steps: int
    without_replacement: bool
    history: List[StepRecord] = field(default_factory=list)
    summaries: Dict[str, EstimatorSummary] = field(default_factory=dict)


@dataclass
class MonteCarloResult:
    """
    Çoklu simülasyon (Monte Carlo) deney sonuçlarının veri modeli.
    """
    iterations: int
    observations_per_run: int
    min_limit: int
    max_limit: int
    true_limits: List[int] = field(default_factory=list)
    final_estimates: Dict[str, List[float]] = field(default_factory=dict)
    final_pct_errors: Dict[str, List[float]] = field(default_factory=dict)
    step_mean_pct_errors: Dict[str, List[float]] = field(default_factory=dict)
    metrics: Dict[str, Dict[str, float]] = field(default_factory=dict)
