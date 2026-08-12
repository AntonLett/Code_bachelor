import matplotlib
matplotlib.use("pgf")
import matplotlib.pyplot as plt
import numpy as np
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

if __name__ == "__main__":
    filesFolder = "testing/results/"
    files = ["300k_flat", "300k_flat_filled", "300k_flat_filled_more"]
    names = ["0B", "2kB", "4kB"]
    outputName = "fileSize.pgf"
    content = []
    means = []
    stddevs = []
    for f in files:
        jobj = read_json_file(op.join(filesFolder, f))
        content += jobj["results"]
        means.append(jobj["results"][0]["mean"])
        stddevs.append(jobj["results"][0]["stddev"])

    # Plot
    fig, ax = plt.subplots(figsize=(6, 4))
    ax.bar(names, means, yerr=stddevs, capsize=5, alpha=0.8, color=["chocolate", "teal"])

    ax.set_ylabel('Ausf["u]hrungszeit (ms)')
    ax.set_title('Vergleich unterschiedliche Größen')
    ax.grid(axis='y', alpha=0.3)

    plt.savefig(outputName)