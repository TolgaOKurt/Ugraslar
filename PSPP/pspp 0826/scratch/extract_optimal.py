import json
import os

with open('pspp_database.json', 'r', encoding='utf-8') as f:
    db = json.load(f)

for p in range(1, 9):
    item = db[str(p)]
    score = item['score']
    sols = item['solutions']
    print(f"P={p} | M={score} | Total={len(sols)}")
    for i, s in enumerate(sols):
        print(f"  [{i+1}] delta={s['delta']} | dizi={s['dizi']}")
