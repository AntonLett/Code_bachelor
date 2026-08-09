# Generated using AI
# 30.07.2026
# in Chat "Integrating libCZI with CMake"
cmake_minimum_required(VERSION 3.14)
project(TypefinderProject VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/executables)

# =============================================================================
# argsparse einbinden
# =============================================================================
include(FetchContent)
FetchContent_Declare(
    argparse
    GIT_REPOSITORY https://github.com/p-ranav/argparse.git
)
FetchContent_MakeAvailable(argparse)

# =============================================================================
# zlib einbinden
# =============================================================================

FetchContent_Declare(
    zlib
    GIT_REPOSITORY https://github.com/madler/zlib.git
    GIT_TAG        v1.3.1
)
FetchContent_MakeAvailable(zlib)

set(ZLIB_ROOT ${zlib_SOURCE_DIR})
set(ZLIB_INCLUDE_DIR ${zlib_SOURCE_DIR})
set(ZLIB_LIBRARY ${zlib_BINARY_DIR}/zlibstatic.lib)

# =============================================================================
# libczi einbinden
# =============================================================================
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
# Exiv2 einbinden
# =============================================================================
find_package(exiv2 CONFIG REQUIRED)

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
    argparse
)
target_compile_options(typefinderV4 PRIVATE)

# =============================================================================
# Executable: server
# =============================================================================
add_executable(server
    server.cpp
    includes/code/simdjson.cpp
)
target_link_libraries(server PUBLIC
    Threads::Threads
    libCZI
    Boost::asio
    Boost::beast
    Boost::system
    Exiv2::exiv2lib
    argparse
)
target_compile_options(server PRIVATE)

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
    argparse
)
target_compile_options(updater PRIVATE)