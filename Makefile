WARMUP ?= 3
RUNS ?= 10
PERCENT ?= 20

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

find . -type f -printf '%T@;%C@;%A@;%s;%p\n' | LC_ALL=C sort >> mnew.txt

test_crawler:
	hyperfine \
	--warmup 1 \
	--runs 5 \
	--export-json ergebnisse_crawler_local \
	'./executables/typefinderV4 -s ./settings/settings.toml -t ..' \
	--cleanup 'curl -X GET -k http://localhost:9200/clean_test/_count && curl -s -X DELETE -k http://localhost:9200/clean_test > /dev/null'