import itertools
import math
import json
import os
import sys

BASE_DIR = r"c:\Users\icduser\Documents\GitHub\Ugraslar\PSPP\pspp 0826"
sys.path.insert(0, BASE_DIR)
from db_manager import submit_solution

def calculate_pspp_score(dizi):
    seen = set(dizi)
    n = len(dizi)
    for i in range(n):
        for j in range(i, n):
            seen.add(dizi[i] + dizi[j])
            seen.add(abs(dizi[j] - dizi[i]))
    m = 0
    while (m + 1) in seen:
        m += 1
    return m

def calculate_addition_only_score(dizi):
    seen = set(dizi)
    n = len(dizi)
    for i in range(n):
        for j in range(i, n):
            seen.add(dizi[i] + dizi[j])
    m = 0
    while (m + 1) in seen:
        m += 1
    return m

# Klasik 2-Posta Pulu optimal/yarı-optimal aileleri (Hofmeister / Rohrbach / Mrose):
# Tipik yapı: r adet 1'lik adım (1, 2, ..., r) ve ardından S = (r+1) veya (2r+2) adımları
def generate_classical_postage_stamp_basis(p):
    # En iyi klasik 2-posta pulu katsayısı: r ≈ p // 2
    best_m_add = 0
    best_dizi = []
    
    # Farklı r ve step parametrelerini tara
    for r in range(1, p):
        # 1..r arası elemanlar
        prefix = list(range(1, r + 1))
        rem = p - r
        # Büyük adım S
        # Teorik en iyi S: r + 1 veya 2r + 1
        for s_step in range(r + 1, 3 * r + 10):
            # suffix: prefix[-1] + k * s_step
            suffix = [prefix[-1] + k * s_step for k in range(1, rem + 1)]
            dizi = prefix + suffix
            if len(dizi) == p:
                sc = calculate_addition_only_score(dizi)
                if sc > best_m_add:
                    best_m_add = sc
                    best_dizi = dizi
                    
    # Mrose / Lunnon 2-posta pulu tipi:
    for k in range(1, p // 2 + 2):
        # Dizi: 1..k ve k*(1..rem)
        dizi = list(range(1, k + 1)) + [k + i * (k + 1) for i in range(1, p - k + 1)]
        if len(dizi) == p:
            sc = calculate_addition_only_score(dizi)
            if sc > best_m_add:
                best_m_add = sc
                best_dizi = dizi
                
    return best_dizi, best_m_add

print("="*90)
print("KLASIK 2-POSTA PULU DIZILERININ PSPP (TOPLAM + FARK) ILE TESTI")
print("="*90)

for p in [10, 20, 30, 40, 50, 60, 70, 80, 85, 90, 100]:
    classical_dizi, add_score = generate_classical_postage_stamp_basis(p)
    pspp_score = calculate_pspp_score(classical_dizi)
    classical_formula_bound = (p*p + 6*p + 1) // 4
    
    delta = [classical_dizi[0]] + [classical_dizi[i] - classical_dizi[i-1] for i in range(1, len(classical_dizi))]
    
    print(f"P = {p:3d} | Klasik Toplam Skoru: {add_score:4d} (Formul: {classical_formula_bound:4d}) | PSPP (Toplam+Fark): M = {pspp_score:4d} | Delta: {delta[:4]}...{delta[-3:]}")
