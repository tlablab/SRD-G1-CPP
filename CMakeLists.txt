cmake_minimum_required(VERSION 3.20)

project(srd_g1_renderer LANGUAGES CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

#---------------------------------------------------------------------------

add_executable(srd-render
    apps/srd-render/main.cpp
    src/core/framebuffer.cpp
    src/core/depthbuffer.cpp
    src/io/ppm_writer.cpp
    src/raster/line_raster.cpp
    src/raster/triangle_raster.cpp
    src/core/coveragebuffer.cpp
    src/core/multisamplebuffer.cpp
    src/resolve/resolve.cpp
)

target_include_directories(srd-render PRIVATE src)

#---------------------------Testing---------------------------------

enable_testing()

# Line raster tests

add_test(NAME line_raster COMMAND srd-tests)

add_executable(srd-tests
    tests/unit/line_raster_test.cpp
    src/core/framebuffer.cpp
    src/raster/line_raster.cpp
)

target_include_directories(srd-tests PRIVATE src)

# Triangle tests

add_test(NAME triangle_raster COMMAND triangle-tests)

add_executable(triangle-tests
    tests/unit/triangle_raster_test.cpp
    src/core/coveragebuffer.cpp
    src/core/multisamplebuffer.cpp
    src/raster/triangle_raster.cpp
)

target_include_directories(triangle-tests PRIVATE src)

# Coloured triangle samples

add_test(NAME triangle_colour COMMAND triangle-colour-tests)

add_executable(triangle-colour-tests
    tests/unit/triangle_colour_test.cpp
    src/core/coveragebuffer.cpp
    src/core/multisamplebuffer.cpp
    src/raster/triangle_raster.cpp
)

target_include_directories(triangle-colour-tests PRIVATE src)

# Sample mask tests

add_test(NAME sample_mask COMMAND sample-mask-tests)

add_executable(sample-mask-tests
    tests/unit/sample_mask_test.cpp
)

target_include_directories(sample-mask-tests PRIVATE src)

# Sample pattern tests

add_test(NAME sample_pattern COMMAND sample-pattern-tests)

add_executable(sample-pattern-tests
    tests/unit/sample_pattern_test.cpp
)

target_include_directories(sample-pattern-tests PRIVATE src)

# Coverage buffer tests

add_test(NAME coverage_buffer COMMAND coverage-buffer-tests)

add_executable(coverage-buffer-tests
    tests/unit/coveragebuffer_test.cpp
    src/core/coveragebuffer.cpp
)

target_include_directories(coverage-buffer-tests PRIVATE src)

# Multisample colour buffer tests

add_executable(multisample-buffer-tests
    tests/unit/multisamplebuffer_test.cpp
    src/core/multisamplebuffer.cpp
)

target_include_directories(multisample-buffer-tests PRIVATE src)

add_test(NAME multisample_buffer COMMAND multisample-buffer-tests)

# Resolve tests

add_executable(resolve-tests
    tests/unit/resolve_test.cpp
    src/core/coveragebuffer.cpp
    src/core/multisamplebuffer.cpp
    src/core/framebuffer.cpp
    src/resolve/resolve.cpp
)

target_include_directories(resolve-tests PRIVATE src)

add_test(NAME resolve COMMAND resolve-tests)

# RGB multisample resolve tests

add_executable(rgb-resolve-tests
    tests/unit/rgb_resolve_test.cpp
    src/core/coveragebuffer.cpp
    src/core/multisamplebuffer.cpp
    src/core/framebuffer.cpp
    src/resolve/resolve.cpp
)

target_include_directories(rgb-resolve-tests PRIVATE src)

add_test(NAME rgb_resolve COMMAND rgb-resolve-tests)


#---------------------------------------------------------------------------

if(MSVC)
    target_compile_options(srd-render PRIVATE /W4 /permissive-)
else()
    target_compile_options(srd-render PRIVATE -Wall -Wextra -Wpedantic)
endif()
