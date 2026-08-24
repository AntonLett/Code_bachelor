$lines = Get-Content .\testing\lists\350k_list

$items = foreach ($line in $lines) {
    [PSCustomObject]@{
        Line = $line
        Path = ($line -split ' -- ', 2)[1]
    }
}

[Array]::Sort(
    $items,
    [System.Collections.Generic.Comparer[object]]::Create({
        param($a, $b)
        [StringComparer]::Ordinal.Compare($a.Path, $b.Path)
    })
)

$items.Line | Set-Content .\testing\lists\350k_list_sorted