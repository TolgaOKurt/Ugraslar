"""
SSMT Visualizer (Görselleştirici) Modülü
Tahmin edicilerin performansını hem grafiksel (Matplotlib) hem de
konsol tablosu (ASCII / Tablo) olarak görselleştirir.
X ekseni: Gönderilen sayı miktarı (k)
Y ekseni: Tahmin edilen sayı (N_tahmin)
"""

import os
from typing import Optional, Dict
import matplotlib.pyplot as plt
import matplotlib.ticker as ticker
from core.contracts import SimulationRun, MonteCarloResult


class Visualizer:
    """
    Simülasyon sonuçlarını görselleştiren ve analiz eden sınıf.
    """

    # 8 Tahmin edici için zengin ve ayırt edici renk paleti
    COLOR_PALETTE = {
        "mvue": "#10B981",               # Zümrüt Yeşili (Optimal MVUE)
        "bayes_mean": "#06B6D4",         # Camgöbeği / Cyan (Bayes Ortalama)
        "bayes_median": "#6366F1",       # İndigo / Gece Mavisi (Bayes Medyan)
        "moments": "#3B82F6",            # Klasik Mavi (2x Ortalama)
        "corrected_moments": "#8B5CF6",   # Menekşe Moru (Düzeltilmiş Moment)
        "sample_median": "#EC4899",      # Pembe (Örneklem Medyanı)
        "symmetric_spacing": "#F97316",  # Turuncu (Simetrik Aralık)
        "sample_max": "#F59E0B",          # Kehribar Sarısı (Bilinen Maks)
        "true_limit": "#EF4444",          # Kırmızı (Gerçek N çizgisi)
        "default": "#64748B",             # Gri (Varsayılan)
    }

    # Çizgi stilleri ve işaretçiler
    STYLE_MAP = {
        "mvue": {"marker": "o", "linestyle": "-"},
        "bayes_mean": {"marker": "v", "linestyle": "-"},
        "bayes_median": {"marker": "^", "linestyle": "--"},
        "moments": {"marker": "s", "linestyle": "--"},
        "corrected_moments": {"marker": "p", "linestyle": "-."},
        "sample_median": {"marker": "h", "linestyle": ":"},
        "symmetric_spacing": {"marker": "*", "linestyle": "-."},
        "sample_max": {"marker": "D", "linestyle": ":"},
        "default": {"marker": ".", "linestyle": "-"},
    }

    @classmethod
    def print_terminal_table(cls, run: SimulationRun) -> None:
        """
        Konsola hizalanmış, renkli ve zengin bir özet tablosu basar.
        """
        if not run.history:
            print("Görselleştirilecek simülasyon verisi bulunamadı.")
            return

        estimator_names = list(run.history[0].estimates.keys())
        header_est = "  ".join(f"{name.upper():>10}" for name in estimator_names)
        header_err = "  ".join(f"Err_{name[:5]:>8}" for name in estimator_names)

        print("\n" + "=" * 90)
        print(f"  SSMT (Alman Tank Problemi) Simülasyonu | Gerçek Üst Limit (N) = {run.true_limit}")
        print(f"  Toplam Gözlem (k) = {run.total_steps} | İadesiz Örnekleme = {run.without_replacement}")
        print("=" * 90)
        print(f"{'k':>3} {'Sayı':>6} {'Maks':>6} | {header_est} | {header_err}")
        print("-" * 90)

        for step in run.history:
            est_str = "  ".join(f"{step.estimates[name]:10.1f}" for name in estimator_names)
            err_str = "  ".join(f"{step.pct_errors[name]:7.1f}%" for name in estimator_names)
            print(f"{step.step:3d} {step.sample:6d} {step.max_observed:6d} | {est_str} | {err_str}")

        print("=" * 90)
        print("\n--- ÖZET BAŞARI METRİKLERİ ---")
        print(f"{'Tahmin Edici':<20} {'MAE (Ort. Mutlak Hata)':<25} {'Ort. Yüzde Hata':<20} {'Son Adım Hatası':<20}")
        print("-" * 85)
        for name, summary in run.summaries.items():
            print(
                f"{name.upper():<20} "
                f"{summary.mae:<25.2f} "
                f"%{summary.mean_pct_error:<19.2f} "
                f"%{summary.final_pct_error:<19.2f}"
            )
        print("-" * 85 + "\n")

    @classmethod
    def print_monte_carlo_table(cls, mc_res: MonteCarloResult) -> None:
        """
        Monte Carlo deney sonuçlarını konsola düzenli bir tablo olarak basar.
        """
        print("\n" + "=" * 75)
        print(f"  MONTE CARLO SONUCLARI ({mc_res.iterations} Bagimsiz Deney | k={mc_res.observations_per_run})")
        print(f"  Evren Araligi: [{mc_res.min_limit}, {mc_res.max_limit}]")
        print("=" * 75)
        print(f"{'Tahmin Edici':<20} {'Ort. Hata %':<15} {'Std Sapma':<15} {'Min-Maks Hata':<22}")
        print("-" * 75)
        for name, metrics in mc_res.metrics.items():
            print(
                f"{name.upper():<20} "
                f"%{metrics['mean_final_pct_error']:<14.2f} "
                f"+-{metrics['std_final_pct_error']:<13.2f} "
                f"[%{metrics['min_error']:.1f} - %{metrics['max_error']:.1f}]"
            )
        print("=" * 75 + "\n")

    @classmethod
    def plot_simulation(
        cls,
        run: SimulationRun,
        save_path: Optional[str] = None,
        show_plot: bool = True,
    ) -> Optional[str]:
        """
        Matplotlib ile 3 panelli tekil simülasyon görselleştirmesi üretir.
        """
        if not run.history:
            print("Çizilecek simülasyon verisi yok.")
            return None

        plt.style.use("seaborn-v0_8-whitegrid" if "seaborn-v0_8-whitegrid" in plt.style.available else "default")
        fig = plt.figure(figsize=(15, 10), constrained_layout=True)
        gs = fig.add_gridspec(2, 2, height_ratios=[1.3, 1.0])

        ax_main = fig.add_subplot(gs[0, :])   # Üst panel: X: Gözlem Sayısı, Y: Tahmin Değerleri
        ax_error = fig.add_subplot(gs[1, 0])  # Alt sol: Hata Yüzdesi Gelişimi
        ax_bars = fig.add_subplot(gs[1, 1])   # Alt sağ: Başarı Metrikleri Karşılaştırma Barı

        steps = [rec.step for rec in run.history]
        true_n = run.true_limit
        estimator_names = list(run.history[0].estimates.keys())

        # 1. ANA GRAFİK (X: k, Y: Tahmin Edilen Sayı)
        ax_main.axhline(
            y=true_n,
            color=cls.COLOR_PALETTE["true_limit"],
            linestyle="--",
            linewidth=2.5,
            label=f"Gercek Ust Limit (N = {true_n})",
            zorder=4,
        )

        samples = [rec.sample for rec in run.history]
        ax_main.scatter(
            steps,
            samples,
            color="#9CA3AF",
            alpha=0.6,
            s=40,
            edgecolors="none",
            label="Gozlemlenen Sayilar",
            zorder=2,
        )

        for name in estimator_names:
            color = cls.COLOR_PALETTE.get(name, cls.COLOR_PALETTE["default"])
            style = cls.STYLE_MAP.get(name, cls.STYLE_MAP["default"])
            values = [rec.estimates[name] for rec in run.history]

            ax_main.plot(
                steps,
                values,
                label=f"Tahmin: {name.upper()}",
                color=color,
                linewidth=2.2,
                marker=style["marker"],
                markersize=6,
                linestyle=style["linestyle"],
                zorder=5,
            )

        ax_main.set_title(
            f"SSMT (Alman Tank Problemi) - Gozlem Sayisina Gore Ust Limit Tahmini (Gercek N={true_n})",
            fontsize=14,
            fontweight="bold",
            pad=12,
        )
        ax_main.set_xlabel("Gonderilen Sayi Miktari (Gozlem Sayisi - k)", fontsize=12)
        ax_main.set_ylabel("Tahmin Edilen Sayi (N_tahmin)", fontsize=12)
        ax_main.xaxis.set_major_locator(ticker.MaxNLocator(integer=True))
        ax_main.legend(loc="best", frameon=True, framealpha=0.9)

        # 2. HATA YUZDESI GELISIMI
        for name in estimator_names:
            color = cls.COLOR_PALETTE.get(name, cls.COLOR_PALETTE["default"])
            style = cls.STYLE_MAP.get(name, cls.STYLE_MAP["default"])
            pct_errors = [rec.pct_errors[name] for rec in run.history]

            ax_error.plot(
                steps,
                pct_errors,
                label=f"{name.upper()}",
                color=color,
                linewidth=2.0,
                marker=style["marker"],
                markersize=5,
                linestyle=style["linestyle"],
            )

        ax_error.set_title("Gozlem Sayisina Gore Hata Yuzdesi (%)", fontsize=12, fontweight="bold")
        ax_error.set_xlabel("Gozlem Sayisi (k)", fontsize=11)
        ax_error.set_ylabel("Mutlak Hata Payi (%)", fontsize=11)
        ax_error.xaxis.set_major_locator(ticker.MaxNLocator(integer=True))
        ax_error.legend(loc="upper right", framealpha=0.9)

        # 3. BASARI METRIKLERI BAR GRAFIGI
        est_labels = [name.upper() for name in estimator_names]
        final_errs = [run.summaries[name].final_pct_error for name in estimator_names]
        colors = [cls.COLOR_PALETTE.get(name, cls.COLOR_PALETTE["default"]) for name in estimator_names]

        x_pos = range(len(est_labels))
        bars = ax_bars.bar(x_pos, final_errs, color=colors, alpha=0.85, width=0.55, edgecolor="black")

        for bar in bars:
            height = bar.get_height()
            ax_bars.annotate(
                f"%{height:.1f}",
                xy=(bar.get_x() + bar.get_width() / 2, height),
                xytext=(0, 3),
                textcoords="offset points",
                ha="center",
                va="bottom",
                fontweight="bold",
            )

        ax_bars.set_xticks(list(x_pos))
        ax_bars.set_xticklabels(est_labels, fontweight="bold", rotation=20, ha="right")
        ax_bars.set_title("Son Adim Bagil Hata Yuzdesi Karsilastirmasi", fontsize=12, fontweight="bold")
        ax_bars.set_ylabel("Son Adim Hata Orani (%)", fontsize=11)

        if save_path:
            os.makedirs(os.path.dirname(os.path.abspath(save_path)), exist_ok=True)
            plt.savefig(save_path, dpi=300, bbox_inches="tight")
            print(f"[Grafik Kaydedildi] -> {save_path}")

        if show_plot:
            try:
                plt.show()
            except Exception:
                pass

        plt.close(fig)
        return save_path

    @classmethod
    def plot_monte_carlo(
        cls,
        mc_res: MonteCarloResult,
        save_path: Optional[str] = None,
        show_plot: bool = True,
    ) -> Optional[str]:
        """
        Monte Carlo simülasyonu için her analizi AYRI BİR PENCEREDE (4 ayrı bağımsız pencere) açar:
        1. Pencere: Hata Dağılımı Histogramı
        2. Pencere: Kutu Grafiği (Boxplot / Dağılım Yayılımı)
        3. Pencere: Gerçek N vs Tahmin Edilen N Saçılım Grafiği (Scatter Plot)
        4. Pencere: Gözlem Adımlarına Göre Ortalama Hata Azalma Eğrisi (Convergence)
        """
        plt.style.use("seaborn-v0_8-whitegrid" if "seaborn-v0_8-whitegrid" in plt.style.available else "default")
        estimator_names = list(mc_res.final_pct_errors.keys())

        base_dir = os.path.dirname(os.path.abspath(save_path)) if save_path else os.getcwd()
        base_name = os.path.splitext(os.path.basename(save_path))[0] if save_path else "ssmt_monte_carlo"

        # ====================================================
        # PENCERE 1: HATA DAGILIMI HISTOGRAMI
        # ====================================================
        fig1, ax1 = plt.subplots(figsize=(11, 6.5))
        for name in estimator_names:
            errors = mc_res.final_pct_errors[name]
            color = cls.COLOR_PALETTE.get(name, cls.COLOR_PALETTE["default"])
            ax1.hist(
                errors,
                bins=20,
                alpha=0.45,
                color=color,
                label=f"{name.upper()} (Ort: %{mc_res.metrics[name]['mean_final_pct_error']:.1f})",
                edgecolor=color,
                linewidth=1.2,
            )

        ax1.set_title(
            f"SSMT Monte Carlo (1/4) - Hata Yuzdesi Dagilim Frekansi ({mc_res.iterations} Tur | k={mc_res.observations_per_run})",
            fontsize=13,
            fontweight="bold",
            pad=12,
        )
        ax1.set_xlabel("Mutlak Hata Payi (%)", fontsize=11, fontweight="bold")
        ax1.set_ylabel("Frekans (Deney Sayisi)", fontsize=11, fontweight="bold")
        ax1.legend(loc="upper right", framealpha=0.95, fontsize=10)
        fig1.tight_layout()

        if save_path:
            p1 = os.path.join(base_dir, f"{base_name}_1_histogram.png")
            fig1.savefig(p1, dpi=300, bbox_inches="tight")
            print(f"[Grafik 1/4 Kaydedildi] -> {p1}")

        # ====================================================
        # PENCERE 2: KUTU GRAFIGI (BOXPLOT)
        # ====================================================
        fig2, ax2 = plt.subplots(figsize=(11, 6.5))
        data_to_box = [mc_res.final_pct_errors[name] for name in estimator_names]
        box_labels = [name.upper() for name in estimator_names]

        bp = ax2.boxplot(
            data_to_box,
            tick_labels=box_labels,
            patch_artist=True,
            medianprops=dict(color="red", linewidth=2.5),
            flierprops=dict(marker="o", markersize=4, alpha=0.6),
        )

        for patch, name in zip(bp["boxes"], estimator_names):
            color = cls.COLOR_PALETTE.get(name, cls.COLOR_PALETTE["default"])
            patch.set_facecolor(color)
            patch.set_alpha(0.65)

        ax2.set_xticklabels(box_labels, fontweight="bold", rotation=20, ha="right")
        ax2.set_title(
            f"SSMT Monte Carlo (2/4) - Hata Payi Dagilimi (Kutu Grafigi / Boxplot)",
            fontsize=13,
            fontweight="bold",
            pad=12,
        )
        ax2.set_ylabel("Hata Payi (%)", fontsize=11, fontweight="bold")
        fig2.tight_layout()

        if save_path:
            p2 = os.path.join(base_dir, f"{base_name}_2_boxplot.png")
            fig2.savefig(p2, dpi=300, bbox_inches="tight")
            print(f"[Grafik 2/4 Kaydedildi] -> {p2}")

        # ====================================================
        # PENCERE 3: GERCEK N vs TAHMIN EDILEN N SACILIMI (SCATTER)
        # ====================================================
        fig3, ax3 = plt.subplots(figsize=(11, 6.5))
        min_n = min(mc_res.true_limits) if mc_res.true_limits else mc_res.min_limit
        max_n = max(mc_res.true_limits) if mc_res.true_limits else mc_res.max_limit

        ax3.plot(
            [min_n, max_n],
            [min_n, max_n],
            color=cls.COLOR_PALETTE["true_limit"],
            linestyle="--",
            linewidth=2.5,
            label="Ideal Tahmin (Y = X)",
            zorder=3,
        )

        for name in estimator_names:
            estimates = mc_res.final_estimates[name]
            color = cls.COLOR_PALETTE.get(name, cls.COLOR_PALETTE["default"])
            style = cls.STYLE_MAP.get(name, cls.STYLE_MAP["default"])
            ax3.scatter(
                mc_res.true_limits,
                estimates,
                alpha=0.6,
                color=color,
                label=f"{name.upper()}",
                s=40,
                marker=style["marker"],
                zorder=4,
            )

        ax3.set_title(
            f"SSMT Monte Carlo (3/4) - Gercek N vs Tahmin Edilen N (Sacilim Grafigi)",
            fontsize=13,
            fontweight="bold",
            pad=12,
        )
        ax3.set_xlabel("Gercek Ust Limit (N)", fontsize=11, fontweight="bold")
        ax3.set_ylabel("Tahmin Edilen Deger (N_tahmin)", fontsize=11, fontweight="bold")
        ax3.legend(loc="upper left", framealpha=0.95, fontsize=10)
        fig3.tight_layout()

        if save_path:
            p3 = os.path.join(base_dir, f"{base_name}_3_scatter.png")
            fig3.savefig(p3, dpi=300, bbox_inches="tight")
            print(f"[Grafik 3/4 Kaydedildi] -> {p3}")

        # ====================================================
        # PENCERE 4: GOZLEM SAYISINA GORE ORTALAMA HATA AZALMA EGRISI
        # ====================================================
        fig4, ax4 = plt.subplots(figsize=(11, 6.5))
        steps = list(range(1, mc_res.observations_per_run + 1))
        for name in estimator_names:
            mean_step_errs = mc_res.step_mean_pct_errors[name]
            color = cls.COLOR_PALETTE.get(name, cls.COLOR_PALETTE["default"])
            style = cls.STYLE_MAP.get(name, cls.STYLE_MAP["default"])

            ax4.plot(
                steps,
                mean_step_errs,
                label=f"{name.upper()}",
                color=color,
                linewidth=2.2,
                marker=style["marker"],
                markersize=6,
                linestyle=style["linestyle"],
            )

        ax4.set_title(
            f"SSMT Monte Carlo (4/4) - Gozlem Sayisina Gore Ortalama Hatanin Azalmasi ({mc_res.iterations} Tur)",
            fontsize=13,
            fontweight="bold",
            pad=12,
        )
        ax4.set_xlabel("Gozlem Sayisi (k)", fontsize=11, fontweight="bold")
        ax4.set_ylabel("Ortalama Bagil Hata (%)", fontsize=11, fontweight="bold")
        ax4.xaxis.set_major_locator(ticker.MaxNLocator(integer=True))
        ax4.legend(loc="upper right", framealpha=0.95, fontsize=10)
        fig4.tight_layout()

        if save_path:
            p4 = os.path.join(base_dir, f"{base_name}_4_convergence.png")
            fig4.savefig(p4, dpi=300, bbox_inches="tight")
            print(f"[Grafik 4/4 Kaydedildi] -> {p4}")
            # Ana dosya yolu olarak da convergence veya özet grafiği bağlayalım
            if save_path != p4:
                fig4.savefig(save_path, dpi=300, bbox_inches="tight")

        if show_plot:
            try:
                plt.show()
            except Exception:
                pass

        plt.close(fig1)
        plt.close(fig2)
        plt.close(fig3)
        plt.close(fig4)
        return save_path
