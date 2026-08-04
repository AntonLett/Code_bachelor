# Generated using AI
# 30.07.2026
# in Chat "Integrating libCZI with CMake"
cmake_minimum_required(VERSION 3.14)
project(TypefinderProject VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_CXX_FLAGS_RELEASE "-O3")
set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/executables)

# =============================================================================
# libczi einbinden
# =============================================================================
include(FetchContent)

FetchContent_Declare(
    libCZI  # Name konsistent halten
    GIT_REPOSITORY https://github.com/ZEISS/libczi.git
    GIT_TAG        main
)
FetchContent_MakeAvailable(libCZI)

# =============================================================================
# Boost einbinden
# Quelle: https://github.com/boostorg/cmake/blob/develop/README.md
# =============================================================================
FetchContent_Declare(
  Boost
  URL https://github.com/boostorg/boost/releases/download/boost-1.84.0/boost-1.84.0.tar.xz
  URL_MD5 893b5203b862eb9bbd08553e24ff146a
  DOWNLOAD_EXTRACT_TIMESTAMP ON
)
set(BOOST_INCLUDE_LIBRARIES algorithm asio beast)
FetchContent_MakeAvailable(Boost)

# =============================================================================
# Executable: typefinderV4
# =============================================================================
add_executable(typefinderV4
    crawler.cpp
)
target_link_libraries(typefinderV4 PUBLIC
    Threads::Threads
    libCZI
    Boost::asio
)
target_compile_options(typefinderV4 PRIVATE -O3)

# =============================================================================
# Executable: server
# =============================================================================
add_executable(server
    server.cpp
    includes/exiftool/src/ExifTool.cpp
    includes/exiftool/src/ExifToolPipe.cpp
    includes/exiftool/src/TagInfo.cpp
    includes/code/simdjson.cpp
)
target_include_directories(server PUBLIC
    includes/exiftool/inc
)
target_link_libraries(server PUBLIC
    Threads::Threads
    libCZI
    Boost::asio
    Boost::beast
    Boost::system
)
target_compile_options(server PRIVATE -O3)

# =============================================================================
# Executable: Updater
# =============================================================================
add_executable(updater
    list_compare.cpp
)
target_compile_options(updater PRIVATE -O3)