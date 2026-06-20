cmake_minimum_required(VERSION 3.24)
project(stfs CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(MSVC)
    add_compile_options(/W4 /WX)
else()
    add_compile_options(-Wall -Wextra -Wpedantic -Werror)
endif()

add_library(stfs
    src/stfs.cpp
)

target_include_directories(stfs PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_compile_features(stfs PUBLIC cxx_std_23)