hyperfine --warmup 1 --runs 15 --export-json .\testing\results\results_windows_d_updater_0_perc.json `
>> --prepare "curl.exe -s -X DELETE -k http://localhost:9200/clean_test" `
>> " .\executables\Release\updater -s .\settings\settings.toml -o .\testing\lists\dateiListe_d_og_sorted.txt -c .\testing\lists\dateiListe_d_0_perc.txt" --show-output
