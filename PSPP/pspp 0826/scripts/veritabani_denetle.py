#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
PSPP Veritabani Denetleme ve Temizlik Araci (veritabani_denetle.py)
------------------------------------------------------------------
pspp_database.json dosyasindaki tum boyutlari (veya secilen tek bir boyutu)
matematiksel olarak bastan hesaplar, skorlari dogrular, eski/dusuk skorlu
cozumleri siler ve veritabaninin butunlugunu garanti altina alir.

Kullanim:
1. Komut Satirindan:
   python veritabani_denetle.py          (Tum veritabanini denetler ve temizler)
   python veritabani_denetle.py --p 15   (Sadece P = 15 boyutunu denetler)

2. Parametresiz / Cift Tiklama:
   Terminalden secenek secmenizi ister ve pencereyi acik tutar.
"""

import sys
import os
import json
import argparse

BASE_DIR = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
DB_PATH = os.path.join(BASE_DIR, "pspp_database.json")

def evaluate_array(dizi):
    """Bir dizinin kesintisiz 1..M menzil skorunu hesaplar."""
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

def delta_to_dizi(delta):
    res = []
    s = 0
    for d in delta:
        s += d
        res.append(s)
    return res

def run_audit(target_p=None):
    if not os.path.exists(DB_PATH):
        print(f"[HATA] Veritabani bulunamadi: {DB_PATH}")
        return
        
    with open(DB_PATH, "r", encoding="utf-8") as f:
        db = json.load(f)
        
    print("=" * 70)
    print("PSPP VERITABANI MATEMATIKSEL DENETLEME VE TEMIZLIK RAPORU")
    print("=" * 70)
    
    cleaned_db = {}
    total_removed = 0
    total_fixed_scores = 0
    
    items = sorted(db.items(), key=lambda x: int(x[0]))
    if target_p is not None:
        items = [(k, v) for k, v in items if int(k) == target_p]
        if not items:
            print(f"[BILGI] P = {target_p} boyutu veritabaninda bulunamadi.")
            return
            
    for p_str, data in items:
        p = int(p_str)
        recorded_score = data.get("score", 0)
        sols = data.get("solutions", [])
        
        scored_sols = []
        for s in sols:
            delta = s["delta"]
            dizi = delta_to_dizi(delta)
            actual_m = evaluate_array(dizi)
            scored_sols.append((actual_m, delta, dizi))
            
        if not scored_sols:
            max_score = recorded_score
            valid_sols = []
        else:
            max_score = max(s[0] for s in scored_sols)
            valid_sols = [s for s in scored_sols if s[0] == max_score]
            sub_sols = [s for s in scored_sols if s[0] < max_score]
            
            if sub_sols:
                print(f"\n[TEMIZLIK] P = {p:2d} (Zirve Skor: M = {max_score}):")
                for sub in sub_sols:
                    print(f"   -> Dusuk skorlu eski cozum silindi (M = {sub[0]:3d}): {sub[1]}")
                    total_removed += 1
                    
            if max_score != recorded_score:
                print(f"[DUZELTME] P = {p:2d}: Kayitli Skor ({recorded_score}) -> Gercek Skor ({max_score}) olarak guncellendi.")
                total_fixed_scores += 1
                
        cleaned_sols = []
        seen_deltas = set()
        for s in valid_sols:
            d_tuple = tuple(s[1])
            if d_tuple not in seen_deltas:
                seen_deltas.add(d_tuple)
                cleaned_sols.append({
                    "delta": s[1],
                    "dizi": s[2]
                })
                
        cleaned_db[p_str] = {
            "p": p,
            "score": max_score,
            "is_optimal": data.get("is_optimal", False),
            "solutions": cleaned_sols
        }
        
    if target_p is None:
        with open(DB_PATH, "w", encoding="utf-8") as f:
            json.dump(cleaned_db, f, indent=2, ensure_ascii=False)
            
    print("\n" + "=" * 70)
    print("GUNCEL VE TEMIZ VERITABANI OZET TABLOSU")
    print("=" * 70)
    print(f"{'P':>3} | {'Zirve Skor (M)':>14} | {'Gecerli Cozum':>14} | {'Ornek Delta':<30}")
    print("-" * 70)
    
    for p_str, data in sorted(cleaned_db.items(), key=lambda x: int(x[0])):
        p = int(p_str)
        score = data["score"]
        s_count = len(data["solutions"])
        first_delta = str(data["solutions"][0]["delta"][:5]) + "..." if data["solutions"] else "[]"
        print(f"{p:3d} | {score:14d} | {s_count:14d} | {first_delta:<30}")
        
    print("=" * 70)
    print(f"Denetim tamamlandi! ({total_removed} adet alt cozum silindi, {total_fixed_scores} skor duzeltildi).\n")

def interactive_mode():
    print("=" * 70)
    print("   PSPP VERITABANI DENETLEME VE TEMIZLIK ARACI")
    print("=" * 70)
    print(" [1] Tum veritabanini denetle ve temizle")
    print(" [2] Sadece belirli bir P boyutunu denetle")
    secim = input("Seciminiz (1 veya 2, Varsayilan: 1): ").strip()
    
    if secim == "2":
        p_in = input("Denetlenecek P boyutunu girin (Ornek: 15): ").strip()
        if p_in.isdigit():
            run_audit(target_p=int(p_in))
        else:
            print("[HATA] Gecersiz P degeri!")
    else:
        run_audit(target_p=None)
        
    input("\nCikis yapmak icin Enter'a basin...")

def main():
    if len(sys.argv) == 1:
        interactive_mode()
        return
        
    parser = argparse.ArgumentParser(description="PSPP Veritabani Denetleyici ve Temizleyici")
    parser.add_argument("--p", type=int, default=None, help="Sadece belirli bir P boyutunu denetle")
    args = parser.parse_args()
    
    run_audit(target_p=args.p)

if __name__ == "__main__":
    main()
