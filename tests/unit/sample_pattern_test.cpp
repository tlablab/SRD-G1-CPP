#include <cassert>

#include "sampling/sample_pattern.hpp"

int main() {


    SamplePattern single = makeSingleSamplePattern();

    assert(single.count == 1);
    assert(single.positions[0].x4 == 2);
    assert(single.positions[0].y4 == 2);

    SamplePattern four = makeFourSamplePattern();

    assert(four.count == 4);
    assert(four.count <= four.positions.size());

    for (std::size_t i = 0; i < four.count; ++i) {
        const SamplePosition& position = four.positions[i];

        assert(position.x4 >= 0 && position.x4 < 4);
        assert(position.y4 >= 0 && position.y4 < 4);
    }

    for (std::size_t i = 0; i < four.count; ++i) {
        for (std::size_t j = i + 1; j < four.count; ++j) {
            const SamplePosition& a = four.positions[i];
            const SamplePosition& b = four.positions[j];

            assert(a.x4 != b.x4 || a.y4 != b.y4);
        }
    }

    const SamplePattern eight = makeEightSamplePattern();
    const SamplePosition expectedEight[] = {
        {1, 1}, {2, 1}, {3, 1}, {1, 2},
        {3, 2}, {1, 3}, {2, 3}, {3, 3}
    };

    assert(eight.count == 8);
    assert(eight.count <= eight.positions.size());
    assert(eight.count <= SampleMask::kMaxSamples);

    int sumX4 = 0;
    int sumY4 = 0;
    for (std::size_t i = 0; i < eight.count; ++i) {
        const SamplePosition& position = eight.positions[i];
        assert(position.x4 == expectedEight[i].x4);
        assert(position.y4 == expectedEight[i].y4);
        assert(position.x4 > 0 && position.x4 < 4);
        assert(position.y4 > 0 && position.y4 < 4);
        sumX4 += position.x4;
        sumY4 += position.y4;

        for (std::size_t j = i + 1; j < eight.count; ++j) {
            assert(position.x4 != eight.positions[j].x4 ||
                   position.y4 != eight.positions[j].y4);
        }
    }
    assert(sumX4 == 16);
    assert(sumY4 == 16);

    return 0;
}
