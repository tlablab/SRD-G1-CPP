#include <stdexcept>

#include "sampling/coverage.hpp"
#include "core/multisamplebuffer.hpp"
#include "core/pixel.hpp"

MultisampleBuffer::MultisampleBuffer(std::size_t sampleCount)
    : sampleCount_(sampleCount)
{
    if (sampleCount == 0 || sampleCount > SampleMask::kMaxSamples) {
        throw std::invalid_argument("Sample count must be between 1 and 8");
    }
}

bool MultisampleBuffer::inBounds(int x, int y) const{
    return x >= 0 &&
           x < static_cast<int>(kWidth) &&
           y >= 0 &&
           y < static_cast<int>(kHeight);
}

Pixel MultisampleBuffer::read(int x, int y, std::size_t sample) const{
    if (!inBounds(x, y) || sample >= sampleCount_) {
        return Pixel{};
    }

    std::size_t index = static_cast<std::size_t>(y) * kWidth + static_cast<std::size_t>(x);

    return samples_[index][sample];
}

void MultisampleBuffer::writeSample(int x, int y, std::size_t sample, Pixel colour){
    if (!inBounds(x, y) || sample >= sampleCount_) {
        return;
    }

    std::size_t index = static_cast<std::size_t>(y) * kWidth + static_cast<std::size_t>(x);
    samples_[index][sample] = colour;
}

void MultisampleBuffer::fill(Pixel background){
    std::array<Pixel, SampleMask::kMaxSamples> bg;
    bg.fill(background);
    samples_.fill(bg);
    return;
}
