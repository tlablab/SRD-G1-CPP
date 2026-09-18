cmake_minimum_required(VERSION 3.20)

project(srd_g1_renderer LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(srd-render
    apps/srd-render/main.cpp
    src/core/framebuffer.cpp
    src/io/ppm_writer.cpp
    src/raster/line_raster.cpp
    src/raster/triangle_raster.cpp
)

enable_testing()

add_executable(srd-tests
    tests/unit/line_raster_test.cpp
    src/core/framebuffer.cpp
    src/raster/line_raster.cpp
)

target_include_directories(srd-tests PRIVATE src)

add_test(NAME line_raster COMMAND srd-tests)

add_executable(triangle-tests
    tests/unit/triangle_raster_test.cpp
    src/core/framebuffer.cpp
    src/raster/triangle_raster.cpp
)

target_include_directories(triangle-tests PRIVATE src)

add_test(NAME triangle_raster COMMAND triangle-tests)

add_executable(sample-mask-tests
    tests/unit/sample_mask_test.cpp
)

target_include_directories(sample-mask-tests PRIVATE src)

add_test(NAME sample_mask COMMAND sample-mask-tests)

add_executable(sample-pattern-tests
    tests/unit/sample_pattern_test.cpp
)

target_include_directories(sample-pattern-tests PRIVATE src)

add_test(NAME sample_pattern COMMAND sample-pattern-tests)

target_include_directories(srd-render PRIVATE src)

if(MSVC)
    target_compile_options(srd-render PRIVATE /W4 /permissive-)
else()
    target_compile_options(srd-render PRIVATE -Wall -Wextra -Wpedantic)
endif()
