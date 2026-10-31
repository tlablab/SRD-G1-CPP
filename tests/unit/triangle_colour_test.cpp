#include <cassert>
#include <cstddef>
#include <cstdint>

#include "core/multisamplebuffer.hpp"
#include "raster/triangle_raster.hpp"
#include "sampling/sample_pattern.hpp"

static Pixel makeColour(std::uint8_t r, std::uint8_t g, std::uint8_t b) {
    Pixel colour;
    colour.r = r;
    colour.g = g;
    colour.b = b;
    return colour;
}

static void assertColour(const Pixel& actual, const Pixel& expected) {
    assert(actual.r == expected.r);
    assert(actual.g == expected.g);
    assert(actual.b == expected.b);
}

static void testColouredTriangles(const SamplePattern& pattern) {
    MultisampleBuffer buffer(pattern.count);
    const Pixel background = makeColour(12, 34, 56);
    const Pixel red = makeColour(230, 17, 41);
    const Pixel blue = makeColour(29, 73, 211);

    const Triangle first{Point{10, 10}, Point{20, 10}, Point{10, 20}};
    const Triangle second{Point{20, 10}, Point{20, 20}, Point{10, 20}};
    const Triangle large{Point{10, 10}, Point{30, 10}, Point{10, 30}};
    const Triangle reversedFirst{first.a, first.c, first.b};

    for (int scenario = 0; scenario < 5; ++scenario) {
        buffer.fill(background);

        switch (scenario) {
        case 0: // One triangle
            drawTriangle(buffer, setupTriangle(first), pattern, red);
            break;
        case 1: // Adjacent triangles
            drawTriangle(buffer, setupTriangle(first), pattern, red);
            drawTriangle(buffer, setupTriangle(second), pattern, blue);
            break;
        case 2: // A smaller blue triangle overwrites only its covered samples
            drawTriangle(buffer, setupTriangle(large), pattern, red);
            drawTriangle(buffer, setupTriangle(first), pattern, blue);
            break;
        case 3: // Reverse draw order
            drawTriangle(buffer, setupTriangle(first), pattern, blue);
            drawTriangle(buffer, setupTriangle(large), pattern, red);
            break;
        case 4: // Reversing winding must preserve the coloured samples
            drawTriangle(buffer, setupTriangle(reversedFirst), pattern, red);
            break;
        }

        for (int y = 0; y < static_cast<int>(MultisampleBuffer::kHeight); ++y) {
            for (int x = 0; x < static_cast<int>(MultisampleBuffer::kWidth); ++x) {
                for (std::size_t sample = 0; sample < pattern.count; ++sample) {
                    const int x4 = 4 * x + pattern.positions[sample].x4;
                    const int y4 = 4 * y + pattern.positions[sample].y4;
                    const bool inFirst = x4 >= 40 && y4 >= 40 && x4 + y4 < 120;
                    const bool inLarge = x4 >= 40 && y4 >= 40 && x4 + y4 < 160;
                    const bool inRectangle = x4 >= 40 && x4 < 80 && y4 >= 40 && y4 < 80;

                    Pixel expected = background;
                    if (scenario == 0 || scenario == 4) {
                        if (inFirst) expected = red;
                    } else if (scenario == 1) {
                        if (inRectangle) expected = inFirst ? red : blue;
                    } else if (scenario == 2) {
                        if (inLarge) expected = red;
                        if (inFirst) expected = blue;
                    } else {
                        if (inLarge) expected = red;
                    }
                    assertColour(buffer.read(x, y, sample), expected);
                }
            }
        }

        if (scenario == 2 && pattern.count == 4) {
            assertColour(buffer.read(14, 15, 0), blue);
            for (std::size_t sample = 1; sample < 4; ++sample) {
                assertColour(buffer.read(14, 15, sample), red);
            }
        }
    }
}

int main() {
    testColouredTriangles(makeSingleSamplePattern());
    testColouredTriangles(makeFourSamplePattern());
    testColouredTriangles(makeEightSamplePattern());
    return 0;
}
