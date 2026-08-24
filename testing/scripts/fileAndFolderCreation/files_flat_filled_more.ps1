$ordner = "."

New-Item -ItemType Directory -Path $ordner -Force | Out-Null

$Content = "ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789" * 90

1..300000 | ForEach-Object {
    Set-Content -Path "$ordner\datei_$_.txt" -Value $Content -NoNewline
}

Write-Host "300.000 Textdateien mit Inhalt wurden erstellt."
