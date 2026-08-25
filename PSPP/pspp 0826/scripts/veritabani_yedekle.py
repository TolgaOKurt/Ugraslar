#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
PSPP Veritabani Yedekleme ve Disa Aktarma Araci (veritabani_yedekle.py)
---------------------------------------------------------------------
pspp_database.json dosyasinin tarih damgali yedegini alir ve istenirse
duz metin (OEIS / Makale formati) veya CSV olarak disa aktarir.

Kullanim:
1. Komut Satirindan:
   python veritabani_yedekle.py              (Otomatik tarih damgali yedek alir)
   python veritabani_yedekle.py --export-txt (OEIS / Duz metin listesi cikarir)

2. Parametresiz / Cift Tiklama:
   Terminalden secenek secmenizi ister ve pencereyi acik tutar.
"""

import sys
import os
import json
import shutil
from datetime import datetime
import argparse

BASE_DIR = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
DB_PATH = os.path.join(BASE_DIR, "pspp_database.json")
BACKUP_DIR = os.path.join(BASE_DIR, "backups")

def backup_db(export_txt=False):
    if not os.path.exists(DB_PATH):
        print(f"[HATA] Veritabani bulunamadi: {DB_PATH}")
        return
        
    os.makedirs(BACKUP_DIR, exist_ok=True)
    timestamp = datetime.now().strftime("%Y%m%d_%H%M%S")
    backup_file = os.path.join(BACKUP_DIR, f"pspp_database_backup_{timestamp}.json")
    
    shutil.copy2(DB_PATH, backup_file)
    print(f"[BASARILI] Veritabani JSON yedegi alindi: {backup_file}")
    
    if export_txt:
        with open(DB_PATH, "r", encoding="utf-8") as f:
            db = json.load(f)
            
        txt_file = os.path.join(BACKUP_DIR, f"pspp_diziler_oeis_{timestamp}.txt")
        lines = [
            "# PSPP (Postage Stamp with Subtraction) Maksimum Skorlar ve Diziler",
            f"# Olusturulma Tarihi: {datetime.now().strftime('%Y-%m-%d %H:%M:%S')}",
            "# Format: P, M(P), Delta Dizisi, Kumulatif Dizi",
            "=" * 75
        ]
        
        sequence_m = []
        for p_str, data in sorted(db.items(), key=lambda x: int(x[0])):
            p = int(p_str)
            score = data["score"]
            sequence_m.append(str(score))
            sols = data.get("solutions", [])
            for s_idx, sol in enumerate(sols):
                d = sol["delta"]
                p_arr = sol["dizi"]
                label = f"P = {p:2d} | M = {score:3d}" + (f" (Cozum #{s_idx+1})" if len(sols) > 1 else "")
                lines.append(f"{label} | Delta: {d}")
                lines.append(f"          | Dizi : {p_arr}")
                
        lines.append("=" * 75)
        lines.append("# OEIS Dizi Terimleri (M(P)):")
        lines.append(", ".join(sequence_m))
        
        with open(txt_file, "w", encoding="utf-8") as f:
            f.write("\n".join(lines) + "\n")
            
        print(f"[BASARILI] OEIS / Duz metin listesi kaydedildi: {txt_file}")

def interactive_mode():
    print("=" * 70)
    print("   PSPP VERITABANI YEDEKLEME VE DISA AKTARMA ARACI")
    print("=" * 70)
    print(" [1] Yalnizca JSON Yedegi Al")
    print(" [2] Hem JSON Yedegi Al Hem de OEIS / Duz Metin Listesi Cikar")
    secim = input("Seciminiz (1 veya 2, Varsayilan: 2): ").strip()
    
    export_txt = (secim != "1")
    backup_db(export_txt=export_txt)
    input("\nCikis yapmak icin Enter'a basin...")

def main():
    if len(sys.argv) == 1:
        interactive_mode()
        return
        
    parser = argparse.ArgumentParser(description="PSPP Veritabani Yedekleme Araci")
    parser.add_argument("--export-txt", action="store_true", help="OEIS ve makaleler icin duz metin ciktisi da uret")
    args = parser.parse_args()
    
    backup_db(export_txt=args.export_txt)

if __name__ == "__main__":
    main()
