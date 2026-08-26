"""
Simetrik Aralık (Symmetric Spacing / Min-Max Gap) Tahmin Edici
Dağılımın her iki ucundaki boşlukların (min(X) - 1 ile N - m)
simetrik olacağı sezgisine dayanır.
Formül: N_tahmin = m + min(X) - 1
"""

from core.base_estimator import BaseEstimator
from estimators.registry import register_estimator


@register_estimator
class SymmetricSpacingEstimator(BaseEstimator):
    """
    Simetrik Aralık (Symmetric Spacing / Min-Max Gap) Tahmin Edici.
    Minimum gözlemin 1'e olan mesafesi kadar maksimumun üzerine ekleme yapar.
    """

    def __init__(self):
        super().__init__(
            name="symmetric_spacing",
            display_name="Simetrik Aralık",
            description="m + min(X) - 1 (Alt/Üst Aralık Simetrisi)"
        )

    def _calculate_estimate(self) -> float:
        k = self.observation_count
        m = self.max_observed
        min_val = self.min_observed

        if k == 0:
            return 0.0

        # Formül: m + min(X) - 1
        return float(m + min_val - 1)
