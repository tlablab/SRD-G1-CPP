#include "resolve/resolve.hpp"
#include <cstdint>
#include <cassert>

void resolve(
    const CoverageBuffer& buffer,
    Framebuffer& frame,
    std::size_t sampleCount
){
    Pixel pixel;

    assert(sampleCount > 0 && sampleCount <= SampleMask::kMaxSamples);

    for(int x=0; x < static_cast<int>(Framebuffer::kWidth); x++){
        for(int y=0; y < static_cast<int>(Framebuffer::kHeight); y++){
            const SampleMask coverage = buffer.read(x, y);
            std::size_t coveredSamples = 0;

            for (std::size_t sample = 0; sample < sampleCount; ++sample) {
                if (coverage.test(sample)) {
                    ++coveredSamples;
                }
            }
            pixel.intensity = static_cast<std::uint8_t>(
                (coveredSamples * 255 + sampleCount / 2) / sampleCount
            );
            frame.write(x,y,pixel);
        }
    }

}
