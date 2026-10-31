#include <cassert>
#include <cstddef>
#include <cstdint>
#include <limits>
#include <stdexcept>

#include "core/multisamplebuffer.hpp"

static Pixel makeColour(std::uint8_t r, std::uint8_t g, std::uint8_t b) {
    Pixel colour;
    colour.r = r;
    colour.g = g;
    colour.b = b;
    return colour;
}

static void assertColour(const Pixel& actual, const Pixel& expected) {
    assert(actual.r == expected.r);
    assert(actual.g == expected.g);
    assert(actual.b == expected.b);
}

static void assertFilled(const MultisampleBuffer& buffer, const Pixel& expected) {
    for (int y = 0; y < static_cast<int>(MultisampleBuffer::kHeight); ++y) {
        for (int x = 0; x < static_cast<int>(MultisampleBuffer::kWidth); ++x) {
            for (std::size_t sample = 0; sample < buffer.sampleCount(); ++sample) {
                assertColour(buffer.read(x, y, sample), expected);
            }
        }
    }
}

static void testBuffer(std::size_t sampleCount) {
    MultisampleBuffer buffer(sampleCount);
    assert(buffer.sampleCount() == sampleCount);

    assertFilled(buffer, Pixel{});

    const Pixel background = makeColour(12, 34, 56);
    buffer.fill(background);
    assertFilled(buffer, background);

    for (std::size_t sample = 0; sample < sampleCount; ++sample) {
        const Pixel colour = makeColour(
            static_cast<std::uint8_t>(20 + sample),
            static_cast<std::uint8_t>(80 + sample),
            static_cast<std::uint8_t>(140 + sample)
        );
        buffer.writeSample(10, 10, sample, colour);
    }

    for (std::size_t sample = 0; sample < sampleCount; ++sample) {
        const Pixel expected = makeColour(
            static_cast<std::uint8_t>(20 + sample),
            static_cast<std::uint8_t>(80 + sample),
            static_cast<std::uint8_t>(140 + sample)
        );
        assertColour(buffer.read(10, 10, sample), expected);
        assertColour(buffer.read(11, 10, sample), background);
        assertColour(buffer.read(10, 11, sample), background);
    }

    const Pixel replacement = makeColour(201, 7, 93);
    buffer.writeSample(10, 10, 0, replacement);
    assertColour(buffer.read(10, 10, 0), replacement);
    for (std::size_t sample = 1; sample < sampleCount; ++sample) {
        assertColour(buffer.read(10, 10, sample), makeColour(
            static_cast<std::uint8_t>(20 + sample),
            static_cast<std::uint8_t>(80 + sample),
            static_cast<std::uint8_t>(140 + sample)
        ));
    }

    const int width = static_cast<int>(MultisampleBuffer::kWidth);
    const int height = static_cast<int>(MultisampleBuffer::kHeight);
    const int corners[][2] = {
        {0, 0}, {width - 1, 0}, {0, height - 1}, {width - 1, height - 1}
    };
    for (const auto& corner : corners) {
        assert(buffer.inBounds(corner[0], corner[1]));
        buffer.writeSample(corner[0], corner[1], sampleCount - 1, replacement);
        assertColour(buffer.read(corner[0], corner[1], sampleCount - 1), replacement);
    }

    buffer.fill(background);
    assertFilled(buffer, background);

    const int invalidCoordinates[][2] = {
        {-1, 0}, {0, -1}, {width, 0}, {0, height},
        {width, height - 1}, {width - 1, height}, {width, height}
    };
    for (const auto& position : invalidCoordinates) {
        assert(!buffer.inBounds(position[0], position[1]));
        buffer.writeSample(position[0], position[1], 0, replacement);
        assertColour(buffer.read(position[0], position[1], 0), Pixel{});
    }

    const std::size_t invalidSamples[] = {
        sampleCount, SampleMask::kMaxSamples, std::numeric_limits<std::size_t>::max()
    };
    for (std::size_t sample : invalidSamples) {
        buffer.writeSample(10, 10, sample, replacement);
        assertColour(buffer.read(10, 10, sample), Pixel{});
    }
    assertFilled(buffer, background);

    buffer.fill(Pixel{});
    assertFilled(buffer, Pixel{});
    assert(buffer.sampleCount() == sampleCount);
}

static void testInvalidCount(std::size_t sampleCount) {
    bool rejected = false;
    try {
        MultisampleBuffer buffer(sampleCount);
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
