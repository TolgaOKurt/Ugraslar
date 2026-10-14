#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
==============================================================================
DOSYA ADI : scripts/step_ladder_search.py
PROJE     : PSPP (Postage Stamp with Subtraction)
TANIM     : P=1'den P=100'e Canlı Güncellemeli Basamaklı Rekor Arama Motoru
==============================================================================

ÖZELLİKLER:
1. Kombinatoryel Patlamasız (Anti-10^100) Arama:
   Yalnızca matematiksel olarak darboğazı aşabilecek (M+1, M+2 açıklarını
   kapatan) ve periyodik adım genişlemesi sağlayan adayları (O(K)) test eder.
2. Canlı Adım Adım İlerleme (Real-time Stream):
   Her P adımı anında konsola yazdırılır; nerede olduğu, hızı, skoru ve
   veritabanına göre yüzdelik başarısı gecikmesiz gösterilir.
3. Çoklu Yöntem Yarışması (Tournament):
   Her P adımında İnkremental Kök Büyütme, Optimal Kuadratik Fark Tabanı
   ve Periyodik Modüler Tabanlar (Taban-12/13) yarıştırılır.
4. Merkezi Veritabanı Kapısı:
   En yüksek skoru veren dizi db_manager.py üzerinden doğrulatılır ve
   pspp_database.json tekil veritabanına otomatik işlenir.
"""

import sys
import os
import time

BASE_DIR = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
sys.path.insert(0, BASE_DIR)

# Windows Konsol Karakter Uyumluluğu
if sys.stdout.encoding and sys.stdout.encoding.lower() != 'utf-8':
    try:
        sys.stdout.reconfigure(encoding='utf-8')
    except Exception:
        pass

from db_manager import submit_solution, load_database, evaluate_array
from scripts.root_grower import RootGrower, fast_score_dizi, dizi_to_delta


def make_optimal_subtractive(p: int):
    """
    Verilen P boyutu için optimal 2-parçalı modüler taban dizisini üretir.
    r eleman 1..r yoğun bölge, (p-r) eleman ise adım katlarıdır.
    """
    best_m = 0
    best_d = None
    min_k = max(1, p // 2 - 5)
    max_k = min(p, p // 2 + 5)
    for k in range(min_k, max_k + 1):
        num_steps = p - k
        if num_steps <= 0:
            continue
        for step in (2 * k - 1, 2 * k, 2 * k + 1, 2 * k + 2, 2 * k + 3):
            if step <= 0:
                continue
            dizi = list(range(1, k + 1))
            for j in range(1, num_steps + 1):
                dizi.append(j * step)
            sc = fast_score_dizi(dizi)
            if sc > best_m:
                best_m = sc
                best_d = dizi
    return best_m, best_d


def make_modular_family(p: int, base: int = 12):
    """Taban 12 veya Taban 13 kuyruk tabanlı dizileri üretir."""
    if base == 12:
        tail = [2, 5, 3, 6, 1, 1, 2, 1, 2, 2, 2]
    else:
        tail = [2, 6, 3, 7, 1, 1, 2, 1, 2, 2, 2, 1]
    tail_len = len(tail)
    if p <= tail_len:
        return 0, None
    body_len = p - tail_len
    delta = [base] * body_len + tail
    # delta to dizi
    dizi = []
    s = 0
    for d in delta:
        s += d
        dizi.append(s)
    sc = fast_score_dizi(dizi)
    return sc, dizi


def run_step_ladder_search(max_target_p: int = 100, beam_width: int = 5):
    grower = RootGrower()
    db = load_database()

    print("\n" + "=" * 125, flush=True)
    print(f"      PSPP P=100 CANLI BASAMAKLI REKOR ARAMA VE VERITABANI GUNCELLEME MOTORU", flush=True)
    print(f"      Hedef: P=3'ten P={max_target_p}'e kadar kesintisiz, patlamasiz rekor tırmanışı", flush=True)
    print(f"      Beam Genisligi: {beam_width} | Akilli Aday Filtresi: Aktif (O(K))", flush=True)
    print("=" * 125, flush=True)

    stages = [
        ("Basamak 1: Küçük Boyutlar", 3, 10),
        ("Basamak 2: Orta Boyutlar", 10, 20),
        ("Basamak 3: Geçiş Bölgesi", 20, 34),
        ("Basamak 4: Kuadratik Dönem", 34, 50),
        ("Basamak 5: Büyük Boyutlar", 50, 70),
        ("Basamak 6: İleri Boyutlar", 70, 85),
        ("Basamak 7: Zirve Koşusu (P=100)", 85, max_target_p)
    ]

    total_new_records = 0
    total_alternatives = 0
    t_global_start = time.time()

    for stage_name, p_start, p_end in stages:
        if p_start >= max_target_p:
            break
        p_end = min(p_end, max_target_p)

        print("\n" + "-" * 125, flush=True)
        print(f">>> {stage_name}: P={p_start} -> P={p_end} Tırmanışı Başlatılıyor...", flush=True)
        print("-" * 125, flush=True)

        grower.reload_db()
        db = grower.db

        # 1. Başlangıç rampasını belirle
        p_str = str(p_start)
        seed = None
        if p_str in db and db[p_str].get("solutions"):
            seed = db[p_str]["solutions"][0]["dizi"]
        else:
            _, seed = make_optimal_subtractive(p_start)

        start_score = fast_score_dizi(seed)
        print(f"   Sıçrama Rampası : P={p_start} | Başlangıç Skoru M={start_score} | Kök: {seed[:6]}...", flush=True)
        print(f"{'P':>3} | {'Skor (M)':>8} | {'DB Zirve':>8} | {'Basari %':>9} | {'Yöntem':<22} | {'Durum':<14} | {'Sure':>6} | Delta Özeti", flush=True)
        print("-" * 125, flush=True)

        # Beam başlat
        beam = [(tuple(sorted(seed)), start_score, None)]

        # Adım adım tırmanış
        for p in range(p_start + 1, p_end + 1):
            t_step_start = time.time()
            db_entry = db.get(str(p), {})
            cur_best = db_entry.get("score", 0)

            # --- YÖNTEM 1: Akıllı İnkremental Beam Genişlemesi ---
            candidates = []
            seen_seqs = set()

            for cur_seq, cur_score, _ in beam:
                smart_cands = grower.get_smart_candidates(
                    cur_seq=cur_seq,
                    cur_score=cur_score,
                    p_curr=p - 1,
                    append_only=False,
                    max_gaps=5
                )
                for cand in smart_cands:
                    cand_seq = tuple(sorted(cur_seq + (cand,)))
                    if cand_seq in seen_seqs:
                        continue
                    seen_seqs.add(cand_seq)
                    sc = fast_score_dizi(list(cand_seq))
                    candidates.append((sc, cand_seq, cand))

            if candidates:
                candidates.sort(key=lambda x: (x[0], -x[1][-1]), reverse=True)
                beam = [(seq, sc, cand) for sc, seq, cand in candidates[:beam_width]]
                cand_beam_sc, cand_beam_seq, cand_added = beam[0]
            else:
                cand_beam_sc, cand_beam_seq, cand_added = 0, None, None

            # --- YÖNTEM 2: Optimal Subtractive / Kuadratik Taban ---
            cand_sub_sc, cand_sub_seq = make_optimal_subtractive(p)

            # --- YÖNTEM 3: Taban-12 / 13 Modüler Taban ---
            cand_mod_sc, cand_mod_seq = 0, None
            if p <= 45:
                sc12, seq12 = make_modular_family(p, base=12)
                sc13, seq13 = make_modular_family(p, base=13)
                if sc12 > cand_mod_sc:
                    cand_mod_sc, cand_mod_seq = sc12, seq12
                if sc13 > cand_mod_sc:
                    cand_mod_sc, cand_mod_seq = sc13, seq13

            # En yüksek skora sahip olanı seç (Yarışma)
            best_sc = -1
            best_seq = None
            best_method = ""

            if cand_beam_sc > best_sc and cand_beam_seq:
                best_sc = cand_beam_sc
                best_seq = list(cand_beam_seq)
                best_method = f"Kok[{p_start}] Buyutme"

            if cand_sub_sc > best_sc and cand_sub_seq:
                best_sc = cand_sub_sc
                best_seq = cand_sub_seq
                best_method = "Kuadratik Moduler"

            if cand_mod_sc > best_sc and cand_mod_seq:
                best_sc = cand_mod_sc
                best_seq = cand_mod_seq
                best_method = "Taban-12/13 Moduler"

            # Eğer Kuadratik veya Modüler, Beam'den daha iyiyse Beam'e de enjekte et
            if best_seq and (cand_sub_sc > cand_beam_sc or cand_mod_sc > cand_beam_sc):
                beam.append((tuple(sorted(best_seq)), best_sc, None))
                beam.sort(key=lambda x: (x[1], -x[0][-1]), reverse=True)
                beam = beam[:beam_width]

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

            pct_val = (best_sc / cur_best * 100.0) if cur_best > 0 else 100.0
            delta = dizi_to_delta(best_seq)
            delta_str = str(delta)
            if len(delta_str) > 36:
                delta_str = delta_str[:33] + "..."

            t_step = time.time() - t_step_start
            db_str = str(cur_best) if cur_best > 0 else "YOK"
            print(f"{p:3d} | {best_sc:8d} | {db_str:>8} | %{pct_val:8.1f} | {best_method:<22} | {status_badge:<14} | {t_step:5.2f}s | {delta_str}", flush=True)

            # DB'yi tazele
            db = load_database()

    t_total = time.time() - t_global_start
    print("\n" + "=" * 125, flush=True)
    print(f"[BASAMAKLI ARAMA TAMAMLANDI] (P=3 .. {max_target_p})", flush=True)
    print(f"   Toplam Gecen Sure                   : {t_total:.2f} saniye", flush=True)
    print(f"   Bulunan Yeni Rekor Sayisi          : {total_new_records}", flush=True)
    print(f"   Eklenen Yeni Alternatif Cozum       : {total_alternatives}", flush=True)
    print(f"   Veritabanindaki En Yuksek Boyut     : P={max_target_p}", flush=True)
    print("=" * 125 + "\n", flush=True)


if __name__ == "__main__":
    target = 100
    if len(sys.argv) > 1:
        target = int(sys.argv[1])
    run_step_ladder_search(max_target_p=target, beam_width=5)
