#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
PSPP Tohum Tabanli Tahmin ve Mutasyon Motoru (seed_extrapolator.py)
------------------------------------------------------------------
Safe Area (P < 9) ve veritabanindaki tum optimum tohumlari kullanarak,
kor arama yapmadan daha buyuk P boyutlarina (P -> P+1, P+2, ... P+k)
optimum ve rekor dizi tahminleri uretir, mutasyonla iyilestirir
ve pspp_database.json veritabanina otomatik kaydeder.

Kullanim:
1. Otomatik Mod (P=7'den P=16'ya kadar tum tohumlari buyut):
   python scripts/seed_extrapolator.py

2. Belirli Bir Hedef P Boyutuna Tahmin:
   python scripts/seed_extrapolator.py --target-p 16

3. Ozel Bir Tohum Dizisini Genislet:
   python scripts/seed_extrapolator.py --seed 6,6,6,3,1,1,2,1 --steps 4

4. P=6'dan P=7'ye Tahmin Deneyi:
   python scripts/seed_extrapolator.py --from-p 6 --target-p 7
"""

import sys
import os
import json
import itertools
import argparse
import random
import time
from collections import defaultdict

# Proje Kök Dizini ve Veritabanı
BASE_DIR = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
DB_PATH = os.path.join(BASE_DIR, "pspp_database.json")
sys.path.insert(0, BASE_DIR)

try:
    from db_manager import submit_solution, load_database, save_database
except ImportError:
    submit_solution = None

# -----------------------------------------------------------------------------
# ULTRA HIZLI SKORLAMA (Bytearray & Bit Haritası)
# -----------------------------------------------------------------------------
def score_delta(delta: list[int]) -> int:
    """Delta dizisinden maksimum kesintisiz skoru (M) hesaplar."""
    if not delta:
        return 0
    p_arr = list(itertools.accumulate(delta))
    n = len(p_arr)
    max_possible = (p_arr[-1] << 1) + 2
    seen = bytearray(max_possible)

    for i in range(n):
        pi = p_arr[i]
        seen[pi] = 1
        for j in range(i, n):
            pj = p_arr[j]
            seen[pj + pi] = 1
            seen[pj - pi] = 1

    res = seen.find(0, 1)
    return res - 1 if res != -1 else max_possible - 1

def delta_to_dizi(delta: list[int]) -> list[int]:
    return list(itertools.accumulate(delta))

# -----------------------------------------------------------------------------
# KUYRUK VE MODÜLER ŞABLON HAVUZU
# -----------------------------------------------------------------------------
KNOWN_TAILS = [
    [2, 1, 2],
    [3, 1, 1, 2, 1],
    [2, 1, 1, 2],
    [4, 2, 3, 1, 1, 2, 2],
    [6, 2, 3, 4, 2, 1, 2, 1],
    [1, 4, 3, 3, 1, 1, 2, 1],
    [2, 3, 1, 5, 2, 1, 3, 1],
    [1, 2, 1],
    [1, 1, 2, 1],
    [2, 2, 1, 1],
    [3, 2, 1, 1, 2],
    [1, 1, 1, 2, 1],
    [2, 1, 2, 1, 1]
]

class SeedExtrapolator:
    def __init__(self, db_path=DB_PATH):
        self.db_path = db_path
        self.db = self._load_db()
        self.tested_count = 0
        self.new_records = []
        self.alternatives = []

    def _load_db(self):
        if os.path.exists(self.db_path):
            with open(self.db_path, "r", encoding="utf-8") as f:
                return json.load(f)
        return {}

    def get_seed_solutions(self, max_p: int = 8):
        """Veritabanından P <= max_p aralığındaki tohumları çeker."""
        seeds = []
        for p in range(1, max_p + 1):
            p_str = str(p)
            if p_str in self.db:
                rec = self.db[p_str]
                for sol in rec.get("solutions", []):
                    seeds.append({
                        "p": p,
                        "score": rec.get("score", 0),
                        "delta": sol["delta"],
                        "dizi": sol.get("dizi", delta_to_dizi(sol["delta"]))
                    })
        return seeds

    def _test_and_record(self, delta: list[int], source_desc: str, target_p: int):
        if len(delta) != target_p:
            return 0
        self.tested_count += 1
        score = score_delta(delta)
        p_str = str(target_p)
        cur_entry = self.db.get(p_str, {})
        cur_best = cur_entry.get("score", 0)

        if score > cur_best:
            self.new_records.append({
                "p": target_p,
                "score": score,
                "old_score": cur_best,
                "delta": delta,
                "dizi": delta_to_dizi(delta),
                "source": source_desc
            })
            print(f"\n[REKOR] P = {target_p} | Eski: {cur_best} -> YENI SKOR M = {score}")
            print(f"  Kaynak: {source_desc}")
            print(f"  Delta : {delta}")
            print(f"  Dizi  : {delta_to_dizi(delta)}\n")
            if submit_solution:
                submit_solution(delta, is_delta=True, verbose=False)
            self.db = self._load_db()

        elif score == cur_best and cur_best > 0:
            existing = [s["delta"] for s in cur_entry.get("solutions", [])]
            if delta not in existing:
                self.alternatives.append({
                    "p": target_p,
                    "score": score,
                    "delta": delta,
                    "dizi": delta_to_dizi(delta),
                    "source": source_desc
                })
                print(f"[ALTERNATIF] P = {target_p} | M = {score} | Kaynak: {source_desc}")
                print(f"  Delta : {delta}")
                print(f"  Dizi  : {delta_to_dizi(delta)}\n")
                if submit_solution:
                    submit_solution(delta, is_delta=True, verbose=False)
                self.db = self._load_db()
            else:
                # Daha önce bu oturumda listelenmediyse göster
                already_shown = any(m["delta"] == delta and m["p"] == target_p for m in getattr(self, "matched_predictions", []))
                if not already_shown:
                    if not hasattr(self, "matched_predictions"):
                        self.matched_predictions = []
                    self.matched_predictions.append({
                        "p": target_p,
                        "score": score,
                        "delta": delta,
                        "dizi": delta_to_dizi(delta),
                        "source": source_desc
                    })
                    print(f"[BASARILI TAHMIN] P = {target_p} | M = {score} (Optimum Hedefe Ulasildi)")
                    print(f"  Kaynak / Operator : {source_desc}")
                    print(f"  Tahmin Delta      : {delta}")
                    print(f"  Kumulatif Dizi    : {delta_to_dizi(delta)}\n")

        return score

    # -------------------------------------------------------------------------
    # OPERATÖR 1: Modüler Gövde Genişlemesi (Modular Step Extrapolator)
    # -------------------------------------------------------------------------
    def extrapolate_modular(self, seed: list[int], target_p: int):
        seed_len = len(seed)
        if target_p <= seed_len:
            return
        extra = target_p - seed_len
        base_step = seed[0]

        # 1.1: Başa base_step ekleme
        cand1 = [base_step] * extra + seed
        self._test_and_record(cand1, f"Op 1.1 (Başa {extra}x {base_step} ekleme: seed {seed})", target_p)

        # 1.2: Gövde periyot tekrarı + Kuyruk kütüphanesi
        for step in [base_step, base_step + 1, base_step + 2, base_step + 3, base_step + 4]:
            if step <= 0:
                continue
            for tail in KNOWN_TAILS:
                t_len = len(tail)
                if t_len >= target_p:
                    continue
                body_len = target_p - t_len
                cand2 = [step] * body_len + tail
                self._test_and_record(cand2, f"Op 1.2 (Modüler {step}x{body_len} + Kuyruk {tail})", target_p)

                # Kuyruk son adımı {1, 2} varyasyonları
                for last_d in [1, 2]:
                    cand_var = list(cand2)
                    cand_var[-1] = last_d
                    self._test_and_record(cand_var, f"Op 1.2 (Modüler {step} + Kuyruk + d_son={last_d})", target_p)

    # -------------------------------------------------------------------------
    # OPERATÖR 2: Bipartite Çift Kümeli Köprü Formülü (Bridge Extrapolator)
    # -------------------------------------------------------------------------
    def extrapolate_bipartite(self, target_p: int):
        if target_p < 4:
            return
        bridge = 4 * target_p - 14
        lower_len = (target_p - 1) // 2
        upper_len = target_p - 1 - lower_len

        # Alt küme: [2, 2, ..., 1]
        lower_cluster = [2] * (lower_len - 1) + [1]
        
        # Üst küme varyasyonları
        gap = 11 + 4 * (target_p - 7)
        if gap < 1:
            gap = 5

        upper_cluster = [1] + [1] * max(0, upper_len - 3) + [gap, 1]
        if len(upper_cluster) > upper_len:
            upper_cluster = upper_cluster[:upper_len]
        elif len(upper_cluster) < upper_len:
            upper_cluster = upper_cluster + [1] * (upper_len - len(upper_cluster))

        cand = lower_cluster + [bridge] + upper_cluster
        if len(cand) == target_p:
            self._test_and_record(cand, f"Op 2 (Bipartite Köprü Formülü: bridge={bridge})", target_p)

    # -------------------------------------------------------------------------
    # OPERATÖR 3: Asimetrik Uç Sıçraması (Leapfrog Extrapolator)
    # -------------------------------------------------------------------------
    def extrapolate_leapfrog(self, seed: list[int], target_p: int):
        seed_len = len(seed)
        if target_p <= seed_len:
            return
        
        cur_sum = sum(seed)
        cand_prefix = list(seed)
        for i in range(seed_len, target_p - 1):
            val = 2 if i % 2 == 0 else 1
            cand_prefix.append(val)
            cur_sum += val

        # Son eleman = cur_sum (M/2)
        cand = cand_prefix + [cur_sum]
        self._test_and_record(cand, f"Op 3 (Uç Sıçraması: d_son={cur_sum})", target_p)

    # -------------------------------------------------------------------------
    # OPERATÖR 4: Komşuluk Mutasyonu & Tepe Tırmanışı (Hill Climbing)
    # -------------------------------------------------------------------------
    def mutate_and_hill_climb(self, base_delta: list[int], target_p: int, iterations: int = 500):
        if len(base_delta) != target_p:
            return
        
        best_d = list(base_delta)
        best_score = score_delta(best_d)
        self._test_and_record(best_d, "Op 4 (Hill Climbing Başlangıç)", target_p)

        for _ in range(iterations):
            cand = list(best_d)
            # 1-2 elemanı +-1 veya +-2 değiştir
            num_mut = random.randint(1, 2)
            for _ in range(num_mut):
                pos = random.randint(0, target_p - 1)
                change = random.choice([-2, -1, 1, 2])
                if 1 <= cand[pos] + change <= 35:
                    cand[pos] += change

            # Komşu takas olasılığı
            if random.random() < 0.25 and target_p >= 2:
                pos = random.randint(0, target_p - 2)
                cand[pos], cand[pos + 1] = cand[pos + 1], cand[pos]

            sc = score_delta(cand)
            self._test_and_record(cand, "Op 4 (Hill Climbing Mutasyonu)", target_p)
            if sc > best_score:
                best_score = sc
                best_d = list(cand)

    # -------------------------------------------------------------------------
    # HEDEF P İÇİN TÜM YÖNTEMLERİ ÇALIŞTIR
    # -------------------------------------------------------------------------
    def run_for_target(self, target_p: int, iterations: int = 500):
        print(f"\n================================================================")
        print(f">> P = {target_p} HEDEFİ İÇİN TOHUM TABANLI TAHMİN ÇALIŞTIRILIYOR...")
        print(f"================================================================")

        seeds = self.get_seed_solutions(max_p=min(target_p - 1, 14))
        
        # 1. Modüler ve Sıçrama Tahminleri
        for s in seeds:
            self.extrapolate_modular(s["delta"], target_p)
            self.extrapolate_leapfrog(s["delta"], target_p)

        # 2. Bipartite Köprü
        self.extrapolate_bipartite(target_p)

        # 3. Mevcut En İyi Adaylar Üzerinde Hill Climbing
        p_str = str(target_p)
        if p_str in self.db:
            rec = self.db[p_str]
            for sol in rec.get("solutions", []):
                self.mutate_and_hill_climb(sol["delta"], target_p, iterations=iterations)


def main():
    parser = argparse.ArgumentParser(description="PSPP Tohum Tabanli Tahmin ve Mutasyon Motoru")
    parser.add_argument("--target-p", type=int, default=None, help="Tahmin yapilacak tekil hedef P boyutu")
    parser.add_argument("--from-p", type=int, default=7, help="Tahmin baslangic P boyutu (Varsayilan: 7)")
    parser.add_argument("--to-p", type=int, default=16, help="Tahmin bitis P boyutu (Varsayilan: 16)")
    parser.add_argument("--seed", type=str, default=None, help="Ozel tohum delta dizisi (virgul ile ayrilmis)")
    parser.add_argument("--steps", type=int, default=1, help="Ozel tohumun kac adim genisletilecegi")
    parser.add_argument("--iter", type=int, default=300, help="Hill climbing iterasyon sayisi")

    args = parser.parse_args()

    extrapolator = SeedExtrapolator()
    start_t = time.perf_counter()

    print("=" * 70)
    print("   PSPP TOHUM TABANLI TAHMIN VE MUTASYON MOTORU (SEED EXTRAPOLATOR)")
    print("=" * 70)

    if args.seed:
        custom_seed = [int(x.strip()) for x in args.seed.split(",") if x.strip()]
        seed_len = len(custom_seed)
        target_p = seed_len + args.steps
        print(f"[ÖZEL TOHUM]: {custom_seed} (P={seed_len}) -> Hedef P = {target_p}")
        extrapolator.extrapolate_modular(custom_seed, target_p)
        extrapolator.extrapolate_leapfrog(custom_seed, target_p)
        extrapolator.mutate_and_hill_climb(custom_seed + [custom_seed[0]] * args.steps, target_p, iterations=args.iter)
    elif args.target_p:
        extrapolator.run_for_target(args.target_p, iterations=args.iter)
    else:
        for p in range(args.from_p, args.to_p + 1):
            extrapolator.run_for_target(p, iterations=args.iter)

    elapsed = time.perf_counter() - start_t
    print("\n" + "=" * 70)
    print("GENEL ÇALIŞMA RAPORU:")
    print(f"  Toplam Test Edilen Aday Sayısı : {extrapolator.tested_count:,} adet")
    print(f"  Tahmin Edilen Bilinen Optimum  : {len(getattr(extrapolator, 'matched_predictions', []))} adet")
    print(f"  Bulunan Yeni Rekor Sayısı      : {len(extrapolator.new_records)} adet")
    print(f"  Bulunan Alternatif Çözüm Sayısı: {len(extrapolator.alternatives)} adet")
    print(f"  Toplam Süre                    : {elapsed:.4f} saniye")
    print("=" * 70)

if __name__ == "__main__":
    main()
