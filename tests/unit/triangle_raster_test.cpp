#include <cassert>

#include "core/coveragebuffer.hpp"
#include "core/triangle.hpp"
#include "raster/triangle_raster.hpp"
#include "sampling/coverage.hpp"
#include "sampling/sample_pattern.hpp"

// Evaluate coverage at one pixel without walking the whole bounding box.
static SampleMask coverageAt(
    const TriSetup& setup,
    int x,
    int y,
    const SamplePattern& pattern
) {
    long long centreEdges[3];

    for (int i = 0; i < 3; ++i) {
        centreEdges[i] =
            setup.e0[i]
            + setup.a[i] * (x - setup.minX)
            + setup.b[i] * (y - setup.minY);
    }

    return triangleCoverage(setup, centreEdges, pattern);
}

// Run the rasterizer integration checks with both supported sample patterns.
static void testTriangleRaster(const SamplePattern& pattern) {
    CoverageBuffer buffer;
    const std::uint32_t fullMask = (std::uint32_t(1) << pattern.count) - 1;

    Triangle triangle{
        Point{10, 10},
        Point{30, 10},
        Point{10, 30}
    };

    drawTriangle(buffer, setupTriangle(triangle), pattern);

    assert(buffer.read(11, 11).bits == fullMask);
    assert(buffer.read(15, 15).bits == fullMask);
    assert(buffer.read(25, 25).bits == 0);
    assert(buffer.read(31, 10).bits == 0);

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

    const TriSetup firstSetup = setupTriangle(first);
    const TriSetup secondSetup = setupTriangle(second);
    const SamplePattern fourSamples = makeFourSamplePattern();

    // All four samples are inside at (11, 11), and outside at (19, 19).
    assert(coverageAt(firstSetup, 11, 11, fourSamples).bits == 0xF);
    assert(coverageAt(firstSetup, 19, 19, fourSamples).bits == 0);

    // At (14, 15), sample 0 is below x + y = 30, samples 1 and 2
    // lie exactly on it, and sample 3 is above it. The top-left rule
    // assigns the two on-edge samples to the second triangle.
    const SampleMask firstMask = coverageAt(firstSetup, 14, 15, fourSamples);
    const SampleMask secondMask = coverageAt(secondSetup, 14, 15, fourSamples);

    assert(firstMask.bits == 0x1);
    assert(secondMask.bits == 0xE);
    assert((firstMask.bits & secondMask.bits) == 0);    // No overlap.
    assert((firstMask.bits | secondMask.bits) == 0xF);  // No gaps.

    CoverageBuffer firstOnly;
    CoverageBuffer secondOnly;
    CoverageBuffer combined;

    drawTriangle(firstOnly, firstSetup, pattern);
    drawTriangle(secondOnly, secondSetup, pattern);
    drawTriangle(combined, firstSetup, pattern);
    drawTriangle(combined, secondSetup, pattern);

    // With 1x, the centre belongs only to the second triangle. With 4x,
    // sample 0 belongs to the first and the remaining three to the second.
    assert(firstOnly.read(14, 15).bits == (pattern.count == 1 ? 0x0 : 0x1));
    assert(secondOnly.read(14, 15).bits == (pattern.count == 1 ? 0x1 : 0xE));
    assert(combined.read(14, 15).bits == fullMask);

    for (int y = 0; y < static_cast<int>(CoverageBuffer::kHeight); ++y) {
        for (int x = 0; x < static_cast<int>(CoverageBuffer::kWidth); ++x) {
            const bool inRectangle = x >= 10 && x < 20 && y >= 10 && y < 20;
            assert(combined.read(x, y).bits == (inRectangle ? fullMask : 0));

            // Inspect stored results, not just the standalone coverage helper.
            const SampleMask a = firstOnly.read(x, y);
            const SampleMask b = secondOnly.read(x, y);
            assert((a.bits & b.bits) == 0);
            assert((a.bits | b.bits) == combined.read(x, y).bits);

            // Compare incremental edge stepping with evaluation at this pixel.
            assert(a.bits == coverageAt(firstSetup, x, y, pattern).bits);
            assert(b.bits == coverageAt(secondSetup, x, y, pattern).bits);
        }
    }

    Triangle forward{
        Point{10, 10},
        Point{30, 10},
        Point{10, 30}
    };

    Triangle reversed{
        Point{10, 10},
        Point{10, 30},
        Point{30, 10}
    };

    CoverageBuffer forwardBuffer;
    CoverageBuffer reversedBuffer;

    drawTriangle(forwardBuffer, setupTriangle(forward), pattern);
    drawTriangle(reversedBuffer, setupTriangle(reversed), pattern);

    for (int y = 0; y < static_cast<int>(CoverageBuffer::kHeight); ++y) {
        for (int x = 0; x < static_cast<int>(CoverageBuffer::kWidth); ++x) {
            assert(forwardBuffer.read(x, y).bits == reversedBuffer.read(x, y).bits);
        }
    }
}

int main() {
    testTriangleRaster(makeSingleSamplePattern());
    testTriangleRaster(makeFourSamplePattern());
    return 0;
}
