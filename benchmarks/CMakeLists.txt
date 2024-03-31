cmake_minimum_required(VERSION 3.14)
project(leftwrite_benchmarks)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_FLAGS "-std=c++2b -O3  -Wall -Wextra -march=native")

include(FetchContent)
FetchContent_Declare(
        nanobench
        GIT_REPOSITORY https://github.com/martinus/nanobench.git
        GIT_TAG master
)
FetchContent_MakeAvailable(nanobench)
get_filename_component(LEFTWRITE_DIR ${CMAKE_CURRENT_SOURCE_DIR} DIRECTORY)

add_executable(leftwrite_benchmarks bench.cpp)
target_include_directories(leftwrite_benchmarks PRIVATE ${LEFTWRITE_DIR})
target_link_libraries(leftwrite_benchmarks PRIVATE nanobench)
