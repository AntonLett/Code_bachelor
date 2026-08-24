@echo off
hyperfine --warmup 100 --runs 500 --export-json testing\results\300k_flat_huge.json ".\executables\Release\typefinderV4.exe -s .\settings\settings.toml -t D:\bachelor_tests_flat_huge"
timeout /t 5
hyperfine --warmup 100 --runs 500 --export-json testing\results\300k_even_more.json ".\executables\Release\typefinderV4.exe -s .\settings\settings.toml -t D:\bachelor_tests_flat_filled_evenmore"
timeout /t 5
hyperfine --warmup 100 --runs 500 --export-json testing\results\300k_more_v2.json ".\executables\Release\typefinderV4.exe -s .\settings\settings.toml -t D:\bachelor_tests_flat_filled_more"
timeout /t 5
hyperfine --warmup 100 --runs 500 --export-json testing\results\300k_filled.json ".\executables\Release\typefinderV4.exe -s .\settings\settings.toml -t D:\bachelor_tests_flat_filled"
timeout /t 5
hyperfine --warmup 100 --runs 500 --export-json testing\results\300k_flat.json ".\executables\Release\typefinderV4.exe -s .\settings\settings.toml -t D:\bachelor_tests_flat"
timeout /t 5
hyperfine --warmup 100 --runs 500 --export-json testing\results\300k_flat_huge.json ".\executables\Release\typefinderV4.exe -s .\settings\settings.toml -t D:\bachelor_tests_flat_huge"
timeout /t 5
hyperfine --warmup 100 --runs 500 --export-json testing\results\300k_even_more.json ".\executables\Release\typefinderV4.exe -s .\settings\settings.toml -t D:\bachelor_tests_flat_filled_evenmore"