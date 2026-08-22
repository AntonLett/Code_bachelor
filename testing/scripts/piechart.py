import matplotlib
matplotlib.use("pgf")
import matplotlib.pyplot as plt
import os.path as op
from pathlib import Path

plt.rcParams.update({
    "pgf.texsystem": "pdflatex",
    'font.family': 'serif',
    'font.serif': ['Computer Modern Roman'],
    'text.usetex': False,
    'pgf.preamble': r'\usepackage[utf8]{inputenc}'
})


def read_data_from_file(filename):
    data = {}
    with open(filename, 'r', encoding='utf-8') as f:
        lines = f.readlines()
        
    # Erste Zeile überspringen (Beschreibung)
    for line in lines[1:]:
        line = line.strip()
        if ':' in line and line:
            key, value = line.split(':', 1)
            data[key.strip()] = int(value.strip())
    
    return data

if __name__ == "__main__":

    #######################################################
    # SETTINGS
    #######################################################
    targetFolder = "filetype_distribution/"
    outputName = "dateitypen_verteilung.pgf"
    input_file = "testing/laurent_filetypes.txt"  # Pfad anpassen
    TOP_N = 10
    title = 'Verteilung der Dateitypen'


    # 1. Daten einlesen
    data = read_data_from_file(input_file)

    # 2. Strategie: Top N behalten, Rest gruppieren
    sorted_data = sorted(data.items(), key=lambda x: x[1], reverse=True)

    top_items = sorted_data[:TOP_N]
    rest_items = sorted_data[TOP_N:]

    sum_rest = sum(value for key, value in rest_items)
    count_rest_types = len(rest_items)

    labels = [item[0] for item in top_items]
    sizes = [item[1] for item in top_items]

    # "Sonstige" Label OHNE \n - stattdessen Leerzeichen
    if sum_rest > 0:
        labels.append(f"Sonstige ({count_rest_types} Typen)")
        sizes.append(sum_rest)

    # 3. Plot erstellen
    fig, ax = plt.subplots(figsize=(6, 4))
    
    colors = plt.cm.Set3(range(len(labels)))
    
    wedges, texts, autotexts = ax.pie(sizes, labels=labels, autopct='%1.1f%%', 
                                       startangle=90, colors=colors, 
                                       pctdistance=0.85, textprops={'fontsize': 8})
    
    # Donut-Effekt
    centre_circle = plt.Circle((0, 0), 0.70, fc='white')
    ax.add_artist(centre_circle)

    ax.set_title(title, fontsize=10)
    ax.axis('equal')

    plt.savefig(outputName, bbox_inches='tight')
    print(f"Wrote file: {outputName}")

    # 4. Statistik
    print("=" * 50)
    print("STATISTIK")
    print("=" * 50)
    print(f"Gesamtanzahl Dateitypen: {len(data)}")
    print(f"Angezeigte Typen: {TOP_N}")
    print(f"Zusammengefasste Typen: {count_rest_types}")
    print(f"Gesamtanzahl Dateien: {sum(sizes):,}")
    if sum(sizes) > 0:
        print(f"Anteil 'Sonstige': {sum_rest:,} Dateien ({sum_rest/sum(sizes)*100:.1f}%)")
    print("=" * 50)