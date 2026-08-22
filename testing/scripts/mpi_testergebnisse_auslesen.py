#!/usr/bin/env python3
import re
import statistics

# Text einlesen (entweder aus Datei oder Variable)
with open('testing/results/results_laurent_sorted_change_sorted.txt', 'r') as f:
    content = f.read()

# Alle "real"-Zeiten finden (Format: XmYs oder Xs oder Ys)
pattern = r'real\s+(\d+m)?(\d+\.?\d*)s'
matches = re.findall(pattern, content)

# In Sekunden umwandeln
times_seconds = []
for m in matches:
    minutes = float(m[0].replace('m', '')) if m[0] else 0
    seconds = float(m[1]) if m[1] else 0
    times_seconds.append(minutes * 60 + seconds)

# Statistik berechnen
mean = statistics.mean(times_seconds)
stdev = statistics.stdev(times_seconds) if len(times_seconds) > 1 else 0
min_val = min(times_seconds)
max_val = max(times_seconds)

# Ausgabe
print(f"=== Statistik über {len(times_seconds)} Durchläufe ===")
print(f"Mean:   {mean:.3f} s ({mean/60:.2f} min)")
print(f"StdDev: {stdev:.3f} s")
print(f"Min:    {min_val:.3f} s")
print(f"Max:    {max_val:.3f} s")
print(f"\nErgebnis für Bachelorarbeit:")
print(f"{mean:.2f} s ± {stdev:.2f} s")