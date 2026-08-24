# Anzahl der gewünschten Dateien
$TargetFiles = 100000

# Hauptordner
$Root = "D:\bachelor_tests_subfolders"

# Zähler
$FileCount = 0
$FolderQueue = New-Object System.Collections.Generic.Queue[string]
$FolderQueue.Enqueue($Root)

# Hauptordner erstellen
New-Item -ItemType Directory -Path $Root -Force | Out-Null

while ($FolderQueue.Count -gt 0 -and $FileCount -lt $TargetFiles) {

    $CurrentFolder = $FolderQueue.Dequeue()

    # 2 leere Textdateien pro Ebene/Ordner
    for ($i = 1; $i -le 2 -and $FileCount -lt $TargetFiles; $i++) {
        $FileCount++
        $FilePath = Join-Path $CurrentFolder "Datei_$FileCount.txt"

        New-Item -ItemType File -Path $FilePath -Force | Out-Null
    }

    # 2 Unterordner erstellen
    if ($FileCount -lt $TargetFiles) {

        for ($i = 1; $i -le 2; $i++) {

            $NewFolder = Join-Path $CurrentFolder "Ordner_$i"

            New-Item -ItemType Directory -Path $NewFolder -Force | Out-Null

            # Unterordner für spätere Bearbeitung vormerken
            $FolderQueue.Enqueue($NewFolder)
        }
    }

    # Fortschritt anzeigen
    if ($FileCount % 1000 -eq 0) {
        Write-Host "Erstellt: $FileCount / $TargetFiles Dateien"
    }
}

Write-Host ""
Write-Host "Fertig!"
Write-Host "Erstellt: $FileCount Dateien"
Write-Host "Basisordner: $Root"