#!/usr/bin/env python3
"""
Extrahiert Dateierweiterungen aus einer großen Metadaten-Datei
und zählt deren Häufigkeit.

Format der Eingabedatei:
mtime,ctime,atime,size -- path
"""

import os
from collections import Counter
from pathlib import Path
import sys

def extract_extension(filepath):
    """
    Extrahiert die Dateierweiterung aus einem Pfad.
    Gibt '.' + extension zurück (z.B. '.png') oder 'no_extension' wenn keine vorhanden.
    """
    _, ext = os.path.splitext(filepath)
    if ext:
        return ext.lower()  # Kleinbuchstaben für Konsistenz
    else:
        return 'no_extension'

def process_large_file(input_file, output_file=None, show_progress=True):
    """
    Verarbeitet eine große Datei zeilenweise und zählt Dateierweiterungen.
    """
    extension_counter = Counter()
    line_count = 0
    error_count = 0
    
    print(f"Verarbeite Datei: {input_file}")
    print("Dies kann einige Minuten dauern...")
    print("-" * 25)
    
    try:
        with open(input_file, 'r', encoding='utf-8', errors='ignore') as f:
            for line in f:
                line_count += 1
                
                # Fortschrittsanzeige alle 1 Million Zeilen
                if show_progress and line_count % 1_000_000 == 0:
                    print(f"  ... {line_count // 1_000_000}M Zeilen verarbeitet, "
                          f"{len(extension_counter)} einzigartige Extensions")
                
                try:
                    line = line.strip()
                    if not line:
                        continue
                    
                    # Format: mtime,ctime,atime,size -- path
                    if ' -- ' in line:
                        parts = line.split(' -- ', 1)
                        if len(parts) == 2:
                            path = parts[1].strip()
                            ext = extract_extension(path)
                            extension_counter[ext] += 1
                        else:
                            error_count += 1
                    else:
                        error_count += 1
                        
                except Exception as e:
                    error_count += 1
                    continue
                    
    except FileNotFoundError:
        print(f"Fehler: Datei '{input_file}' nicht gefunden!")
        sys.exit(1)
    except Exception as e:
        print(f"Fehler beim Lesen der Datei: {e}")
        sys.exit(1)
    
    # Zusammenfassung
    print("-" * 25)
    print(f"Verarbeitung abgeschlossen!")
    print(f"  Gesamtzeilen: {line_count:,}")
    print(f"  Fehlerhafte Zeilen: {error_count:,}")
    print(f"  Einzigartige Extensions: {len(extension_counter)}")
    print(f"  Gesamte Dateien mit Extension: {sum(extension_counter.values()):,}")
    print("-" * 25)
    
    # Sortieren nach Häufigkeit
    sorted_extensions = sorted(extension_counter.items(), key=lambda x: x[1], reverse=True)
    
    # Output schreiben
    if output_file:
        with open(output_file, 'w', encoding='utf-8') as f:
            f.write("Dateiendungen und ihre Häufigkeit:\n")
            for ext, count in sorted_extensions:
                f.write(f"{ext}: {count}\n")
        print(f"Ergebnisse geschrieben nach: {output_file}")
    else:
        # Top 20 im Terminal anzeigen
        print("\nTop 20 Dateierweiterungen:")
        for i, (ext, count) in enumerate(sorted_extensions[:20], 1):
            percentage = (count / sum(extension_counter.values()) * 100) if sum(extension_counter.values()) > 0 else 0
            print(f"{i:3}. {ext:15} {count:12,} ({percentage:5.2f}%)")
    
    return sorted_extensions

if __name__ == "__main__":
    # Konfiguration
    INPUT_FILE = "/gpfs/scic/personal/kretschmerf/laurent.list.FileStatistics"      # Pfad zu der Datei
    OUTPUT_FILE = "/gpfs/scic/personal/kretschmerf/laurent_filetypes"        # Output-Datei für die Extension-Statistik
    
    # Verarbeitung starten
    results = process_large_file(INPUT_FILE, OUTPUT_FILE)
    
    print("\nFertig!")
