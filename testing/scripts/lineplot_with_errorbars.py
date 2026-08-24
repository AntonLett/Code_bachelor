# KI Generiert
import matplotlib
matplotlib.use("pgf")
import matplotlib.pyplot as plt
import numpy as np
from pathlib import Path
import json
import os.path as op

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

targetFolder = "updater_foldersize/"   #### MODIFY 
outputName = "updater_foldersize.pgf"

folder_path = Path("testing/results/" + targetFolder)

filename = "all.json"
content = []
means = []
stddevs = []
jobj = read_json_file(op.join(folder_path, filename))
for r in jobj["results"]:
    means.append(r["mean"])
    stddevs.append(r["stddev"])

# Beispieldaten (ersetze dies mit deinen echten Daten)
groessen = ["50k", "100k", "150k", "200k", "250k", "300k", "350k"]  # X-Achse

plt.figure(figsize=(6, 4))

# Plot mit Fehlerbalken (capsize macht die kleinen Querstriche an den Balken)
plt.errorbar(groessen, means, yerr=stddevs, 
             marker='o', linestyle='-', capsize=5, 
             label='Durchschnittliche Ausführdauer', 
             color="teal", ecolor='chocolate')

plt.xlabel('Datengröße (Anzahl Datensätze)')
plt.ylabel('Ausf"uhrdauer (s)')
plt.title('Skalierbarkeit des Updaters')
plt.grid(True, linestyle='--', alpha=0.5)
plt.legend()

# Wichtig: Speichern in hoher Auflösung für die Arbeit
plt.savefig(outputName)
plt.show()