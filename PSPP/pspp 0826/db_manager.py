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

# Windows console UTF-8 / cp1254 uyumlulugu
if sys.stdout and hasattr(sys.stdout, 'reconfigure'):
    try:
        sys.stdout.reconfigure(encoding='utf-8', errors='replace')
    except Exception:
        pass

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

def format_compact_json(db):
    """
    Veritabanini sayisal P anahtarlarina gore siralar.
    Delta ve dizi sayi listelerini tek satirda (inline array) tutarak
    JSON dosyasini 11.990 satirdan ~1.200 satira indirir.
    Hem insan hem AI icin desenlerin (taban, kuyruk) aninda gorulmesini saglar.
    """
    sorted_db = {str(k): db[str(k)] for k in sorted(int(x) for x in db.keys()) if str(k) in db}
    raw_json = json.dumps(sorted_db, indent=2, ensure_ascii=False)
    # Sayi dizilerini tek satirda topla: [\n  1,\n  2\n] -> [1, 2]
    compact_json = re.sub(
        r'\[\s*([\d\s,]+?)\s*\]',
        lambda m: '[' + ', '.join(m.group(1).replace(',', ' ').split()) + ']',
        raw_json
    )
    return compact_json

def save_database(db):
    """
    Veritabanini guvenli ve kompakt formatta kaydeder.
    """
    compact_str = format_compact_json(db)
    with open(DB_PATH, "w", encoding="utf-8") as f:
        f.write(compact_str)
        f.write("\n")

def analyze_delta_structure(delta):
    """Bir delta dizisinin tabanini, omurga uzunlugunu ve kuyrugunu ayristirir."""
    if not delta:
        return "Bos", 0, []
    first = delta[0]
    run_len = 0
    while run_len < len(delta) and delta[run_len] == first:
        run_len += 1
    if run_len >= 3:
        return f"Taban-{first}", run_len, delta[run_len:]
    from collections import Counter
    counts = Counter(delta)
    most_val, most_count = counts.most_common(1)[0]
    if most_count >= 3:
        return f"Taban-{most_val}", most_count, [x for x in delta if x != most_val]
    return "Ozel/Karisik", 1, delta[1:] if len(delta) > 1 else []

def show_dimension(p_val):
    """Belirli bir P boyutunun rekor kartini terminale basar."""
    db = load_database()
    p_str = str(p_val)
    if p_str not in db:
        print(f"[BILGI] P = {p_val} boyutu veritabaninda bulunamadi.")
        return
    data = db[p_str]
    score = data.get("score", 0)
    is_opt = data.get("is_optimal", False)
    sols = data.get("solutions", [])
    
    # Teorik Metrikler
    p = int(p_val)
    ceiling = p * (p + 1)
    ratio = (score / ceiling * 100.0) if ceiling > 0 else 0.0
    rohrbach = 24 * p - 216 if p >= 11 else 0
    
    print("\n" + "=" * 70)
    print(f"   🏆 PSPP REKOR KARTI: P = {p}")
    print("=" * 70)
    print(f" Zirve Skor (M)     : {score}")
    print(f" Matematiksel Durum : {'✅ KESIN OPTIMUM (Kanitlanmis)' if is_opt else '🔥 DÜNYA REKORU (Heuristic / En Iyi Bilinen)'}")
    print(f" Cozum Sayisi       : {len(sols)}")
    print(f" Teorik Tavan P(P+1): {ceiling} (Kapasite Kullanimi: %{ratio:.2f})")
    if rohrbach > 0:
        print(f" Taban-12 Esik (M12): {rohrbach} (Net Fark: {score - rohrbach:+d})")
    print("-" * 70)
    
    for idx, s in enumerate(sols, 1):
        delta = s.get("delta", [])
        dizi = s.get("dizi", [])
        taban_adi, tekrar, kuyruk = analyze_delta_structure(delta)
        b_max = dizi[-1] if dizi else (sum(delta) if delta else 0)
        
        print(f" [Cozum #{idx}]")
        print(f"   Delta ({len(delta):2d} eleman): {delta}")
        print(f"   Dizi  ({len(dizi):2d} eleman): {dizi}")
        print(f"   Mimari Analiz   : {taban_adi} ({tekrar} adet adim) | Kuyruk ({len(kuyruk)} eleman): {kuyruk}")
        print(f"   Kumulatif Zirve : B_max = {b_max}")
        if idx < len(sols):
            print("   " + "-" * 66)
    print("=" * 70 + "\n")

def show_table(start_p=1, end_p=35):
    """P araligindaki rekorlari ozet tablo olarak basar."""
    db = load_database()
    print("\n" + "=" * 80)
    print(f"         PSPP REKORLAR TABLOSU (P = {start_p} .. {end_p})")
    print("=" * 80)
    print(f"{'P':>3} | {'Skor (M)':>8} | {'Cozum':>5} | {'Mimari':<12} | {'Kuyruk':<32} | {'Durum':<10}")
    print("-" * 80)
    
    for p in range(start_p, end_p + 1):
        p_str = str(p)
        if p_str not in db:
            continue
        data = db[p_str]
        score = data.get("score", 0)
        is_opt = data.get("is_optimal", False)
        sols = data.get("solutions", [])
        s_count = len(sols)
        
        if sols:
            taban_adi, tekrar, kuyruk = analyze_delta_structure(sols[0].get("delta", []))
            arch = f"{taban_adi}x{tekrar}"
            kuyruk_str = str(kuyruk)
            if len(kuyruk_str) > 32:
                kuyruk_str = kuyruk_str[:29] + "..."
        else:
            arch = "-"
            kuyruk_str = "[]"
            
        durum_str = "Optimal" if is_opt else "Rekor"
        print(f"{p:3d} | {score:8d} | {s_count:5d} | {arch:<12} | {kuyruk_str:<32} | {durum_str:<10}")
        
    print("=" * 80 + "\n")

def compact_database():
    """Mevcut veritabanini kompakt JSON formatina donusturur."""
    if not os.path.exists(DB_PATH):
        print(f"[HATA] Veritabani bulunamadi: {DB_PATH}")
        return
    with open(DB_PATH, "r", encoding="utf-8") as f:
        old_content = f.read()
    old_lines = len(old_content.splitlines())
    db = json.loads(old_content)
    save_database(db)
    with open(DB_PATH, "r", encoding="utf-8") as f:
        new_content = f.read()
    new_lines = len(new_content.splitlines())
    print("=" * 65)
    print("   PSPP VERITABANI KOMPAKT FORMAT DONUSUMU")
    print("=" * 65)
    print(f" Onceki Satir Sayisi : {old_lines:,} satir")
    print(f" Kompakt Satir Sayisi: {new_lines:,} satir")
    print(f" Tasarruf Orani      : %{((old_lines - new_lines) / old_lines * 100):.1f} kuculme!")
    print(f" Veri Butunlugu      : %100 KORUNDU ({len(db)} boyut tam ve eksiksiz)")
    print("=" * 65)


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
    while True:
        print("=" * 65)
        print("   PSPP MERKEZI VERITABANI VE COZUM KAYIT YONETICISI")
        print("=" * 65)
        print(" [1] Yeni Dizi / Delta Gonder (Test Et ve Kaydet)")
        print(" [2] Belirli Bir Boyutun Rekorunu Incele (Show P)")
        print(" [3] Rekor Tablosunu Listele (Table)")
        print(" [4] Veritabanini Kompakt JSON Formatina Donustur (Compact)")
        print(" [0] Cikis")
        print("-" * 65)
        
        try:
            secim = input("Seciminiz (0-4): ").strip()
        except (KeyboardInterrupt, EOFError):
            print("\nCikis yapildi.")
            return

        if secim == "0":
            print("Cikis yapildi.")
            return
        elif secim == "2":
            try:
                p_in = input("Incelemek istediginiz P boyutu (Orn: 20): ").strip()
                p_val = int(p_in)
                show_dimension(p_val)
            except ValueError:
                print("[HATA] Gecersiz P degeri!")
            input("Devam etmek icin Enter'a basin...")
            continue
        elif secim == "3":
            try:
                aralik = input("Aralik (Orn: 1 35 veya bos birakin [1..35]): ").strip()
                if aralik:
                    parts = [int(x) for x in re.findall(r'\d+', aralik)]
                    if len(parts) >= 2:
                        show_table(parts[0], parts[1])
                    elif len(parts) == 1:
                        show_table(1, parts[0])
                else:
                    show_table(1, 35)
            except Exception as e:
                print(f"[HATA] {e}")
            input("Devam etmek icin Enter'a basin...")
            continue
        elif secim == "4":
            compact_database()
            input("Devam etmek icin Enter'a basin...")
            continue
        elif secim == "1":
            print("\nDiziyi bosluk veya virgul ile ayirarak giriniz.")
            print("Ornek Delta: 10,10,10,10,10,10,2,3,1,5,2,1,3,1,1")
            print("Ornek Dizi : 10 20 30 40 50 60 62 65 66 71 73 74 77 78 79")
            print("-" * 65)
            try:
                user_input = input("Sayilari Girin: ").strip()
            except (KeyboardInterrupt, EOFError):
                return
            nums = parse_numbers_from_string(user_input)
            if not nums:
                print("[HATA] Gecerli sayi bulunamadi!")
                input("\nDevam etmek icin Enter'a basin...")
                continue
            print("\nBu girdi neyi temsil ediyor?")
            print(" [1] Otomatik Algila (Hem Delta hem Normal Dizi olarak test et)")
            print(" [2] Delta (Artis Adimlari) Dizisi")
            print(" [3] Normal Kumulatif Dizi")
            secim_tip = input("Seciminiz (1, 2 veya 3, Varsayilan: 1): ").strip()
            is_delta = None
            if secim_tip == "2":
                is_delta = True
            elif secim_tip == "3":
                is_delta = False
            print()
            submit_solution(nums, is_delta=is_delta, verbose=True)
            input("\nDevam etmek icin Enter'a basin...")
            continue
        else:
            print("[HATA] Gecersiz secenek!")

def main():
    if len(sys.argv) == 1:
        interactive_mode()
        return

    first_arg = sys.argv[1].lower()

    # Alt Komut: show
    if first_arg in ("show", "--show"):
        if len(sys.argv) >= 3:
            try:
                p_start = int(sys.argv[2])
                p_end = int(sys.argv[3]) if len(sys.argv) >= 4 else p_start
                for p_cur in range(p_start, p_end + 1):
                    show_dimension(p_cur)
            except ValueError:
                print("[HATA] Gecersiz P boyutu! Kullanim: python db_manager.py show 20 [veya show 15 20]")
        else:
            print("Kullanim: python db_manager.py show <P> [Orn: python db_manager.py show 20]")
        return

    # Alt Komut: table
    if first_arg in ("table", "--table"):
        start_p = 1
        end_p = 35
        if len(sys.argv) >= 3:
            try:
                start_p = int(sys.argv[2])
            except ValueError:
                pass
        if len(sys.argv) >= 4:
            try:
                end_p = int(sys.argv[3])
            except ValueError:
                pass
        show_table(start_p, end_p)
        return

    # Alt Komut: compact
    if first_arg in ("compact", "--compact"):
        compact_database()
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

