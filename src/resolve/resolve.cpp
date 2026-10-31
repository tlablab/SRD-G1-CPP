#include "resolve/resolve.hpp"
#include "core/multisamplebuffer.hpp"
#include <cstddef>
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
            const std::uint8_t intensity = static_cast<std::uint8_t>(
                (coveredSamples * 255 + sampleCount / 2) / sampleCount
            );
            pixel.r = intensity;
            pixel.g = intensity;
            pixel.b = intensity;
            frame.write(x,y,pixel);
        }
    }

}

void resolve(
    const MultisampleBuffer& buffer,
    Framebuffer& frame
){
    Pixel pixel;
    std::size_t sampleCount = buffer.sampleCount();

    for(int x=0; x < static_cast<int>(Framebuffer::kWidth); x++){
        for(int y=0; y < static_cast<int>(Framebuffer::kHeight); y++){

            unsigned int rsum = 0;
            unsigned int gsum = 0;
            unsigned int bsum = 0;
            for(std::size_t sample = 0; sample < sampleCount; ++sample){
                rsum += buffer.read(x, y, sample).r;
                gsum += buffer.read(x, y, sample).g;
                bsum += buffer.read(x, y, sample).b;
            }

            pixel.r = (rsum + sampleCount / 2)/sampleCount;
            pixel.g = (gsum + sampleCount / 2)/sampleCount;
            pixel.b = (bsum + sampleCount / 2)/sampleCount;
            frame.write(x,y,pixel);
        }
    }

}
