import matplotlib.pyplot as plt
import numpy as np

# Deine Messwerte (Beispiel)
# Nehmen wir an, du hast 5 Messungen wie zuvor, aber als Balken
labels = ['0', '2', '4', '15', '85']  # Die echten Parameter-Werte als Labels
values = [10, 12, 11, 25, 24]         # Die Messergebnisse (Höhe der Balken)

# X-Positionen für die Balken (0, 1, 2, 3, 4)
# Wir wollen aber zwischen Index 2 (Wert 4) und Index 3 (Wert 15) eine Trennung
x_pos = np.arange(len(labels))

fig, ax = plt.subplots(figsize=(8, 5))

# 1. Balken zeichnen
# Wir können die ersten 3 und die letzten 2 auch unterschiedlich färben
colors = ['skyblue'] * 3 + ['salmon'] * 2 
bars = ax.bar(x_pos, values, color=colors, edgecolor='black')

# 2. Die Trennlinie zeichnen (Das "--" das du wolltest)
# Position 2.5 liegt genau zwischen dem 3. und 4. Balken (Index 2 und 3)
ax.axvline(x=2.5, color='gray', linestyle='--', linewidth=2, label='Parameter-Sprung')

# 3. Achsen beschriften
# Wichtig: Die X-Ticks müssen deine echten Werte (0, 2, 4...) anzeigen, nicht 0,1,2...
ax.set_xticks(x_pos)
ax.set_xticklabels(labels)

ax.set_ylabel('Messwert')
ax.set_title('Messungen mit logischer Trennung')
ax.grid(axis='y', alpha=0.3)

# Optional: Text zur Erklärung der Lücke
ax.text(2.5, max(values) * 0.9, '  Lücke  ', rotation=90, va='center', color='gray', fontsize=10)

plt.tight_layout()
plt.show()