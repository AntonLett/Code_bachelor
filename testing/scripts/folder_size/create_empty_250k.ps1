$ordner = "."

New-Item -ItemType Directory -Path $ordner -Force | Out-Null

1..250000 | ForEach-Object {
    New-Item -ItemType File -Path "$ordner\datei_$_.txt" -Force | Out-Null
}

Write-Host "250.000 leere Textdateien wurden erstellt."