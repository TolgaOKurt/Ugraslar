"""
SSMT Base Estimator Modülü
Tüm tahmin algoritmalarının türeyeceği soyut temel sınıfı içerir.
DRY prensibi gereği ortak durum yönetimi (k, m, toplam vb.) bu sınıfta toplanır.
"""

from abc import ABC, abstractmethod
from typing import List, Optional


class BaseEstimator(ABC):
    """
    Tüm tahmin ediciler için soyut temel sınıf (Base Estimator).
    Gelecekte eklenecek yeni tahmin ediciler bu sınıftan miras almalıdır.
    """

    def __init__(self, name: str, display_name: Optional[str] = None, description: str = ""):
        self._name = name
        self._display_name = display_name or name
        self._description = description
        
        # Ortak durum değişkenleri
        self._samples: List[int] = []
        self._max_observed: int = 0
        self._sum_observed: int = 0
        self._current_estimate: float = 0.0

    @property
    def name(self) -> str:
        """Tahmin edicinin benzersiz teknik adı."""
        return self._name

    @property
    def display_name(self) -> str:
        """Grafik ve tablolarda görünecek açıklayıcı isim."""
        return self._display_name

    @property
    def description(self) -> str:
        """Yöntemin matematiksel formülü veya çalışma mantığı."""
        return self._description

    @property
    def observation_count(self) -> int:
        """Toplam gözlem sayısı (k)."""
        return len(self._samples)

    @property
    def max_observed(self) -> int:
        """Şimdiye kadar görülen en büyük sayı (m)."""
        return self._max_observed

    @property
    def sum_observed(self) -> int:
        """Gözlemlenen sayıların toplamı."""
        return self._sum_observed

    @property
    def mean_observed(self) -> float:
        """Gözlemlerin aritmetik ortalaması."""
        if not self._samples:
            return 0.0
        return self._sum_observed / len(self._samples)

    @property
    def samples(self) -> List[int]:
        """Gözlemlenen tüm örneklemlerin kopyası."""
        return list(self._samples)

    @property
    def min_observed(self) -> int:
        """Şimdiye kadar görülen en küçük sayı."""
        return min(self._samples) if self._samples else 0

    @property
    def median_observed(self) -> float:
        """Gözlemlerin medyan (ortanca) değeri."""
        if not self._samples:
            return 0.0
        sorted_s = sorted(self._samples)
        n = len(sorted_s)
        mid = n // 2
        if n % 2 == 1:
            return float(sorted_s[mid])
        else:
            return (sorted_s[mid - 1] + sorted_s[mid]) / 2.0

    @property
    def current_estimate(self) -> float:
        """En son hesaplanan tahmin değeri."""
        return self._current_estimate

    def update(self, sample: int) -> float:
        """
        Yeni bir örneklem sayısı alır, durumunu günceller ve yeni tahmini döner.
        """
        self._samples.append(sample)
        if sample > self._max_observed:
            self._max_observed = sample
        self._sum_observed += sample

        self._current_estimate = self._calculate_estimate()
        return self._current_estimate

    @abstractmethod
    def _calculate_estimate(self) -> float:
        """
        Her tahmin edicinin kendi özel tahmin formülünü uygulayacağı soyut metot.
        Returns:
            float: Güncel N tahmini.
        """
        pass

    def reset(self) -> None:
        """Tahmin edicinin iç durumunu sıfırlar."""
        self._samples.clear()
        self._max_observed = 0
        self._sum_observed = 0
        self._current_estimate = 0.0
