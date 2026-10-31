#include <cstdint>
#include <iostream>

#include "core/framebuffer.hpp"
#include "core/multisamplebuffer.hpp"
#include "core/pixel.hpp"
#include "core/triangle.hpp"
#include "core/point.hpp"
#include "io/ppm_writer.hpp"
#include "raster/triangle_raster.hpp"
#include "sampling/sample_pattern.hpp"
#include "resolve/resolve.hpp"

static Pixel makeColour(std::uint8_t r, std::uint8_t g, std::uint8_t b) {
    Pixel colour;
    colour.r = r;
    colour.g = g;
    colour.b = b;
    return colour;
}

struct ColouredTriangle {
    Triangle triangle;
    Pixel colour;
};

static void drawComparisonScene(MultisampleBuffer& buffer, const SamplePattern& pattern) {
    const ColouredTriangle triangles[] = {
        {{Point{6, 5}, Point{59, 12}, Point{17, 29}}, makeColour(240, 100, 55)},
        {{Point{26, 8}, Point{63, 26}, Point{23, 32}}, makeColour(40, 210, 220)},

        {{Point{72, 5}, Point{119, 5}, Point{72, 29}}, makeColour(60, 150, 240)},
        {{Point{119, 5}, Point{119, 29}, Point{72, 29}}, makeColour(245, 205, 65)},

        {{Point{6, 43}, Point{59, 57}, Point{58, 59}}, makeColour(90, 230, 150)},

        {{Point{85, 36}, Point{140, 47}, Point{98, 76}}, makeColour(185, 100, 235)}
    };

    for (const ColouredTriangle& item : triangles) {
        drawTriangle(buffer, setupTriangle(item.triangle), pattern, item.colour);
    }
}

static bool renderComparison(const SamplePattern& pattern, const char* filename) {
    MultisampleBuffer buffer(pattern.count);
    Framebuffer framebuffer;

    buffer.fill(makeColour(18, 25, 38));
    drawComparisonScene(buffer, pattern);
    resolve(buffer, framebuffer);

    if (!writePpm(framebuffer, filename)) {
        std::cerr << "Could not write " << filename << '\n';
        return false;
    }

    std::cout << "Wrote " << filename << " (" << pattern.count << "x sampling)\n";
    return true;
}

int main() {
    if (!renderComparison(makeSingleSamplePattern(), "scene-1x.ppm") ||
        !renderComparison(makeFourSamplePattern(),   "scene-4x.ppm")   ||
        !renderComparison(makeEightSamplePattern(),  "scene-8x.ppm")) {
        return 1;
    }
    return 0;
}
