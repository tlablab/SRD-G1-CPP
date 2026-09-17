#include <cassert>

#include "core/framebuffer.hpp"
#include "raster/line_raster.hpp"

int main() {
    Framebuffer framebuffer;

    // Horizontal line: includes both endpoints
    drawLine(framebuffer, Point{0, 0}, Point{3, 0});

    assert(framebuffer.read(0, 0).on);
    assert(framebuffer.read(1, 0).on);
    assert(framebuffer.read(2, 0).on);
    assert(framebuffer.read(3, 0).on);
    assert(!framebuffer.read(4, 0).on);

    // Vertical line
    framebuffer.clear();
    drawLine(framebuffer, Point{3, 0}, Point{3, 3});

    assert(framebuffer.read(3, 0).on);
    assert(framebuffer.read(3, 1).on);
    assert(framebuffer.read(3, 2).on);
    assert(framebuffer.read(3, 3).on);

    // Negative-slope diagonal
    framebuffer.clear();
    drawLine(framebuffer, Point{0, 3}, Point{3, 0});

    assert(framebuffer.read(0, 3).on);
    assert(framebuffer.read(1, 2).on);
    assert(framebuffer.read(2, 1).on);
    assert(framebuffer.read(3, 0).on);

    return 0;
}