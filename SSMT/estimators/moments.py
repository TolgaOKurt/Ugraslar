"""
2x Ortalama (Momentler Yöntemi - Method of Moments) Tahmin Edici
Ayrık tekdüze (uniform) dağılımda ortalamanın iki katını alarak üst sınırı tahmin eder.
Formül: N_tahmin = 2 * ortalama
"""

from core.base_estimator import BaseEstimator
from estimators.registry import register_estimator


@register_estimator
class MomentsEstimator(BaseEstimator):
    """
    Momentler Yöntemi (Method of Moments) Tahmin Edici.
    Gözlemlerin ortalamasını 2 ile çarparak popülasyon sınırını tahmin eder.
    """

    def __init__(self):
        super().__init__(
            name="moments",
            display_name="2x Ortalama",
            description="2 * Ortalama (Momentler Yöntemi)"
        )

    def _calculate_estimate(self) -> float:
        if self.observation_count == 0:
            return 0.0

        # Formül: 2 * ortalama
        return 2.0 * self.mean_observed
