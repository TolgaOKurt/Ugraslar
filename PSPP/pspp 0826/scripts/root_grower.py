#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
==============================================================================
DOSYA ADI : scripts/root_grower.py
PROJE     : PSPP (Postage Stamp with Subtraction)
TANIM     : Sağlıklı Kök Tabanlı İnkremental Büyütme Motoru (Root Grower)
==============================================================================

ÇALIŞMA PRENSİBİ:
-----------------
Kombinatoryel arama uzayını O(C(N, P)) yerine, her adımda sadece kümeye
eklenecek TEK BİR SAYI arayarak O(K) lineer seviyeye indirger.

Bir başlangıç kökü (örneğin P=3 veya P=4) alır ve her adımda kümedeki tüm
sayıları muhafaza ederek en yüksek M skorunu veren yeni sayıyı (x) ekler:
    S_{p+1} = S_p ∪ {x}

Özellikler:
1. Her P adımı için elde edilen M skoru ile veritabanındaki (pspp_database.json)
   mevcut zirve skoru karşılaştırılır ve YÜZDELİK BAŞARI ORANI (% Verimlilik)
   hesaplanır:
       Başarı % = (M_kök / M_db) * 100

2. En sağlıklı kökleri (Health Index) tespit etmek için çoklu kök taraması yapar.

3. Yüksek P değerlerine tırmanırken DB zirvesine eşit veya daha iyi bir dizi
   bulunduğunda isteğe bağlı olarak otomatik db_manager.py kaydı yapar.
"""

import sys
import os
import json
import itertools
import argparse
import time
from collections import defaultdict

# Proje Kök Dizini ve Veritabanı Yolu
BASE_DIR = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
DB_PATH = os.path.join(BASE_DIR, "pspp_database.json")
sys.path.insert(0, BASE_DIR)

# Windows Konsol Karakter Uyumluluğu
if sys.stdout.encoding and sys.stdout.encoding.lower() != 'utf-8':
    try:
        sys.stdout.reconfigure(encoding='utf-8')
    except Exception:
        pass
if sys.stderr.encoding and sys.stderr.encoding.lower() != 'utf-8':
    try:
        sys.stderr.reconfigure(encoding='utf-8')
    except Exception:
        pass

try:
    from db_manager import submit_solution, load_database, save_database, evaluate_array
except ImportError:
    submit_solution = None
    def load_database():
        if os.path.exists(DB_PATH):
            with open(DB_PATH, "r", encoding="utf-8") as f:
                return json.load(f)
        return {}
    def evaluate_array(dizi):
        seen = set(dizi)
        n = len(dizi)
        for i in range(n):
            for j in range(i, n):
                seen.add(dizi[i] + dizi[j])
                diff = abs(dizi[j] - dizi[i])
                if diff > 0:
                    seen.add(diff)
        m = 0
        while (m + 1) in seen:
            m += 1
        return m


# -----------------------------------------------------------------------------
# ULTRA HIZLI SKORLAMA (Bytearray Destekli)
# -----------------------------------------------------------------------------
def fast_score_dizi(dizi: list[int]) -> int:
    """Sıralı dizi için C memchr hızında bytearray ile skor hesaplar."""
    if not dizi:
        return 0
    n = len(dizi)
    max_possible = (dizi[-1] << 1) + 2
    seen = bytearray(max_possible)
    for i in range(n):
        pi = dizi[i]
        seen[pi] = 1
        for j in range(i, n):
            pj = dizi[j]
            seen[pj + pi] = 1
            seen[pj - pi] = 1
    res = seen.find(0, 1)
    return res - 1 if res != -1 else max_possible - 1


def dizi_to_delta(dizi: list[int]) -> list[int]:
    """Sıralı normal diziyi ardışık delta farklarına dönüştürür."""
    if not dizi:
        return []
    s = sorted(dizi)
    delta = [s[0]]
    for i in range(1, len(s)):
        delta.append(s[i] - s[i - 1])
    return delta


# -----------------------------------------------------------------------------
# KÖK BÜYÜTME MOTORU (Root Grower Class)
# -----------------------------------------------------------------------------
class RootGrower:
    def __init__(self, db_path=DB_PATH):
        self.db_path = db_path
        self.db = load_database()

    def reload_db(self):
        self.db = load_database()

    def get_db_best(self, p: int) -> int:
        p_str = str(p)
        if p_str in self.db:
            return self.db[p_str].get("score", 0)
        return 0

    def grow_single_seed(
        self,
        seed: list[int],
        target_p: int,
        beam_width: int = 5,
        append_only: bool = False,
        cand_multiplier: float = 2.2,
        cand_margin: int = 30,
        submit_to_db: bool = False,
        verbose: bool = True
    ) -> dict:
        """
        Verilen kök diziyi target_p boyutuna kadar her adımda tek bir sayı
        ekleyerek büyütür. Her adımda DB ile kıyaslayarak yüzdelik başarı oranını hesaplar.
        """
        self.reload_db()
        sorted_seed = tuple(sorted(seed))
        start_p = len(sorted_seed)
        start_score = fast_score_dizi(list(sorted_seed))

        # Beam: list of tuples (sequence, score, added_element_history)
        beam = [(sorted_seed, start_score, None)]
        
        # Tarihçe: p -> {score, dizi, added, db_best, pct, status}
        history = {}
        db_best_start = self.get_db_best(start_p)
        pct_start = (start_score / db_best_start * 100.0) if db_best_start > 0 else 100.0
        
        history[start_p] = {
            "p": start_p,
            "score": start_score,
            "dizi": list(sorted_seed),
            "added": None,
            "db_best": db_best_start,
            "pct": pct_start,
            "status": "KOK"
        }

        if verbose:
            print("\n" + "=" * 115)
            print(f">>> KOK BUYUTME BASLATILIYOR: Baslangic P={start_p} -> Hedef P={target_p}")
            print(f"   Kok Dizisi : {list(sorted_seed)} -> Delta: {dizi_to_delta(list(sorted_seed))}")
            print(f"   Arama Tipi : {'Sadece Sona Ekleme (Monoton)' if append_only else 'Serbest Araya/Sona Ekleme'}")
            print(f"   Beam Genisligi : {beam_width} | DB Kaydi : {'Aktif' if submit_to_db else 'Pasif'}")
            print("=" * 115)
            print(f"{'P':>3} | {'Skor (M)':>8} | {'DB Zirve':>8} | {'Basari %':>9} | {'Eklenen':>7} | {'Durum':<13} | {'Delta (Farklar)':<30} | Normal Dizi")
            print("-" * 115)
            self._print_row(history[start_p])

        for p in range(start_p, target_p):
            candidates = []
            seen_seqs = set()

            for cur_seq, cur_score, _ in beam:
                max_val = cur_seq[-1]
                min_cand = (max_val + 1) if append_only else 1
                max_cand = max(int(max_val * cand_multiplier), int(cur_score * 2.0)) + cand_margin

                for cand in range(min_cand, max_cand):
                    if cand in cur_seq:
                        continue
                    cand_seq = tuple(sorted(cur_seq + (cand,)))
                    if cand_seq in seen_seqs:
                        continue
                    seen_seqs.add(cand_seq)
                    sc = fast_score_dizi(list(cand_seq))
                    candidates.append((sc, cand_seq, cand))

            if not candidates:
                if verbose:
                    print(f"[UYARI] P={p+1} için geçerli aday bulunamadı.")
                break

            # Skora göre büyükten küçüğe, eşitlikte daha küçük max değere göre sırala
            candidates.sort(key=lambda x: (x[0], -x[1][-1]), reverse=True)
            beam = [(seq, sc, cand) for sc, seq, cand in candidates[:beam_width]]

            next_p = p + 1
            best_seq, best_score, best_added = beam[0]
            db_best = self.get_db_best(next_p)
            pct = (best_score / db_best * 100.0) if db_best > 0 else 100.0

            status = "NORMAL"
            if db_best > 0:
                if best_score > db_best:
                    status = "[REKOR]"
                elif best_score == db_best:
                    status = "[ALTERNATIF]"
                elif pct >= 90.0:
                    status = "[%90+ ELIT]"
                elif pct >= 80.0:
                    status = "[%80+ GUCLU]"
                elif pct >= 70.0:
                    status = "[%70+ IYI]"
                else:
                    status = "[%60- DUSUK]"

            row = {
                "p": next_p,
                "score": best_score,
                "dizi": list(best_seq),
                "added": best_added,
                "db_best": db_best,
                "pct": pct,
                "status": status
            }
            history[next_p] = row

            if verbose:
                self._print_row(row)

            # DB'ye kayıt seçeneği
            if submit_to_db and submit_solution and db_best > 0:
                if best_score >= db_best:
                    res = submit_solution(list(best_seq), is_delta=False, verbose=False)
                    if res.get("status") in ("NEW_RECORD", "ALTERNATIVE"):
                        self.reload_db()

        # Özet İstatistikler
        pct_list = [h["pct"] for h in history.values() if h["db_best"] > 0]
        avg_pct = (sum(pct_list) / len(pct_list)) if pct_list else 0.0
        max_pct = max(pct_list) if pct_list else 0.0
        min_pct = min(pct_list) if pct_list else 0.0

        if verbose:
            print("-" * 90)
            print(f"[PERFORMANS OZETI]:")
            print(f"   Ortalama Basari Orani : %{avg_pct:.2f}")
            print(f"   Zirve Basari Orani    : %{max_pct:.2f} (Min: %{min_pct:.2f})")
            print(f"   Ulasilan Son Skor     : P={target_p} -> M={history[target_p]['score']}")
            print("=" * 90 + "\n")

        return {
            "seed": list(sorted_seed),
            "start_p": start_p,
            "target_p": target_p,
            "avg_pct": avg_pct,
            "max_pct": max_pct,
            "min_pct": min_pct,
            "history": history
        }

    def _print_row(self, row: dict):
        dizi = row['dizi']
        delta = dizi_to_delta(dizi)
        delta_str = str(delta)
        if len(delta_str) > 30:
            delta_str = delta_str[:27] + "..."
        dizi_str = str(dizi)
        if len(dizi_str) > 36:
            dizi_str = dizi_str[:33] + "..."
        added_str = f"+{row['added']}" if row['added'] is not None else "-"
        db_str = str(row['db_best']) if row['db_best'] > 0 else "-"
        print(f"{row['p']:3d} | {row['score']:8d} | {db_str:>8} | %{row['pct']:8.1f} | {added_str:>7} | {row['status']:<13} | {delta_str:<30} | {dizi_str}")

    def screen_healthy_roots(
        self,
        p_root: int = 3,
        eval_up_to: int = 10,
        beam_width: int = 5,
        top_n: int = 5
    ) -> list[dict]:
        """
        Belirtilen p_root boyutundaki potansiyel kökleri tarar, her birini
        eval_up_to boyutuna kadar büyütür ve 'Ortalama Başarı %' (Health Index)
        değerine göre sıralayarak en sağlıklı kökleri döndürür.
        """
        self.reload_db()
        candidate_seeds = []

        # 1. Veritabanındaki hazır optimumlar
        db_rec = self.db.get(str(p_root), {})
        for sol in db_rec.get("solutions", []):
            d = sol.get("dizi")
            if d and d not in candidate_seeds:
                candidate_seeds.append(d)

        # 2. P=3 ise sistematik kombinasyonlar
        if p_root == 3:
            for a in range(1, 10):
                for b in range(a + 1, 14):
                    for c in range(b + 1, 18):
                        seq = [a, b, c]
                        sc = fast_score_dizi(seq)
                        if sc >= 7 and seq not in candidate_seeds:
                            candidate_seeds.append(seq)
                            
        # 3. P=4 ise sistematik yüksek skorlu adaylar
        elif p_root == 4:
            # DB dışı popüler modüler kökler
            extra_seeds = [
                [1, 2, 4, 7],
                [1, 3, 5, 8],
                [2, 3, 7, 11],
                [1, 3, 7, 12],
                [2, 5, 8, 12],
                [1, 4, 9, 14]
            ]
            for es in extra_seeds:
                if es not in candidate_seeds:
                    candidate_seeds.append(es)

        print("\n" + "=" * 90)
        print(f"[SAGLIKLI KOK TARAMASI]: P={p_root} Kokleri Test Ediliyor (Hedef Degerlendirme P={eval_up_to})")
        print(f"   Toplam Taranacak Aday Kok Sayisi: {len(candidate_seeds)}")
        print("=" * 90)

        results = []
        for idx, seed in enumerate(candidate_seeds, 1):
            res = self.grow_single_seed(
                seed=seed,
                target_p=eval_up_to,
                beam_width=beam_width,
                submit_to_db=False,
                verbose=False
            )
            results.append(res)
            print(f"[{idx:2d}/{len(candidate_seeds)}] Kok: {seed} | Skor: M={res['history'][p_root]['score']} | Ortalama Basari: %{res['avg_pct']:.2f}")

        # Sağlık İndeksine göre sırala (avg_pct azalan)
        results.sort(key=lambda x: x["avg_pct"], reverse=True)

        print("\n" + "=" * 90)
        print(f"[EN SAGLIKLI {top_n} KOK SIRALAMASI] (P={p_root} -> P={eval_up_to}):")
        print("=" * 90)
        print(f"{'Sira':>4} | {'Kok Dizisi':<24} | {'Ort. Basari %':>13} | {'Zirve %':>8} | Yol Haritasi (Skorlar)")
        print("-" * 90)
        for rank, item in enumerate(results[:top_n], 1):
            scores_str = " -> ".join([f"P{p}={h['score']}" for p, h in sorted(item["history"].items())])
            print(f"{rank:4d} | {str(item['seed']):<24} | %{item['avg_pct']:11.2f} | %{item['max_pct']:6.1f} | {scores_str}")
        print("=" * 90 + "\n")

        return results[:top_n]


# -----------------------------------------------------------------------------
# KOMUT SATIRI ARAYÜZÜ (CLI)
# -----------------------------------------------------------------------------
def parse_args():
    parser = argparse.ArgumentParser(
        description="PSPP Sağlıklı Kök Tabanlı İnkremental Büyütme Motoru"
    )
    parser.add_argument(
        "--seed",
        type=str,
        default=None,
        help="Başlangıç kök dizisi (Virgülle veya boşlukla ayrılmış, örn: 2,4,5 veya '4 6 7 9')"
    )
    parser.add_argument(
        "--target-p",
        type=int,
        default=12,
        help="Büyütülecek hedef P boyutu (varsayılan: 12)"
    )
    parser.add_argument(
        "--beam",
        type=int,
        default=10,
        help="Beam search genişliği (varsayılan: 10, saf greedy için 1)"
    )
    parser.add_argument(
        "--append-only",
        action="store_true",
        help="Sadece sona ekleme yap (mevcut elemanlardan daha küçük sayı eklemez)"
    )
    parser.add_argument(
        "--scan-roots",
        action="store_true",
        help="Belirli bir P boyutu için tüm potansiyel kökleri tara ve en sağlıklıları sırala"
    )
    parser.add_argument(
        "--p-root",
        type=int,
        default=3,
        help="Kök taraması için başlangıç P boyutu (varsayılan: 3)"
    )
    parser.add_argument(
        "--from-db",
        type=int,
        default=None,
        help="Veritabanındaki belirtilen P boyutunun optimum çözümünü doğrudan kök olarak çek (örn: --from-db 15)"
    )
    parser.add_argument(
        "--submit",
        action="store_true",
        help="DB zirvesine eşit veya daha iyi bir dizi bulunduğunda otomatik veritabanına kaydet"
    )
    return parser.parse_args()


def parse_seed_string(seed_str: str) -> list[int]:
    cleaned = seed_str.replace(",", " ").strip()
    return [int(x) for x in cleaned.split()]


def interactive_menu(grower: RootGrower):
    print("\n" + "=" * 65)
    print("      [PSPP] SAGLIKLI KOK BUYUTME MOTORU (ROOT GROWER)        ")
    print("=" * 65)
    print("1. Belirli bir kökü büyüt (örn: 2,4,5 veya 4,6,7,9)")
    print("2. Veritabanından hazır bir P optimumunu kök alıp büyüt (örn: P=15)")
    print("3. En sağlıklı P=3 köklerini tara ve sırala")
    print("4. En sağlıklı P=4 köklerini tara ve sırala")
    print("5. Çıkış")
    print("-" * 65)
    choice = input("Seçiminiz (1-5): ").strip()

    if choice == "1":
        s_input = input("Kök diziyi girin (örn: 2,4,5): ").strip()
        seed = parse_seed_string(s_input)
        target = int(input("Hedef P boyutu (örn: 15): ").strip() or "15")
        beam = int(input("Beam genişliği (örn: 10): ").strip() or "10")
        grower.grow_single_seed(seed=seed, target_p=target, beam_width=beam, submit_to_db=True)

    elif choice == "2":
        p_db = int(input("Veritabanından hangi P boyutu kök alınsın? (örn: 15): ").strip() or "15")
        p_str = str(p_db)
        if p_str in grower.db and grower.db[p_str].get("solutions"):
            seed = grower.db[p_str]["solutions"][0]["dizi"]
            print(f"[BILGI] P={p_db} için veritabanındaki optimum dizi çekildi: {seed}")
            target = int(input(f"Hangi P boyutuna kadar büyütülsün? (örn: {p_db + 10}): ").strip() or str(p_db + 10))
            beam = int(input("Beam genişliği (örn: 10): ").strip() or "10")
            grower.grow_single_seed(seed=seed, target_p=target, beam_width=beam, submit_to_db=True)
        else:
            print(f"[HATA] P={p_db} için veritabanında kayıtlı çözüm bulunamadı!")

    elif choice == "3":
        target = int(input("Hangi P'ye kadar değerlendirilsin? (örn: 10): ").strip() or "10")
        grower.screen_healthy_roots(p_root=3, eval_up_to=target, top_n=5)

    elif choice == "4":
        target = int(input("Hangi P'ye kadar değerlendirilsin? (örn: 11): ").strip() or "11")
        grower.screen_healthy_roots(p_root=4, eval_up_to=target, top_n=5)


def main():
    args = parse_args()
    grower = RootGrower()

    if args.scan_roots:
        grower.screen_healthy_roots(
            p_root=args.p_root,
            eval_up_to=args.target_p,
            beam_width=args.beam,
            top_n=5
        )
    elif args.from_db is not None:
        p_str = str(args.from_db)
        if p_str in grower.db and grower.db[p_str].get("solutions"):
            seed = grower.db[p_str]["solutions"][0]["dizi"]
            print(f"\n[DB CEKIMI] P={args.from_db} icin optimum cozum veritabanindan cekildi:")
            print(f"   Dizi : {seed}")
            print(f"   Skor : M={grower.db[p_str].get('score', 0)}")
            grower.grow_single_seed(
                seed=seed,
                target_p=args.target_p,
                beam_width=args.beam,
                append_only=args.append_only,
                submit_to_db=args.submit
            )
        else:
            print(f"[HATA] P={args.from_db} icin veritabaninda kayitli cozum bulunamadi!")
    elif args.seed:
        seed = parse_seed_string(args.seed)
        grower.grow_single_seed(
            seed=seed,
            target_p=args.target_p,
            beam_width=args.beam,
            append_only=args.append_only,
            submit_to_db=args.submit
        )
    else:
        interactive_menu(grower)


if __name__ == "__main__":
    main()
