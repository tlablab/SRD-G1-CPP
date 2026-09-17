#pragma once

#include "core/framebuffer.hpp"
#include "core/triangle.hpp"
#include "core/pixel.hpp"

struct TriSetup {
    bool topLeft[3];
    long long e0[3];
    long long a[3];
    long long b[3];
    int minX;
    int maxX;
    int minY;
    int maxY;
    bool empty;
};

TriSetup setupTriangle(
    const Triangle& triangle
);

void drawTriangle(
    Framebuffer& framebuffer, 
    const TriSetup& setup
);