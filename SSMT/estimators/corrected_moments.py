"""
Düzeltilmiş Momentler Yöntemi (Corrected Method of Moments)
[1, N] ayrık dağılımındaki E[X] = (N + 1) / 2 eşitliğini tam çözerek
2 * ortalama formülündeki +1 sapmasını giderir.
Formül: N_tahmin = 2 * ortalama - 1
"""

from core.base_estimator import BaseEstimator
from estimators.registry import register_estimator


@register_estimator
class CorrectedMomentsEstimator(BaseEstimator):
    """
    Düzeltilmiş Momentler Yöntemi (Corrected Method of Moments).
    Popülasyon beklenen değer eşitliğini (N = 2*mu - 1) tam olarak uygular.
    """

    def __init__(self):
        super().__init__(
            name="corrected_moments",
            display_name="Düzeltilmiş Moment",
            description="2 * Ortalama - 1 (Yansız Momentler Yöntemi)"
        )

    def _calculate_estimate(self) -> float:
        if self.observation_count == 0:
            return 0.0

        # Formül: 2 * ortalama - 1
        return 2.0 * self.mean_observed - 1.0
