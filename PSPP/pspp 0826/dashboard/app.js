// PSPP Knowledge Base Dashboard Core Logic

const KNOWLEDGE_BASE = {
    1: {
        p: 1, score: 2, is_optimal: true,
        solutions: [
            { delta: [1], dizi: [1] }
        ]
    },
    2: {
        p: 2, score: 6, is_optimal: true,
        solutions: [
            { delta: [2, 1], dizi: [2, 3] }
        ]
    },
    3: {
        p: 3, score: 10, is_optimal: true,
        solutions: [
            { delta: [2, 2, 1], dizi: [2, 4, 5] },
            { delta: [3, 1, 1], dizi: [3, 4, 5] }
        ]
    },
    4: {
        p: 4, score: 16, is_optimal: true,
        solutions: [
            { delta: [1, 1, 6, 5], dizi: [1, 2, 8, 13] },
            { delta: [3, 3, 1, 1], dizi: [3, 6, 7, 8] },
            { delta: [4, 2, 1, 2], dizi: [4, 6, 7, 9] }
        ]
    },
    5: {
        p: 5, score: 24, is_optimal: true,
        solutions: [
            { delta: [4, 4, 2, 1, 2], dizi: [4, 8, 10, 11, 13] }
        ]
    },
    6: {
        p: 6, score: 32, is_optimal: true,
        solutions: [
            { delta: [4, 4, 4, 2, 1, 2], dizi: [4, 8, 12, 14, 15, 17] },
            { delta: [5, 2, 5, 1, 2, 1], dizi: [5, 7, 12, 13, 15, 16] }
        ]
    },
    7: {
        p: 7, score: 40, is_optimal: true,
        solutions: [
            { delta: [2, 2, 1, 14, 1, 11, 1], dizi: [2, 4, 5, 19, 20, 31, 32] },
            { delta: [2, 9, 2, 1, 4, 1, 2],   dizi: [2, 11, 13, 14, 18, 19, 21] },
            { delta: [2, 11, 1, 2, 1, 5, 1],  dizi: [2, 13, 14, 16, 17, 22, 23] },
            { delta: [4, 4, 4, 4, 2, 1, 2],   dizi: [4, 8, 12, 16, 18, 19, 21] },
            { delta: [5, 5, 5, 2, 1, 1, 2],   dizi: [5, 10, 15, 17, 18, 19, 21] },
            { delta: [5, 6, 2, 4, 2, 1, 20],  dizi: [5, 11, 13, 17, 19, 20, 40] },
            { delta: [6, 6, 3, 1, 1, 2, 1],   dizi: [6, 12, 15, 16, 17, 19, 20] },
            { delta: [7, 2, 6, 2, 2, 1, 20],  dizi: [7, 9, 15, 17, 19, 20, 40] }
        ]
    },
    8: {
        p: 8, score: 52, is_optimal: true,
        solutions: [
            { delta: [2, 2, 2, 1, 18, 1, 15, 1], dizi: [2, 4, 6, 7, 25, 26, 41, 42] },
            { delta: [5, 3, 11, 1, 2, 1, 6, 26], dizi: [5, 8, 19, 20, 22, 23, 29, 55] },
            { delta: [6, 6, 6, 3, 1, 1, 2, 1],   dizi: [6, 12, 18, 21, 22, 23, 25, 26] }
        ]
    },
    9: {
        p: 9, score: 64, is_optimal: false,
        solutions: [
            { delta: [6, 6, 6, 6, 3, 1, 1, 2, 1], dizi: [6, 12, 18, 24, 27, 28, 29, 31, 32] }
        ]
    },
    10: {
        p: 10, score: 76, is_optimal: false,
        solutions: [
            { delta: [6, 6, 6, 6, 6, 3, 1, 1, 2, 1], dizi: [6, 12, 18, 24, 30, 33, 34, 35, 37, 38] }
        ]
    },
    11: {
        p: 11, score: 90, is_optimal: false,
        solutions: [
            { delta: [8, 8, 8, 8, 4, 2, 3, 1, 1, 2, 2], dizi: [8, 16, 24, 32, 36, 38, 41, 42, 43, 45, 47] }
        ]
    },
    12: {
        p: 12, score: 106, is_optimal: false,
        solutions: [
            { delta: [8, 8, 8, 8, 8, 4, 2, 3, 1, 1, 2, 2], dizi: [8, 16, 24, 32, 40, 44, 46, 49, 50, 51, 53, 55] }
        ]
    },
    13: {
        p: 13, score: 122, is_optimal: false,
        solutions: [
            { delta: [8, 8, 8, 8, 8, 8, 4, 2, 3, 1, 1, 2, 2], dizi: [8, 16, 24, 32, 40, 48, 52, 54, 57, 58, 59, 61, 63] }
        ]
    }
};

let currentP = 7;
let currentSolIdx = 0;

// Sayfa Yüklendiğinde pspp_database.json'dan yükle
document.addEventListener('DOMContentLoaded', async () => {
    try {
        const resp = await fetch('../pspp_database.json');
        if (resp.ok) {
            const data = await resp.json();
            Object.assign(KNOWLEDGE_BASE, data);
        }
    } catch (e) {
        console.log('Statik veritabanı kullanılıyor.');
    }
    initNavigation();
    renderView(currentP, currentSolIdx);
    setupEventListeners();
});

// Sol Sidebar Navigasyonunu Doldur
function initNavigation() {
    const navList = document.getElementById('pNavList');
    navList.innerHTML = '';

    Object.keys(KNOWLEDGE_BASE).forEach(pKey => {
        const item = KNOWLEDGE_BASE[pKey];
        const el = document.createElement('div');
        el.className = `p-nav-item ${item.p === currentP ? 'active' : ''}`;
        el.id = `pNav_${item.p}`;
        el.innerHTML = `
            <div class="p-nav-left">
                <div class="p-nav-badge">${item.p}</div>
                <div class="p-nav-info">
                    <span class="p-nav-title">Boyut P = ${item.p}</span>
                    <span class="p-nav-sub">${item.solutions.length} Alternatif</span>
                </div>
            </div>
            <div class="p-nav-score">${item.score}</div>
        `;
        el.addEventListener('click', () => {
            currentP = item.p;
            currentSolIdx = 0;
            updateActiveNav();
            renderView(currentP, currentSolIdx);
        });
        navList.appendChild(el);
    });
}

function updateActiveNav() {
    document.querySelectorAll('.p-nav-item').forEach(el => el.classList.remove('active'));
    const activeEl = document.getElementById(`pNav_${currentP}`);
    if (activeEl) activeEl.classList.add('active');
}

// Ana Görünümü Güncelle
function renderView(p, solIdx) {
    const record = KNOWLEDGE_BASE[p];
    if (!record) return;

    const sol = record.solutions[solIdx] || record.solutions[0];
    const ceil = p * p + p;
    const ratio = (record.score / p).toFixed(2);
    const efficiency = ((record.score / ceil) * 100).toFixed(1);

    // Hero Bilgileri
    document.getElementById('heroPBadge').textContent = `p = ${p}`;
    const statusBadge = document.getElementById('heroStatusBadge');
    statusBadge.textContent = record.is_optimal ? 'Optimum (Kanıtlı)' : 'En İyi Bilinen (Heuristic)';
    statusBadge.className = `status-badge ${record.is_optimal ? '' : 'heuristic'}`;
    document.getElementById('heroScore').textContent = record.score;

    document.getElementById('metricRatio').textContent = ratio;
    document.getElementById('metricCeil').textContent = ceil;
    document.getElementById('metricSolCount').textContent = `${record.solutions.length} Adet`;
    document.getElementById('metricEfficiency').textContent = `%${efficiency}`;

    // Alternatif Çözüm Sekmeleri
    const solTabs = document.getElementById('solutionTabs');
    solTabs.innerHTML = '';
    record.solutions.forEach((s, idx) => {
        const btn = document.createElement('button');
        btn.className = `sol-tab-btn ${idx === solIdx ? 'active' : ''}`;
        btn.textContent = `Çözüm #${idx + 1}`;
        btn.addEventListener('click', () => {
            currentSolIdx = idx;
            renderView(p, idx);
        });
        solTabs.appendChild(btn);
    });

    // Delta Chipleri
    const deltaChips = document.getElementById('deltaChips');
    deltaChips.innerHTML = '';
    sol.delta.forEach((d, i) => {
        const chip = document.createElement('span');
        chip.className = 'chip chip-delta';
        chip.textContent = `Δ${i}=${d}`;
        deltaChips.appendChild(chip);
    });

    // Dizi Chipleri
    const diziChips = document.getElementById('diziChips');
    diziChips.innerHTML = '';
    sol.dizi.forEach((v, i) => {
        const chip = document.createElement('span');
        chip.className = 'chip chip-dizi';
        chip.textContent = `P[${i}]=${v}`;
        diziChips.appendChild(chip);
    });

    // Kapsama Haritası ve Sayı Doğrusu
    renderCoverageGrid(sol.dizi, record.score);

    // 64-Bit Register Simülasyonu
    renderBitmaskRegisters(sol.dizi);
}

// Sayıların nasıl üretildiğini hesaplar ve tipini belirler (Ayrıştırma)
function analyzeDecomposition(dizi, target, maxScore) {
    if (target > maxScore) {
        return {
            typeClass: 'cell-missing',
            badge: '<span class="badge" style="background:#EF4444; color:#fff;">Eksik Sayı</span>',
            html: `<strong>Üretilemez (Eksik Değer):</strong> Bu dizi ile ${target} sayısı oluşturulamaz! (Zincir kopuş noktası)`
        };
    }

    const n = dizi.length;
    let isNatural = false;
    let naturalIdx = -1;
    let sumPairs = [];
    let diffPairs = [];

    // 1. Doğal / Tekli kontrol
    for (let i = 0; i < n; i++) {
        if (dizi[i] === target) {
            isNatural = true;
            naturalIdx = i;
            break;
        }
    }

    // 2. Toplam kontrol
    for (let i = 0; i < n; i++) {
        for (let j = i; j < n; j++) {
            if (dizi[i] + dizi[j] === target) {
                sumPairs.push(`P[${i}]+P[${j}] (${dizi[i]}+${dizi[j]})`);
            }
        }
    }

    // 3. Fark kontrol
    for (let i = 0; i < n; i++) {
        for (let j = i + 1; j < n; j++) {
            if (dizi[j] - dizi[i] === target) {
                diffPairs.push(`P[${j}]-P[${i}] (${dizi[j]}-${dizi[i]})`);
            }
        }
    }

    if (isNatural) {
        let extra = '';
        if (sumPairs.length > 0) extra += `<br><span style="color:#38BDF8; font-weight:600;">Alternatif Toplam:</span> ${sumPairs.join(', ')}`;
        if (diffPairs.length > 0) extra += `<br><span style="color:#C084FC; font-weight:600;">Alternatif Fark:</span> ${diffPairs.join(', ')}`;
        return {
            typeClass: 'cell-natural',
            badge: '<span class="badge" style="background:#10B981; color:#0B0F19; font-weight:700;">Doğal Eleman</span>',
            html: `Doğal Eleman: <strong>P[${naturalIdx}] = ${target}</strong>${extra}`
        };
    }

    if (sumPairs.length > 0 && diffPairs.length > 0) {
        return {
            typeClass: 'cell-both',
            badge: '<span class="badge" style="background:#F59E0B; color:#0B0F19; font-weight:700;">Toplam & Fark</span>',
            html: `<span style="color:#38BDF8; font-weight:600;">Toplam:</span> ${sumPairs.join(', ')} <br> <span style="color:#C084FC; font-weight:600;">Fark:</span> ${diffPairs.join(', ')}`
        };
    }

    if (sumPairs.length > 0) {
        return {
            typeClass: 'cell-sum',
            badge: '<span class="badge" style="background:#06B6D4; color:#0B0F19; font-weight:700;">Toplam (P+P)</span>',
            html: `İki Eleman Toplamı: <strong>${sumPairs.join(', ')} = ${target}</strong>`
        };
    }

    if (diffPairs.length > 0) {
        return {
            typeClass: 'cell-diff',
            badge: '<span class="badge" style="background:#A855F7; color:#fff; font-weight:700;">Fark (P-P)</span>',
            html: `İki Eleman Farkı: <strong>${diffPairs.join(', ')} = ${target}</strong>`
        };
    }

    return {
        typeClass: 'cell-missing',
        badge: '<span class="badge" style="background:#EF4444; color:#fff;">Eksik Sayı</span>',
        html: `<strong>Üretilemez:</strong> Bu dizi ile ${target} sayısı oluşturulamaz!`
    };
}

// Kapsama Gridini Çiz
function renderCoverageGrid(dizi, maxScore) {
    const grid = document.getElementById('coverageGrid');
    grid.innerHTML = '';

    const decompBox = document.getElementById('decompText');
    decompBox.innerHTML = `Ayrışım detayını incelemek için aşağıdaki renkli karelerden birine tıklayın.`;

    const totalToShow = maxScore + 1; // 1..M kapsanan, M+1 eksik olan ilk sayı

    for (let num = 1; num <= totalToShow; num++) {
        const info = analyzeDecomposition(dizi, num, maxScore);
        const cell = document.createElement('div');
        cell.className = `num-cell ${info.typeClass}`;
        cell.textContent = num;
        cell.title = `Sayı ${num} | ${info.typeClass.replace('cell-', '').toUpperCase()}`;

        cell.addEventListener('click', () => {
            document.querySelectorAll('.num-cell').forEach(c => c.classList.remove('selected'));
            cell.classList.add('selected');
            decompBox.innerHTML = `<div style="display:flex; align-items:center; gap:8px; margin-bottom:4px;"><strong>Sayı ${num}:</strong> ${info.badge}</div><div>${info.html}</div>`;
        });

        grid.appendChild(cell);
    }
}

// 64-Bit Register Bitmask Gösterimi (Dinamik Genişleyen Register Dizisi)
function renderBitmaskRegisters(dizi) {
    const container = document.getElementById('bitmaskRegisters');
    container.innerHTML = '';

    const n = dizi.length;
    const seen = new Set();
    let maxSeen = 0;

    dizi.forEach(pi => { seen.add(pi); if (pi > maxSeen) maxSeen = pi; });
    for (let i = 0; i < n; i++) {
        for (let j = i; j < n; j++) {
            const s = dizi[i] + dizi[j];
            const d = Math.abs(dizi[j] - dizi[i]);
            seen.add(s);
            seen.add(d);
            if (s > maxSeen) maxSeen = s;
        }
    }

    // Gerekli toplam 64-bit kelime sayısını dinamik hesapla (en az 4 kelime = 256 bit, gerekiyorsa 512/1024 bit)
    const numWords = Math.max(4, Math.ceil((maxSeen + 1) / 64));

    for (let w = 0; w < numWords; w++) {
        const start = w * 64;
        const end = start + 63;
        const regName = `mask${w} (Bit ${start} .. ${end})`;

        let activeBits = 0;
        let regBigInt = 0n;

        for (let bit = start; bit <= end; bit++) {
            if (seen.has(bit)) {
                activeBits++;
                regBigInt |= (1n << BigInt(bit - start));
            }
        }

        const percentage = ((activeBits / 64) * 100).toFixed(1);
        const hex = '0x' + regBigInt.toString(16).toUpperCase().padStart(16, '0');

        const box = document.createElement('div');
        box.className = 'reg-box';
        box.innerHTML = `
            <div class="reg-header">
                <span>${regName}</span>
                <span>${activeBits}/64 Bit (%${percentage})</span>
            </div>
            <div class="reg-hex">${hex}</div>
            <div class="reg-bar">
                <div class="reg-fill" style="width: ${percentage}%"></div>
            </div>
        `;
        container.appendChild(box);
    }
}

// İnteraktif Test Laboratuvarı Hesaplayıcısı
function calculateCustomScore(arr, isDelta) {
    let dizi = [];
    if (isDelta) {
        let sum = 0;
        dizi = arr.map(d => { sum += d; return sum; });
    } else {
        dizi = [...arr].sort((a, b) => a - b);
    }

    const n = dizi.length;
    const seen = new Set();

    dizi.forEach(pi => seen.add(pi));
    for (let i = 0; i < n; i++) {
        for (let j = i; j < n; j++) {
            seen.add(dizi[i] + dizi[j]);
            seen.add(Math.abs(dizi[j] - dizi[i]));
        }
    }

    let score = 0;
    while (seen.has(score + 1)) {
        score++;
    }

    return { score, dizi };
}

function setupEventListeners() {
    // Kopyalama Butonları
    document.getElementById('btnCopyDelta').addEventListener('click', () => {
        const sol = KNOWLEDGE_BASE[currentP].solutions[currentSolIdx];
        navigator.clipboard.writeText(JSON.stringify(sol.delta));
        alert(`Delta dizisi panoya kopyalandı: [${sol.delta.join(', ')}]`);
    });

    document.getElementById('btnCopyDizi').addEventListener('click', () => {
        const sol = KNOWLEDGE_BASE[currentP].solutions[currentSolIdx];
        navigator.clipboard.writeText(JSON.stringify(sol.dizi));
        alert(`Kümülatif dizi panoya kopyalandı: [${sol.dizi.join(', ')}]`);
    });

    // JSON İndir
    document.getElementById('btnExportJson').addEventListener('click', () => {
        const dataStr = "data:text/json;charset=utf-8," + encodeURIComponent(JSON.stringify(KNOWLEDGE_BASE, null, 2));
        const dlAnchor = document.createElement('a');
        dlAnchor.setAttribute("href", dataStr);
        dlAnchor.setAttribute("download", "pspp_knowledge_base.json");
        dlAnchor.click();
    });

    // Modal Kontrolleri
    const modal = document.getElementById('playgroundModal');
    let inputIsDelta = true;

    document.getElementById('btnOpenPlayground').addEventListener('click', () => {
        modal.classList.add('open');
    });

    document.getElementById('btnClosePlayground').addEventListener('click', () => {
        modal.classList.remove('open');
    });

    document.getElementById('tabInputDelta').addEventListener('click', (e) => {
        inputIsDelta = true;
        e.target.classList.add('active');
        document.getElementById('tabInputDizi').classList.remove('active');
        document.getElementById('customArrayInput').placeholder = "Örn: 2, 2, 1, 14, 1, 11, 1";
    });

    document.getElementById('tabInputDizi').addEventListener('click', (e) => {
        inputIsDelta = false;
        e.target.classList.add('active');
        document.getElementById('tabInputDelta').classList.remove('active');
        document.getElementById('customArrayInput').placeholder = "Örn: 2, 4, 5, 19, 20, 31, 32";
    });

    document.getElementById('btnCalculateCustom').addEventListener('click', () => {
        const inputStr = document.getElementById('customArrayInput').value;
        const numbers = inputStr.split(',').map(x => parseInt(x.trim(), 10)).filter(x => !isNaN(x) && x > 0);

        if (numbers.length === 0) {
            alert('Lütfen geçerli pozitif sayılar giriniz.');
            return;
        }

        const res = calculateCustomScore(numbers, inputIsDelta);
        document.getElementById('customScoreVal').textContent = res.score;
        document.getElementById('customInfoText').textContent = `Dizi: [${res.dizi.join(', ')}] • Boyut: P = ${res.dizi.length}`;

        const grid = document.getElementById('customCoverageGrid');
        grid.innerHTML = '';
        for (let num = 1; num <= res.score + 1; num++) {
            const info = analyzeDecomposition(res.dizi, num, res.score);
            const cell = document.createElement('div');
            cell.className = `num-cell ${info.typeClass}`;
            cell.textContent = num;
            cell.title = `Sayı ${num} | ${info.typeClass.replace('cell-', '').toUpperCase()}`;
            grid.appendChild(cell);
        }
    });
}
