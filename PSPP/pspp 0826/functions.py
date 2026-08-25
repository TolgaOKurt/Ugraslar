import time
import itertools

def dizitomax(dizi: list[int]) -> int:
    """
    Python'a ozgu, Set (kume) veri yapisi kullanan en hizli ve temiz alternatif.
    O(1) kume sorgulari ile hedef ardisik sayi dizisini kontrol eder.
    """
    n = len(dizi)
    olusan_sayilar = set(dizi)

    # Toplamlar ve farklar
    for i in range(n):
        for j in range(i, n):
            olusan_sayilar.add(dizi[i] + dizi[j])
            olusan_sayilar.add(abs(dizi[i] - dizi[j]))

    max_val = 0
    while (max_val + 1) in olusan_sayilar:
        max_val += 1

    return max_val


def dizitomaxv2(dizi: list[int]) -> int:
    """
    dizitomax fonksiyonunun bytearray ile optimize edilmiş versiyonu.
    Girdi: Küçükten büyüğe sıralı normal dizi.
    Çıktı: 1'den başlayan ardışık maksimum oluşturulabilir değer.
    
    Optimizasyonlar:
    1. Set veri yapısı ve hashing maliyeti yerine bytearray (doğrudan bellek adresleme) kullanılır.
    2. Python while döngüsü yerine C seviyesinde çalışan bytearray.find(0, 1) (SIMD/memchr) kullanılır.
    """
    if not dizi:
        return 0
    
    n = len(dizi)
    # Maksimum ulaşılabilecek değer: en büyük iki elemanın toplamı
    max_possible = (dizi[-1] << 1) + 2
    seen = bytearray(max_possible)

    for i in range(n):
        pi = dizi[i]
        seen[pi] = 1
        for j in range(i, n):
            pj = dizi[j]
            seen[pj + pi] = 1
            seen[pj - pi] = 1

    # 1 indeksinden itibaren değeri 0 olan ilk baytın indeksini bul
    res = seen.find(0, 1)
    return res - 1 if res != -1 else max_possible - 1

    
def dizitodelta(dizi: list[int]) -> list[int]:
    """
    Sıralı bir diziyi delta (fark) dizisine dönüştürür.
    İlk eleman normal dizinin ilk elemanıdır, sonraki elemanlar ise ardışık farklardır (artış miktarları).
    """
    if not dizi:
        return []
    
    delta = [dizi[0]]
    for i in range(1, len(dizi)):
        delta.append(dizi[i] - dizi[i - 1])
    return delta


def deltatodizi(delta: list[int]) -> list[int]:
    """
    Delta dizisini tekrar kümülatif normal diziye dönüştürür.
    """
    if not delta:
        return []
    
    dizi = [delta[0]]
    for i in range(1, len(delta)):
        dizi.append(dizi[-1] + delta[i])
    return dizi


def deltatomax(delta: list[int]) -> int:
    """
    Delta dizisinden doğrudan ardışık maksimum oluşturulabilir değeri (max_val) hesaplar.
    
    Matematiksel & Yazılımsal Optimizasyonlar:
    1. Kümülatif toplamlar (prefix sums) itertools.accumulate ile C hızında hesaplanır.
    2. Her eleman (P[i]), iki eleman farkı (P[j] - P[i]) ve iki eleman toplamı (P[j] + P[i]) 
       doğrudan bytearray (bit dizisi benzeri hızlı bellek) üzerinde işaretlenir (Set hashing maliyeti yoktur).
    3. 1'den itibaren ilk eksik sayı bytearray.find(0, 1) ile donanımsal C (memchr) hızında taranır.
    """
    if not delta:
        return 0

    # Prefix sums (Kümülatif toplamlar -> Orijinal dizinin elemanları)
    P = list(itertools.accumulate(delta))
    n = len(P)

    # Maksimum ulaşılabilecek değer: en büyük iki elemanın toplamı (2 * P[-1])
    max_possible = (P[-1] << 1) + 2
    seen = bytearray(max_possible)

    for i in range(n):
        pi = P[i]
        seen[pi] = 1
        for j in range(i, n):
            pj = P[j]
            seen[pj + pi] = 1
            seen[pj - pi] = 1

    # 1 indeksinden itibaren değeri 0 olan ilk baytın indeksini bul
    res = seen.find(0, 1)
    return res - 1 if res != -1 else max_possible - 1


def get_bilinen_max(p: int) -> int:
    """
    Verilen p boyutu için bilinen alt sınır / optimum hedef max değerini döndürür.
    """
    bilinen_tablo = {
        1: 2,
        2: 6,
        3: 10,
        4: 16,
        5: 24,
        6: 32,
        7: 40,
        8: 52,
        9: 61,   # Benzetimli tavlama ile 64 bulundu
        10: 73,  # Benzetimli tavlama ile 76 bulundu
        11: 85,  # Benzetimli tavlama ile 90 bulundu
        12: 97,  # Benzetimli tavlama ile 106 bulundu
        13: 109, # Benzetimli tavlama ile 122 bulundu
        14: 121,
    }
    if p in bilinen_tablo:
        return bilinen_tablo[p]
    elif p >= 15:
        # Genel formül: (p*p + p) * 2 // 3
        return (p * p + p) * 2 // 3
    return 0


def delta_arama(p: int = 5, delta_limit: int = None, bilinen_max: int = None) -> dict:
    """
    Delta dizileri üzerinde Branch & Bound (Budamalı DFS) arama yaparak
    en yüksek deltatomax sonucunu veren optimum delta dizilerini bulur.
    
    Arama Stratejisi & Limitler:
    - İlk p - 1 eleman için: delta_limit = 2 * p (Kompakt ve aritmetik adımlar)
    - Son eleman için: Tavan sıçramalarını (Leapfrog) kaçırmamak adına (p*p + p)//2 - current_sum
    - Dinamik Alt Sınır: Yeni ve daha yüksek bir skor bulunduğunda bilinen_max otomatik yükseltilir (Asla == kullanılmaz).
    
    Uygulanan Budama Kuralları:
    - Kural 1: max(dizi) >= ceil(bilinen_max / 2)
    - Kural 2: ortanca eleman <= (p*p + p) // 2
    - Kural 3: delta[1] <= (p - 1) * delta[0]
    """
    if bilinen_max is None:
        bilinen_max = get_bilinen_max(p)

    mid_idx = (p - 1) // 2
    best_score = 0
    best_solutions = []
    stats = {"tested": 0, "pruned": 0}
    current_delta = []

    # p eleman ile oluşabilecek dizinin en büyük elemanının mutlak teorik tavanı: (p*p + p) // 2
    dizi_tavan = (p * p + p) // 2
    normal_delta_limit = 2 * p if delta_limit is None else delta_limit

    def dfs(depth: int, current_sum: int):
        nonlocal best_score, best_solutions, bilinen_max

        # p elemanlı delta tamamlandığında
        if depth == p:
            # Kural 1: Toplam en az bilinen_max / 2 olmalıdır (Dinamik alt sınır)
            if current_sum < (bilinen_max + 1) // 2:
                stats["pruned"] += 1
                return

            stats["tested"] += 1
            if stats["tested"] % 1_000_000 == 0:
                cur_t = time.perf_counter() - start_time
                print(f"[İterasyon {stats['tested']:,}] Süre: {cur_t:.2f} sn | "
                      f"En İyi Skor: {best_score} | Bulunan Çözüm: {len(best_solutions)} | "
                      f"İncelenen Delta: {current_delta}", flush=True)

            score = deltatomax(current_delta)

            # Dinamik en iyi skor ve bilinen_max güncelleme (Yeni rekor bulundukça eşik yükselir)
            if score > best_score:
                best_score = score
                sol_dizi = deltatodizi(current_delta)
                best_solutions = [(list(current_delta), sol_dizi)]
                cur_t = time.perf_counter() - start_time
                print(f"  >>> [YENİ EN İYİ SKOR: {best_score}] Süre: {cur_t:.2f} sn | "
                      f"Delta: {current_delta} -> Dizi: {sol_dizi}", flush=True)
                if score > bilinen_max:
                    bilinen_max = score
            elif score == best_score and best_score > 0:
                best_solutions.append((list(current_delta), deltatodizi(current_delta)))
            return

        # Dallanma: İlk p-1 eleman için normal_delta_limit (2*p),
        # son eleman için evrendeki hiçbir yeni rekoru kaçırmamak adına mutlak teorik tavan (p*p + p):
        teorik_mutlak_tavan = p * p + p
        if depth == p - 1:
            max_d = max(normal_delta_limit, teorik_mutlak_tavan - current_sum)
        else:
            max_d = normal_delta_limit

        for d in range(1, max_d + 1):
            # Kural 3: 2. eleman sınırı (delta[1] <= (p - 1) * delta[0])
            if depth == 1 and d > (p - 1) * current_delta[0]:
                break

            # Kural 2: Ortanca eleman sınırı (Dizinin ortancası mutlak tavanın yarısını aşamaz)
            if depth == mid_idx and (current_sum + d) > teorik_mutlak_tavan // 2:
                break

            current_delta.append(d)
            dfs(depth + 1, current_sum + d)
            current_delta.pop()

    start_time = time.perf_counter()
    dfs(0, 0)
    elapsed_ms = (time.perf_counter() - start_time) * 1000

    return {
        "p": p,
        "delta_limit": normal_delta_limit,
        "bilinen_max": bilinen_max,
        "best_score": best_score,
        "best_solutions": best_solutions,
        "tested_count": stats["tested"],
        "elapsed_ms": elapsed_ms,
    }


def main():
    
    print("\n" + "=" * 50)
    print("--- DELTA İLE OPTİMUM ARAMA ---")
    print("=" * 50)
    arama_sonucu = delta_arama(p=5)
    print(f"p = {arama_sonucu['p']}, Delta Limit = {arama_sonucu['delta_limit']}, Nihai En Yüksek Max = {arama_sonucu['best_score']}")
    print(f"Test Edilen Kombinasyon: {arama_sonucu['tested_count']}")
    print(f"Arama Süresi           : {arama_sonucu['elapsed_ms']:.4f} ms")
    print("\nOptimal Çözümler:")
    for sol_delta, sol_dizi in arama_sonucu['best_solutions']:
        print(f"  -> Delta: {sol_delta}  |  Orijinal Dizi: {sol_dizi}")


if __name__ == "__main__":
    main()
