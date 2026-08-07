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

WARMUP ?= 3
RUNS ?= 10
PERCENT ?= 20

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
    --export-json ergebnisse_crawler_lizard10x \
    --prepare 'curl -s -X DELETE -k http://localhost:9201/clean_test > /dev/null \
    './executables/typefinderV4 -s ./settings/settings.toml -t /gpfs/scic/data/projects/Lizard10x' \
    --cleanup 'curl -X GET -k 'https://localhost:9200/clean_test/_count'