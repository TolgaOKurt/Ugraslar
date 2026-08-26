"""
Bayesci Medyan (Bayesian Median) Tahmin Edici
Düz öncül altında sonsal dağılımın %50 olasılık eşiğini (medyanını) hesaplar.
Formül (k > 1 için): N_tahmin = m * 2^(1 / (k - 1))
"""

from core.base_estimator import BaseEstimator
from estimators.registry import register_estimator


@register_estimator
class BayesianMedianEstimator(BaseEstimator):
    """
    Bayesci Sonsal Dağılım Medyan Tahmin Edicisi (Bayesian Median Estimator).
    """

    def __init__(self):
        super().__init__(
            name="bayes_median",
            display_name="Bayes Medyan",
            description="m * 2^(1 / (k - 1)) (Bayesci Sonsal Medyan)"
        )

    def _calculate_estimate(self) -> float:
        k = self.observation_count
        m = self.max_observed

        if k == 0:
            return 0.0
        if k == 1:
            return float(2 * m - 1)

        # Formül: m * 2^(1 / (k - 1))
        return float(m * (2.0 ** (1.0 / (k - 1))))
