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
        
    return 0;
}