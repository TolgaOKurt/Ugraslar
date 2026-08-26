"""
SSMT Producer (Üretici) Modülü
Bu modül, simülasyon evrenini ve rastgele örneklem üretimini yönetir.
Gerçek üst sınırı (N) belirler ve bu sınıra uygun gözlemler üretir.
"""

import random
from typing import Optional, Set, List


class Producer:
    """
    Rastgele bir üst limit (N) belirleyen ve bu limit dahilinde
    sayı üreten / gönderen üretici bileşeni.
    """

    def __init__(
        self,
        min_limit: int = 500,
        max_limit: int = 1500,
        fixed_limit: Optional[int] = None,
        without_replacement: bool = True,
        seed: Optional[int] = None,
    ):
        """
        Args:
            min_limit: Rastgele seçilecek N için alt sınır.
            max_limit: Rastgele seçilecek N için üst sınır.
            fixed_limit: Belirli bir N değeri ile test yapmak istenirse kullanılır.
            without_replacement: True ise iadesiz örnekleme (her seri no en fazla 1 kez),
                                 False ise iadeli örnekleme yapılır.
            seed: Tekrarlanabilir deneyler için rastgelelik tohumu.
        """
        self._rng = random.Random(seed)
        self._without_replacement = without_replacement

        if fixed_limit is not None:
            self._true_limit = fixed_limit
        else:
            self._true_limit = self._rng.randint(min_limit, max_limit)

        self._available_pool: List[int] = list(range(1, self._true_limit + 1))
        if self._without_replacement:
            self._rng.shuffle(self._available_pool)

        self._generated_samples: List[int] = []

    @property
    def true_limit(self) -> int:
        """
        Gerçek üst limit (N).
        NOT: Tahmin ediciler bu değere erişmemeli, yalnızca Karşılaştırıcı (Comparator)
        değerlendirme amacıyla okumalıdır.
        """
        return self._true_limit

    @property
    def generated_samples(self) -> List[int]:
        """Şimdiye kadar üretilen tüm örneklemler."""
        return list(self._generated_samples)

    def generate_sample(self) -> int:
        """
        [1, N] aralığından yeni bir sayı üretir ve döndürür.
        """
        if self._without_replacement:
            if not self._available_pool:
                raise RuntimeError(
                    f"Popülasyondaki tüm elemanlar ({self._true_limit}) zaten tüketildi!"
                )
            sample = self._available_pool.pop()
        else:
            sample = self._rng.randint(1, self._true_limit)

        self._generated_samples.append(sample)
        return sample

    def reset(self, new_limit: Optional[int] = None) -> None:
        """Üreticiyi sıfırlar veya yeni bir sınır ile başlatır."""
        if new_limit is not None:
            self._true_limit = new_limit
        self._available_pool = list(range(1, self._true_limit + 1))
        if self._without_replacement:
            self._rng.shuffle(self._available_pool)
        self._generated_samples.clear()
