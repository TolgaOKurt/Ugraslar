"""
Bayesci Ortalama (Bayesian Mean) Tahmin Edici
Düz öncül (uninformative flat prior) altında sonsal dağılımın beklenen değerini hesaplar.
Formül (k > 2 için): N_tahmin = (m - 1) * (k - 1) / (k - 2)
"""

from core.base_estimator import BaseEstimator
from estimators.registry import register_estimator


@register_estimator
class BayesianMeanEstimator(BaseEstimator):
    """
    Bayesci Sonsal Dağılım Ortalama Tahmin Edicisi (Bayesian Mean Estimator).
    k > 2 olduğunda sonsal olasılık dağılımının kütle merkezini hesaplar.
    """

    def __init__(self):
        super().__init__(
            name="bayes_mean",
            display_name="Bayes Ortalama",
            description="(m - 1) * (k - 1) / (k - 2) (Bayesci Sonsal Ortalama)"
        )

    def _calculate_estimate(self) -> float:
        k = self.observation_count
        m = self.max_observed

        if k == 0:
            return 0.0
        if k == 1:
            return float(2 * m - 1)
        if k == 2:
            # k=2 durumunda sonsal dağılımın integral ortalaması sonsuza ıraksayabildiği için
            # MVUE veya 2m - 1 kullanılır.
            return float(2 * m - 1)

        # Formül: (m - 1) * (k - 1) / (k - 2)
        return float((m - 1) * (k - 1) / (k - 2))
