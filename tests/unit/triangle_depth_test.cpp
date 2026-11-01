#include <cassert>
#include <cstddef>
#include <cstdint>
#include <limits>

#include "core/depthbuffer.hpp"
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

static void testTriangleDepth(const SamplePattern& pattern) {
    MultisampleBuffer colours(pattern.count);
    DepthBuffer depths(pattern.count);
    const Depth farDepth = std::numeric_limits<Depth>::max();
    const Pixel background = makeColour(12, 34, 56);
    const Pixel red = makeColour(230, 17, 41);
    const Pixel blue = makeColour(29, 73, 211);
    const Triangle small{Point{10, 10}, Point{20, 10}, Point{10, 20}};
    const Triangle large{Point{10, 10}, Point{30, 10}, Point{10, 30}};
    const TriSetup smallSetup = setupTriangle(small);
    const TriSetup largeSetup = setupTriangle(large);

    enum Scenario {
        NearThenFar,
        FarThenNear,
        EqualLargeFirst,
        EqualSmallFirst,
        MaximumDepthRejected,
        ZeroDepthWins
    };
    const Scenario scenarios[] = {
        NearThenFar, FarThenNear, EqualLargeFirst, EqualSmallFirst,
        MaximumDepthRejected, ZeroDepthWins
    };

    for (Scenario scenario : scenarios) {
        colours.fill(background);
        depths.clear();

        switch (scenario) {
        case NearThenFar:
            drawTriangle(colours, depths, smallSetup, pattern, red, 100);
            drawTriangle(colours, depths, largeSetup, pattern, blue, 200);
            break;
        case FarThenNear:
            drawTriangle(colours, depths, largeSetup, pattern, blue, 200);
            drawTriangle(colours, depths, smallSetup, pattern, red, 100);
            break;
        case EqualLargeFirst:
            drawTriangle(colours, depths, largeSetup, pattern, blue, 100);
            drawTriangle(colours, depths, smallSetup, pattern, red, 100);
            break;
        case EqualSmallFirst:
            drawTriangle(colours, depths, smallSetup, pattern, red, 100);
            drawTriangle(colours, depths, largeSetup, pattern, blue, 100);
            break;
        case MaximumDepthRejected:
            drawTriangle(colours, depths, smallSetup, pattern, red, farDepth);
            break;
        case ZeroDepthWins:
            drawTriangle(colours, depths, smallSetup, pattern, red, 0);
            drawTriangle(colours, depths, largeSetup, pattern, blue, 200);
            break;
        }

        for (int y = 0; y < static_cast<int>(DepthBuffer::kHeight); ++y) {
            for (int x = 0; x < static_cast<int>(DepthBuffer::kWidth); ++x) {
                for (std::size_t sample = 0; sample < pattern.count; ++sample) {
                    const int x4 = 4 * x + pattern.positions[sample].x4;
                    const int y4 = 4 * y + pattern.positions[sample].y4;
                    const bool inSmall = x4 >= 40 && y4 >= 40 && x4 + y4 < 120;
                    const bool inLarge = x4 >= 40 && y4 >= 40 && x4 + y4 < 160;
                    Pixel expectedColour = background;
                    Depth expectedDepth = farDepth;

                    if (scenario != MaximumDepthRejected) {
                        if (inLarge) {
                            expectedColour = blue;
                            expectedDepth = (scenario == EqualLargeFirst ||
                                             scenario == EqualSmallFirst) ? 100 : 200;
                        }
                        if (inSmall && scenario != EqualLargeFirst) {
                            expectedColour = red;
                            expectedDepth = scenario == ZeroDepthWins ? 0 : 100;
                        }
                    }

                    assertColour(colours.read(x, y, sample), expectedColour);
                    assert(depths.read(x, y, sample) == expectedDepth);
                }
            }
        }
    }
}

static void testIndependentSampleDepths(const SamplePattern& pattern) {
    MultisampleBuffer colours(pattern.count);
    DepthBuffer depths(pattern.count);
    const Pixel previous = makeColour(31, 79, 143);
    const Pixel incoming = makeColour(211, 53, 97);
    const Depth storedDepths[] = {50, 100, 150, std::numeric_limits<Depth>::max()};
    const Triangle triangle{Point{10, 10}, Point{20, 10}, Point{10, 20}};

    for (std::size_t sample = 0; sample < pattern.count; ++sample) {
        colours.writeSample(11, 11, sample, previous);
        depths.write(11, 11, sample, storedDepths[sample % 4]);
    }
    drawTriangle(colours, depths, setupTriangle(triangle), pattern, incoming, 100);

    for (std::size_t sample = 0; sample < pattern.count; ++sample) {
        if (sample % 4 < 2) {
            assertColour(colours.read(11, 11, sample), previous);
            assert(depths.read(11, 11, sample) == storedDepths[sample % 4]);
        } else {
            assertColour(colours.read(11, 11, sample), incoming);
            assert(depths.read(11, 11, sample) == 100);
        }
    }
}

int main() {
    const SamplePattern patterns[] = {
        makeSingleSamplePattern(), makeFourSamplePattern(), makeEightSamplePattern()
    };
    for (const SamplePattern& pattern : patterns) {
        testTriangleDepth(pattern);
        testIndependentSampleDepths(pattern);
    }
    return 0;
}
