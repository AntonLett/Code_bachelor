import statistics

dateiname = 'testing/results/25_perc_server/extraction_times_1.txt'  # Hier den Namen deiner Datei eintragen

werte = []

try:
    with open(dateiname, 'r', encoding='utf-8') as f:
        for zeile in f:
            zeile = zeile.strip()
            if zeile:  # Leere Zeilen überspringen
                werte.append(float(zeile))

    if not werte:
        print("Keine Zahlenwerte in der Datei gefunden.")
    else:
        mittelwert = statistics.mean(werte)
        # standardabweichung (Stichprobe)
        std_abw = statistics.stdev(werte) 
        
        # Falls du die Standardabweichung der Grundgesamtheit willst, nutze:
        # std_abw = statistics.pstdev(werte)

        print(f"Anzahl Werte: {len(werte)}")
        print(f"Mittelwert:   {mittelwert}")
        print(f"Std-Abweichung: {std_abw}")

except FileNotFoundError:
    print(f"Die Datei '{dateiname}' wurde nicht gefunden.")
except ValueError:
    print("Fehler: Die Datei enthält Werte, die keine Zahlen sind.")