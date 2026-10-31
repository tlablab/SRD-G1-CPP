#include "core/coveragebuffer.hpp"
#include "sampling/coverage.hpp"
#include <cassert>

int main(){
    CoverageBuffer buffer;

    for (int i=0; i < CoverageBuffer::kWidth; i++){
        for (int j = 0; j < CoverageBuffer::kHeight; j++){
            assert(buffer.read(i,j).bits == 0);
        }
    }

    SampleMask a, b, empty;
    a.bits     = 0x3;
    b.bits     = 0x6;
    empty.bits = 0x0;

    buffer.merge(15, 15, a);
    buffer.clear();
    for (int i=0; i < CoverageBuffer::kWidth; i++){
        for (int j = 0; j < CoverageBuffer::kHeight; j++){
            assert(buffer.read(i,j).bits == 0);
        }
    }

    buffer.merge(10, 10, a);
    assert(buffer.read(10,10).bits == 0x3);
    buffer.merge(10, 10, b);
    assert(buffer.read(10,10).bits == 0x7);
    buffer.clear();

    buffer.merge(128, 64, a);
    assert(buffer.read(128,64).bits == 0);
    buffer.merge(127, 63, a);
    assert(buffer.read(127,63).bits == 0x3);
    buffer.clear();

    assert(buffer.inBounds(127, 63));
    assert(!buffer.inBounds(128, 63));
    assert(!buffer.inBounds(127, 64));
    assert(!buffer.inBounds(-1, 0));
    assert(!buffer.inBounds(0, -1));

    buffer.merge(10, 10, a);
    buffer.merge(10, 10, a);
    assert(buffer.read(10, 10).bits == 0x3);

    buffer.merge(10, 10, b);
    assert(buffer.read(10, 10).bits == 0x7);

    buffer.merge(10, 10, a);
    assert(buffer.read(10, 10).bits == 0x7);

    buffer.merge(10, 10, empty);
    assert(buffer.read(10, 10).bits == 0x7);

    buffer.merge(11, 11, empty);
    assert(buffer.read(11,11).bits == 0x0);

    return 0;
}
