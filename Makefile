WARMUP ?= 3
RUNS ?= 10
PERCENT ?= 20
COMMAND ?= find . -type f -printf '%T@;%C@;%A@;%s;%p\n' | LC_ALL=C sort >> mnew.txt
RESULTS_FOLDER = tests\results\
RESULTS_FILE = result_windows.json


typefinderV4:
	g++ -o executables/typefinderV4 crawler.cpp -pthread -O3

server:
	g++ -o executables/server \
	server.cpp \
	includes/code/simdjson.cpp \
	includes/exiftool/src/ExifTool.cpp \
	includes/exiftool/src/ExifToolPipe.cpp \
	includes/exiftool/src/TagInfo.cpp \
	-Iincludes/exiftool/inc

test_updater:
	hyperfine \
	--warmup $(WARMUP) \
	--runs $(RUNS) \
	--export-json ergebnisse_$(PERCENT)_perc.json \
	'executables/updater /gpfs/scic/personal/lettowh/laurent_test_og /gpfs/scic/personal/lettowh/laurent_$(PERCENT)_perc'


test_crawler:
	hyperfine \
	--warmup 1 \
	--runs 5 \
	--export-json results_windows \
	--prepare 'curl.exe -s -X DELETE -k http://localhost:9200/clean_test > /dev/null; sleep 1; echo "Index deleted: $?"' \
	'./executables/typefinderV4 -s ./settings/settings.toml -t ..' \
	--cleanup 'curl.exe -s -X DELETE -k http://localhost:9200/clean_test > /dev/null' 

test_windows_crawler:
	hyperfine --warmup 1 --runs 5 --export-json $(RESULTS_FOLDER)$(RESULTS_FILE) --prepare "curl.exe -s -X DELETE -k http://localhost:9200/clean_test" ".\executables\Release\typefinderV4.exe -s ./settings/settings.toml -t .." --cleanup "curl.exe -s -X DELETE -k http://localhost:9200/clean_test"

test_windows_updater:
	hyperfine --warmup 1 --runs 5 --export-json .\testing\results\results_windows_d_updater.json --prepare "curl.exe -s -X DELETE -k http://localhost:9200/clean_test" ".\executables\Release\typefinderV4.exe -s ./settings/settings.toml -t D:\ && .\executables\Release\updater -s .\settings\settings.toml -o .\testing\dateiListe_d_og_sorted.txt" --cleanup "curl.exe -s -X DELETE -k http://localhost:9200/clean_test"

	hyperfine --warmup 1 --runs 5 --export-json .\testing\results\results_windows_d_crawler2.json --prepare "curl.exe -X PUT -k http://localhost:9200/clean_test" ".\executables\Release\typefinderV4.exe -s ./settings/settings.toml -t D:\" --cleanup "curl.exe -s -X DELETE -k http://localhost:9200/clean_test"
	hyperfine --warmup 1 --runs 5 --export-json .\testing\results\results_windows_d_updater_100_perc.json --prepare "curl.exe -X PUT -k http://localhost:9200/clean_test" ".\executables\Release\typefinderV4.exe -s ./settings/settings.toml -t D:\ && .\executables\Release\updater -s .\settings\settings.toml -o .\testing\dateiListe_d_og_sorted.txt -c .\testing\dateiListe_d_100_perc.txt" --cleanup "curl.exe -s -X DELETE -k http://localhost:9200/clean_test"