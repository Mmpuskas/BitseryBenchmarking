cmake_minimum_required(VERSION 3.13)

project(BitseryBenchmarks)

add_executable(BitseryBenchmarks
    Src/main.cpp
)

target_include_directories(BitseryBenchmarks
    PRIVATE
        Src
)

target_link_libraries(BitseryBenchmarks
    PRIVATE
        -static-libgcc
        -static-libstdc++
)

# Compile with C++17
target_compile_features(BitseryBenchmarks PRIVATE cxx_std_17)
set_target_properties(BitseryBenchmarks PROPERTIES CXX_EXTENSIONS OFF)

# Enable compile warnings.
target_compile_options(BitseryBenchmarks PUBLIC -Wall -Wextra)
