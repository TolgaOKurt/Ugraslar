#!/usr/bin/env python3
"""
PSPP Z3 SMT KISIT PROGRAMLAMA COZUCU (z3_solver.py)
---------------------------------------------------
Bu motor, Para Sayma Problemi'ni (PSPP) Boolean/Integer kısıtlarına 
dönüştürerek Z3 SMT çözücüsü (CDCL - Çatışma Odaklı Madde Öğrenme) 
ile çözer.

Kullanım Örnekleri:
  python z3_solver.py 7 --target 40
  python z3_solver.py 8 --target 52 --all
  python z3_solver.py 8 --optimize
"""

import sys
import os
import time
import json
import argparse
from z3 import *

DB_FILE = os.path.join(os.path.dirname(__file__), "pspp_database.json")
if not os.path.exists(DB_FILE):
    DB_FILE = "pspp_database.json"

def load_database():
    if os.path.exists(DB_FILE):
        try:
            with open(DB_FILE, "r", encoding="utf-8") as f:
                return json.load(f)
        except Exception:
            pass
    return {}

def save_database(db):
    try:
        with open(DB_FILE, "w", encoding="utf-8") as f:
            json.dump(db, f, indent=2)
    except Exception as e:
        print(f"Veritabani yazma hatasi: {e}")

def get_known_max(db, P):
    p_key = str(P)
    if p_key in db:
        return db[p_key].get("score", 0)
    return 0

def db_add_solution(db, P, score, p_sol, delta_sol):
    p_key = str(P)
    if p_key not in db:
        db[p_key] = {
            "p": P,
            "score": score,
            "is_optimal": False,
            "solutions": []
        }
    
    rec = db[p_key]
    existing_score = rec.get("score", 0)
    
    if score > existing_score:
        rec["score"] = score
        rec["solutions"] = [{
            "delta": delta_sol,
            "dizi": p_sol
        }]
        return "NEW_RECORD"
    elif score == existing_score:
        # Daha once kaydedilmis mi kontrol et
        for s in rec.get("solutions", []):
            if s.get("delta") == delta_sol:
                return "ALREADY_EXISTS"
        if "solutions" not in rec:
            rec["solutions"] = []
        rec["solutions"].append({
            "delta": delta_sol,
            "dizi": p_sol
        })
        return "NEW_ALTERNATIVE"
    return "LOWER_SCORE"

def create_pspp_solver(P, M_target, timeout_ms=0, symmetry_break=True):
    solver = Solver()
    if timeout_ms > 0:
        solver.set("timeout", timeout_ms)
    
    # 1. Normal Dizi Değişkenleri: 0 < p[0] < p[1] < ... < p[P-1]
    p = [Int(f"p_{i}") for i in range(P)]
    
    solver.add(p[0] >= 1)
    for i in range(P - 1):
        solver.add(p[i] < p[i+1])
    
    # 2. Matematiksel Kanun Kısıtları
    # Kanun 1: Alt Eşik (p[P-1] >= (M_target + 1) / 2)
    min_required_sum = (M_target + 1 + 1) // 2
    solver.add(p[P-1] >= min_required_sum)
    
    # Kanun 3: Güvercin Yuvası Tavanı
    teorik_tavan = P * P + P
    solver.add(p[P-1] <= teorik_tavan)
    
    # İlk eleman sınırı
    if symmetry_break:
        solver.add(p[0] <= P)
        
    # Kanun 2: 1'in Uretimi Zorunlulugu
    has_one_ways = [p[0] == 1] + [p[i+1] - p[i] == 1 for i in range(P - 1)]
    solver.add(Or(has_one_ways))
    
    # Kanun 2b: 2'nin Uretimi Zorunlulugu
    has_two_ways = [p[0] <= 2] + [p[i+1] - p[i] == 2 for i in range(P - 1)]
    for i in range(P - 2):
        has_two_ways.append(And(p[i+1] - p[i] == 1, p[i+2] - p[i+1] == 1))
    solver.add(Or(has_two_ways))
    
    # 3. KESİNTİSİZ 1..M MENZİL KAPSAMA KISITLARI
    for k in range(1, M_target + 1):
        ways = []
        
        # Tekil pullar
        for i in range(P):
            ways.append(p[i] == k)
            
        # İkili toplamlar ve farklar (Difference Logic)
        for i in range(P):
            for j in range(i, P):
                ways.append(p[i] + p[j] == k)
                if i != j:
                    ways.append(p[j] - p[i] == k)
        
        # Her k için en az bir yol geçerli olmalı
        solver.add(Or(ways))
    
    return solver, p

def verify_solution(p_arr, delta_arr, P):
    seen = set()
    for pi in p_arr:
        seen.add(pi)
    for i in range(P):
        for j in range(i, P):
            seen.add(p_arr[i] + p_arr[j])
            if i != j:
                seen.add(p_arr[j] - p_arr[i])
    m = 1
    while m in seen:
        m += 1
    return m - 1

def run_target_search(P, M_target, find_all=False, timeout_sec=60):
    print("=====================================================================")
    print("          PSPP Z3 SMT KISIT COZUCU (CDCL MANTIK MOTORU)              ")
    print("=====================================================================")
    print(f"Boyut (P)        : {P}")
    print(f"Hedef Esik (M)   : {M_target}")
    print(f"Arama Modu       : {'Tum Cozumleri Bul (All Models)' if find_all else 'Tek Cozum (Single SAT)'}")
    print(f"Zaman Asimi      : {timeout_sec} saniye")
    print("---------------------------------------------------------------------")
    
    db = load_database()
    known_m = get_known_max(db, P)
    if known_m > 0:
        print(f"Bilgi Tabanindaki Mevcut Rekor: M = {known_m}")
    print("=====================================================================\n")
    
    t_start = time.time()
    solver, p = create_pspp_solver(P, M_target, timeout_ms=int(timeout_sec * 1000))
    
    solutions_found = []
    
    while True:
        t0 = time.time()
        res = solver.check()
        step_elapsed = time.time() - t0
        
        if res == sat:
            model = solver.model()
            p_sol = [model[p[i]].as_long() for i in range(P)]
            delta_sol = [p_sol[0]] + [p_sol[i] - p_sol[i-1] for i in range(1, P)]
            verified_m = verify_solution(p_sol, delta_sol, P)
            
            solutions_found.append((p_sol, delta_sol, verified_m))
            sol_count = len(solutions_found)
            
            print(f"  >>> [COZUM #{sol_count} BULUNDU] (Adim Suresi: {step_elapsed:.3f} sn)")
            print(f"      Normal Dizi : {p_sol}")
            print(f"      Delta Dizi  : {delta_sol}")
            print(f"      Gercek Skor : M = {verified_m}")
            print()
            
            if not find_all:
                break
                
            # Bulunan çözümü engelleyen kural ekle (Blocking Clause)
            block_clause = Or([p[i] != p_sol[i] for i in range(P)])
            solver.add(block_clause)
            
        elif res == unsat:
            if len(solutions_found) == 0:
                print(f"  [UNSAT] M = {M_target} icin hicbir cozum BULUNAMAZ (Kanitlandi).")
            else:
                print(f"  [TAMAMLANDI] Baska alternatif cozum bulunmuyor (Toplam: {len(solutions_found)} cozum).")
            break
        else:
            print(f"  [TIMEOUT] Zaman asimina ugrandi ({timeout_sec} sn).")
            break
            
    total_time = time.time() - t_start
    print("\n=====================================================================")
    print("                        ARAMA RAPORU                                 ")
    print("=====================================================================")
    print(f"Toplam Gecen Sure     : {total_time:.4f} saniye")
    print(f"Bulunan Cozum Sayisi  : {len(solutions_found)}")
    if solutions_found:
        print(f"Ulasilan Zirve Skor   : M = {solutions_found[0][2]}")
    print("=====================================================================")
    
    # DB Güncelleme
    if solutions_found:
        updated = False
        for p_sol, d_sol, m_sol in solutions_found:
            res = db_add_solution(db, P, m_sol, p_sol, d_sol)
            if res in ("NEW_RECORD", "NEW_ALTERNATIVE"):
                updated = True
        if updated:
            save_database(db)
            print("[Veritabani Guncellendi ve Kaydedildi: pspp_database.json]")

def optimize_search(P, start_m=0, timeout_per_step=30):
    print("=====================================================================")
    print("          PSPP Z3 SMT OPTIMIZASYON ARAYICISI (REKOR TIRMANMA)        ")
    print("=====================================================================")
    print(f"Boyut (P)        : {P}")
    print(f"Adim Zaman Asimi : {timeout_per_step} saniye")
    print("=====================================================================\n")
    
    db = load_database()
    known_m = get_known_max(db, P)
    current_m = start_m if start_m > 0 else (known_m if known_m > 0 else 1)
    
    best_solution = None
    
    while True:
        print(f">> M = {current_m} icin Z3 cozucu calistiriliyor...", end=" ", flush=True)
        t0 = time.time()
        solver, p = create_pspp_solver(P, current_m, timeout_ms=int(timeout_per_step * 1000))
        res = solver.check()
        elapsed = time.time() - t0
        
        if res == sat:
            model = solver.model()
            p_sol = [model[p[i]].as_long() for i in range(P)]
            delta_sol = [p_sol[0]] + [p_sol[i] - p_sol[i-1] for i in range(1, P)]
            best_solution = (current_m, p_sol, delta_sol)
            print(f"[SAT] Basarili! ({elapsed:.3f} sn) -> Delta: {delta_sol}")
            current_m += 1
        elif res == unsat:
            print(f"[UNSAT] ({elapsed:.3f} sn) -> Maksimum Zirve: M = {current_m - 1}")
            break
        else:
            print(f"[TIMEOUT] ({elapsed:.3f} sn)")
            break
            
    if best_solution:
        m_best, p_best, d_best = best_solution
        print("\n=====================================================================")
        print(f"ZIRVE REKOR BULUNDU: M = {m_best}")
        print(f"Normal Dizi : {p_best}")
        print(f"Delta Dizi  : {d_best}")
        print("=====================================================================")
        
        res = db_add_solution(db, P, m_best, p_best, d_best)
        if res in ("NEW_RECORD", "NEW_ALTERNATIVE"):
            save_database(db)
            print("[Veritabani Guncellendi ve Kaydedildi: pspp_database.json]")

def interactive_mode():
    print("=====================================================================")
    print("        PSPP Z3 SMT KISIT COZUCU (INTERAKTIF MOD)                    ")
    print("=====================================================================")
    
    p_input = input("Hedef Boyut P [Varsayilan: 7]: ").strip()
    P = int(p_input) if p_input else 7
    
    db = load_database()
    known_m = get_known_max(db, P)
    
    print("\nIslem Secin:")
    print("  1) Belirli bir M skoru icin cozum ara")
    print("  2) Belirli bir M skoru icin TUM alternatif cozumleri bul")
    print("  3) Zirve skoru otomatik bul (Optimizasyon / Rekor Tirmanma)")
    secim = input("Seciminiz [1/2/3, Varsayilan: 1]: ").strip()
    
    if secim == "2":
        m_input = input(f"Hedef M Esiği [Varsayilan: {known_m}]: ").strip()
        M = int(m_input) if m_input else known_m
        run_target_search(P, M, find_all=True)
    elif secim == "3":
        optimize_search(P, start_m=known_m)
    else:
        m_input = input(f"Hedef M Esiği [Varsayilan: {known_m}]: ").strip()
        M = int(m_input) if m_input else known_m
        run_target_search(P, M, find_all=False)

if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="PSPP Z3 SMT Constraint Solver")
    parser.add_argument("P", type=int, nargs="?", default=0, help="Boyut P")
    parser.add_argument("--target", type=int, default=0, help="Hedef M skoru")
    parser.add_argument("--all", action="store_true", help="Tum alternatif modelleri bul")
    parser.add_argument("--optimize", action="store_true", help="Zirve M skorunu otomatik maksimize et")
    parser.add_argument("--timeout", type=int, default=60, help="Zaman asimi (saniye)")
    
    args = parser.parse_args()
    
    if args.P == 0:
        interactive_mode()
    else:
        db = load_database()
        known_m = get_known_max(db, args.P)
        target_m = args.target if args.target > 0 else known_m
        
        if args.optimize:
            optimize_search(args.P, start_m=target_m, timeout_per_step=args.timeout)
        else:
            run_target_search(args.P, target_m, find_all=args.all, timeout_sec=args.timeout)
