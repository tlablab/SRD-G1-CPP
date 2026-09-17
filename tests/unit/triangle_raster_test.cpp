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

    // two triangles (10-by-10 rectangle) which share the diagonal from
    // (20, 10) to (10, 20). Pixel (14, 15) has centre (14.5, 15.5)
    Triangle first{
        Point{10, 10},
        Point{20, 10},
        Point{10, 20}
    };

    Triangle second{
        Point{20, 10},
        Point{20, 20},
        Point{10, 20}
    };

    Framebuffer firstOnly;
    Framebuffer secondOnly;
    Framebuffer combined;

    drawTriangle(firstOnly, setupTriangle(first));
    drawTriangle(secondOnly, setupTriangle(second));
    drawTriangle(combined, setupTriangle(first));
    drawTriangle(combined, setupTriangle(second));

    assert(!firstOnly.read(14, 15).on);
    assert(secondOnly.read(14, 15).on);
    assert(combined.read(14, 15).on);

    for (int y = 10; y < 20; ++y) {
        for (int x = 10; x < 20; ++x) {
            assert(combined.read(x, y).on);
        }
    }

    return 0;
}
