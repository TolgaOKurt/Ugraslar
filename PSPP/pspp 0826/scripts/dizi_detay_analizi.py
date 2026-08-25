#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
PSPP Ayrintili Dizi ve Matematiksel Kapsama Analizoru (dizi_detay_analizi.py)
-------------------------------------------------------------------------
Herhangi bir boyuttaki (P) bir dizinin veya veritabanindaki kayitli bir cozumu
1'den M'ye kadar tum sayilari nasil urettigini, tekilleri, toplamlari, farklari,
kopus noktasini ve verimliligini ayrintili olarak raporlar.

Kullanim:
1. Komut Satirindan:
   python dizi_detay_analizi.py --p 15
   python dizi_detay_analizi.py --delta 9,9,9,9,9,1,4,3,3,1,1,2,1
   python dizi_detay_analizi.py 10 20 30 40 50 60 62 65 66 71 73 74 77 78 79

2. Parametresiz / Cift Tiklama:
   Terminalden secenek ve sayi girmeyi ister.
"""

import sys
import os
import json
import argparse
import re

BASE_DIR = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
DB_PATH = os.path.join(BASE_DIR, "pspp_database.json")

def delta_to_dizi(delta):
    res = []
    s = 0
    for d in delta:
        s += d
        res.append(s)
    return res

def dizi_to_delta(dizi):
    sorted_d = sorted(dizi)
    delta = [sorted_d[0]]
    for i in range(1, len(sorted_d)):
        delta.append(sorted_d[i] - sorted_d[i-1])
    return delta

def parse_numbers_from_string(text):
    return [int(x) for x in re.findall(r'-?\d+', text)]

def analyze_and_print(dizi, delta, title="DIZI ANALIZI"):
    p = len(dizi)
    seen = set(dizi)
    n = len(dizi)
    sum_map = {}
    diff_map = {}
    
    for i in range(n):
        for j in range(i, n):
            s = dizi[i] + dizi[j]
            seen.add(s)
            sum_map.setdefault(s, []).append((dizi[i], dizi[j]))
            
            d = dizi[j] - dizi[i]
            if d > 0:
                seen.add(d)
                diff_map.setdefault(d, []).append((dizi[j], dizi[i]))
                
    m = 0
    while (m + 1) in seen:
        m += 1
        
    sum_delta = sum(delta)
    upper_ceil = p * (p + 1)
    efficiency = (m / upper_ceil) * 100.0 if upper_ceil > 0 else 0
    
    print("\n" + "=" * 70)
    print(f"[ANALIZ] {title}")
    print("=" * 70)
    print(f"Boyut (P)             : {p}")
    print(f"Delta Dizisi (D)      : {delta}")
    print(f"Kumulatif Dizi (P)    : {dizi}")
    print(f"Dizi Toplami (P_son)  : {dizi[-1]} (Delta Toplami: {sum_delta})")
    print(f"Maksimum Kesintisiz M : {m}")
    print(f"Ilk Kopus / Eksik Sayi: {m + 1} ({m + 1} sayisi uretilemiyor)")
    print(f"Teorik Sifir Cakisma  : {upper_ceil}")
    print(f"Kapasite Verimliligi  : %{efficiency:.1f}")
    
    if m == 2 * dizi[-1]:
        print(f"Tepe Kapanis Durumu   : %100 KUSURSUZ (M = 2 x P_son = {m})")
    else:
        print(f"Tepe Kapanis Durumu   : 2 x P_son = {2 * dizi[-1]} (Fark: {2 * dizi[-1] - m})")
        
    print("-" * 70)
    print("SAYILARIN URETIM HARITASI (Ozet Dagilim):")
    
    nat_count = 0
    sum_count = 0
    diff_count = 0
    both_count = 0
    
    for num in range(1, m + 1):
        is_nat = num in dizi
        is_sum = num in sum_map
        is_diff = num in diff_map
        
        if is_nat:
            nat_count += 1
        elif is_sum and is_diff:
            both_count += 1
        elif is_sum:
            sum_count += 1
        elif is_diff:
            diff_count += 1
            
    print(f" * Dogal Elemanlar (Dogrudan P icinde) : {nat_count:3d} adet")
    print(f" * Yalnizca Toplam ile Uretilenler     : {sum_count:3d} adet")
    print(f" * Yalnizca Fark ile Uretilenler       : {diff_count:3d} adet")
    print(f" * Hem Toplam Hem Fark ile Uretilenler : {both_count:3d} adet")
    print(f" * TOPLAM KESINTISIZ MENZIL            : {m:3d} adet sayi")
    print("=" * 70 + "\n")

def analyze_p_from_db(p_val):
    if not os.path.exists(DB_PATH):
        print(f"[HATA] Veritabani bulunamadi: {DB_PATH}")
        return
    with open(DB_PATH, "r", encoding="utf-8") as f:
        db = json.load(f)
    p_str = str(p_val)
    if p_str not in db:
        print(f"[HATA] P = {p_val} veritabaninda bulunamadi!")
        return
    sols = db[p_str].get("solutions", [])
    for idx, sol in enumerate(sols):
        d = sol["delta"]
        p_arr = sol["dizi"]
        analyze_and_print(p_arr, d, title=f"P = {p_val} VERITABANI ANALIZI (Cozum #{idx+1})")

def interactive_mode():
    print("=" * 70)
    print("   PSPP AYRINTILI DIZI VE MATEMATIKSEL ANALIZORU")
    print("=" * 70)
    print(" [1] Veritabanindaki kayitli bir P boyutunu analiz et")
    print(" [2] Ozel bir Delta veya Normal Dizi girerek analiz et")
    secim = input("Seciminiz (1 veya 2, Varsayilan: 1): ").strip()
    
    if secim == "2":
        raw = input("\nSayilari girin (Bosluk veya virgul ile): ").strip()
        nums = parse_numbers_from_string(raw)
        if not nums:
            print("[HATA] Gecerli sayi girilmedi!")
            input("\nCikis icin Enter'a basin...")
            return
        is_increasing = all(nums[i] < nums[i+1] for i in range(len(nums)-1))
        if is_increasing and len(nums) > 3 and nums[-1] > sum(nums[:3]):
            dizi = nums
            delta = dizi_to_delta(dizi)
        else:
            delta = nums
            dizi = delta_to_dizi(delta)
        analyze_and_print(dizi, delta, title="OZEL DIZI ANALIZI")
    else:
        p_in = input("\nIncelenecek P boyutunu girin (Ornek: 15): ").strip()
        if p_in.isdigit():
            analyze_p_from_db(int(p_in))
        else:
            print("[HATA] Gecersiz P degeri!")
            
    input("\nCikis yapmak icin Enter'a basin...")

def main():
    if len(sys.argv) == 1:
        interactive_mode()
        return

    parser = argparse.ArgumentParser(description="PSPP Ayrintili Dizi Analizoru")
    parser.add_argument("numbers", nargs="*", type=int, help="Boslukla ayrilmis sayilar")
    parser.add_argument("--p", type=int, default=None, help="Veritabanindaki P boyutunu analiz et")
    parser.add_argument("--delta", type=str, default=None, help="Virgulle ayrilmis Delta dizisi")
    parser.add_argument("--dizi", type=str, default=None, help="Virgulle ayrilmis normal dizi")
    
    args = parser.parse_args()
    
    if args.p is not None:
        analyze_p_from_db(args.p)
        sys.exit(0)
        
    if args.delta:
        delta = parse_numbers_from_string(args.delta)
        dizi = delta_to_dizi(delta)
        analyze_and_print(dizi, delta, title="OZEL DELTA DIZISI ANALIZI")
    elif args.dizi:
        dizi = sorted(parse_numbers_from_string(args.dizi))
        delta = dizi_to_delta(dizi)
        analyze_and_print(dizi, delta, title="OZEL KUMULATIF DIZI ANALIZI")
    elif args.numbers:
        nums = args.numbers
        is_increasing = all(nums[i] < nums[i+1] for i in range(len(nums)-1))
        if is_increasing and len(nums) > 3 and nums[-1] > sum(nums[:3]):
            dizi = nums
            delta = dizi_to_delta(dizi)
        else:
            delta = nums
            dizi = delta_to_dizi(delta)
        analyze_and_print(dizi, delta, title="OZEL DIZI ANALIZI")
    else:
        interactive_mode()

if __name__ == "__main__":
    main()
