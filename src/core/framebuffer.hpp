#pragma once

#include <array>
#include <cstddef>

#include "core/pixel.hpp"

class Framebuffer {
public:
    static constexpr std::size_t kWidth = 128;
    static constexpr std::size_t kHeight = 64;

    bool inBounds(int x, int y) const;

    Pixel read(int x, int y) const;
    void write(int x, int y, Pixel pixel);
    void clear();

private:
    std::array<Pixel, kWidth * kHeight> pixels_;
};