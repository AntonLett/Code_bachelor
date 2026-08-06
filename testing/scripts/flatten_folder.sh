#!/bin/bash

# --- KONFIGURATION ---
QUELLE="./deep"
ZIEL="./flat"
# ---------------------

# Zielordner erstellen
mkdir -p "$ZIEL"

echo "Starte Analyse..."
# Anzahl Dateien zählen (kann bei 2TB dauern)
ANZAHL=$(find "$QUELLE" -type f | wc -l)
echo "Gesamtanzahl Dateien: $ANZAHL"
echo "Starte Kopiervorgang..."

# Zähler für Fortschritt
COUNT=0

# Find nutzt -print0 für sichere Behandlung von Leerzeichen/Sonderzeichen im Namen
find "$QUELLE" -type f -print0 | while IFS= read -r -d '' file; do
    COUNT=$((COUNT + 1))
    
    # Fortschrittsanzeige alle 1000 Dateien
    if [ $((COUNT % 1000)) -eq 0 ]; then
        echo "Verarbeitet: $COUNT / $ANZAHL"
    fi

    filename=$(basename "$file")
    target="$ZIEL/$filename"

    # Prüfen auf Kollision
    if [ -e "$target" ]; then
        # Bei Kollision: Zeitstempel und Zufallszahl anhängen
        # Beispiel: bild.jpg -> bild_17156234_12345.jpg
        name="${filename%.*}"
        ext="${filename##*.}"
        
        # Falls keine Extension vorhanden (z.B. Makefile)
        if [ "$name" == "$ext" ]; then
            target="$ZIEL/${filename}_$(date +%s%N)_$RANDOM"
        else
            target="$ZIEL/${name}_$(date +%s%N)_$RANDOM.$ext"
        fi
    fi

    # Kopieren (cp ist schnell, --preserve=mode,ownership versuche ich zu vermeiden für Speed)
    cp "$file" "$target"
done

echo "Fertig!"