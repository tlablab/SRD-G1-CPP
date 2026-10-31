#pragma once

#include <array>
#include <cstddef>

#include "core/pixel.hpp"
#include "sampling/coverage.hpp"

class MultisampleBuffer {
public:
    static constexpr std::size_t kWidth = 128;
    static constexpr std::size_t kHeight = 64;

    bool inBounds(int x, int y) const;

    Pixel read(int x, int y, std::size_t sample) const;

    void writeSample(
        int x, int y,
        std::size_t sample,
        Pixel colour
    );

    void fill(Pixel background);

    explicit MultisampleBuffer(std::size_t sampleCount);

    std::size_t sampleCount() const {
        return sampleCount_;
    }
private:
    std::size_t sampleCount_;
    std::array<
        std::array<Pixel, SampleMask::kMaxSamples>,
        kWidth * kHeight
    > samples_;
};
