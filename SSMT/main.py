"""
SSMT (Seçim Sonrası Maksimum Tahmini / Alman Tank Problemi)
Ana Çalıştırma Dosyası (İnteraktif Menü ve CLI Destekli)

Kullanım:
    1. İnteraktif Menü (Doğrudan çalıştırma):
       python main.py

    2. Komut Satırı Argümanları ile çalıştırma:
       python main.py --observations 20 --save-plot ssmt_simulation.png
       python main.py --fixed-limit 1000 --observations 25
       python main.py --monte-carlo 100 --observations 15
"""

import argparse
import sys
import os

# Windows konsolunda UTF-8 karakter desteği sağla
if sys.stdout and hasattr(sys.stdout, "reconfigure"):
    try:
        sys.stdout.reconfigure(encoding="utf-8", errors="replace")
    except Exception:
        pass

# Proje dizinini Python path'e ekle
CURRENT_DIR = os.path.dirname(os.path.abspath(__file__))
if CURRENT_DIR not in sys.path:
    sys.path.insert(0, CURRENT_DIR)

from core.producer import Producer
from evaluation.comparator import Comparator
from evaluation.visualizer import Visualizer
from estimators.registry import get_registered_estimators


def parse_args():
    parser = argparse.ArgumentParser(
        description="SSMT (Alman Tank Problemi) Modüler Simülasyon ve Görselleştirme Aracı",
        formatter_class=argparse.ArgumentDefaultsHelpFormatter,
    )
    parser.add_argument(
        "--min-limit",
        type=int,
        default=500,
        help="Rastgele belirlenecek gerçek üst limit (N) için alt sınır",
    )
    parser.add_argument(
        "--max-limit",
        type=int,
        default=1500,
        help="Rastgele belirlenecek gerçek üst limit (N) için üst sınır",
    )
    parser.add_argument(
        "--fixed-limit",
        type=int,
        default=None,
        help="Sabit bir N değeri belirlemek için (Örn: 1000)",
    )
    parser.add_argument(
        "-k", "--observations",
        type=int,
        default=10,
        help="Üreticiden çekilecek gözlem sayısı",
    )
    parser.add_argument(
        "--with-replacement",
        action="store_true",
        help="İadeli örnekleme kullan (varsayılan: iadesiz örnekleme)",
    )
    parser.add_argument(
        "--monte-carlo",
        type=int,
        default=0,
        metavar="ITERS",
        help="Monte Carlo simülasyon modu (belirtilen tur sayısı kadar bağımsız deney yapar)",
    )
    parser.add_argument(
        "--save-plot",
        type=str,
        default="ssmt_simulation.png",
        help="Oluşturulan grafiğin kaydedileceği dosya yolu",
    )
    parser.add_argument(
        "--no-show",
        action="store_true",
        help="Etkileşimli GUI grafik penceresini açma (yalnızca dosyaya kaydet)",
    )
    parser.add_argument(
        "--seed",
        type=int,
        default=None,
        help="Rastgelelik için başlangıç tohumu (seed)",
    )
    parser.add_argument(
        "--interactive",
        action="store_true",
        help="İnteraktif menü modunu zorla başlat",
    )
    return parser.parse_args()


def _prompt_int(prompt: str, default: int) -> int:
    """Kullanıcıdan tamsayı girdisi alır, boş bırakılırsa varsayılanı döner."""
    while True:
        try:
            val = input(f"{prompt} [Varsayılan: {default}]: ").strip()
        except EOFError:
            return default
        if not val:
            return default
        try:
            return int(val)
        except ValueError:
            print("  [!] Lütfen geçerli bir tam sayı girin.")


def _prompt_bool(prompt: str, default_yes: bool = True) -> bool:
    """Kullanıcıdan evet/hayır girdisi alır."""
    def_str = "E" if default_yes else "H"
    try:
        val = input(f"{prompt} (E/H) [Varsayılan: {def_str}]: ").strip().upper()
    except EOFError:
        return default_yes
    if not val:
        return default_yes
    return val.startswith("E") or val.startswith("Y")


def interactive_menu():
    """Kullanici dostu interaktif terminal sihirbazi."""
    while True:
        print("\n" + "=" * 65)
        print("     SSMT: SECIM SONRASI MAKSIMUM TAHMINI SIMULASYONU")
        print("=" * 65)

        registered = get_registered_estimators()
        print(f"Kayitli Tahmin Ediciler ({len(registered)} adet):")
        for key, cls_ref in registered.items():
            inst = cls_ref()
            print(f"  - {inst.display_name:<20} : {inst.description}")

        print("\nLutfen calistirmak istediginiz modu secin:")
        print("  [1] Tekil Simulasyon (Rastgele N ve Cok Panelli Grafik)")
        print("  [2] Sabit Ust Limitli Simulasyon (Orn: N=1000 ve Grafik)")
        print("  [3] Monte Carlo Basari Analizi (4 Ayri Pencerede Grafikler)")
        print("  [0] Cikis")

        try:
            choice = input("\nSeciminiz [1/2/3/0]: ").strip()
        except EOFError:
            break

        if choice == "0":
            print("\nCikis yapiliyor. Iyi calismalar!\n")
            break

        elif choice == "1":
            print("\n--- TEKIL SIMULASYON AYARLARI (RASTGELE N) ---")
            k = _prompt_int("Gozlem sayisi (k)", 10)
            min_n = _prompt_int("Minimum N alt siniri", 500)
            max_n = _prompt_int("Maksimum N ust siniri", 1500)
            with_repl = not _prompt_bool("Iadesiz ornekleme yapilsin mi?", True)

            producer = Producer(min_limit=min_n, max_limit=max_n, without_replacement=not with_repl)
            comparator = Comparator(producer=producer)

            print(f"\n[*] Uretici rastgele N = {producer.true_limit} degerini belirledi.")
            print(f"[*] {k} adet gozlem ile simulasyon kosturuluyor...")
            run_record = comparator.run_simulation(total_observations=k)

            Visualizer.print_terminal_table(run_record)
            save_path = os.path.join(CURRENT_DIR, "ssmt_simulation.png")
            show_gui = _prompt_bool("Grafik penceresi ekranda acilsin mi?", True)
            Visualizer.plot_simulation(run=run_record, save_path=save_path, show_plot=show_gui)
            print(f"[OK] Simulasyon tamamlandi. Grafik kaydedildi: {save_path}")

        elif choice == "2":
            print("\n--- SABIT UST LIMITLI SIMULASYON AYARLARI ---")
            fixed_n = _prompt_int("Sabit gercek ust limit (N)", 1000)
            k = _prompt_int("Gozlem sayisi (k)", 10)
            with_repl = not _prompt_bool("Iadesiz ornekleme yapilsin mi?", True)

            producer = Producer(fixed_limit=fixed_n, without_replacement=not with_repl)
            comparator = Comparator(producer=producer)

            print(f"\n[*] Uretici N = {fixed_n} olarak ayarlandi.")
            print(f"[*] {k} adet gozlem ile simulasyon kosturuluyor...")
            run_record = comparator.run_simulation(total_observations=k)

            Visualizer.print_terminal_table(run_record)
            save_path = os.path.join(CURRENT_DIR, "ssmt_simulation.png")
            show_gui = _prompt_bool("Grafik penceresi ekranda acilsin mi?", True)
            Visualizer.plot_simulation(run=run_record, save_path=save_path, show_plot=show_gui)
            print(f"[OK] Simulasyon tamamlandi. Grafik kaydedildi: {save_path}")

        elif choice == "3":
            print("\n--- MONTE CARLO COKLU DENEY AYARLARI ---")
            iters = _prompt_int("Deney (Tur) sayisi", 100)
            k = _prompt_int("Her turdaki gozlem sayisi (k)", 10)
            min_n = _prompt_int("Minimum N alt siniri", 500)
            max_n = _prompt_int("Maksimum N ust siniri", 1500)

            print(f"\n[*] Monte Carlo baslatiliyor: {iters} Tur, Her turda {k} gozlem (Aralik: [{min_n}, {max_n}])...")
            comparator = Comparator()
            mc_results = comparator.run_monte_carlo(
                iterations=iters,
                observations_per_run=k,
                min_limit=min_n,
                max_limit=max_n,
            )

            Visualizer.print_monte_carlo_table(mc_results)
            save_path = os.path.join(CURRENT_DIR, "ssmt_monte_carlo.png")
            show_gui = _prompt_bool("Monte Carlo grafikleri 4 ayri pencerede ekranda acilsin mi?", True)
            Visualizer.plot_monte_carlo(mc_res=mc_results, save_path=save_path, show_plot=show_gui)
            print(f"[OK] Monte Carlo tamamlandi. 4 ayri grafik kaydedildi.")

        else:
            print("\n[!] Gecersiz secim, lutfen 1, 2, 3 veya 0 girin.")

        try:
            input("\nDevam etmek icin [Enter] tusuna basin...")
        except EOFError:
            break


def run_cli(args):
    """Komut satiri argumanlari ile dogrudan calistirma."""
    print("=" * 60)
    print("  SSMT: SECIM SONRASI MAKSIMUM TAHMINI SIMULASYONU")
    print("=" * 60)

    registered = get_registered_estimators()
    print(f"[Kayitli Tahmin Ediciler] ({len(registered)} adet):")
    for key, cls_ref in registered.items():
        inst = cls_ref()
        print(f"  - {inst.display_name:<20} : {inst.description}")
    print()

    # 1. MONTE CARLO MODU
    if args.monte_carlo > 0:
        print(f"[*] Monte Carlo Modu Baslatiliyor: {args.monte_carlo} Tur | Gozlem: {args.observations}...")
        comparator = Comparator()
        mc_results = comparator.run_monte_carlo(
            iterations=args.monte_carlo,
            observations_per_run=args.observations,
            min_limit=args.min_limit,
            max_limit=args.max_limit,
        )

        Visualizer.print_monte_carlo_table(mc_results)

        plot_file = args.save_plot if args.save_plot != "ssmt_simulation.png" else "ssmt_monte_carlo.png"
        save_path = os.path.join(CURRENT_DIR, plot_file) if not os.path.isabs(plot_file) else plot_file
        Visualizer.plot_monte_carlo(
            mc_res=mc_results,
            save_path=save_path,
            show_plot=not args.no_show,
        )
        print(f"[OK] Monte Carlo analizi tamamlandi. Grafik kaydedildi: {save_path}")
        return

    # 2. TEKİL SİMÜLASYON VE GÖRSELLEŞTİRME MODU
    producer = Producer(
        min_limit=args.min_limit,
        max_limit=args.max_limit,
        fixed_limit=args.fixed_limit,
        without_replacement=not args.with_replacement,
        seed=args.seed,
    )

    comparator = Comparator(producer=producer)
    print(f"[*] Üretici başlatıldı. Gerçek N = {producer.true_limit} olarak seçildi.")
    print(f"[*] Simülasyon koşturuluyor ({args.observations} gözlem adımı)...")

    run_record = comparator.run_simulation(total_observations=args.observations)

    # Konsol tablosunu bas
    Visualizer.print_terminal_table(run_record)

    # Matplotlib grafiğini çiz ve kaydet
    save_path = os.path.join(CURRENT_DIR, args.save_plot) if not os.path.isabs(args.save_plot) else args.save_plot
    Visualizer.plot_simulation(
        run=run_record,
        save_path=save_path,
        show_plot=not args.no_show,
    )
    print(f"[OK] Islem tamamlandi. Grafik kaydedildi: {save_path}")


def main():
    # Eğer hiçbir argüman verilmemişse veya --interactive belirtilmişse interaktif menüyü aç
    if len(sys.argv) == 1:
        interactive_menu()
    else:
        args = parse_args()
        if args.interactive:
            interactive_menu()
        else:
            run_cli(args)


if __name__ == "__main__":
    main()
