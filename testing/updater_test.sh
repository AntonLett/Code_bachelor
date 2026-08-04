#!/bin/bash

max=20
executable="../executables/updater"
input1="/gpfs/scic/personal/lettowh/laurent.list.FileStatistics"
input2="/gpfs/scic/personal/lettowh/schuman.list.FileStatistics"

file1=$(awk -F/ '{print $(NF)}' <<< "$input1")
file2=$(awk -F/ '{print $(NF)}' <<< "$input2")
results="results_laurent_schuman.txt"

echo "=== Sorting Files ===" > $results
echo "--- File 1: $input1 --- " >> $results
time LC_ALL=C sort -k$(head -1 "$input1" | awk '{print NF}') -o "$input1" "$input1" 2>> $results 

echo "--- File 2: $input2 --- " >> $results
time LC_ALL=C sort -k$(head -1 "$input2" | awk '{print NF}') -o "$input2" "$input2" 2>> $results 

echo "=== Performance Test: $max Durchläufe ===" >> $results
echo "Programm: $executable" >> $results
echo "Start: $(date)" >> $results
echo "" >> $results

for i in $(seq $max); do
    echo "--- Durchlauf $i ---" >> $results
    { time $executable $input1 $input2; } 2>> $results
    echo "" >> $results
    
    # Optional: Cache zwischen Durchläufen leeren (nur mit sudo!)
    # sync && sudo sh -c 'echo 3 > /proc/sys/vm/drop_caches'
done

echo "Ende: $(date)" >> $results

# Statistik berechnen (wenn alle Durchläufe fertig sind)
echo "" >> $results
echo "=== Zusammenfassung ===" >> $results
grep "real" $results | awk '{print $2}' | awk '
    BEGIN { min=999999; max=0; sum=0; count=0 }
    {
        # Konvertiere mm:ss.ss Format zu Sekunden
        split($1, t, ":")
        sec = t[1]*60 + t[2]
        sum += sec
        count++
        if(sec < min) min = sec
        if(sec > max) max = sec
    }
    END {
        printf "Durchläufe: %d\n", count
        printf "Durchschnitt: %.3f Sekunden\n", sum/count
        printf "Minimum: %.3f Sekunden\n", min
        printf "Maximum: %.3f Sekunden\n", max
        printf "Schwankung: %.3f Sekunden\n", max-min
    }
' >> $results

echo "Fertig! Ergebnisse in $results"