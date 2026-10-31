#pragma once

#include <array>
#include <cstddef>

#include "sampling/coverage.hpp"

class Coveragebuffer {
public:
    static constexpr std::size_t kWidth = 128;
    static constexpr std::size_t kHeight = 64;

    bool inBounds(int x, int y) const;

    SampleMask read(int x, int y) const;
    void write(int x, int y, SampleMask mask);
    void clear();

private:
    std::array<SampleMask, kWidth * kHeight> masks_;
};
