#include <cassert>

#include "core/framebuffer.hpp"
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

            // The same ownership rule must hold throughout the rectangle.
            const SampleMask a = coverageAt(firstSetup, x, y, fourSamples);
            const SampleMask b = coverageAt(secondSetup, x, y, fourSamples);
            assert((a.bits & b.bits) == 0);
            assert((a.bits | b.bits) == 0xF);
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

    Framebuffer first_fb;
    Framebuffer second_fb;

    drawTriangle(first_fb, setupTriangle(forward));
    drawTriangle(second_fb, setupTriangle(reversed));

    for (int y = 0; y < static_cast<int>(Framebuffer::kHeight); ++y) {
        for (int x = 0; x < static_cast<int>(Framebuffer::kWidth); ++x) {
            assert(first_fb.read(x, y).on == second_fb.read(x, y).on);
        }
    }


    return 0;
}
