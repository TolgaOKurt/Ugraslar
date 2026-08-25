#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
PSPP Merkezi Veritabani ve Cozum Kayit Yoneticisi (db_manager.py)
----------------------------------------------------------------
Tum C arama motorlari ve kullanici scriptleri bu modulu cagirarak
bulduklari dizileri veritabanina otomatik olarak isleyebilir.

Kullanim:
1. Komut Satirindan / Diger Programlardan:
   python db_manager.py --delta 10,10,10,10,10,10,2,3,1,5,2,1,3,1,1
   python db_manager.py --dizi 10,20,30,40,50,60,62,65,66,71,73,74,77,78,79
   python db_manager.py 10 10 10 10 10 10 2 3 1 5 2 1 3 1 1

2. Cift Tiklayarak / Parametresiz Calistirma:
   Girdi girilmezse interaktif olarak terminalden sayilari ister.
"""

import sys
import os
import json
import argparse
import re

# Veritabani Dosya Yolu
BASE_DIR = os.path.dirname(os.path.abspath(__file__))
DB_PATH = os.path.join(BASE_DIR, "pspp_database.json")

def evaluate_array(dizi):
    """Bir dizinin PSPP skorunu hesaplar."""
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

def dizi_to_delta(dizi):
    sorted_d = sorted(dizi)
    delta = [sorted_d[0]]
    for i in range(1, len(sorted_d)):
        delta.append(sorted_d[i] - sorted_d[i-1])
    return delta

def load_database():
    if os.path.exists(DB_PATH):
        try:
            with open(DB_PATH, "r", encoding="utf-8") as f:
                return json.load(f)
        except Exception as e:
            print(f"[UYARI] Veritabani okunamadi ({e}), yeni baslatiliyor.")
    return {}

def save_database(db):
    with open(DB_PATH, "w", encoding="utf-8") as f:
        json.dump(db, f, indent=2, ensure_ascii=False)

def _process_single_interpretation(delta, dizi, verbose=True):
    p = len(dizi)
    p_str = str(p)
    score = evaluate_array(dizi)
    sum_delta = sum(delta)
    
    db = load_database()
    current_entry = db.get(p_str, {})
    current_best = current_entry.get("score", 0)
    current_sols = current_entry.get("solutions", [])
    
    result = {
        "p": p,
        "score": score,
        "delta": delta,
        "dizi": dizi,
        "sum_delta": sum_delta
    }
    
    if score > current_best:
        db[p_str] = {
            "p": p,
            "score": score,
            "is_optimal": False,
            "solutions": [
                {
                    "delta": delta,
                    "dizi": dizi
                }
            ]
        }
        save_database(db)
        result["status"] = "NEW_RECORD"
        result["message"] = f"YENI REKOR! P = {p} icin eski skor ({current_best}) gecildi -> Yeni Skor M = {score}"
        if verbose:
            print("=" * 65)
            print(f"[REKOR] {result['message']}")
            print(f"   Delta: {delta}")
            print(f"   Dizi : {dizi}")
            print(f"   Veritabani guncellendi ve eski alt cozumler temizlendi.")
            print("=" * 65)
            
    elif score == current_best:
        is_duplicate = any(s["delta"] == delta for s in current_sols)
        if is_duplicate:
            result["status"] = "DUPLICATE"
            result["message"] = f"P = {p} icin bu cozum zaten veritabaninda kayitli (M = {score})."
            if verbose:
                print(f"[BILGI] {result['message']}")
        else:
            current_sols.append({
                "delta": delta,
                "dizi": dizi
            })
            db[p_str]["solutions"] = current_sols
            save_database(db)
            result["status"] = "ALTERNATIVE"
            result["message"] = f"ALTERNATIF COZUM! P = {p} icin M = {score} skoruna sahip yeni bir cozum eklendi (Toplam {len(current_sols)} cozum oldu)."
            if verbose:
                print("=" * 65)
                print(f"[ALTERNATIF] {result['message']}")
                print(f"   Delta: {delta}")
                print(f"   Dizi : {dizi}")
                print("=" * 65)
    else:
        result["status"] = "INFERIOR"
        result["message"] = f"YETERSIZ: P = {p} icin bulunan skor M = {score}, ancak veritabanindaki mevcut zirve M = {current_best}."
        if verbose:
            print(f"[YETERSIZ] {result['message']} (Veritabani degistirilmedi).")
            
    return result

def submit_solution(numbers, is_delta=None, verbose=True):
    if not numbers:
        return {"status": "ERROR", "message": "Bos dizi gonderildi!"}
    
    if is_delta is True:
        delta = list(numbers)
        dizi = delta_to_dizi(delta)
        return _process_single_interpretation(delta, dizi, verbose=verbose)
        
    elif is_delta is False:
        dizi = sorted(list(numbers))
        delta = dizi_to_delta(dizi)
        return _process_single_interpretation(delta, dizi, verbose=verbose)
        
    else:
        if verbose:
            print("-" * 65)
            print(f"[OTOMATIK ANALIZ] Girdi: {numbers}")
            print("Girdi hem DELTA hem de NORMAL DIZI olarak cift yonlu test ediliyor...")
            print("-" * 65)
            
        delta_as_delta = list(numbers)
        dizi_as_delta = delta_to_dizi(delta_as_delta)
        score_as_delta = evaluate_array(dizi_as_delta)
        
        dizi_as_dizi = sorted(list(numbers))
        delta_as_dizi = dizi_to_delta(dizi_as_dizi)
        score_as_dizi = evaluate_array(dizi_as_dizi)
        
        if verbose:
            print(f"1. DELTA kabul edilirse      -> Dizi: {dizi_as_delta[:6]}... -> Skor M = {score_as_delta}")
            print(f"2. NORMAL DIZI kabul edilirse -> Delta: {delta_as_dizi[:6]}... -> Skor M = {score_as_dizi}")
            print("-" * 65)
            
        if score_as_delta >= score_as_dizi:
            if verbose:
                print(f"=> En yuksek skor DELTA yorumundan geldi (M = {score_as_delta}). Bu yorum isleniyor:")
            return _process_single_interpretation(delta_as_delta, dizi_as_delta, verbose=verbose)
        else:
            if verbose:
                print(f"=> En yuksek skor NORMAL DIZI yorumundan geldi (M = {score_as_dizi}). Bu yorum isleniyor:")
            return _process_single_interpretation(delta_as_dizi, dizi_as_dizi, verbose=verbose)

def parse_numbers_from_string(text):
    """Metin içindeki tüm tam sayıları bulur."""
    return [int(x) for x in re.findall(r'-?\d+', text)]

def interactive_mode():
    print("=" * 65)
    print("   PSPP MERKEZI VERITABANI VE COZUM KAYIT YONETICISI")
    print("=" * 65)
    print("Diziyi bosluk veya virgul ile ayirarak giriniz.")
    print("Ornek Delta: 10,10,10,10,10,10,2,3,1,5,2,1,3,1,1")
    print("Ornek Dizi : 10 20 30 40 50 60 62 65 66 71 73 74 77 78 79")
    print("-" * 65)
    
    try:
        user_input = input("Sayilari Girin: ").strip()
    except (KeyboardInterrupt, EOFError):
        print("\nCikis yapildi.")
        return
        
    nums = parse_numbers_from_string(user_input)
    if not nums:
        print("[HATA] Gecerli sayi bulunamadi!")
        input("\nCikis yapmak icin Enter'a basin...")
        return
        
    print("\nBu girdi neyi temsil ediyor?")
    print(" [1] Otomatik Algila (Hem Delta hem Normal Dizi olarak test et)")
    print(" [2] Delta (Artis Adimlari) Dizisi")
    print(" [3] Normal Kumulatif Dizi")
    secim = input("Seciminiz (1, 2 veya 3, Varsayilan: 1): ").strip()
    
    is_delta = None
    if secim == "2":
        is_delta = True
    elif secim == "3":
        is_delta = False
        
    print()
    submit_solution(nums, is_delta=is_delta, verbose=True)
    input("\nCikis yapmak icin Enter'a basin...")

def main():
    # Eger hic arguman verilmemisse interaktif moda gec
    if len(sys.argv) == 1:
        interactive_mode()
        return

    parser = argparse.ArgumentParser(description="PSPP Merkezi Cozum Kayit ve Skor Hesaplayici")
    parser.add_argument("numbers", nargs="*", type=int, help="Dizi veya Delta elemanlari (boslukla ayrilmis)")
    parser.add_argument("--delta", type=str, help="Virgulle ayrilmis Delta dizisi (Orn: 10,10,10,2,1)")
    parser.add_argument("--dizi", type=str, help="Virgulle ayrilmis kumulatif dizi (Orn: 10,20,30,32,33)")
    
    args = parser.parse_args()
    
    raw_nums = []
    is_delta = None
    
    if args.delta:
        raw_nums = parse_numbers_from_string(args.delta)
        is_delta = True
    elif args.dizi:
        raw_nums = parse_numbers_from_string(args.dizi)
        is_delta = False
    elif args.numbers:
        raw_nums = args.numbers
        is_delta = None
        
    if not raw_nums:
        interactive_mode()
        return
        
    res = submit_solution(raw_nums, is_delta=is_delta)
    
    if res["status"] == "NEW_RECORD":
        sys.exit(2)
    elif res["status"] == "ALTERNATIVE":
        sys.exit(1)
    else:
        sys.exit(0)

if __name__ == "__main__":
    main()
