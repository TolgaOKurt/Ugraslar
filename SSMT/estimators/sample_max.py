"""
Bilinen Maksimum Değer (Sample Maximum) Tahmin Edici
Gözlemlenen en büyük seri numarasını doğrudan tahmin olarak kullanır.
Formül: N_tahmin = m
(m: şu ana kadar gözlemlenen en büyük sayı)
"""

from core.base_estimator import BaseEstimator
from estimators.registry import register_estimator


@register_estimator
class SampleMaxEstimator(BaseEstimator):
    """
    Bilinen Maksimum Değer (Sample Maximum) Tahmin Edici.
    Gerçek N'in en azından gözlemlenen m kadar olduğunu bildiğimiz için
    kesin alt sınırı (alt eşiği) temsil eder.
    """

    def __init__(self):
        super().__init__(
            name="sample_max",
            display_name="Bilinen Maks (BM)",
            description="m (Gözlemlenen En Büyük Değer)"
        )

    def _calculate_estimate(self) -> float:
        return float(self.max_observed)
