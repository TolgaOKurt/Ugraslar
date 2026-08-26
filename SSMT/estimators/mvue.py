"""
MVUE (Minimum Variance Unbiased Estimator) Tahmin Edici
Alman Tank Problemi için literatürdeki en optimal yansız tahmin edici.
Formül: N_tahmin = m + (m / k) - 1
(m: gözlemlenen maksimum değer, k: gözlem sayısı)
"""

from core.base_estimator import BaseEstimator
from estimators.registry import register_estimator


@register_estimator
class MVUEEstimator(BaseEstimator):
    """
    Minimum Variance Unbiased Estimator (En Düşük Varyanslı Yansız Tahmin Edici).
    İadesiz örneklemede teorik olarak en doğru beklenen değeri verir.
    """

    def __init__(self):
        super().__init__(
            name="mvue",
            display_name="MVUE",
            description="m + (m / k) - 1 (En Düşük Varyanslı Yansız Tahmin Edici)"
        )

    def _calculate_estimate(self) -> float:
        k = self.observation_count
        m = self.max_observed

        if k == 0:
            return 0.0

        # Formül: m + (m / k) - 1
        return m + (m / k) - 1.0
