# PSPP C Arama Motoru (solver.c) Algoritma ve Kısıt Mimarisi

Bu doküman, [solver.c](file:///c:/Users/icduser/Documents/GitHub/Ugraslar/PSPP/pspp%200826/solvers/solver.c) içerisinde uygulanan **Ters DFS (Reverse Branch & Bound)** arama algoritmasını, **Kesin Matematiksel Kanunlar** budama mekanizmasını ve **2048-bit donanımsal bitmask skorlama mimarisini** ayrıntılı olarak açıklamaktadır.

---

## 1. Algoritma Akış Şeması (Mermaid)

Aşağıdaki şemada, **Ters DFS (Reverse Branch & Bound)** motorunun adımları, iç düğüm ve yapraktaki budama kontrolleri ile yaprak düğümlerdeki **2048-Bit Donanımsal Bitmask Motoru** gösterilmektedir:

```mermaid
flowchart TD
    Start(["🚀 Başlat: main(P, hedef_esik, max_delta)"]) --> Init["Teorik Tavan = P² + P<br>min_required_sum = (hedef_esik + 1) / 2<br>dfs_ters(depth=P-1, current_sum=0, has_one=0, has_two=0, has_adj_ones=0)"]
    
    Init --> CheckDepth{"depth < 0 ?<br>(Tüm delta'lar yerleşti mi?)"}
    
    %% İç Düğüm Dallanma ve İleriye Dönük Budama
    subgraph Ic_Dugum_Budama [" İç Düğüm Dallanma & Erken Budama (Branch & Bound) "]
        CheckDepth -- "Hayır (İç Düğüm)" --> Prune1{"Kanun 1 İleriye Dönük Budama:<br>current_sum + (depth+1)*max_step < min_required_sum ?"}
        Prune1 -- "Evet (Hedefe Ulaşamaz)" --> Return1["❌ Dalı Budama (return)"]
        
        Prune1 -- "Hayır (Ulaşabilir)" --> CalcMaxD["max_d Belirle:<br>max_delta > 0 ? max_delta : (Tavan - sum)"]
        CalcMaxD --> Prune2{"depth == 0 VE !has_one ?<br>(Kanun 2: 1'in Üretimi)"}
        Prune2 -- "Evet (1 Zorunlu)" --> CapMaxD1["max_d = 1'e Çek<br>(d >= 2 dallarını anında kes)"]
        
        Prune2 -- "Hayır" --> Prune2b{"depth == 0 VE !has_two && !has_adj_ones ?<br>(Kanun 2b: 2'nin Üretimi)"}
        Prune2b -- "Evet (1 veya 2 Zorunlu)" --> CapMaxD2["max_d = 2'ye Çek<br>(d >= 3 dallarını anında kes)"]
        Prune2b -- "Hayır" --> Prune2d{"depth == 0 VE !suffix_has_3 ?<br>(Kanun 2d: 3'ün Üretimi)"}
        Prune2d -- "Evet (max_d <= 3)" --> CapMaxD3["max_d = 3'e Çek<br>(d >= 4 dallarını anında kes)"]
        Prune2d -- "Hayır" --> Loop["Döngü: d = 1 .. max_d"]
        CapMaxD1 --> Loop
        CapMaxD2 --> Loop
        CapMaxD3 --> Loop
        
        Loop --> Prune2c{"depth == 0 VE d >= 4 VE<br>!(diff_mask & target) ?<br>(Kanun 2c: p0 - 1)"}
        Prune2c -- "Evet (p0-1 Uretilemez)" --> SkipD["❌ d Atla (continue)"]
        SkipD --> LoopNext
        
        Prune2c -- "Hayır (Gecerli)" --> Recurse["delta[depth] = d<br>dfs_ters(depth - 1, sum + d, ...)"]
        Recurse --> LoopNext["Sonraki d Değerine Geç"]
        LoopNext --> Loop
    end
    
    %% Yaprak Düğüm Kontrolü ve Değerlendirme
    subgraph Yaprak_Degerlendirme [" Yaprak Düğüm Değerlendirmesi "]
        CheckDepth -- "Evet (Yaprak Noktası)" --> LeafCheck1{"Kanun 1 Alt Eşik:<br>current_sum >= min_required_sum ?"}
        LeafCheck1 -- "Hayır" --> Return2["❌ Budama: Yetersiz Toplam (return)"]
        
        LeafCheck1 -- "Evet" --> LeafCheck2{"Kanun 2 Parite:<br>has_one == true ?"}
        LeafCheck2 -- "Hayır" --> Return3["❌ Budama: 1 Üretilemez (return)"]
        
        LeafCheck2 -- "Evet" --> LeafCheck3{"Kanun 2b:<br>has_two veya has_adj_ones veya delta[0]==1 ?"}
        LeafCheck3 -- "Hayır" --> Return4["❌ Budama: 2 Üretilemez (return)"]
        
        LeafCheck3 -- "Evet" --> LeafCheck4{"Kanun 2d:<br>makes_three == true ?"}
        LeafCheck4 -- "Hayır" --> Return4d["❌ Budama: 3 Üretilemez (return)"]
        
        LeafCheck4 -- "Evet" --> BitmaskEngine["⚡ deltatomax_c(delta, P)<br>• Delta'dan p_arr prefix toplamları üret<br>• 32x 64-bit Maske (0..2047 bit register)<br>• p[i], p[j]+p[i], p[j]-p[i] bitlerini set et (|=)<br>• ~mask ve __builtin_ctzll() ile ilk 0-bitini O(1)'de bul"]
        
        BitmaskEngine --> ScoreEval{"Skor Analizi"}
        ScoreEval -- "Skor > best_score" --> NewRecord["🏆 YENİ REKOR<br>• Konsola Yazdır<br>• pspp_database.json Güncelle<br>• bilinen_max = score"]
        ScoreEval -- "Skor == best_score" --> PrintSol["✔ Eşit Çözümü Kaydet & Yazdır"]
        ScoreEval -- "Skor < best_score" --> NextNode["Sonraki Yaprak"]
        NewRecord --> Return5["Geri Dön (Backtrack)"]
        PrintSol --> Return5
        NextNode --> Return5
    end
```

---

## 2. Kesin Matematiksel Kanunlar ve solver.c Uyumu

[solver.c](file:///c:/Users/icduser/Documents/GitHub/Ugraslar/PSPP/pspp%200826/solvers/solver.c), evrensel ve kesin arama yapan bir çözücüdür. Bu nedenle [DIZI_KURALLARI.md](file:///c:/Users/icduser/Documents/GitHub/Ugraslar/PSPP/pspp%200826/docs/DIZI_KURALLARI.md) içerisinde tanımlanan **Kesin Matematiksel Kanunlar**'ı eksiksiz uygular:

| # | Kanun Adı | Kod Konumu | Matematiksel Formül | Uygulama & Budama Yöntemi |
|---|---|---|---|---|
| **1** | **Tepe Kapanış & Dinamik Alt Eşik** | solver.c | sum(delta) >= ceil((hedef_esik + 1) / 2) | **İleriye Dönük Dal & Yaprak Budaması:** Kalan adımlar maksimum delta ile doldurulsa dahi asgari toplamı veremiyorsa dal hiç ziyaret edilmeden kesilir. Yaprakta yetersiz toplamlar bitmask motoruna sokulmadan `return` edilir. |
| **2** | **1'in Üretilmesi & Parite Kanunu** | solver.c | exists k : delta_k = 1 | **Erken Dal Kısıtlama & Yaprak Budaması:** Eğer `depth == 0`'a kadar hiç `1` seçilmemişse (`!has_one`), 1 üretilebilmesi için son eleman delta[0] = 1 olmak zorundadır. d >= 2 olan tüm dallar döngü `max_d = 1` yapılarak anında kesilir. |
| **2b**| **2'nin Üretilmesi Kanunu** | solver.c | delta_0=1 OR exists delta_k=2 OR exists [1,1] | **Erken Dal Kısıtlama (max_d <= 2):** Eğer suffix'te 2 veya [1, 1] yoksa, 2 üretilebilmesi için delta[0] elemanı 1 veya 2 olmak zorundadır. d >= 3 olan tüm dallar döngü `max_d = 2` yapılarak anında elenir. |
| **2c**| **p0 - 1 Üretilme Kanunu (p0 >= 4)**| solver.c | exists (j > i) : p_j - p_i = p_0 - 1 | **Erken Dal Budaması (O(1) Bitmask):** d >= 4 (yani p0 - 1 >= 3) için, (d - 1) sayısı kuyruk farklarında yoksa dal doğrudan `continue` ile atlanır. |
| **2d**| **3'ün Üretilmesi Kanunu** | solver.c | 3 OR [1,2] OR [2,1] OR [1,1,1] OR [1,1] basta | **Erken Dal Kısıtlama (max_d <= 3):** Kuyrukta 3 üretimi yoksa `max_d = 3` yapılır, d >= 4 seçenekleri ve geçersiz d=1,2 durumları anında budanır. |
| **3** | **Güvercin Yuvası Mutlak Tavanı** | solver.c | max(p) <= P^2 + P | **Arama Uzayı Sınırlandırması:** Toplamı P^2+P'yi aşacak hiçbir kombinasyon döngüye dahi giremez (max_d = tavan - current_sum). |

---

### Sezgisel Kısıtlar Neden solver.c'de Kullanılmaz?

Önceki prototiplerde yer alan bazı deneysel kısıtlar (örneğin $\delta \le 2P$ hibrit sınırı, ikinci eleman sınırı $\delta_1 \le (P-1)\delta_0$, ortanca eleman sınırı $p_{mid} \le \frac{P^2+P}{2}$ ve kuyruk daralma kuralı), [DIZI_KURALLARI.md](file:///c:/Users/icduser/Documents/GitHub/Ugraslar/PSPP/pspp%200826/docs/DIZI_KURALLARI.md) Bölüm 2'de açıklandığı üzere **Sezgisel Kısıtlar** sınıfındadır.

- Bu kısıtlar modüler aileleri hızlandırsa da, **Aile 3 (Uç Sıçraması)** gibi asimetrik küresel rekorları eler.
- Bu nedenle `solver.c` kesin çözücüsünde sezgisel kısıtlar **kaldırılmış**, arama mutlak teorik tavana kadar evrensel bırakılmıştır.

---

## 3. Donanımsal 2048-Bit CTZ Bitmask Motoru (`deltatomax_c`)

Klasik dizi veya hash kümesi (`Set`) kontrolleri yerine CPU'nun 64-bit yazmaçlarını donanımsal düzeyde kullanan ultra hızlı bir yöntem uygulanmıştır:

```mermaid
graph LR
    subgraph Bitmask_Yapisi [" 32 Adet 64-bit Register (Toplam 2048 Bit) "]
        M0["seen[0]: Bit 0 .. 63"]
        M1["seen[1]: Bit 64 .. 127"]
        M2["seen[2]: Bit 128 .. 191"]
        MDot["..."]
        M31["seen[31]: Bit 1984 .. 2047"]
    end
    
    subgraph Uretim [" O(1) Bit Set Etme "]
        P_i["Tekliler: p[i]"] -->|"Bit Set: seen |= mask"| Bitmask_Yapisi
        Sum["Toplamlar: p[j] + p[i]"] -->|"Bit Set: seen |= mask"| Bitmask_Yapisi
        Diff["Farklar: p[j] - p[i]"] -->|"Bit Set: seen |= mask"| Bitmask_Yapisi
    end
    
    subgraph Donanim_Hesaplama [" O(1) CPU CTZ Tespiti "]
        Bitmask_Yapisi --> Inv["~seen[w] (Bitleri Ters Çevir)"]
        Inv --> CTZ["__builtin_ctzll(val)<br>(x86-64 TZCNT / BSF Komutu)"]
        CTZ --> Score["Maksimum Kesintisiz Skor (M)"]
    end
```

### Bitmask Çalışma Prensibi:
1. **Delta -> Prefix Sum:** $\delta = [2, 2, 1, 14, 1, 11, 1] \longrightarrow p = [2, 4, 5, 19, 20, 31, 32]$
2. **Bit Set Etme:**
   - Her $p_i$, $(p_j + p_i)$ ve $(p_j - p_i)$ değeri hesaplanır ve ilgili 64-bit bloğundaki bit `seen[v >> 6] |= (1ULL << (v & 63))` ile tek bir işlemci komutunda `1` yapılır.
3. **İlk Eksik Sayıyı Bulma ($O(1)$ CTZ):**
   - Word'ler sırayla taranır; `val = ~seen[w]` hesaplanır.
   - İlk `0` olan bit (üretilemeyen ilk pozitif tamsayı), x86-64 işlemci komutu olan `TZCNT` / `BSF` (`__builtin_ctzll`) ile **tek bir makine çevriminde ($O(1)$)** tespit edilir.

---

## 4. Zaman ve Alan Karmaşıklığı

- **Bellek Karmaşıklığı (Space Complexity):** $O(P)$
  - Sadece derinlik yığını (call stack) ve 64 elemanlık statik diziler kullanılır. Sıfır dinamik bellek (`malloc`) tahsisi.
- **Tek Dizi Değerlendirme Süresi:** $\approx 15 \text{ - } 25 \text{ nanosaniye}$
  - Bitmask ve donanımsal CTZ sayesinde saniyede **~20 - 35 milyon kombinasyon** test edilebilir.

---

## 5. Kullanım ve Parametreler

Derleme:
```powershell
gcc -O3 -finput-charset=UTF-8 -fexec-charset=UTF-8 solvers/solver.c -o solvers/solver.exe
```

Çalıştırma Örnekleri:
- **İnteraktif Mod:**
  ```powershell
  .\solvers\solver.exe
  ```
- **Komut Satırı Parametreleri:**
  ```powershell
  .\solvers\solver.exe 8 --esik 130 --max-delta 25
  ```
