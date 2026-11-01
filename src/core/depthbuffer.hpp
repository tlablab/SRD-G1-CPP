#pragma once

#include <array>
#include <cstddef>
#include <cstdint>

#include "sampling/coverage.hpp"

using Depth = std::uint32_t;

class DepthBuffer {
public:
    static constexpr std::size_t kWidth = 128;
    static constexpr std::size_t kHeight = 64;

    bool inBounds(int x, int y) const;

    explicit DepthBuffer(std::size_t sampleCount);

    Depth read(int x, int y, std::size_t sample) const;
    void write(int x, int y, std::size_t sample, Depth depth);
    void clear();

    std::size_t sampleCount() const;

private:
    std::size_t sampleCount_;
    std::array<
        std::array<Depth, SampleMask::kMaxSamples>,
        kWidth * kHeight
    > depths_;
};
