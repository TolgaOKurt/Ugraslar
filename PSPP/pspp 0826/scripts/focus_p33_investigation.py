#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
==============================================================================
DOSYA ADI : scripts/focus_p33_investigation.py
PROJE     : PSPP (Postage Stamp with Subtraction)
TANIM     : P=33 Kırılma Noktası ve Çevresi (P=28..36) Derin Analiz ve Rekor Motoru
==============================================================================

Bu motor, PSPP tarihindeki en kritik faz değişimi olan P=33 kırılma noktasını
ve çevresini (P=28..36) tüm teorik ve algoritmik modellerle derinlemesine
yarıştırır:
  1. Klasik Modüler Aileler (Taban-12, Taban-13)
  2. Eski Kuadratik Fark Temeli (Ön-Yoğun: 1..r + adımlar)
  3. Yeni Keşfedilen Kuyruk-Yoğun Modüler Temel (Tail-End: s adım + ardışık kuyruk)
  4. İnkremental Kök Büyütme (Beam Search S_p = S_{p-1} ∪ {x})

Tüm sonuçlar merkezi db_manager.py üzerinden bağımsız test edilir ve
veritabanı güncellenir.
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
from scripts.root_grower import RootGrower, fast_score_dizi, dizi_to_delta


def eval_taban12(p: int):
    tail = [2, 5, 3, 6, 1, 1, 2, 1, 2, 2, 2]
    if p <= len(tail): return 0, None
    body_len = p - len(tail)
    delta = [12] * body_len + tail
    dizi = []
    s = 0
    for d in delta: s += d; dizi.append(s)
    return fast_score_dizi(dizi), dizi


def eval_taban13(p: int):
    tail = [2, 6, 3, 7, 1, 1, 2, 1, 2, 2, 2, 1]
    if p <= len(tail): return 0, None
    body_len = p - len(tail)
    delta = [13] * body_len + tail
    dizi = []
    s = 0
    for d in delta: s += d; dizi.append(s)
    return fast_score_dizi(dizi), dizi


def eval_old_quadratic(p: int):
    """Eski ön-yoğun kuadratik temel (1..r + katlar)"""
    best_m = 0
    best_d = None
    for r in range(max(1, p // 2 - 3), min(p, p // 2 + 3)):
        s = p - r
        step = 2 * r + 1
        dizi = list(range(1, r + 1)) + [(j + 1) * step for j in range(s)]
        sc = fast_score_dizi(dizi)
        if sc > best_m:
            best_m = sc
            best_d = dizi
    return best_m, best_d


def eval_tail_end_modular(p: int):
    """Yeni keşfedilen kuyruk-yoğun modüler temel (s adım + ardışık kuyruk)"""
    best_m = 0
    best_d = None
    best_b = 0
    best_s = 0
    for B in range(max(2, p // 2 - 4), min(p + 2, p // 2 + 6)):
        tail_len = B - 1
        s = p - tail_len
        if s <= 0: continue
        dizi = [j * B for j in range(1, s + 1)] + [s * B + k for k in range(1, B)]
        sc = fast_score_dizi(dizi)
        if sc > best_m:
            best_m = sc
            best_d = dizi
            best_b = B
            best_s = s
    return best_m, best_d, best_b, best_s


def run_investigation():
    db = load_database()
    grower = RootGrower()

    print("\n" + "=" * 130)
    print("      PSPP P=33 KIRILMA NOKTASI VE ÇEVRESİ (P=28..36) DERİNLEMESİNE ARAŞTIRMA")
    print("=" * 130)
    print(f"{'P':>3} | {'DB Eski':>8} | {'Taban-12':>8} | {'Ön-Kuad':>8} | {'Kuyruk-Yoğun (Yeni)':>20} | {'Şampiyon M':>10} | {'Fark':>6} | Durum / Kazanan Model")
    print("-" * 130)

    summary_records = []

    for p in range(28, 37):
        db_m = db.get(str(p), {}).get("score", 0)

        # 1. Modelleri hesapla
        m12, d12 = eval_taban12(p)
        m_quad, d_quad = eval_old_quadratic(p)
        m_tail, d_tail, b_val, s_val = eval_tail_end_modular(p)

        # 2. Şampiyonu seç
        candidates = [
            (m12, d12, "Taban-12 Modüler"),
            (m_quad, d_quad, "Eski Ön-Kuadratik"),
            (m_tail, d_tail, f"Kuyruk-Yoğun (B={b_val}, s={s_val})")
        ]
        candidates.sort(key=lambda x: x[0], reverse=True)
        top_sc, top_dizi, top_model = candidates[0]

        # 3. DB Kayıt ve durum
        res = submit_solution(top_dizi, is_delta=False, verbose=False)
        status = res.get("status", "INFERIOR")

        diff = top_sc - db_m
        if status == "NEW_RECORD":
            badge = f"[YENİ REKOR (+{diff})]"
            summary_records.append((p, db_m, top_sc, diff, top_model))
        elif status == "ALTERNATIVE":
            badge = "[ALTERNATİF]"
        elif status == "DUPLICATE":
            badge = "[MEVCUT ZİRVE]"
        else:
            badge = f"%{top_sc / db_m * 100:.1f}"

        print(f"{p:3d} | {db_m:8d} | {m12:8d} | {m_quad:8d} | {m_tail:8d} (B={b_val:2d}, s={s_val:2d}) | {top_sc:10d} | {diff:+6d} | {badge:<18} -> {top_model}")

        # DB güncellemesini tazele
        db = load_database()

    print("\n" + "=" * 130)
    print("   P=33 KIRILMA NOKTASI VE FAZ DEĞİŞİMİ SONUÇ RAPORU")
    print("=" * 130)
    print(f"Toplam Güncellenen / Kırılan Rekor Sayısı: {len(summary_records)}")
    for p, old, new, diff, model in summary_records:
        print(f"  * P={p:2d}: Eski M={old:4d} -> YENİ REKOR M={new:4d} (+{diff:2d} puan artış!) | Model: {model}")
    print("=" * 130 + "\n")


if __name__ == "__main__":
    run_investigation()
