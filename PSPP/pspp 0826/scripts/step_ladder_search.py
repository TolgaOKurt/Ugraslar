#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
==============================================================================
DOSYA ADI : scripts/step_ladder_search.py
PROJE     : PSPP (Postage Stamp with Subtraction)
TANIM     : P=1'den P=100'e Kadar Basamaklı (Step-Ladder) Rekor Arama Motoru
==============================================================================

ÇALIŞMA PRENSİBİ:
-----------------
1. Düşük vitese hapsolmamak ve kombinatoryel patlamaya takılmamak için
   P=100 hedefine basamaklar (sıçrama tahtaları) halinde tırmanır:
     - Basamak 1: P=10 -> P=20
     - Basamak 2: P=20 -> P=34
     - Basamak 3: P=34 -> P=50
     - Basamak 4: P=50 -> P=63
     - Basamak 5: P=64 -> P=80   (Veritabanında ilk kez rekor keşfi)
     - Basamak 6: P=80 -> P=100  (Veritabanında ilk kez rekor keşfi)

2. Her basamakta:
   - Başlangıç P boyutundaki en iyi çözüm veritabanından çekilir.
   - İleriye doğru hem inkremental kök büyütme (Beam Search) hem de
     optimal modüler/subtractive tabanlar yarıştırılır.
   - En yüksek skoru veren dizi db_manager.py üzerinden otomatik kaydedilir.
   - Yeni rekorlar ([REKOR]) ve alternatifler ([ALTERNATIF]) anında listelenir.
"""

import sys
import os
import time

BASE_DIR = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
sys.path.insert(0, BASE_DIR)

from db_manager import submit_solution, load_database, evaluate_array
from scripts.root_grower import RootGrower, fast_score_dizi, dizi_to_delta


def make_optimal_subtractive(p: int):
    """Verilen P boyutu için optimal 2-parçalı modüler taban dizisini üretir."""
    best_m = 0
    best_d = None
    min_k = max(1, p // 2 - 4)
    max_k = min(p, p // 2 + 4)
    for k in range(min_k, max_k + 1):
        step = 2 * k + 1
        num_steps = p - k
        dizi = list(range(1, k + 1))
        for j in range(1, num_steps + 1):
            dizi.append(j * step)
        sc = fast_score_dizi(dizi)
        if sc > best_m:
            best_m = sc
            best_d = dizi
    return best_m, best_d


def run_step_ladder_search(max_target_p: int = 100, beam_width: int = 10):
    grower = RootGrower()
    db = load_database()

    print("\n" + "=" * 115)
    print(f"      PSPP P=100 BASAMAKLI REKOR ARAMA VE VERITABANI GENISLETME MOTORU")
    print(f"      Hedef: P=1'den P={max_target_p}'e kadar kesintisiz rekor tırmanışı")
    print("=" * 115)

    stages = [
        ("Basamak 1 (Orta Boyut)", 10, 20),
        ("Basamak 2 (Geçiş Bölgesi)", 20, 34),
        ("Basamak 3 (Kuadratik Dönem)", 34, 50),
        ("Basamak 4 (Büyük Boyut)", 50, 63),
        ("Basamak 5 (Keşif Bölgesi: P=64..80)", 63, 80),
        ("Basamak 6 (Zirve Koşusu: P=81..100)", 80, max_target_p)
    ]

    total_new_records = 0
    total_alternatives = 0

    for stage_name, p_start, p_end in stages:
        if p_start >= max_target_p:
            break
        p_end = min(p_end, max_target_p)

        print("\n" + "-" * 115)
        print(f">>> {stage_name}: P={p_start} -> P={p_end} Tırmanışı Başlatılıyor...")
        print("-" * 115)

        grower.reload_db()
        db = grower.db

        # 1. Başlangıç kökünü belirle
        p_str = str(p_start)
        seed = None
        if p_str in db and db[p_str].get("solutions"):
            seed = db[p_str]["solutions"][0]["dizi"]
        else:
            _, seed = make_optimal_subtractive(p_start)

        start_score = fast_score_dizi(seed)
        print(f"   Sıçrama Rampası: P={p_start} | Başlangıç Skoru M={start_score} | Kök Boyutu: {len(seed)}")

        # 2. İnkremental Büyütme (Beam Search)
        beam_res = grower.grow_single_seed(
            seed=seed,
            target_p=p_end,
            beam_width=beam_width,
            submit_to_db=False,
            verbose=False
        )

        # 3. Her P için İnkremental vs Modüler yarışması
        print(f"{'P':>3} | {'Skor (M)':>8} | {'Eski Zirve':>10} | {'Yöntem':<22} | {'Durum':<14} | Delta Özeti")
        print("-" * 115)

        for p in range(p_start + 1, p_end + 1):
            db_entry = db.get(str(p), {})
            cur_best = db_entry.get("score", 0)

            # Aday 1: İnkremental Beam Çözümü
            cand1_seq = beam_res["history"][p]["dizi"]
            cand1_sc = beam_res["history"][p]["score"]
            cand1_method = f"Kok [{p_start}] Buyutme"

            # Aday 2: Optimal Subtractive Çözüm
            cand2_sc, cand2_seq = make_optimal_subtractive(p)
            cand2_method = "Optimal Subtractive"

            # En yüksek skora sahip olanı seç
            if cand1_sc >= cand2_sc:
                best_sc = cand1_sc
                best_seq = cand1_seq
                best_method = cand1_method
            else:
                best_sc = cand2_sc
                best_seq = cand2_seq
                best_method = cand2_method

            # Veritabanına gönder
            res = submit_solution(best_seq, is_delta=False, verbose=False)
            status = res.get("status", "INFERIOR")

            if status == "NEW_RECORD":
                total_new_records += 1
                status_badge = "[YENI REKOR]"
            elif status == "ALTERNATIVE":
                total_alternatives += 1
                status_badge = "[ALTERNATIF]"
            elif status == "DUPLICATE":
                status_badge = "[MEVCUT]"
            else:
                pct = (best_sc / cur_best * 100.0) if cur_best > 0 else 0
                status_badge = f"%{pct:5.1f}"

            delta = dizi_to_delta(best_seq)
            delta_str = str(delta)
            if len(delta_str) > 36:
                delta_str = delta_str[:33] + "..."

            db_str = str(cur_best) if cur_best > 0 else "YOK"
            print(f"{p:3d} | {best_sc:8d} | {db_str:>10} | {best_method:<22} | {status_badge:<14} | {delta_str}")

            # DB'yi tazele
            db = load_database()

    print("\n" + "=" * 115)
    print(f"[BASAMAKLI ARAMA OZETI] (P=1 .. {max_target_p})")
    print(f"   Toplam Bulunan Yeni Rekor Sayisi    : {total_new_records}")
    print(f"   Toplam Eklenen Alternatif Cozum     : {total_alternatives}")
    print(f"   Veritabanindaki En Yuksek Boyut     : P={max_target_p}")
    print("=" * 115 + "\n")


if __name__ == "__main__":
    target = 100
    if len(sys.argv) > 1:
        target = int(sys.argv[1])
    run_step_ladder_search(max_target_p=target, beam_width=10)
