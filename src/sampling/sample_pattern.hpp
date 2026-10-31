#pragma once

#include <array>
#include <cstddef>

#include "sampling/coverage.hpp"

struct SamplePosition {
    int x4;
    int y4;
};

struct SamplePattern {
    std::size_t count;
    std::array<SamplePosition, SampleMask::kMaxSamples> positions;
};

inline SamplePattern makeSingleSamplePattern() {
    SamplePattern pattern = {};

    pattern.count = 1;
    pattern.positions[0] = SamplePosition{2,2};

    return pattern;
}

inline SamplePattern makeFourSamplePattern() {
    SamplePattern pattern = {};

    pattern.count = 4;
    pattern.positions[0] = SamplePosition{1, 1};
    pattern.positions[1] = SamplePosition{3, 1};
    pattern.positions[2] = SamplePosition{1, 3};
    pattern.positions[3] = SamplePosition{3, 3};

    return pattern;
}

inline SamplePattern makeEightSamplePattern() {
    SamplePattern pattern = {};

    pattern.count = 8;
    pattern.positions[0] = SamplePosition{1, 1};
    pattern.positions[1] = SamplePosition{2, 1};
    pattern.positions[2] = SamplePosition{3, 1};
    pattern.positions[3] = SamplePosition{1, 2};
    pattern.positions[4] = SamplePosition{3, 2};
    pattern.positions[5] = SamplePosition{1, 3};
    pattern.positions[6] = SamplePosition{2, 3};
    pattern.positions[7] = SamplePosition{3, 3};

    return pattern;
}
