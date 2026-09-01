cmake_minimum_required(VERSION 3.28)
project(lob CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_library(lob_warnings INTERFACE)
target_compile_options(lob_warnings INTERFACE
    -Wall
    -Wextra
    -Wpedantic
)

add_executable(lob_main src/main.cc)
target_link_libraries(lob_main PRIVATE lob_warnings)
