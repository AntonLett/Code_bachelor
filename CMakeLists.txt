cmake_minimum_required(VERSION 3.24)
project(Metadata_Extractor VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Threads ist Standard in CMake, kein vcpkg nötig
set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/executables)

# =============================================================================
# libCZI (NICHT in vcpkg verfügbar -> bleibt FetchContent)
# =============================================================================
include(FetchContent)
FetchContent_Declare(
    libCZI
    GIT_REPOSITORY https://github.com/ZEISS/libczi.git
    GIT_TAG        main
)
FetchContent_MakeAvailable(libCZI)

# =============================================================================
# Abhängigkeiten via vcpkg (find_package)
# =============================================================================
# vcpkg stellt diese Pakete bereit, sobald die Toolchain geladen ist.
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
    benchmark::benchmark
)

# =============================================================================
# Executable: server
# =============================================================================
# HINWEIS: Bei vcpkg simdjson muss man die .cpp Datei meist NICHT mehr kompilieren.
# Man linkt einfach gegen die Library.
add_executable(server
    server.cpp
    # includes/code/simdjson.cpp  <-- ENTFERNEN, wenn vcpkg simdjson genutzt wird
)
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
target_include_directories(server PRIVATE ${simdjson_INCLUDE_DIR})

# =============================================================================
# Executable: Updater
# =============================================================================
add_executable(updater
    list_compare.cpp
)
target_link_libraries(updater PRIVATE
    Boost::asio
    Boost::beast
    Boost::system
    argparse::argparse
    benchmark::benchmark
)