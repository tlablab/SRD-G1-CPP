#include <cassert>

#include "core/framebuffer.hpp"
#include "raster/line_raster.hpp"

int main() {
    Framebuffer framebuffer;

    // Horizontal line: includes both endpoints
    drawLine(framebuffer, Point{0, 0}, Point{3, 0});

    assert(framebuffer.read(0, 0).intensity == 255);
    assert(framebuffer.read(1, 0).intensity == 255);
    assert(framebuffer.read(2, 0).intensity == 255);
    assert(framebuffer.read(3, 0).intensity == 255);
    assert(framebuffer.read(4, 0).intensity == 0);

    // Vertical line
    framebuffer.clear();
    drawLine(framebuffer, Point{3, 0}, Point{3, 3});

    assert(!(framebuffer.read(3, 0).intensity == 0));
    assert(!(framebuffer.read(3, 1).intensity == 0));
    assert(!(framebuffer.read(3, 2).intensity == 0));
    assert(!(framebuffer.read(3, 3).intensity == 0));

    // Negative-slope diagonal
    framebuffer.clear();
    drawLine(framebuffer, Point{0, 3}, Point{3, 0});

    assert(!(framebuffer.read(0, 3).intensity == 0));
    assert(!(framebuffer.read(1, 2).intensity == 0));
    assert(!(framebuffer.read(2, 1).intensity == 0));
    assert(!(framebuffer.read(3, 0).intensity == 0));

    return 0;
}
