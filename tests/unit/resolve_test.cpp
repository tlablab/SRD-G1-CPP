#include <cassert>
#include <cstdint>

#include "core/coveragebuffer.hpp"
#include "core/framebuffer.hpp"
#include "resolve/resolve.hpp"
#include "sampling/coverage.hpp"

static void assertGrayscale(const Pixel& pixel, int expected) {
    assert(pixel.r == expected);
    assert(pixel.g == expected);
    assert(pixel.b == expected);
}

int main() {
    CoverageBuffer coverage;
    Framebuffer frame;

    const std::uint32_t masks[] = {0x0, 0x8, 0x5, 0xB, 0xF};
    const int intensities[] = {0, 64, 128, 191, 255};
    const int caseCount = 5;

    for (int i = 0; i < caseCount; ++i) {
        SampleMask mask;
        mask.bits = masks[i];
        coverage.merge(10 + 2 * i, 10, mask);
    }

    Pixel oldPixel;
    oldPixel.r = 42;
    oldPixel.g = 83;
    oldPixel.b = 124;
    for (int y = 0; y < static_cast<int>(Framebuffer::kHeight); ++y) {
        for (int x = 0; x < static_cast<int>(Framebuffer::kWidth); ++x) {
            frame.write(x, y, oldPixel);
        }
    }

    resolve(coverage, frame, 4);

    for (int i = 0; i < caseCount; ++i) {
        const int x = 10 + 2 * i;
        assertGrayscale(frame.read(x, 10), intensities[i]);
        assert(coverage.read(x, 10).bits == masks[i]);

        assertGrayscale(frame.read(x + 1, 10), 0);
        assertGrayscale(frame.read(x, 11), 0);
    }

    for (int y = 0; y < static_cast<int>(Framebuffer::kHeight); ++y) {
        for (int x = 0; x < static_cast<int>(Framebuffer::kWidth); ++x) {
            int expected = 0;
            for (int i = 0; i < caseCount; ++i) {
                if (x == 10 + 2 * i && y == 10) {
                    expected = intensities[i];
                }
            }
            assertGrayscale(frame.read(x, y), expected);
        }
    }

    coverage.clear();
    resolve(coverage, frame, 4);
    for (int y = 0; y < static_cast<int>(Framebuffer::kHeight); ++y) {
        for (int x = 0; x < static_cast<int>(Framebuffer::kWidth); ++x) {
            assertGrayscale(frame.read(x, y), 0);
        }
    }

    return 0;
}
