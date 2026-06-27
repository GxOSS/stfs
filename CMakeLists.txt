cmake_minimum_required(VERSION 3.24)
project(stfs CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_library(stfs
    src/FileTableParser.cpp
    src/HeaderParser.cpp
    src/MetadataParser.cpp
    src/BlockParser.cpp
    src/FileExtractor.cpp
    src/HashVerifier.cpp
    src/Package.cpp
)

target_include_directories(stfs PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_include_directories(stfs PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)