@echo off
hyperfine --warmup 1 --runs 15 --export-json testing\results\300k_more_v2.json ".\executables\Release\crawler.exe -s .\settings\settings.toml -t D:\bachelor_tests_flat_filled_more"
timeout /t 5
hyperfine --warmup 1 --runs 15 --export-json testing\results\300k_filled.json ".\executables\Release\crawler.exe -s .\settings\settings.toml -t D:\bachelor_tests_flat_filled"
timeout /t 5
hyperfine --warmup 1 --runs 15 --export-json testing\results\300k_flat.json ".\executables\Release\crawler.exe -s .\settings\settings.toml -t D:\bachelor_tests_flat"
timeout /t 5
hyperfine --warmup 1 --runs 15 --export-json testing\results\300k_flat_huge.json ".\executables\Release\crawler.exe -s .\settings\settings.toml -t D:\bachelor_tests_flat_huge"
timeout /t 5
hyperfine --warmup 1 --runs 15 --export-json testing\results\300k_even_more.json ".\executables\Release\crawler.exe -s .\settings\settings.toml -t D:\bachelor_tests_flat_filled_evenmore"