#include <iostream>

#include "core/framebuffer.hpp"
#include "core/triangle.hpp"
#include "core/point.hpp"
#include "io/ppm_writer.hpp"
#include "raster/triangle_raster.hpp"

int main() {
    Framebuffer framebuffer;
    
    Point p1{12, 15};
    Point p2{40, 10};
    Point p3{30, 30};

    Triangle tri{p1, p2, p3};
    
    drawTriangle(framebuffer, setupTriangle(tri));

    if (!writePpm(framebuffer, "frame.ppm")) {
        std::cerr << "Could not write frame.ppm\n";
        return 1;
    }

    return 0;
}