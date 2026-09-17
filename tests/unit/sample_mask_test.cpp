#include <cassert>

#include "sampling/coverage.hpp"

int main() {
    SampleMask coverage;

    assert(!coverage.any());

    coverage.set(0);
    coverage.set(3);

    assert(coverage.any());
    assert(coverage.test(0));
    assert(!coverage.test(1));
    assert(!coverage.test(2));
    assert(coverage.test(3));

    coverage.clear();

    assert(!coverage.any());
    assert(!coverage.test(0));
    assert(!coverage.test(3));

    return 0;
}