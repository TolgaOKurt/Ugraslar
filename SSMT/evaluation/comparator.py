"""
SSMT Comparator (Karşılaştırıcı) & Simülasyon Yöneticisi
'One Source of Truth' mimarisinin merkezidir.
Üretici ve Tahmin Edicileri koordine eder, verileri kaydeder ve metrikleri hesaplar.
"""

import math
from typing import List, Dict, Optional
from core.contracts import (
    StepRecord,
    EstimatorSummary,
    SimulationRun,
    MonteCarloResult,
    KEY_STEP,
    KEY_SAMPLE,
    KEY_MAX_OBSERVED,
    KEY_TRUE_LIMIT,
    KEY_ESTIMATES,
    KEY_ABSOLUTE_ERRORS,
    KEY_PERCENT_ERRORS,
)
from core.producer import Producer
from core.base_estimator import BaseEstimator
from estimators.registry import create_all_estimators


class Comparator:
    """
    Simülasyon yöneticisi ve karşılaştırıcı.
    - Üreticinin belirlediği gerçek N'i bilir.
    - Üretilen her örneklemi bilir.
    - Tahmin edicilere SADECE örneklemi iletir.
    - Tahminleri toplayıp doğruluk/hata metriklerini hesaplar.
    - Tek doğruluk kaynağı (Single Source of Truth) olarak tüm geçmişi saklar.
    """

    def __init__(
        self,
        producer: Optional[Producer] = None,
        estimators: Optional[List[BaseEstimator]] = None,
    ):
        self.producer = producer or Producer()
        self.estimators = estimators if estimators is not None else create_all_estimators()
        self._current_run: Optional[SimulationRun] = None

    @property
    def current_run(self) -> Optional[SimulationRun]:
        """En son koşturulan simülasyonun tam veri kaydı."""
        return self._current_run

    def run_simulation(self, total_observations: int = 10) -> SimulationRun:
        """
        Belirtilen sayıda gözlem adımı için simülasyonu koşturur.
        Args:
            total_observations: Yapılacak gözlem sayısı (k_max).
        Returns:
            SimulationRun: Tüm adımları ve metrik özetlerini içeren nesne.
        """
        # Tahmin edicileri sıfırla
        for est in self.estimators:
            est.reset()

        true_limit = self.producer.true_limit
        run = SimulationRun(
            true_limit=true_limit,
            total_steps=total_observations,
            without_replacement=self.producer._without_replacement,
        )

        max_observed = 0

        for step in range(1, total_observations + 1):
            # 1. Üreticiden yeni örneklem al
            sample = self.producer.generate_sample()
            if sample > max_observed:
                max_observed = sample

            # 2. Tahmin edicilere SADECE bu sayıyı ver ve tahminleri topla
            step_estimates: Dict[str, float] = {}
            step_abs_errors: Dict[str, float] = {}
            step_pct_errors: Dict[str, float] = {}

            for est in self.estimators:
                estimate = est.update(sample)
                abs_err = abs(true_limit - estimate)
                pct_err = (abs_err / true_limit) * 100.0 if true_limit > 0 else 0.0

                step_estimates[est.name] = estimate
                step_abs_errors[est.name] = abs_err
                step_pct_errors[est.name] = pct_err

            # 3. Adım kaydını 'One Source of Truth' listesine ekle
            record = StepRecord(
                step=step,
                sample=sample,
                max_observed=max_observed,
                true_limit=true_limit,
                estimates=step_estimates,
                abs_errors=step_abs_errors,
                pct_errors=step_pct_errors,
            )
            run.history.append(record)

        # 4. Genel simülasyon özet metriklerini hesapla
        run.summaries = self._calculate_summaries(run)
        self._current_run = run
        return run

    def _calculate_summaries(self, run: SimulationRun) -> Dict[str, EstimatorSummary]:
        """Her tahmin edici için MAE, MSE, ortalama yüzde hata gibi metrikleri hesaplar."""
        summaries: Dict[str, EstimatorSummary] = {}

        for est in self.estimators:
            name = est.name
            abs_errors = [step.abs_errors[name] for step in run.history]
            pct_errors = [step.pct_errors[name] for step in run.history]

            mae = sum(abs_errors) / len(abs_errors) if abs_errors else 0.0
            mse = sum(err ** 2 for err in abs_errors) / len(abs_errors) if abs_errors else 0.0
            mean_pct = sum(pct_errors) / len(pct_errors) if pct_errors else 0.0

            last_record = run.history[-1] if run.history else None
            final_est = last_record.estimates[name] if last_record else 0.0
            final_pct = last_record.pct_errors[name] if last_record else 0.0

            summaries[name] = EstimatorSummary(
                name=name,
                mae=mae,
                mse=mse,
                mean_pct_error=mean_pct,
                final_estimate=final_est,
                final_pct_error=final_pct,
            )

        return summaries

    def run_monte_carlo(
        self,
        iterations: int = 100,
        observations_per_run: int = 10,
        min_limit: int = 500,
        max_limit: int = 1500,
    ) -> MonteCarloResult:
        """
        Birden fazla bagimsiz rastgele N ve deney kosturarak
        tahmin edicilerin genel istatistiksel basarisini karsilastirir.
        """
        true_limits: List[int] = []
        final_estimates: Dict[str, List[float]] = {est.name: [] for est in self.estimators}
        final_pct_errors: Dict[str, List[float]] = {est.name: [] for est in self.estimators}
        step_pct_errors_accum: Dict[str, List[List[float]]] = {
            est.name: [[] for _ in range(observations_per_run)] for est in self.estimators
        }

        for _ in range(iterations):
            self.producer = Producer(
                min_limit=min_limit,
                max_limit=max_limit,
                without_replacement=self.producer._without_replacement
            )
            run = self.run_simulation(total_observations=observations_per_run)
            true_limits.append(run.true_limit)

            for step_idx, step_rec in enumerate(run.history):
                for est in self.estimators:
                    step_pct_errors_accum[est.name][step_idx].append(step_rec.pct_errors[est.name])

            for est in self.estimators:
                name = est.name
                summary = run.summaries[name]
                final_estimates[name].append(summary.final_estimate)
                final_pct_errors[name].append(summary.final_pct_error)

        # Adim basina ortalama hata egrileri
        step_mean_pct_errors: Dict[str, List[float]] = {}
        for est in self.estimators:
            name = est.name
            step_mean_pct_errors[name] = [
                sum(err_list) / len(err_list) if err_list else 0.0
                for err_list in step_pct_errors_accum[name]
            ]

        # Ozet metrikler
        mc_metrics: Dict[str, Dict[str, float]] = {}
        for name, errors in final_pct_errors.items():
            mean_err = sum(errors) / len(errors) if errors else 0.0
            variance = sum((e - mean_err) ** 2 for e in errors) / len(errors) if errors else 0.0
            mc_metrics[name] = {
                "mean_final_pct_error": mean_err,
                "std_final_pct_error": math.sqrt(variance),
                "min_error": min(errors) if errors else 0.0,
                "max_error": max(errors) if errors else 0.0,
            }

        return MonteCarloResult(
            iterations=iterations,
            observations_per_run=observations_per_run,
            min_limit=min_limit,
            max_limit=max_limit,
            true_limits=true_limits,
            final_estimates=final_estimates,
            final_pct_errors=final_pct_errors,
            step_mean_pct_errors=step_mean_pct_errors,
            metrics=mc_metrics,
        )
