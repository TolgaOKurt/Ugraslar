#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
PSPP Sağlıklı Kök Karşılaştırma ve Yüzdelik Başarı Analizi
---------------------------------------------------------
Tüm aday kökleri aynı anda P=3'ten P=15'e kadar büyütür ve
her boyutta DB optimumuna yüzdelik olarak kaç yaklaşıldığını
matris tablosu olarak sunar.
"""

import sys
import os

BASE_DIR = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
sys.path.insert(0, BASE_DIR)

from scripts.root_grower import RootGrower
from db_manager import load_database

def run_benchmark():
    grower = RootGrower()
    db = load_database()

    seeds = [
        ("P3 [2,4,5]", [2, 4, 5]),
        ("P3 [3,4,5]", [3, 4, 5]),
        ("P3 [2,3,5]", [2, 3, 5]),
        ("P3 [1,2,6]", [1, 2, 6]),
        ("P4 [1,2,8,13]", [1, 2, 8, 13]),
        ("P4 [4,6,7,9]", [4, 6, 7, 9]),
        ("P4 [3,6,7,8]", [3, 6, 7, 8]),
        ("P4 [1,3,7,12]", [1, 3, 7, 12]),
    ]

    p_range = range(3, 16)
    
    print("\n" + "=" * 125)
    print("                    PSPP SAGLIKLI KOK BUYUTME PERFORMANS MATRISI (P=3 -> P=15)")
    print("             Her hucrede: Ulasilan Skor M (DB Optimumuna Yuzdelik Basari Orani %)")
    print("=" * 125)

    header = f"{'Kok Dizisi':<16} | " + " | ".join([f"P={p:<2d}" for p in p_range]) + " | Ort. Basari %"
    print(header)
    print("-" * 125)

    # DB Satırı
    db_vals = [f"{db.get(str(p),{}).get('score',0):>4d}" for p in p_range]
    print(f"{'DB OPTIMUM':<16} | " + " | ".join(db_vals) + " |    %100.0")
    print("-" * 125)

    for name, seed in seeds:
        res = grower.grow_single_seed(seed, target_p=15, beam_width=10, verbose=False)
        cols = []
        pcts = []
        for p in p_range:
            if p in res["history"]:
                sc = res["history"][p]["score"]
                pct = res["history"][p]["pct"]
                pcts.append(pct)
                cols.append(f"{sc:>2d}(%{pct:2.0f})")
            else:
                cols.append("   -   ")
        avg_pct = sum(pcts) / len(pcts) if pcts else 0
        print(f"{name:<16} | " + " | ".join(cols) + f" |    %{avg_pct:5.1f}")

    print("=" * 125 + "\n")

if __name__ == "__main__":
    run_benchmark()
