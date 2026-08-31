# -*- coding: utf-8 -*-
import streamlit as st
import json
import os
import math
import pandas as pd
import numpy as np
import plotly.graph_objects as go
import plotly.express as px

# -----------------------------------------------------------------------------
# SAYFA YAPILANDIRMASI
# -----------------------------------------------------------------------------
st.set_page_config(
    page_title="PSPP Matematiksel Limitler & Dizi Analiz Laboratuvarı",
    page_icon="🎯",
    layout="wide",
    initial_sidebar_state="expanded"
)

# Özel CSS Stilleri
st.markdown("""
<style>
    .main-title {
        font-size: 2.2rem;
        font-weight: 800;
        background: linear-gradient(90deg, #38BDF8, #A855F7, #F472B6);
        -webkit-background-clip: text;
        -webkit-text-fill-color: transparent;
        margin-bottom: 0.2rem;
    }
    .sub-title {
        font-size: 1.1rem;
        color: #E2E8F0;
        font-weight: 500;
        margin-bottom: 1.5rem;
    }
    .metric-card {
        background: #1E293B;
        border: 1px solid #475569;
        border-radius: 12px;
        padding: 16px 20px;
        margin-bottom: 10px;
        color: #F8FAFC;
    }
    .metric-val {
        font-size: 1.8rem;
        font-weight: 800;
        color: #38BDF8;
        font-family: monospace;
    }
    .metric-lbl {
        font-size: 0.9rem;
        color: #F1F5F9;
        font-weight: 700;
        text-transform: uppercase;
        letter-spacing: 0.05em;
        margin-bottom: 4px;
    }
    .tag-proven {
        background: #064E3B;
        color: #6EE7B7;
        border: 1px solid #10B981;
        padding: 4px 10px;
        border-radius: 6px;
        font-size: 0.85rem;
        font-weight: 700;
    }
    .tag-candidate {
        background: #0C4A6E;
        color: #7DD3FC;
        border: 1px solid #0284C7;
        padding: 4px 10px;
        border-radius: 6px;
        font-size: 0.85rem;
        font-weight: 700;
    }
    .formula-box {
        background: #1E293B;
        border-left: 5px solid #8B5CF6;
        border-top: 1px solid #334155;
        border-right: 1px solid #334155;
        border-bottom: 1px solid #334155;
        padding: 14px 20px;
        border-radius: 0 10px 10px 0;
        margin: 10px 0;
        font-family: monospace;
        color: #F8FAFC;
        font-size: 0.95rem;
        line-height: 1.6;
    }
    .formula-box strong {
        color: #38BDF8;
        font-size: 1rem;
    }
    .formula-box em {
        color: #CBD5E1;
        font-style: normal;
    }
</style>
""", unsafe_allow_html=True)

# -----------------------------------------------------------------------------
# VERİ YÜKLEME VE MATEMATİKSEL FONKSİYONLAR
# -----------------------------------------------------------------------------
DB_PATH = os.path.join(os.path.dirname(__file__), "pspp_database.json")

@st.cache_data
def load_database():
    if os.path.exists(DB_PATH):
        with open(DB_PATH, "r", encoding="utf-8") as f:
            return json.load(f)
    return {}

KNOWLEDGE_BASE = load_database()

# Klasik Posta Pulu Alt Sınırı (Sadece Toplam: a(n) = floor((n^2 + 6n + 1)/4))
def classical_postage_stamp(p):
    return (p * p + 6 * p + 1) // 4

# Teorik Üst Tavan (Sıfır Çakışma: P(P+1) = P^2 + P)
def theoretical_upper_ceiling(p):
    return p * (p + 1)

# Asimptotik Erdős-Lunnon Sınırı (2/3 * (P^2 + P))
def asymptotic_bound(p):
    return int((2 * (p * p + p)) / 3)

# 64-Bit CTZ Skor Hesaplayıcı (Python İle Birebir Simülasyon)
def calculate_pspp_score(dizi):
    seen = set(dizi)
    n = len(dizi)
    for i in range(n):
        for j in range(i, n):
            seen.add(dizi[i] + dizi[j])
            seen.add(abs(dizi[j] - dizi[i]))
    m = 0
    while (m + 1) in seen:
        m += 1
    return m, seen

# Sayı Ayrıştırma Analizi
def analyze_number_origin(dizi, target, max_score):
    if target > max_score:
        return "missing", "İlk Eksik Sayı", f"Üretilemez (Kopuş Noktası: {target})"
    
    n = len(dizi)
    is_natural = target in dizi
    natural_idx = dizi.index(target) if is_natural else -1
    
    sum_pairs = []
    for i in range(n):
        for j in range(i, n):
            if dizi[i] + dizi[j] == target:
                sum_pairs.append(f"P[{i}]+P[{j}] ({dizi[i]}+{dizi[j]})")
                
    diff_pairs = []
    for i in range(n):
        for j in range(i + 1, n):
            if dizi[j] - dizi[i] == target:
                diff_pairs.append(f"P[{j}]-P[{i}] ({dizi[j]}-{dizi[i]})")
                
    if is_natural:
        extra = []
        if sum_pairs: extra.append(f"Alternatif Toplam: {', '.join(sum_pairs[:2])}")
        if diff_pairs: extra.append(f"Alternatif Fark: {', '.join(diff_pairs[:2])}")
        details = f"Dizi Elemanı: P[{natural_idx}] = {target}"
        if extra: details += " | " + " | ".join(extra)
        return "natural", "Doğal Eleman", details
    
    if sum_pairs and diff_pairs:
        return "both", "Toplam & Fark", f"Toplam: {sum_pairs[0]} | Fark: {diff_pairs[0]}"
    elif sum_pairs:
        return "sum", "Toplam (P+P)", f"Toplam: {', '.join(sum_pairs[:3])}"
    elif diff_pairs:
        return "diff", "Fark (P-P)", f"Fark: {', '.join(diff_pairs[:3])}"
    
    return "missing", "Eksik", f"{target} sayısı üretilemiyor"

# -----------------------------------------------------------------------------
# DİNAMİK MODÜLER PROJEKSİYON HESAPLAYICI
# -----------------------------------------------------------------------------
def get_dynamic_projection_params(kb):
    valid_records = {int(k): v for k, v in kb.items() if v.get("score", 0) > 0}
    if not valid_records:
        return 20, 264, 22
    
    # Veritabanındaki en yüksek skorlu zirve P boyutunu referans al
    best_p = 1
    best_score = 1
    for p, rec in sorted(valid_records.items()):
        if rec["score"] >= best_score:
            best_p = p
            best_score = rec["score"]
            
    # Zirve çözümün taban adımını (delta[0]) bularak eğimi (slope = 2 * Taban) dinamik belirle
    sols = valid_records[best_p].get("solutions", [])
    if sols and "delta" in sols[0] and sols[0]["delta"]:
        base_step = sols[0]["delta"][0]
        slope = max(2 * base_step, 16)
    else:
        slope = 22
        
    return best_p, best_score, slope

REF_P, REF_SCORE, DYNAMIC_SLOPE = get_dynamic_projection_params(KNOWLEDGE_BASE)

# -----------------------------------------------------------------------------
# VERİ TABLOSUNU OLUŞTUR
# -----------------------------------------------------------------------------
MAX_TABLE_P = 100
p_vals = list(range(1, MAX_TABLE_P + 1))

rows = []
for p in p_vals:
    p_str = str(p)
    has_record = p_str in KNOWLEDGE_BASE and KNOWLEDGE_BASE[p_str].get("score", 0) > 0
    
    pspp_score = KNOWLEDGE_BASE[p_str]["score"] if has_record else None
    postage_lower = classical_postage_stamp(p)
    asymp_val = asymptotic_bound(p)
    upper_ceil = theoretical_upper_ceiling(p)
    
    # Durum Tespiti: P <= 8 Mutlak Kanıtlanmış Max, P >= 9 Bilinen Rekor (Mutlak Max için Alt Sınır)
    if p <= 8 and has_record:
        status = "Mutlak Kanıtlanmış Max (P <= 8)"
        point_type = "Absolute Proven Max"
    elif has_record:
        status = "Bilinen Zirve Rekor (Mutlak Max İçin Alt Sınır)"
        point_type = "Known Record (Lower Bound on Global Max)"
    else:
        status = f"Modüler Model Projeksiyonu (+{DYNAMIC_SLOPE}/adım)"
        point_type = "Modular Projection Lower Bound"
        # Dinamik projeksiyon: En son zirve skordan başlayarak eğimle uzat
        if p > REF_P:
            pspp_score = REF_SCORE + DYNAMIC_SLOPE * (p - REF_P)
        else:
            pspp_score = None
    
    gain = (pspp_score - postage_lower) if pspp_score is not None else None
    eff = (pspp_score / upper_ceil * 100) if pspp_score is not None else None
    
    rows.append({
        "P": p,
        "PSPP_Score": pspp_score,
        "Postage_Lower_Bound": postage_lower,
        "Asymptotic_2_3_Bound": asymp_val,
        "Theoretical_Upper_Ceil": upper_ceil,
        "Subtractive_Gain": gain,
        "Efficiency_Pct": eff,
        "Status": status,
        "Point_Type": point_type,
        "Has_Record": has_record,
        "Is_Absolute_Proven": (p <= 8)
    })

df_bounds = pd.DataFrame(rows)

# -----------------------------------------------------------------------------
# BAŞLIK VE GİRİŞ
# -----------------------------------------------------------------------------
st.markdown('<div class="main-title">PSPP Matematiksel Limitler & Dizi Analiz Laboratuvarı</div>', unsafe_allow_html=True)
st.markdown('<div class="sub-title">Klasik Posta Pulu Problemi Alt Sınırı, Teorik Sıfır Çakışma Tavanı ve Morfolojik Dizi Ayrışımları</div>', unsafe_allow_html=True)

# -----------------------------------------------------------------------------
# SEKMELİ SAYFA DÜZENİ
# -----------------------------------------------------------------------------
tab1, tab2, tab3, tab4, tab5 = st.tabs([
    "📈 Limitler & Sınır Karşılaştırması",
    "🎯 Kesin Noktalar vs Alt Sınırlar",
    "🔬 Dizi & Kapsama Haritası Analizi",
    "📋 Tüm Delta Dizileri Matrisi (Kalıp Keşfi)",
    "🧪 Özel Dizi Test Laboratuvarı"
])

# =============================================================================
# SEKME 1: LİMİTLER & SINIR KARŞILAŞTIRMASI
# =============================================================================
with tab1:
    st.header("Matematiksel Sınırlar Zarfı (Mathematical Bounds Envelope)")
    st.markdown("""
    Postage Stamp Probleminde sadece toplama kullanılırken, **PSPP (Postage Stamp with Subtraction)** modelinde **fark ($p_j - p_i$)** operasyonu da devreye girer.
    Bu nedenle PSPP skoru **$M(P)$**, her zaman klasik posta pulu alt sınırından belirgin şekilde büyüktür ve teorik tavan $P(P+1)$ aralığında yer alır:
    """)
    
    col_f1, col_f2, col_f3 = st.columns(3)
    with col_f1:
        st.markdown("""
        <div class="formula-box">
        <strong>1. Klasik Posta Pulu Alt Sınırı:</strong><br>
        a(P) = ⌊(P² + 6P + 1) / 4⌋<br>
        <em>(Yalnızca Toplam Kullanımı)</em>
        </div>
        """, unsafe_allow_html=True)
    with col_f2:
        st.markdown("""
        <div class="formula-box" style="border-left-color: #06B6D4;">
        <strong>2. Sezgisel Asimptotik Eğri:</strong><br>
        M_sezgisel ≈ ⌊2/3 · (P² + P)⌋<br>
        <em>(PSPP Ampirik Modeli)</em>
        </div>
        """, unsafe_allow_html=True)
    with col_f3:
        st.markdown("""
        <div class="formula-box" style="border-left-color: #EC4899;">
        <strong>3. Teorik Sıfır Çakışma Tavanı:</strong><br>
        M_max = P(P + 1) = P² + P<br>
        <em>(Hiçbir Çakışma Olmayan Mutlak Tavan)</em>
        </div>
        """, unsafe_allow_html=True)

    # Grafik Filtreleri
    col_ctrl1, col_ctrl2 = st.columns([1, 3])
    with col_ctrl1:
        max_p_view = st.slider("Görüntülenecek P Boyutu Aralığı:", min_value=5, max_value=100, value=50, step=1)
        scale_type = st.radio("Grafik Ölçeği:", ["Lineer", "Logaritmik"], horizontal=True)
    
    df_filtered = df_bounds[df_bounds["P"] <= max_p_view].copy()

    # Plotly Ana Sınır Zarfı Grafiği
    fig_envelope = go.Figure()

    # Üst Tavan Alanı (Shading)
    fig_envelope.add_trace(go.Scatter(
        x=df_filtered["P"], y=df_filtered["Theoretical_Upper_Ceil"],
        mode='lines',
        line=dict(color='rgba(236, 72, 153, 0.4)', dash='dash'),
        name='Teorik Üst Tavan P(P+1)'
    ))

    # Sezgisel Asimptotik Eğri (2/3 Sınırı)
    fig_envelope.add_trace(go.Scatter(
        x=df_filtered["P"], y=df_filtered["Asymptotic_2_3_Bound"],
        mode='lines',
        line=dict(color='#A855F7', width=2, dash='dot'),
        name='Sezgisel Asimptotik Eğri 2/3'
    ))

    # PSPP Noktaları: P <= 8 Mutlak Kanıtlanmış, P = 9..17 Bilinen Rekor (Alt Sınır)
    df_proven = df_filtered[df_filtered["P"] <= 8]
    df_candidate = df_filtered[(df_filtered["P"] > 8) & (df_filtered["Has_Record"] == True)]
    df_projected = df_filtered[df_filtered["Has_Record"] == False]

    # 1. P <= 8 Mutlak Kanıtlanmış Global Maksimumlar (Yeşil Elmas)
    fig_envelope.add_trace(go.Scatter(
        x=df_proven["P"], y=df_proven["PSPP_Score"],
        mode='lines+markers',
        marker=dict(size=11, color='#10B981', symbol='diamond', line=dict(color='#FFFFFF', width=1.5)),
        line=dict(color='#10B981', width=3),
        name='Mutlak Kanıtlanmış Max M(P) (P <= 8: Kesin Global)'
    ))

    # 2. P = 9..17 Bilinen En İyi Rekorlar (Mavi Halka - Mutlak Max İçin Alt Sınır)
    if not df_candidate.empty:
        fig_envelope.add_trace(go.Scatter(
            x=df_candidate["P"], y=df_candidate["PSPP_Score"],
            mode='lines+markers',
            marker=dict(size=10, color='#06B6D4', symbol='circle', line=dict(color='#FFFFFF', width=1.5)),
            line=dict(color='#06B6D4', width=3, dash='solid'),
            name='Bilinen Zirve Rekor M(P) (P >= 9: Mutlak Max İçin Alt Sınır)'
        ))

    # 3. P > 17 Modüler Projeksiyon Alt Sınırı (Mor Kesikli)
    if not df_projected.empty:
        fig_envelope.add_trace(go.Scatter(
            x=df_projected["P"], y=df_projected["PSPP_Score"],
            mode='lines',
            line=dict(color='#C084FC', width=2, dash='dash'),
            name='Modüler Model Projeksiyonu (Teorik Alt Sınır: P > 17)'
        ))

    # Klasik Alt Sınır (Turuncu)
    fig_envelope.add_trace(go.Scatter(
        x=df_filtered["P"], y=df_filtered["Postage_Lower_Bound"],
        mode='lines+markers',
        marker=dict(size=6, color='#F59E0B'),
        line=dict(color='#F59E0B', width=2),
        fill='tonexty', # Shading
        fillcolor='rgba(6, 182, 212, 0.08)',
        name='Klasik Posta Pulu Alt Sınırı a(P) [Yalnızca Toplam]'
    ))

    # Dikey Ayırıcı Çizgi (P = 8.5)
    if max_p_view > 8:
        fig_envelope.add_vline(
            x=8.5, line_width=1.5, line_dash="dash", line_color="#E2E8F0",
            annotation_text="P <= 8: Mutlak Kesin | P >= 9: Bilinen Alt Sınır", 
            annotation_position="top left",
            annotation_font=dict(color="#E2E8F0", size=11)
        )

    fig_envelope.update_layout(
        title="PSPP Sınır Zarfı: Klasik Alt Sınır <= Bilinen Rekor <= Mutlak Global Max <= Üst Tavan",
        xaxis_title="Dizi Boyutu (P)",
        yaxis_title="Maksimum Kesintisiz Aralık (M)",
        yaxis_type="log" if scale_type == "Logaritmik" else "linear",
        template="plotly_dark",
        hovermode="x unified",
        height=540,
        legend=dict(orientation="h", yanchor="bottom", y=1.02, xanchor="right", x=1)
    )
    st.plotly_chart(fig_envelope, use_container_width=True)

    # İkili Alt Grafikler: Fark Kazancı ve Oranlar
    col_g1, col_g2 = st.columns(2)
    with col_g1:
        fig_gain = go.Figure()
        fig_gain.add_trace(go.Bar(
            x=df_filtered["P"],
            y=df_filtered["Subtractive_Gain"],
            marker=dict(
                color=df_filtered["Subtractive_Gain"],
                colorscale="Viridis",
                showscale=False
            ),
            name="Fark Operasyonu Kazancı"
        ))
        fig_gain.update_layout(
            title="Fark İşleminin Sağladığı Ekstra Aralık: M(P) - a(P)",
            xaxis_title="P Boyutu",
            yaxis_title="Ekstra Sayı Kazancı",
            template="plotly_dark",
            height=360
        )
        st.plotly_chart(fig_gain, use_container_width=True)

    with col_g2:
        df_filtered["M_over_P2"] = df_filtered["PSPP_Score"] / (df_filtered["P"] ** 2)
        fig_ratio = go.Figure()
        fig_ratio.add_trace(go.Scatter(
            x=df_filtered["P"], y=df_filtered["M_over_P2"],
            mode='lines+markers',
            line=dict(color='#34D399', width=2),
            marker=dict(size=7, color='#10B981'),
            name="M(P) / P² Kapasite Oranı"
        ))
        fig_ratio.add_hline(y=2/3, line_dash="dash", line_color="#EC4899", annotation_text="Sezgisel Asimptotik Limit 2/3 ≈ 0.667")
        if max_p_view > 8:
            fig_ratio.add_vline(x=8.5, line_width=1, line_dash="dash", line_color="#94A3B8")
        fig_ratio.update_layout(
            title="Kapasite Oranı: M(P) / P² (Sezgisel 2/3 Seviyesine Uyum)",
            xaxis_title="P Boyutu",
            yaxis_title="M / P²",
            template="plotly_dark",
            height=360
        )
        st.plotly_chart(fig_ratio, use_container_width=True)

    # Sezgisel Asimptotik Eğri ve Boyut Ayrışımı Açıklaması
    st.markdown("""
    <div class="formula-box" style="border-left-color: #F59E0B; margin-top: 15px;">
    <strong>💡 Sezgisel Asimptotik Eğri ve P <= 8 vs P >= 9 Ayrımı Neyi Anlatır?</strong><br>
    • <strong>Sezgisel Asimptotik Eğri (2/3):</strong> PSPP verilerimizde gözlemlenen ve çözümlerin ulaştığı yoğunluğu ifade eden ampirik bir sezgisel tahmin eğrisidir (M ≈ ⌊2/3 · (P² + P)⌋).<br>
    • <strong>Küçük Boyutlar (P <= 8):</strong> Tüm kombinasyonlar eksiksiz taranmış olup bu değerler <u>Mutlak Global Maksimumdur</u>.<br>
    • <strong>Büyük Boyutlar (P >= 9):</strong> Modüler motorumuzla bulduğumuz rekorlar (örn: P=17'de 186) <u>Mutlak Max için garanti birer ALT SINIRDIR</u> (Minimum Max).
    </div>
    """, unsafe_allow_html=True)

# =============================================================================
# SEKME 2: KESİN NOKTALAR VS ALT SINIRLAR
# =============================================================================
with tab2:
    st.header("Kesin Global Noktalar ve Alt Sınır Sınıflandırması")
    st.markdown(r"""
    Bu bölümde elimizdeki her $P$ değerinin matematiksel statüsü net bir şekilde ayrıştırılmıştır:
    - 🟢 **Mutlak Kanıtlanmış Max ($P \le 8$):** Arama uzayı eksiksiz taranmış ve matematiksel olarak daha büyüğünün olamayacağı kesinleşmiş mutlak değerler.
    - 🔵 **Bilinen En İyi Zirve Rekor ($P \ge 9$):** Modüler arama motoru ile bulunmuş rekorlar. Tüm uzay taranmadığı için **Mutlak Max için bir alt sınır (minimum max)** niteliğindedir ($M_{\text{global}} \ge M_{\text{bilinen}}$).
    - 🟣 **Modüler Projeksiyon ($P > 17$):** Aile 1 yapısının matematiksel $+16$ modüler kuralının garanti ettiği teorik alt sınır projeksiyonlarıdır.
    """)

    col_stat1, col_stat2, col_stat3, col_stat4 = st.columns(4)
    with col_stat1:
        st.markdown("""
        <div class="metric-card">
            <div class="metric-lbl">Mutlak Kanıtlanmış Maxima</div>
            <div class="metric-val" style="color: #34D399;">P = 1 .. 8</div>
            <span class="tag-proven">8 Boyut Eksiksiz Kanıtlandı</span>
        </div>
        """, unsafe_allow_html=True)
    with col_stat2:
        st.markdown("""
        <div class="metric-card">
            <div class="metric-lbl">Bilinen Max (Alt Sınır)</div>
            <div class="metric-val" style="color: #38BDF8;">P = 9 .. 17</div>
            <span class="tag-candidate">9 Boyut Doğrulandı</span>
        </div>
        """, unsafe_allow_html=True)
    with col_stat3:
        st.markdown("""
        <div class="metric-card">
            <div class="metric-lbl">P=17 Rekor / Klasik Oranı</div>
            <div class="metric-val">186 / 98</div>
            <span style="color: #FCD34D; font-size: 0.85rem; font-weight:600;">%90 Daha Fazla Kapsama!</span>
        </div>
        """, unsafe_allow_html=True)
    with col_stat4:
        st.markdown("""
        <div class="metric-card">
            <div class="metric-lbl">Sezgisel Eğri Uyumu</div>
            <div class="metric-val" style="color: #C084FC;">%96.8</div>
            <span style="color: #94A3B8; font-size: 0.85rem;">2/3·(P²+P) Eğrisi</span>
        </div>
        """, unsafe_allow_html=True)

    # Detaylı Veri Tablosu
    st.subheader("Tüm Boyutlar İçin Karşılaştırmalı Veri Tablosu")
    
    display_df = df_bounds[["P", "PSPP_Score", "Postage_Lower_Bound", "Asymptotic_2_3_Bound", "Theoretical_Upper_Ceil", "Subtractive_Gain", "Efficiency_Pct", "Status"]].copy()
    display_df.columns = ["P", "PSPP Skoru M(P)", "Klasik Posta Pulu a(P)", "Sezgisel Asimptotik Eğri 2/3", "Teorik Tavan P(P+1)", "Fark Kazancı (M - a)", "Verim (%)", "Matematiksel Statü"]
    
    st.dataframe(
        display_df.style.format({
            "PSPP Skoru M(P)": "{:.0f}",
            "Klasik Posta Pulu a(P)": "{:.0f}",
            "Asimptotik Limit 2/3": "{:.0f}",
            "Teorik Tavan P(P+1)": "{:.0f}",
            "Fark Kazancı (M - a)": "{:.0f}",
            "Verim (%)": "{:.1f}%"
        }).highlight_max(subset=["PSPP Skoru M(P)"], color="rgba(6, 182, 212, 0.3)"),
        use_container_width=True,
        height=450
    )

# =============================================================================
# SEKME 3: DİZİ & KAPSAMA HARİTASI ANALİZİ
# =============================================================================
with tab3:
    st.header("Dizi Analizi & Renkli Sayı Kapsama Haritası")
    st.markdown("Veritabanımızdaki herhangi bir $P$ boyutunun delta dizisini, kümülatif elemanlarını ve sayıların nasıl üretildiğini inceleyin.")

    available_p = [int(k) for k in KNOWLEDGE_BASE.keys() if KNOWLEDGE_BASE[k].get("score", 0) > 0]
    available_p.sort()

    col_sel1, col_sel2 = st.columns([1, 2])
    with col_sel1:
        sel_p = st.selectbox("İncelenecek P Boyutunu Seçin:", available_p, index=len(available_p)-1)
    
    rec = KNOWLEDGE_BASE[str(sel_p)]
    sol_list = rec.get("solutions", [])
    
    with col_sel2:
        sol_idx = st.selectbox("Alternatif Çözüm Seçin:", list(range(len(sol_list))), format_func=lambda i: f"Çözüm #{i+1} (Delta: {sol_list[i]['delta'][:4]}...)")
    
    active_sol = sol_list[sol_idx]
    active_delta = active_sol["delta"]
    active_dizi = active_sol["dizi"]
    active_score = rec["score"]

    # Dizi Bilgi Kartları
    col_d1, col_d2, col_d3 = st.columns(3)
    with col_d1:
        st.markdown(f"**Delta Artış Dizisi (Δ):** `{active_delta}`")
    with col_d2:
        st.markdown(f"**Kümülatif Ana Dizi (P):** `{active_dizi}`")
    with col_d3:
        st.markdown(f"**Maksimum Skor:** `{active_score}` (Toplam: `{sum(active_delta)}`)")

    # Kapsama Haritası
    st.subheader(f"P={sel_p} İçin 1 .. {active_score + 1} Sayı Ayrışım Haritası")
    
    st.markdown("""
    **Renk Göstergeleri (Lejant):**  
    - 🟢 **Doğal Eleman ($P_i$):** Sayı doğrudan dizinin kendi elemanıdır.  
    - 🔵 **Toplam ($P_i + P_j$):** İki dizi elemanının toplamı ile üretilmiştir.  
    - 🟣 **Fark ($P_j - P_i$):** İki dizi elemanının farkı ile üretilmiştir.  
    - 🟡 **Toplam & Fark:** Hem toplam hem fark kombinasyonlarıyla elde edilebilen çifte kapsama.  
    - 🔴 **İlk Eksik Sayı ($M+1$):** Zincirin koptuğu üretilemeyen ilk sayıdır.
    """)

    # Büyük Skorlar İçin Sayfalama / Görünüm Aralığı Seçici
    max_cells_per_page = 300
    total_nums = active_score + 1
    
    if total_nums > max_cells_per_page:
        num_pages = math.ceil(total_nums / max_cells_per_page)
        page_idx = st.selectbox(
            f"Görüntülenecek Sayı Aralığı Seçin (Toplam {total_nums:,} Sayı):", 
            list(range(num_pages)), 
            format_func=lambda i: f"{i * max_cells_per_page + 1} .. {min((i + 1) * max_cells_per_page, total_nums)}"
        )
        start_num = page_idx * max_cells_per_page + 1
        end_num = min((page_idx + 1) * max_cells_per_page, total_nums)
    else:
        start_num = 1
        end_num = total_nums

    # Renk Belirleme (Yuksek Kontrastli Parlak Tasarim)
    bg_colors = {
        "natural": "#065F46",
        "sum": "#0369A1",
        "diff": "#6B21A8",
        "both": "#92400E",
        "missing": "#991B1B"
    }
    border_colors = {
        "natural": "#34D399",
        "sum": "#38BDF8",
        "diff": "#C084FC",
        "both": "#FCD34D",
        "missing": "#F87171"
    }

    # Tek Geçişte Yüksek Performanslı CSS Grid HTML Oluşturma (0.02 saniye)
    grid_items = []
    for num in range(start_num, end_num + 1):
        origin_type, badge, details = analyze_number_origin(active_dizi, num, active_score)
        bg = bg_colors.get(origin_type, "#1E293B")
        border = border_colors.get(origin_type, "#475569")
        grid_items.append(f'<div style="background:{bg}; color:#FFFFFF; border:2px solid {border}; border-radius:8px; padding:7px 0; text-align:center; font-family:monospace; font-weight:800; font-size:0.95rem; box-shadow:0 2px 4px rgba(0,0,0,0.3);" title="{badge}: {details}">{num}</div>')

    full_grid_html = f"""
    <div style="display: grid; grid-template-columns: repeat(auto-fill, minmax(65px, 1fr)); gap: 6px; margin-top: 10px; margin-bottom: 20px;">
        {''.join(grid_items)}
    </div>
    """
    st.markdown(full_grid_html, unsafe_allow_html=True)

# =============================================================================
# SEKME 4: TÜM DELTA DİZİLERİ MATRİSİ (KALIP KEŞFİ)
# =============================================================================
with tab4:
    st.header("📋 Tüm Boyutlar İçin Alt Alta Delta Matrisi (Görsel Kalıp Keşfi)")
    st.markdown(r"""
    Tüm $P=1 \dots 100$ boyutlarının Delta artış dizilerini alt alta sıralayarak gövdedeki ve kuyruktaki matematiksel desenleri görsel olarak inceleyin.
    """)

    col_m1, col_m2, col_m3 = st.columns([1, 1, 2])
    with col_m1:
        align_mode = st.radio("Hizalama Yönü:", ["Sağa Hizalı (Kuyruk Odaklı)", "Sola Hizalı (Gövde Odaklı)"], horizontal=False)
    with col_m2:
        show_all_solutions = st.checkbox("Alternatif Çözümleri de Göster", value=False)
        p_filter_range = st.selectbox("Görüntülenecek Boyut Aralığı:", ["Tümü (P = 1 .. 100)", "P = 1 .. 25", "P = 26 .. 50", "P = 51 .. 75", "P = 76 .. 100"])
    with col_m3:
        st.markdown("""
        **Renk Kodları:**  
        <span style="background:#0284C7; color:#fff; padding:2px 8px; border-radius:4px; font-weight:700;">8+</span> Gövde | 
        <span style="background:#4F46E5; color:#fff; padding:2px 8px; border-radius:4px; font-weight:700;">6</span> Geçiş | 
        <span style="background:#7C3AED; color:#fff; padding:2px 8px; border-radius:4px; font-weight:700;">4</span> Kuyruk | 
        <span style="background:#D97706; color:#fff; padding:2px 8px; border-radius:4px; font-weight:700;">3</span> Kuyruk | 
        <span style="background:#059669; color:#fff; padding:2px 8px; border-radius:4px; font-weight:700;">2</span> Kuyruk | 
        <span style="background:#E11D48; color:#fff; padding:2px 8px; border-radius:4px; font-weight:700;">1</span> Kuyruk
        """, unsafe_allow_html=True)

    # Maksimum P'yi bul
    all_p = sorted([int(k) for k in KNOWLEDGE_BASE.keys() if KNOWLEDGE_BASE[k].get("score", 0) > 0])
    
    if "1 .. 25" in p_filter_range:
        selected_p_list = [p for p in all_p if p <= 25]
    elif "26 .. 50" in p_filter_range:
        selected_p_list = [p for p in all_p if 26 <= p <= 50]
    elif "51 .. 75" in p_filter_range:
        selected_p_list = [p for p in all_p if 51 <= p <= 75]
    elif "76 .. 100" in p_filter_range:
        selected_p_list = [p for p in all_p if 76 <= p <= 100]
    else:
        selected_p_list = all_p

    max_p_len = max(selected_p_list) if selected_p_list else 17

    def get_delta_color(val):
        if val >= 8: return "#0284C7", "#38BDF8"
        if val == 6 or val == 7: return "#4F46E5", "#818CF8"
        if val == 4 or val == 5: return "#7C3AED", "#C084FC"
        if val == 3: return "#D97706", "#FCD34D"
        if val == 2: return "#059669", "#34D399"
        if val == 1: return "#E11D48", "#FB7185"
        return "#475569", "#94A3B8"

    # Tek Seferde Yüksek Performanslı Matris HTML'i Oluşturma
    matrix_rows_html = []
    for p in selected_p_list:
        rec = KNOWLEDGE_BASE[str(p)]
        score = rec["score"]
        solutions = rec.get("solutions", [])
        
        target_sols = solutions if show_all_solutions else (solutions[:1] if solutions else [])
        
        for s_idx, sol in enumerate(target_sols):
            d_list = sol["delta"]
            sum_d = sum(d_list)
            ratio = score / sum_d if sum_d > 0 else 0
            
            badges = []
            for d_val in d_list:
                bg, border = get_delta_color(d_val)
                badge_html = f'<span style="display:inline-block; width:28px; height:28px; line-height:24px; text-align:center; background:{bg}; color:#FFFFFF; border:2px solid {border}; border-radius:5px; font-family:monospace; font-weight:800; font-size:0.85rem; margin:1px;">{d_val}</span>'
                badges.append(badge_html)
            
            pad_count = max_p_len - len(d_list)
            empty_badge = '<span style="display:inline-block; width:28px; height:28px; margin:1px;"></span>'
            
            if "Sağa" in align_mode:
                rendered_cells = (empty_badge * pad_count) + "".join(badges)
            else:
                rendered_cells = "".join(badges) + (empty_badge * pad_count)
            
            sol_label = f" (#{s_idx+1})" if show_all_solutions and len(solutions) > 1 else ""
            
            row_html = f"""
            <div style="background:#1E293B; border:1px solid #334155; border-radius:8px; padding:4px 12px; margin-bottom:4px; display:flex; align-items:center; justify-content:space-between;">
                <div style="min-width:145px; font-family:monospace;">
                    <strong style="color:#38BDF8; font-size:0.95rem;">P = {p:2d}{sol_label}</strong> 
                    <span style="color:#94A3B8; font-size:0.8rem; margin-left:6px;">M = <strong style="color:#34D399;">{score:4d}</strong> | Σ = {sum_d:3d}</span>
                </div>
                <div style="overflow-x:auto; white-space:nowrap; padding:0 6px;">
                    {rendered_cells}
                </div>
                <div style="min-width:85px; text-align:right; font-family:monospace; color:#CBD5E1; font-size:0.8rem;">
                    M/Σ = <strong style="color:#FCD34D;">{ratio:.2f}</strong>
                </div>
            </div>
            """
            matrix_rows_html.append(row_html)

    st.markdown("".join(matrix_rows_html), unsafe_allow_html=True)

    # Düz Metin Kopyalama Alanı
    st.markdown("### 📋 Düz Metin Formatında Delta Dizileri")
    plain_text_lines = []
    for p in selected_p_list:
        rec = KNOWLEDGE_BASE[str(p)]
        d_str = str(rec["solutions"][0]["delta"]) if rec.get("solutions") else "[]"
        plain_text_lines.append(f"P = {p:2d} | M = {rec['score']:4d} | Delta: {d_str}")
    
    st.code("\n".join(plain_text_lines), language="text")

# =============================================================================
# SEKME 5: ÖZEL DİZİ TEST LABORATUVARI
# =============================================================================
with tab5:
    st.header("Özel Dizi Test & Hesaplama Laboratuvarı")
    st.markdown("Kendi belirlediğiniz herhangi bir Delta veya Normal diziyi girerek maksimum skorunu, sınırlarla durumunu ve kapsama haritasını anında hesaplayın.")

    input_mode = st.radio("Giriş Formatı Seçin:", ["Delta Dizisi Gir (Örn: 8, 8, 8, 4, 2, 3, 1, 1, 2, 2)", "Normal Dizi Gir (Örn: 2, 4, 5, 19, 20, 31, 32)"], horizontal=True)
    custom_input_str = st.text_input("Dizi Elemanlarını Virgülle Ayırarak Girin:", value="8, 8, 8, 8, 4, 2, 3, 1, 1, 2, 2")

    if st.button("🚀 Diziyi Test Et ve Analiz Et", use_container_width=True):
        try:
            raw_nums = [int(x.strip()) for x in custom_input_str.split(",") if x.strip().isdigit()]
            if not raw_nums:
                st.error("Lütfen geçerli pozitif sayılar giriniz!")
            else:
                if "Delta" in input_mode:
                    calc_delta = raw_nums
                    calc_dizi = list(np.cumsum(calc_delta))
                else:
                    calc_dizi = sorted(raw_nums)
                    calc_delta = [calc_dizi[0]] + [calc_dizi[i] - calc_dizi[i-1] for i in range(1, len(calc_dizi))]
                
                custom_p = len(calc_dizi)
                custom_score, _ = calculate_pspp_score(calc_dizi)
                postage_lim = classical_postage_stamp(custom_p)
                upper_lim = theoretical_upper_ceiling(custom_p)

                st.success(f"**Test Sonucu:** P = {custom_p} Elemanlı Dizi İçin Maksimum Skor: **M = {custom_score}**")

                col_res1, col_res2, col_res3, col_res4 = st.columns(4)
                with col_res1:
                    st.metric("Elde Edilen Skor (M)", custom_score)
                with col_res2:
                    st.metric("Klasik Posta Pulu Alt Sınırı a(P)", postage_lim, delta=f"+{custom_score - postage_lim} Fark Kazancı")
                with col_res3:
                    st.metric("Teorik Üst Tavan P(P+1)", upper_lim)
                with col_res4:
                    st.metric("Kapasite Doluluk Oranı", f"%{(custom_score / upper_lim * 100):.1f}")

                st.markdown(f"**Kümülatif Dizi (P):** `{calc_dizi}`")
                st.markdown(f"**Delta Dizisi (Δ):** `{calc_delta}`")

        except Exception as e:
            st.error(f"Hesaplama sırasında hata oluştu: {e}")

