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