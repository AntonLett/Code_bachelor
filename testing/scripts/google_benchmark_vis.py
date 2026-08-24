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

def parse_benchmark_data(data):
    """Extrahiert Iterations-Daten aus Google Benchmark JSON."""
    benchmarks = {}
    for entry in data['benchmarks']:
        if entry['run_type'] == 'iteration':
            name = entry['name']
            if name not in benchmarks:
                benchmarks[name] = []
            benchmarks[name].append(entry['real_time'])
    return benchmarks

if __name__ == "__main__":

    #######################################################
    # SETTINGS
    #######################################################
    targetFolder = ""
    outputName = "updater_benchmark_analysis.pgf"
    jsonFile = "testing/results/updater_benchmark.json"  # Deine JSON-Datei
    
    y_label = 'Ausf"uhrdauer (s)'
    title = 'Vergleich "Anderungen erkennen gegen Daten versenden'
    
    # Farben (konsistent mit deinem bestehenden Code)
    color_compare = "teal"
    color_senddata = "chocolate"

    #######################################################
    # DATEN LADEN
    #######################################################
    jobj = read_json_file(jsonFile)
    benchmarks = parse_benchmark_data(jobj)
    
    compare_times = benchmarks.get('UpdaterFixture/BM_Compare', [])
    send_times = benchmarks.get('UpdaterFixture/BM_SendData', [])
    
    # Warmup (Lauf 0) von stabilen Läufen trennen
    send_warmup = send_times[0] if len(send_times) > 0 else None
    send_stable = send_times[1:] if len(send_times) > 1 else []
    
    # Statistik berechnen
    compare_mean = np.mean(compare_times)
    compare_std = np.std(compare_times)
    
    send_stable_mean = np.mean(send_stable)
    send_stable_std = np.std(send_stable)

    #######################################################
    # PLOT ERSTELLEN
    #######################################################
    fig, (ax1, ax2) = plt.subplots(1, 2, figsize=(12, 4))  # Breite verdoppelt für 2 Plots
    fig.suptitle(title, fontsize=12, fontweight='bold', y=1.05)
    
    # --- PLOT A: Balkendiagramm (Stabile Performance) ---
    labels = ['BM\\_Compare', 'BM\\_SendData\\,(stabil)']
    means = [compare_mean, send_stable_mean]
    stds = [compare_std, send_stable_std]
    x_pos = np.arange(len(labels))
    
    bars = ax1.bar(x_pos, means, yerr=stds, capsize=5, 
                   color=[color_compare, color_senddata], 
                   alpha=0.8, edgecolor='black', linewidth=0.8)
    
    ax1.set_xticks(x_pos)
    ax1.set_xticklabels(labels, rotation=0)
    ax1.set_ylabel(y_label)
    ax1.set_title('(A) Durchschnittliche Laufzeit (ohne Warmup)', fontweight='bold', fontsize=10)
    ax1.grid(axis='y', alpha=0.3)
    ax1.set_axisbelow(True)
    
    # Werte über die Balken schreiben
    for bar, mean in zip(bars, means):
        height = bar.get_height()
        ax1.text(bar.get_x() + bar.get_width() / 2.0, height, 
                 f'{mean:.1f}', ha='center', va='bottom', fontsize=9)
    
    # --- PLOT B: Boxplot (Verteilung SendData mit Ausreißer) ---
    bp_data = [send_times]
    bp = ax2.boxplot(bp_data, labels=['BM\\_SendData\\,(alle)'], 
                     patch_artist=True, widths=0.6)
    
    # Box färben
    bp['boxes'][0].set_facecolor(color_senddata)
    bp['boxes'][0].set_alpha(0.6)
    bp['boxes'][0].set_edgecolor('black')
    bp['boxes'][0].set_linewidth(0.8)
    
    # Whisker und Caps stylen
    for whisker in bp['whiskers']:
        whisker.set_color('black')
        whisker.set_linewidth(0.8)
    for cap in bp['caps']:
        cap.set_color('black')
        cap.set_linewidth(0.8)
    bp['medians'][0].set_color('black')
    bp['medians'][0].set_linewidth(1.2)
    
    # Den Warmup-Ausreißer explizit markieren
    if send_warmup:
        ax2.plot(1, send_warmup, 'D', color='black', markersize=8, 
                 label=f'Warmup: {send_warmup:.1f}s')
        ax2.legend(loc='upper right', fontsize=8)
    
    ax2.set_ylabel(y_label)
    ax2.set_title('(B) Verteilung \& Ausreißer-Analyse', fontweight='bold', fontsize=10)
    ax2.grid(axis='y', alpha=0.3)
    ax2.set_axisbelow(True)
    
    # Layout anpassen
    plt.tight_layout(rect=[0, 0.03, 1, 0.95])
    plt.savefig(outputName)
    print(f"Wrote file: {outputName}")

    #######################################################
    # TEXTVORSCHLAG FÜR DIE ARBEIT
    #######################################################
    print("\n" + "="*70)
    print("TEXTVORSCHLAG FÜR DIE BACHELORARBEIT")
    print("="*70)
    print(f"""
Abbildung X zeigt die Benchmark-Ergebnisse des Updaters. Teil (A) vergleicht die 
durchschnittliche Ausf"uhrdauer beider Funktionen im stabilen Zustand. Der erste 
Messlauf von BM\\_SendData wurde als Warmup exkludiert. Teil (B) visualisiert die 
Verteilung aller Messwerte von BM\\_SendData inklusive des Ausrei"ßers im ersten 
Lauf (markiert mit Raute).

Statistik:
  BM\\_Compare:     Ø = {compare_mean:.2f} s, σ = {compare_std:.2f} s
  BM\\_SendData:    Ø = {send_stable_mean:.2f} s, σ = {send_stable_std:.2f} s (ohne Warmup)
  Warmup-Lauf:      {send_warmup:.2f} s ({((send_warmup/send_stable_mean)-1)*100:.0f}% "uber stabilem Mittelwert)
    """)
    print("="*70)