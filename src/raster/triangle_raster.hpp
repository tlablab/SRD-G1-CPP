#pragma once

#include "core/coveragebuffer.hpp"
#include "core/triangle.hpp"
#include "sampling/sample_pattern.hpp"

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
    CoverageBuffer& buffer,
    const TriSetup& setup,
    const SamplePattern& pattern
);
SampleMask triangleCoverage(
    const TriSetup& setup,
    const long long (&centreEdges)[3],
    const SamplePattern& pattern
);
