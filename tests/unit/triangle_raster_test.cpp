#include <cassert>

#include "core/framebuffer.hpp"
#include "core/triangle.hpp"
#include "raster/triangle_raster.hpp"

int main() {
    Framebuffer framebuffer;

    Triangle triangle{
        Point{10, 10},
        Point{30, 10},
        Point{10, 30}
    };

    drawTriangle(framebuffer, setupTriangle(triangle));

    assert(framebuffer.read(11, 11).on);
    assert(framebuffer.read(15, 15).on);
    assert(!framebuffer.read(25, 25).on);
    assert(!framebuffer.read(31, 10).on);

    return 0;
}