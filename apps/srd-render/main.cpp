#include <cstddef>
#include <cstdint>
#include <cstring>
#include <iostream>

#include "core/depthbuffer.hpp"
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
    Depth depth;
};

static void drawComparisonScene(
    MultisampleBuffer& buffer,
    DepthBuffer& depths,
    const SamplePattern& pattern,
    bool reverseOrder
) {
    const ColouredTriangle triangles[] = {
        {{Point{6, 5}, Point{59, 12}, Point{17, 29}}, makeColour(240, 100, 55), 100},
        {{Point{26, 8}, Point{63, 26}, Point{23, 32}}, makeColour(40, 210, 220), 200},

        {{Point{72, 5}, Point{119, 5}, Point{72, 29}}, makeColour(60, 150, 240), 130},
        {{Point{119, 5}, Point{119, 29}, Point{72, 29}}, makeColour(245, 205, 65), 140},

        {{Point{6, 43}, Point{59, 57}, Point{58, 59}}, makeColour(90, 230, 150), 90},

        {{Point{85, 36}, Point{140, 47}, Point{98, 76}}, makeColour(185, 100, 235), 250}
    };

    const std::size_t triangleCount = sizeof(triangles) / sizeof(triangles[0]);
    for (std::size_t i = 0; i < triangleCount; ++i) {
        const std::size_t index = reverseOrder ? triangleCount - 1 - i : i;
        const ColouredTriangle& item = triangles[index];
        drawTriangle(buffer, depths, setupTriangle(item.triangle), pattern,
                     item.colour, item.depth);
    }
}

static bool renderComparison(
    const SamplePattern& pattern,
    const char* filename,
    bool reverseOrder
) {
    MultisampleBuffer buffer(pattern.count);
    DepthBuffer depths(pattern.count);
    Framebuffer framebuffer;

    buffer.fill(makeColour(18, 25, 38));
    drawComparisonScene(buffer, depths, pattern, reverseOrder);
    resolve(buffer, framebuffer);

    if (!writePpm(framebuffer, filename)) {
        std::cerr << "Could not write " << filename << '\n';
        return false;
    }

    std::cout << "Wrote " << filename << " (" << pattern.count << "x sampling)\n";
    return true;
}

int main(int argc, char* argv[]) {
    const bool reverseOrder = argc == 2 && std::strcmp(argv[1], "--reverse") == 0;
    if (argc > 1 && !reverseOrder) {
        std::cerr << "Usage: srd-render [--reverse]\n";
        return 1;
    }

    if (!renderComparison(makeSingleSamplePattern(), "scene-1x.ppm", reverseOrder) ||
        !renderComparison(makeFourSamplePattern(),   "scene-4x.ppm", reverseOrder) ||
        !renderComparison(makeEightSamplePattern(),  "scene-8x.ppm", reverseOrder)) {
        return 1;
    }
    return 0;
}
