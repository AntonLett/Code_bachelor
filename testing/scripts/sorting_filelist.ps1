Get-Content ..\dateiListe_d_og.txt |
ForEach-Object {
    [PSCustomObject]@{
        Line = $_
        Path = ($_ -split ' -- ')[1]
    }
} |
Sort-Object -Property Path |
Select-Object -ExpandProperty Line |
Set-Content ..\dateiListe_d_og_sorted.txt