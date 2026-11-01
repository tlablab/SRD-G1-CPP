#include <cassert>
#include <cstddef>
#include <limits>
#include <stdexcept>

#include "core/depthbuffer.hpp"

static void assertCleared(const DepthBuffer& buffer) {
    const Depth farDepth = std::numeric_limits<Depth>::max();
    for (int y = 0; y < static_cast<int>(DepthBuffer::kHeight); ++y) {
        for (int x = 0; x < static_cast<int>(DepthBuffer::kWidth); ++x) {
            for (std::size_t sample = 0; sample < buffer.sampleCount(); ++sample) {
                assert(buffer.read(x, y, sample) == farDepth);
            }
        }
    }
}

static Depth testDepth(int x, int y, std::size_t sample) {
    const std::size_t pixel = static_cast<std::size_t>(y) * DepthBuffer::kWidth
                            + static_cast<std::size_t>(x);
    return static_cast<Depth>(1 + pixel * SampleMask::kMaxSamples + sample);
}

static void testBuffer(std::size_t sampleCount) {
    DepthBuffer buffer(sampleCount);
    const Depth farDepth = std::numeric_limits<Depth>::max();
    const int width = static_cast<int>(DepthBuffer::kWidth);
    const int height = static_cast<int>(DepthBuffer::kHeight);

    assert(buffer.sampleCount() == sampleCount);
    assertCleared(buffer);

    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            for (std::size_t sample = 0; sample < sampleCount; ++sample) {
                buffer.write(x, y, sample, testDepth(x, y, sample));
            }
        }
    }
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            for (std::size_t sample = 0; sample < sampleCount; ++sample) {
                assert(buffer.read(x, y, sample) == testDepth(x, y, sample));
            }
        }
    }

    buffer.write(10, 10, 0, 0);
    assert(buffer.read(10, 10, 0) == 0);
    buffer.write(10, 10, 0, farDepth - 1);
    assert(buffer.read(10, 10, 0) == farDepth - 1);
    buffer.write(10, 10, 0, farDepth);
    assert(buffer.read(10, 10, 0) == farDepth);
    for (std::size_t sample = 1; sample < sampleCount; ++sample) {
        assert(buffer.read(10, 10, sample) == testDepth(10, 10, sample));
    }
    assert(buffer.read(11, 10, 0) == testDepth(11, 10, 0));
    assert(buffer.read(10, 11, 0) == testDepth(10, 11, 0));

    assert(buffer.inBounds(0, 0));
    assert(buffer.inBounds(width - 1, 0));
    assert(buffer.inBounds(0, height - 1));
    assert(buffer.inBounds(width - 1, height - 1));

    buffer.clear();
    assertCleared(buffer);
    assert(buffer.sampleCount() == sampleCount);

    const int invalidCoordinates[][2] = {
        {-1, 0}, {0, -1}, {width, 0}, {0, height},
        {width, height - 1}, {width - 1, height}, {width, height}
    };
    for (const auto& position : invalidCoordinates) {
        assert(!buffer.inBounds(position[0], position[1]));
        buffer.write(position[0], position[1], 0, 123);
        assert(buffer.read(position[0], position[1], 0) == farDepth);
    }

    const std::size_t invalidSamples[] = {
        sampleCount, SampleMask::kMaxSamples, std::numeric_limits<std::size_t>::max()
    };
    for (std::size_t sample : invalidSamples) {
        buffer.write(10, 10, sample, 123);
        assert(buffer.read(10, 10, sample) == farDepth);
    }
    assertCleared(buffer);

    buffer.clear();
    assertCleared(buffer);
}

static void testInvalidCount(std::size_t sampleCount) {
    bool rejected = false;
    try {
        DepthBuffer buffer(sampleCount);
    } catch (const std::invalid_argument&) {
        rejected = true;
    }
    assert(rejected);
}

int main() {
    for (std::size_t count = 1; count <= SampleMask::kMaxSamples; ++count) {
        testBuffer(count);
    }
    testInvalidCount(0);
    testInvalidCount(SampleMask::kMaxSamples + 1);
    testInvalidCount(std::numeric_limits<std::size_t>::max());
    return 0;
}
