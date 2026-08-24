import matplotlib.pyplot as plt
import seaborn as sns
import json
import os
from pathlib import Path
import re

# ============================================================
# KONFIGURATION
# ============================================================
RESULTS_FOLDER = "./testing/results/performance_comparison/"  # Ordner mit den JSON-Dateien
OUTPUT_PREFIX = "performance_comparison"  # Name der Ausgabedateien

# ============================================================
# HYPERFINE JSON EINLESEN
# ============================================================
def load_hyperfine_results(folder):
    """Lädt alle JSON-Dateien aus dem Ordner und sortiert sie nach Dateinamen."""
    results = []
    json_files = ["res_new_crawler.json","res_new_0.json", "res_new_25.json", "res_new_50.json", "res_new_75.json", "res_new_100.json"]

    
    if not json_files:
        raise FileNotFoundError(f"Keine JSON-Dateien in {folder} gefunden!")
    
    for json_file in json_files:
        with open(RESULTS_FOLDER + json_file, 'r', encoding='utf-8') as f:
            data = json.load(f)
        
        # Extrahiere die relevanten Daten aus dem ersten Result (falls mehrere Commands)
        result = data["results"][1]
        
        results.append({
            # "filename": json_file.name,
            "command": result["command"],
            "mean": result["mean"],
            "stddev": result["stddev"],
            "median": result["median"],
            "min": result["min"],
            "max": result["max"],
            "times": result["times"]  # Einzelne Messwerte für Boxplot
        })
    
    return results

# ============================================================
# DATEN LADEN
# ============================================================
print(f"Lade Messdaten aus '{RESULTS_FOLDER}'...")
all_results = load_hyperfine_results(RESULTS_FOLDER)

# Extrahiere Namen für die Legende (ohne .json und Nummerierung)
method_names = ["Crawler", "0\%", "25\%", "50\%", "75\%", "100\%"]
# for r in all_results:
#     name = r["filename"].replace(".json", "")
#     # Optional: Entferne führende Zahlen und Unterstriche für sauberere Namen
#     name = " ".join(name.split("_")[1:]) if "_" in name else name
#     method_names.append(name if name else r["filename"])

# Extrahiere die einzelnen Messwerte für den Boxplot
all_times = [r["times"] for r in all_results]

# Extrahiere Mittelwerte und Standardabweichungen
means = [r["mean"] for r in all_results]
stds = [r["stddev"] for r in all_results]
medians = [r["median"] for r in all_results]

print(f"Gefunden: {len(all_results)} Messreihen")
for i, r in enumerate(all_results):
    print(f"  {i+1}. {method_names[i]}: {r['mean']:.3f}s ± {r['stddev']:.3f}s")

# ============================================================
# PLOT EINSTELLUNGEN (Thesis-Style)
# ============================================================
sns.set_theme(style="whitegrid", context="paper")
plt.rcParams['font.size'] = 11
plt.rcParams['axes.linewidth'] = 1.2
plt.rcParams['font.family'] = 'sans-serif'

# Farben: Baseline Grau, Varianten Blau
colors = ['#7f7f7f'] + ['#1f77b4'] * (len(all_results) - 1)

# Erstelle ein Figure mit 3 Subplots nebeneinander
fig, axes = plt.subplots(1, 3, figsize=(18, 6))

# ============================================================
# PLOT 1: Normalisiertes Säulendiagramm (in %)
# ============================================================
ax1 = axes[0]

# Normalisiere auf Baseline (erste Messung = 100%)
baseline_mean = means[0]
normalized_means = [(m / baseline_mean) * 100 for m in means]
normalized_stds = [(s / baseline_mean) * 100 for s in stds]

bars1 = ax1.bar(method_names, normalized_means, yerr=normalized_stds, 
                capsize=6, color=colors, edgecolor='black', linewidth=1.2)
ax1.axhline(y=100, color='red', linestyle='--', linewidth=1.5, label='Baseline (100%)')
ax1.set_ylabel('Ausführungszeit relativ zur Baseline (%)')
ax1.set_title('1. Normalisierte Ausführungszeit')
ax1.set_ylim(0, max(normalized_means) * 1.25)
ax1.legend(loc='upper right')
ax1.grid(axis='y', alpha=0.3, linestyle='--')

# Werte über den Balken anzeigen
for i, (mean, std) in enumerate(zip(normalized_means, normalized_stds)):
    ax1.text(i, mean + normalized_stds[i] + 2, f'{mean:.1f}%', 
             ha='center', va='bottom', fontsize=9)

# ============================================================
# PLOT 2: Speedup Faktor
# ============================================================
ax2 = axes[1]

# Speedup = Baseline / Wert
speedup_means = [baseline_mean / m for m in means]
# Approximation der Fehlerfortpflanzung für Speedup
speedup_stds = [(s / m) * sp for s, m, sp in zip(stds, means, speedup_means)]

bars2 = ax2.bar(method_names, speedup_means, yerr=speedup_stds, 
                capsize=5, color=colors, edgecolor='black', linewidth=1.2)
ax2.axhline(y=1.0, color='red', linestyle='--', linewidth=1.5, label='Baseline (1.0x)')
ax2.set_ylabel('Speedup Faktor (x)')
ax2.set_title('2. Speedup im Vergleich zur Baseline')
ax2.set_ylim(0, max(speedup_means) * 1.25)
ax2.legend(loc='upper right')
ax2.grid(axis='y', alpha=0.3, linestyle='--')

# Werte über den Balken anzeigen
for i, mean in enumerate(speedup_means):
    ax2.text(i, mean + speedup_stds[i] + 0.05, f'{mean:.2f}x', 
             ha='center', va='bottom', fontsize=9)

# ============================================================
# PLOT 3: Boxplot (Verteilung der Einzelmessungen)
# ============================================================
ax3 = axes[2]

bp = ax3.boxplot(all_times, labels=method_names, patch_artist=True, widths=0.6)

# Farben für die Boxen anpassen
for patch, color in zip(bp['boxes'], colors):
    patch.set_facecolor(color)
    patch.set_edgecolor('black')
    patch.set_linewidth(1.2)

# Median Linie hervorheben
for median in bp['medians']:
    median.set_color('black')
    median.set_linewidth(2)

# Whisker und Caps stylen
for whisker in bp['whiskers']:
    whisker.set_color('black')
    whisker.set_linewidth(1.2)

for cap in bp['caps']:
    cap.set_color('black')
    cap.set_linewidth(1.2)

ax3.set_ylabel('Ausführungszeit (s)')
ax3.set_title('3. Verteilung der Messwerte (Boxplot)')
ax3.grid(axis='y', alpha=0.3, linestyle='--')

# ============================================================
# GESAMT
# ============================================================
plt.suptitle('Performance Evaluation: Baseline vs. Optimierungen', fontsize=14, y=1.02, fontweight='bold')
plt.tight_layout()

# Speichern als PDF (Vektorgrafik für LaTeX) und PNG
plt.savefig(f'{OUTPUT_PREFIX}.pdf', format='pdf', bbox_inches='tight')
plt.savefig(f'{OUTPUT_PREFIX}.png', dpi=300, bbox_inches='tight')
print(f"\nGrafiken gespeichert: {OUTPUT_PREFIX}.pdf, {OUTPUT_PREFIX}.png")

plt.show()

# ============================================================
# ZUSÄTZLICH: TABELLE FÜR DIE ARBEIT
# ============================================================
print("\n" + "="*80)
print("ZUSAMMENFASSUNG FÜR DIE BACHELORARBEIT")
print("="*80)
print(f"{'Methode':<25} {'Mean (s)':<12} {'Stddev (s)':<12} {'Relativ (%)':<12} {'Speedup':<10}")
print("-"*80)
for i, r in enumerate(all_results):
    rel = (r['mean'] / baseline_mean) * 100
    sp = baseline_mean / r['mean']
    print(f"{method_names[i]:<25} {r['mean']:<12.4f} {r['stddev']:<12.4f} {rel:<12.2f} {sp:<10.3f}")
print("="*80)