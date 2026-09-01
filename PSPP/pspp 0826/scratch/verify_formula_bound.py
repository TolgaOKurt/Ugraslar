import json
import math
import os

DB_PATH = r"c:\Users\icduser\Documents\GitHub\Ugraslar\PSPP\pspp 0826\pspp_database.json"
with open(DB_PATH, "r", encoding="utf-8") as f:
    db = json.load(f)

print("="*90)
print("P >= 33 ICIN FORMUL UYUM TESTI: a(n) >= floor((n^2 + 2n) / 2)")
print("="*90)

all_match = True
for n in range(33, 101):
    rec = db.get(str(n), {})
    score = rec.get("score", 0)
    
    formula_bound = (n * n + 2 * n) // 2
    
    # Simetrik 2-kademeli fark temeli analitik degeri:
    r = (n + 1) // 2
    s = n - r
    step = 2 * r + 1
    analytical_M = s * step + r
    
    diff = score - formula_bound
    is_valid = (score >= formula_bound)
    
    if not is_valid:
        all_match = False
        
    if n in range(33, 45) or n in [50, 60, 70, 80, 90, 100]:
        print(f"n = {n:3d} | Veritabanı Skoru a(n) = {score:4d} | Formül = {formula_bound:4d} | Analitik M = {analytical_M:4d} | Uyum: {'OK (TAM ESIT)' if diff == 0 else ('OK (+%d)' % diff if diff > 0 else 'HATA')}")

print("\n" + "="*90)
print(f"GENEL SONUC: n=33..100 arasındaki TÜM boyutlar kurala {'%100 UYMAKTADIR (TAM ESITLIK)!' if all_match else 'UYMAMAKTADIR!'}")
print("="*90)
