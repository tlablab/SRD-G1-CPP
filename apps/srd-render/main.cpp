#include <iostream>

#include "core/coveragebuffer.hpp"
#include "core/framebuffer.hpp"
#include "core/triangle.hpp"
#include "core/point.hpp"
#include "io/ppm_writer.hpp"
#include "raster/triangle_raster.hpp"
#include "sampling/sample_pattern.hpp"
#include "resolve/resolve.hpp"
// Keep the geometry identical for every sample pattern.
static void drawComparisonScene(CoverageBuffer& buffer, const SamplePattern& pattern) {
    const Triangle triangles[] = {
        // Top-left: a large triangle with shallow and steep diagonal edges.
        {Point{6, 5}, Point{59, 12}, Point{17, 29}},

        // Top-right: two triangles sharing a diagonal. The rectangle should
        // be solid white, with no seam between its two halves.
        {Point{72, 5}, Point{119, 5}, Point{72, 29}},
        {Point{119, 5}, Point{119, 29}, Point{72, 29}},

        // Bottom-left: a thin triangle, sensitive to sample placement.
        {Point{6, 43}, Point{59, 57}, Point{58, 59}},

        // Bottom-right: extends beyond the right and bottom buffer edges.
        {Point{85, 36}, Point{140, 47}, Point{98, 76}}
    };

    for (const Triangle& triangle : triangles) {
        drawTriangle(buffer, setupTriangle(triangle), pattern);
    }
}

static bool renderComparison(const SamplePattern& pattern, const char* filename) {
    CoverageBuffer buffer;
    Framebuffer framebuffer;

    // Accumulate the whole scene before converting coverage to brightness.
    buffer.clear();
    drawComparisonScene(buffer, pattern);
    resolve(buffer, framebuffer, pattern.count);

    if (!writePpm(framebuffer, filename)) {
        std::cerr << "Could not write " << filename << '\n';
        return false;
    }

    std::cout << "Wrote " << filename << " (" << pattern.count << "x sampling)\n";
    return true;
}

int main() {
    if (!renderComparison(makeSingleSamplePattern(), "scene-1x.ppm") ||
        !renderComparison(makeFourSamplePattern(), "scene-4x.ppm")) {
        return 1;
    }
    return 0;
}
