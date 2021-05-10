cmake_minimum_required(VERSION 3.16)

project(minecraft-in-a-week VERSION 0.0.0
    LANGUAGES C CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS 1)

add_library(glad
    3rd_party/glad-gl4.6-core/src/glad.c)
target_compile_features(glad PRIVATE cxx_std_20)
target_include_directories(glad
    PRIVATE
    3rd_party/glad-gl4.6-core/include)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/bin)

add_compile_options(
        -Wall -Wextra -Wshadow -Wnon-virtual-dtor -Wcast-align -Wunused
        -Woverloaded-virtual -Wpedantic -Wconversion -Wsign-conversion
        -Wmisleading-indentation -Wnull-dereference -Wdouble-promotion -Wformat=2
        -Wduplicated-cond -Wduplicated-branches -Wlogical-op -Wuseless-cast)

add_subdirectory(src)
