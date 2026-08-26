"""
Örneklem Medyanı Tabanlı Tahmin Edici (Sample Median Estimator)
Aşırı uç değerlere karşı dayanıklı (robust) istatistik prensibiyle
örneklemin medyanını kullanarak üst sınırı tahmin eder.
Formül: N_tahmin = 2 * Medyan - 1
"""

from core.base_estimator import BaseEstimator
from estimators.registry import register_estimator


@register_estimator
class SampleMedianEstimator(BaseEstimator):
    """
    Örneklem Medyanı Tabanlı Tahmin Edici (Sample Median Estimator).
    Ortalama yerine medyan kullanarak uç değerlerin etkisini azaltır.
    """

    def __init__(self):
        super().__init__(
            name="sample_median",
            display_name="Örneklem Medyanı",
            description="2 * Medyan - 1 (Dayanıklı / Robust Tahminci)"
        )

    def _calculate_estimate(self) -> float:
        if self.observation_count == 0:
            return 0.0

        # Formül: 2 * Medyan - 1
        return 2.0 * self.median_observed - 1.0
