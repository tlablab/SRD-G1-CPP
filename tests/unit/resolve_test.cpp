#include <cassert>
#include <cstddef>
#include <cstdint>

#include "core/coveragebuffer.hpp"
#include "core/framebuffer.hpp"
#include "resolve/resolve.hpp"
#include "sampling/coverage.hpp"

int main() {
    CoverageBuffer coverage;
    Framebuffer frame;

    // With four active samples, coverage counts 0, 1, 2, 3, 4 must
    // resolve to these rounded intensities. Use different bit positions
    // to check that brightness depends on the count, not the mask value.
    const std::uint32_t masks[] = {0x0, 0x8, 0x5, 0xB, 0xF};
    const int intensities[] = {0, 64, 128, 191, 255};
    const int caseCount = 5;

    for (int i = 0; i < caseCount; ++i) {
        SampleMask mask;
        mask.bits = masks[i];
        coverage.merge(10 + 2 * i, 10, mask);
    }

    // Start with nonblack output so an empty sample mask must actually
    // write black, rather than leaving the previous framebuffer unchanged.
    Pixel oldPixel;
    oldPixel.intensity = 42;
    for (int y = 0; y < static_cast<int>(Framebuffer::kHeight); ++y) {
        for (int x = 0; x < static_cast<int>(Framebuffer::kWidth); ++x) {
            frame.write(x, y, oldPixel);
        }
    }

    resolve(coverage, frame, 4);

    for (int i = 0; i < caseCount; ++i) {
        const int x = 10 + 2 * i;
        assert(frame.read(x, 10).intensity == intensities[i]);
        assert(coverage.read(x, 10).bits == masks[i]); // Resolve preserves input.

        // Coverage must not carry into the next pixel, regardless of
        // whether resolve walks horizontally or vertically first.
        assert(frame.read(x + 1, 10).intensity == 0);
        assert(frame.read(x, 11).intensity == 0);
    }

    // Check the rest of the image too, including the final pixel.
    for (int y = 0; y < static_cast<int>(Framebuffer::kHeight); ++y) {
        for (int x = 0; x < static_cast<int>(Framebuffer::kWidth); ++x) {
            int expected = 0;
            for (int i = 0; i < caseCount; ++i) {
                if (x == 10 + 2 * i && y == 10) {
                    expected = intensities[i];
                }
            }
            assert(frame.read(x, y).intensity == expected);
        }
    }

    // A second resolve must erase the previous image without frame.clear().
    coverage.clear();
    resolve(coverage, frame, 4);
    for (int y = 0; y < static_cast<int>(Framebuffer::kHeight); ++y) {
        for (int x = 0; x < static_cast<int>(Framebuffer::kWidth); ++x) {
            assert(frame.read(x, y).intensity == 0);
        }
    }

    return 0;
}
