#include <iostream>
#include <cassert>

#include "core/coveragebuffer.hpp"
#include "core/framebuffer.hpp"
#include "core/triangle.hpp"
#include "core/point.hpp"
#include "io/ppm_writer.hpp"
#include "raster/triangle_raster.hpp"
#include "sampling/sample_pattern.hpp"
#include "resolve/resolve.hpp"


int main() {
    CoverageBuffer buffer;
    Framebuffer framebuffer;

    Point p1{12, 15};
    Point p2{40, 10};
    Point p3{30, 30};

    Triangle tri{p1, p2, p3};

    SamplePattern pattern;
    pattern = makeFourSamplePattern();

    drawTriangle(buffer, setupTriangle(tri), pattern);
    resolve(buffer, framebuffer, pattern.count);
    if (!writePpm(framebuffer, "frame.ppm")) {
        std::cerr << "Could not write frame.ppm\n";
        return 1;
    }

    return 0;
}
