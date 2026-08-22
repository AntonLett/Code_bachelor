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
    targetFolder = "performance_comparison/"   #### MODIFY 
    outputName = "updater_aenderungsgrad.pgf"
    x_label = ""
    y_label = 'Ausf"uhrdauer (s)'
    title = 'Auswirkung des "Anderungsgard'


    folder_path = Path("testing/results/" + targetFolder)
    files = ["res_new_0.json", "res_new_25.json", "res_new_50.json", "res_new_75.json", "res_new_100.json", "res_new_crawler.json"]
    names = ["0\%", "25\%", "50\%", "75\%", "100\%", "Crawler"]
    content = []
    means = []
    stddevs = []
    for f in files:
        jobj = read_json_file(op.join(folder_path, f))
        content += jobj["results"]
        means.append(jobj["results"][0]["mean"])
        stddevs.append(jobj["results"][0]["stddev"])

    # Plot
    fig, ax = plt.subplots(figsize=(6, 4))
    ax.bar(names, means, yerr=stddevs, capsize=5, alpha=0.8, color=["teal", "teal", "teal", "teal", "chocolate"])
    # ax.axvline(x=2.5, color='gray', linestyle='--', linewidth=2, label='Größen-Sprung')
    ax.axvline(x=4.5, color='gray', linestyle='--', linewidth=2, label='Größen-Sprung')


    if x_label != "":
        ax.set_xlabel(x_label)
    ax.set_ylabel(y_label)
    ax.set_title(title)
    ax.grid(axis='y', alpha=0.3)

    plt.savefig(outputName)
    print("Wrote file")