#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
==============================================================================
DOSYA ADI : scripts/batch_update_all_records.py
PROJE     : PSPP (Postage Stamp with Subtraction)
TANIM     : Kuyruk-Yoğun Modüler Temel ile P=30..100 Arası Tüm Yeni Rekorların
            db_manager.py Üzerinden Veritabanına Tescillenmesi
==============================================================================
"""

import sys
import os
import time

BASE_DIR = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
sys.path.insert(0, BASE_DIR)

if sys.stdout.encoding and sys.stdout.encoding.lower() != 'utf-8':
    try:
        sys.stdout.reconfigure(encoding='utf-8')
    except Exception:
        pass

from db_manager import submit_solution, load_database, evaluate_array
from scripts.root_grower import fast_score_dizi, dizi_to_delta


def find_best_tail_end_solution(p: int):
    """Verilen P boyutu için en yüksek skoru veren Kuyruk-Yoğun parametrelerini bulur."""
    best_m = 0
    best_b = 0
    best_s = 0
    best_d = None

    min_b = max(2, p // 2 - 4)
    max_b = min(p + 2, p // 2 + 6)

    for B in range(min_b, max_b + 1):
        tail_len = B - 1
        s = p - tail_len
        if s <= 0:
            continue
        dizi = [j * B for j in range(1, s + 1)] + [s * B + k for k in range(1, B)]
        sc = fast_score_dizi(dizi)
        if sc > best_m:
            best_m = sc
            best_b = B
            best_s = s
            best_d = dizi

    return best_m, best_b, best_s, best_d


def main():
    print("\n" + "=" * 115)
    print("      PSPP P=30..100 TOPLU REKOR GÜNCELLEME VE MERKEZİ VERİTABANI TESCİLİ")
    print("=" * 115)

    db = load_database()
    total_new_records = 0
    total_alternatives = 0
    total_unchanged = 0
    t0 = time.time()

    record_log = []

    for p in range(30, 101):
        old_score = db.get(str(p), {}).get("score", 0)
        best_m, best_b, best_s, best_d = find_best_tail_end_solution(p)

        # Doğrudan merkezi yöneticiye gönder
        res = submit_solution(best_d, is_delta=False, verbose=False)
        status = res.get("status", "INFERIOR")

        diff = best_m - old_score
        delta = dizi_to_delta(best_d)
        delta_str = str(delta)
        if len(delta_str) > 35:
            delta_str = delta_str[:32] + "..."

        if status == "NEW_RECORD":
            total_new_records += 1
            badge = f"[REKOR (+{diff:2d})]"
            record_log.append((p, old_score, best_m, diff, best_b, best_s))
        elif status == "ALTERNATIVE":
            total_alternatives += 1
            badge = "[ALTERNATİF]"
        elif status == "DUPLICATE":
            total_unchanged += 1
            badge = "[MEVCUT]"
        else:
            badge = f"[YETERSİZ %{best_m/old_score*100:.1f}]"

        print(f"P={p:3d} | Eski M={old_score:4d} -> Yeni M={best_m:4d} | Fark: {diff:+4d} | B={best_b:2d}, s={best_s:2d} | {badge:<15} | Delta: {delta_str}")

        # Her adımda veritabanını tazele
        db = load_database()

    elapsed = time.time() - t0
    print("\n" + "=" * 115)
    print("                      TOPLU GÜNCELLEME VE TESCİL RAPORU")
    print("=" * 115)
    print(f"Toplam İncelenen Boyut Sayısı      : {len(range(30, 101))}")
    print(f"Kırılan ve İşlenen Yeni Rekor Sayısı : {total_new_records}")
    print(f"Eklenen Alternatif Çözüm Sayısı    : {total_alternatives}")
    print(f"Mevcut Zirve Kalan Boyut Sayısı    : {total_unchanged}")
    print(f"Toplam Geçen Süre                  : {elapsed:.2f} saniye")
    print("=" * 115)

    if record_log:
        print("\nÖNE ÇIKAN DÖNÜM NOKTALARI:")
        for p, old, new, diff, b, s in record_log:
            if p in (30, 31, 32, 33, 34, 35, 40, 50, 60, 70, 80, 90, 100):
                print(f"  * P={p:3d}: Eski M={old:4d} -> YENİ REKOR M={new:4d} (+{diff:3d} puan!) [B={b}, s={s}]")
    print("=" * 115 + "\n")


if __name__ == "__main__":
    main()
