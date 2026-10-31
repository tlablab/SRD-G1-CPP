#include <cassert>

#include "core/framebuffer.hpp"
#include "raster/line_raster.hpp"

static void assertGrayscale(const Pixel& pixel, int expected) {
    assert(pixel.r == expected);
    assert(pixel.g == expected);
    assert(pixel.b == expected);
}

int main() {
    Framebuffer framebuffer;

    // Horizontal line
    drawLine(framebuffer, Point{0, 0}, Point{3, 0});

    assertGrayscale(framebuffer.read(0, 0), 255);
    assertGrayscale(framebuffer.read(1, 0), 255);
    assertGrayscale(framebuffer.read(2, 0), 255);
    assertGrayscale(framebuffer.read(3, 0), 255);
    assertGrayscale(framebuffer.read(4, 0), 0);

    // Vertical line
    framebuffer.clear();
    drawLine(framebuffer, Point{3, 0}, Point{3, 3});

    assertGrayscale(framebuffer.read(3, 0), 255);
    assertGrayscale(framebuffer.read(3, 1), 255);
    assertGrayscale(framebuffer.read(3, 2), 255);
    assertGrayscale(framebuffer.read(3, 3), 255);

    // Negative-slope diagonal
    framebuffer.clear();
    drawLine(framebuffer, Point{0, 3}, Point{3, 0});

    assertGrayscale(framebuffer.read(0, 3), 255);
    assertGrayscale(framebuffer.read(1, 2), 255);
    assertGrayscale(framebuffer.read(2, 1), 255);
    assertGrayscale(framebuffer.read(3, 0), 255);

    return 0;
}
