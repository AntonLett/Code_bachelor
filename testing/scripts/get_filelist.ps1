$Pfad = "D:\bachelor_tests_foldersize\350k"
$Fehler_Log = ".\testing\fehler_log"

Get-ChildItem -Path $Pfad -Recurse -File -ErrorVariable AccessErrors -ErrorAction SilentlyContinue |
ForEach-Object{
    $mtime = $_.LastWriteTime.Ticks
    $btime = $_.CreationTime.Ticks
    $atime = $_.LastAccessTime.Ticks
    $size  = $_.Length
    $path  = $_.FullName

    Write-Output "$mtime;$btime;$atime;$size -- $path"
}

if ($AccessErrors){
    foreach($err in $AccessErrors){
        $msg = $err.Exception.Message
        $target = $err.TargetObject
        "Fehler: $msg | Pfad: $target" | Out-File -FilePath $Fehler_Log -Append -Encoding UTF8
    }
}