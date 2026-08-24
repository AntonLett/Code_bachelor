cmake_minimum_required(VERSION 3.24)
project(Metadata_Extractor VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Threads ist Standard in CMake, kein vcpkg nötig
set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)


# =============================================================================
# libCZI (NICHT in vcpkg verfügbar -> bleibt FetchContent)
# =============================================================================
include(FetchContent)
set(LIBCZI_BUILD_UNITTESTS OFF CACHE BOOL "" FORCE)
FetchContent_Declare(
    libCZI
    GIT_REPOSITORY https://github.com/ZEISS/libczi.git
    GIT_TAG        main
)
FetchContent_MakeAvailable(libCZI)
set(LIBCZI_INCLUDE_DIR ${libCZI_SOURCE_DIR}/Src/libCZI)


# =============================================================================
# Abhängigkeiten via vcpkg (find_package)
# =============================================================================
# vcpkg stellt diese Pakete bereit, sobald die Toolchain geladen ist.

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/executables)

find_package(benchmark CONFIG REQUIRED)
find_package(argparse CONFIG REQUIRED)
find_package(ZLIB REQUIRED)
find_package(Boost REQUIRED COMPONENTS asio beast system algorithm)
find_package(exiv2 CONFIG REQUIRED)
find_package(simdjson CONFIG REQUIRED)

# =============================================================================
# Executable: crawler
# =============================================================================
add_executable(crawler
    crawler.cpp
)
target_link_libraries(crawler PRIVATE
    Threads::Threads
    libCZI
    Boost::asio
    argparse::argparse
    Exiv2::exiv2lib
    benchmark::benchmark
)

target_include_directories(crawler PRIVATE ${libCZI_SOURCE_DIR})

# =============================================================================
# Executable: server
# =============================================================================
# HINWEIS: Bei vcpkg simdjson muss man die .cpp Datei meist NICHT mehr kompilieren.
# Man linkt einfach gegen die Library.
set(SERVER_SOURCES
    server.cpp
)

# ExifTool nur einbinden, wenn das Betriebssystem NICHT Windows ist
if(NOT WIN32)
    list(APPEND SERVER_SOURCES
        includes/exiftool/src/ExifTool.cpp
        includes/exiftool/src/ExifToolPipe.cpp
        includes/exiftool/src/TagInfo.cpp
    )
endif()

add_executable(server ${SERVER_SOURCES})

if(NOT WIN32)
    target_include_directories(server PUBLIC
        includes/exiftool/inc
        )
        endif()
        
        target_link_libraries(server PRIVATE
        Threads::Threads
        libCZI
        Boost::asio
        Boost::beast
        Boost::system
        Exiv2::exiv2lib
        argparse::argparse
        benchmark::benchmark
        simdjson::simdjson
        )
        target_include_directories(server PRIVATE ${simdjson_INCLUDE_DIR} ${libCZI_SOURCE_DIR})

# =============================================================================
# Executable: Updater
# =============================================================================
add_executable(updater
    updater.cpp
)
target_link_libraries(updater PRIVATE
    Boost::asio
    Boost::beast
    Boost::system
    argparse::argparse
    benchmark::benchmark
)

# =============================================================================
# Executable: Benchmark Crawler
# =============================================================================
add_executable(benchmark_crawler 
    crawler_benchmark.cpp
)
target_link_libraries(benchmark_crawler PRIVATE
    benchmark::benchmark
)
target_include_directories(benchmark_crawler PRIVATE ${CMAKE_SOURCE_DIR})

target_include_directories(crawler PRIVATE ${libCZI_SOURCE_DIR})
# =============================================================================
# Executable: Benchmark updater
# =============================================================================
add_executable(updater_benchmark
    updater_benchmark.cpp
)
target_link_libraries(updater_benchmark PRIVATE
    benchmark::benchmark
)
target_include_directories(updater_benchmark PRIVATE ${CMAKE_SOURCE_DIR})
