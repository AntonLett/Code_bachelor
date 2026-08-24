import matplotlib
matplotlib.use("pgf")
import matplotlib.pyplot as plt
import numpy as np
import json
import os
import os.path as op
from pathlib import Path

plt.rcParams.update({
    "pgf.texsystem": "pdflatex",
    'font.family': 'serif',
    'font.serif': ['Computer Modern Roman'],
    'text.usetex': False,
    'pgf.preamble': r'\usepackage[utf8]{inputenc}'
})

def read_json_file(file: str):
    with open(file) as f:
        d = json.load(f)
    return d

if __name__ == "__main__":

    #######################################################
    # SETTINGS
    #######################################################
    targetFolder = "testing/results/performance_comparison/"
    outputName = "updater_stacked_comparison.pgf"
    
    # Alle JSON-Dateien, die verglichen werden sollen
    files = [
        "res_new_0.json",
        "res_new_25.json", 
        "res_new_50.json",
        "res_new_75.json",
        "res_new_100.json",
        "res_new_crawler.json"
    ]
    
    # Labels für die X-Achse
    names = ["0\\%", "25\\%", "50\\%", "75\\%", "100\\%", "Crawler"]
    
    x_label = "Test-Szenario"
    y_label = 'Ausf"uhrdauer (s)'
    title = 'Gesamtlaufzeit: Typefinder + Updater'
    
    # Farben (konsistent mit deinem Code)
    color_typefinder = "teal"
    color_updater = "chocolate"

    #######################################################
    # DATEN LADEN
    #######################################################
    folder_path = Path(targetFolder)
    
    # Speicher für die Daten
    typefinder_means = []
    typefinder_stddevs = []
    updater_means = []
    updater_stddevs = []
    
    for f in files:
        file_path = op.join(folder_path, f)
        if not os.path.exists(file_path):
            print(f"Warnung: Datei {file_path} nicht gefunden, übersprungen.")
            continue
            
        jobj = read_json_file(file_path)
        results = jobj.get("results", [])
        
        # Annahme: results[0] = typefinder, results[1] = updater
        if len(results) >= 2:
            typefinder_means.append(results[0].get("mean", 0))
            typefinder_stddevs.append(results[0].get("stddev", 0))
            updater_means.append(results[1].get("mean", 0))
            updater_stddevs.append(results[1].get("stddev", 0))
        elif len(results) == 1:
            typefinder_means.append(results[0].get("mean", 0))
            typefinder_stddevs.append(results[0].get("stddev", 0))
            updater_means.append(0)
            updater_stddevs.append(0)

    #######################################################
    # PLOT ERSTELLEN
    #######################################################
    fig, ax = plt.subplots(figsize=(6, 4))
    
    x_pos = np.arange(len(names))
    bar_width = 0.6
    
    # Typefinder Balken (unten)
    bars_tf = ax.bar(x_pos, typefinder_means, bar_width,
                     label='Typefinder',
                     color=color_typefinder,
                     alpha=0.8,
                     edgecolor='black',
                     linewidth=0.8)
    
    # Updater Balken (oben, gestapelt)
    bars_up = ax.bar(x_pos, updater_means, bar_width,
                     label='Updater',
                     color=color_updater,
                     alpha=0.8,
                     edgecolor='black',
                     linewidth=0.8,
                     bottom=typefinder_means)  # WICHTIG: Auf Typefinder stapeln
    
    # Error-Bars für die GESAMTzeit (Summe beider Stddevs)
    # Hinweis: Bei unabhängigen Messungen: sqrt(σ₁² + σ₂²)
    total_stddevs = [np.sqrt(tf**2 + up**2) for tf, up in zip(typefinder_stddevs, updater_stddevs)]
    total_means = [tf + up for tf, up in zip(typefinder_means, updater_means)]
    
    ax.errorbar(x_pos, total_means, yerr=total_stddevs,
                fmt='none', ecolor='black', capsize=5, linewidth=1.5, 
                label='Gesamt-Stddev')
    
    # Werte in die Balken schreiben
    for i, (tf, up) in enumerate(zip(typefinder_means, updater_means)):
        # Typefinder Wert (unten, zentriert)
        if tf > 0:
            ax.text(i, tf / 2, f'{tf:.1f}', 
                    ha='center', va='center', fontsize=8, fontweight='bold', 
                    color='white' if tf > 50 else 'black')
        
        # Updater Wert (oben, zentriert im oberen Segment)
        if up > 0:
            ax.text(i, tf + up / 2, f'{up:.1f}', 
                    ha='center', va='center', fontsize=8, fontweight='bold',
                    color='white' if up > 50 else 'black')
    
    # Achsen beschriften
    ax.set_xticks(x_pos)
    ax.set_xticklabels(names, rotation=0)
    ax.set_xlabel(x_label)
    ax.set_ylabel(y_label)
    ax.set_title(title)
    ax.grid(axis='y', alpha=0.3)
    ax.set_axisbelow(True)
    
    # Legende
    ax.legend(loc='upper left')
    
    # Layout anpassen
    plt.tight_layout()
    plt.savefig(outputName)
    print(f"Wrote file: {outputName}")

    #######################################################
    # STATISTIK-AUSGABE
    #######################################################
    print("\n" + "="*80)
    print("ZUSAMMENFASSUNG: GESTAPELTE LAUFZEITEN")
    print("="*80)
    print(f"{'Szenario':<12} | {'Typefinder':<15} | {'Updater':<15} | {'Gesamt':<15}")
    print(f"{'':12} | {'(mean ± σ)':<15} | {'(mean ± σ)':<15} | {'(mean ± σ)':<15}")
    print("-"*80)
    
    for i, name in enumerate(names):
        tf = typefinder_means[i]
        tf_std = typefinder_stddevs[i]
        up = updater_means[i]
        up_std = updater_stddevs[i]
        total = tf + up
        total_std = total_stddevs[i]
        
        # Anteil berechnen
        if total > 0:
            tf_pct = (tf / total) * 100
            up_pct = (up / total) * 100
        else:
            tf_pct = up_pct = 0
        
        print(f"{name:<12} | {tf:>6.2f} ± {tf_std:>5.2f} s ({tf_pct:>5.1f}%) | "
              f"{up:>6.2f} ± {up_std:>5.2f} s ({up_pct:>5.1f}%) | "
              f"{total:>6.2f} ± {total_std:>5.2f} s")
    
    print("="*80)
    print("\nHinweis: Gesamt-Stddev berechnet als √(σ₁² + σ₂²) für unabhängige Messungen")
    print("="*80)