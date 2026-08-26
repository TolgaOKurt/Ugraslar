"""
SSMT Estimator Registry
Gelecekte sisteme yeni tahmin ediciler eklemeyi son derece kolaylaştıran
dinamik kayıt (registry) mekanizması.
"""

from typing import Dict, Type, List, Callable
from core.base_estimator import BaseEstimator


_ESTIMATOR_REGISTRY: Dict[str, Type[BaseEstimator]] = {}


def register_estimator(cls: Type[BaseEstimator]) -> Type[BaseEstimator]:
    """
    Yeni bir tahmin edici sınıfını sisteme kaydetmek için kullanılan dekoratör.
    Örnek:
        @register_estimator
        class MyCustomEstimator(BaseEstimator):
            ...
    """
    if not issubclass(cls, BaseEstimator):
        raise TypeError(f"{cls.__name__} must inherit from BaseEstimator")
    
    # Geçici bir örnek oluşturup ismini alalım
    temp_instance = cls()
    name = temp_instance.name
    _ESTIMATOR_REGISTRY[name] = cls
    return cls


def get_registered_estimators() -> Dict[str, Type[BaseEstimator]]:
    """Kayıtlı tüm tahmin edici sınıflarını döndürür."""
    return dict(_ESTIMATOR_REGISTRY)


def create_all_estimators() -> List[BaseEstimator]:
    """Kayıtlı tüm tahmin edicilerden yeni ve temiz birer örnek (instance) oluşturur."""
    return [cls() for cls in _ESTIMATOR_REGISTRY.values()]
