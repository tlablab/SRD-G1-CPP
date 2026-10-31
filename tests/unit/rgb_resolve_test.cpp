#include <cassert>
#include <cstddef>
#include <cstdint>

#include "core/framebuffer.hpp"
#include "core/multisamplebuffer.hpp"
#include "resolve/resolve.hpp"

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

static void assertFilled(const Framebuffer& frame, const Pixel& expected) {
    for (int y = 0; y < static_cast<int>(Framebuffer::kHeight); ++y) {
        for (int x = 0; x < static_cast<int>(Framebuffer::kWidth); ++x) {
            assertColour(frame.read(x, y), expected);
        }
    }
}

static void testUniformSamples(std::size_t sampleCount) {
    MultisampleBuffer buffer(sampleCount);
    Framebuffer frame;

    const Pixel white = makeColour(255, 255, 255);
    buffer.fill(white);
    resolve(buffer, frame);
    assertFilled(frame, white);

    const Pixel background = makeColour(17, 83, 201);
    buffer.fill(background);
    resolve(buffer, frame);
    assertFilled(frame, background);

    buffer.fill(Pixel{});
    resolve(buffer, frame);
    assertFilled(frame, Pixel{});
}

static void testFourSampleMixtures() {
    MultisampleBuffer buffer(4);
    Framebuffer frame;
    const Pixel red = makeColour(255, 0, 0);
    const Pixel green = makeColour(0, 255, 0);
    const Pixel blue = makeColour(0, 0, 255);

    buffer.writeSample(10, 10, 0, red);
    buffer.writeSample(10, 10, 1, red);
    buffer.writeSample(10, 10, 2, blue);
    buffer.writeSample(10, 10, 3, blue);

    buffer.writeSample(12, 10, 0, red);
    buffer.writeSample(12, 10, 1, green);
    buffer.writeSample(12, 10, 2, blue);

    buffer.writeSample(14, 10, 0, makeColour(1, 2, 3));

    const Pixel background = makeColour(20, 40, 60);
    buffer.writeSample(16, 10, 0, makeColour(220, 80, 100));
    for (std::size_t sample = 1; sample < 4; ++sample) {
        buffer.writeSample(16, 10, sample, background);
    }

    resolve(buffer, frame);

    for (int y = 0; y < static_cast<int>(Framebuffer::kHeight); ++y) {
        for (int x = 0; x < static_cast<int>(Framebuffer::kWidth); ++x) {
            Pixel expected;
            if (y == 10) {
                if (x == 10) expected = makeColour(128, 0, 128);
                if (x == 12) expected = makeColour(64, 64, 64);
                if (x == 14) expected = makeColour(0, 1, 1);
                if (x == 16) expected = makeColour(70, 50, 70);
            }
            assertColour(frame.read(x, y), expected);
        }
    }

    assertColour(buffer.read(10, 10, 0), red);
    assertColour(buffer.read(10, 10, 1), red);
    assertColour(buffer.read(10, 10, 2), blue);
    assertColour(buffer.read(10, 10, 3), blue);
}

static void testEightSampleMixtures() {
    MultisampleBuffer buffer(8);
    Framebuffer frame;

    buffer.writeSample(10, 10, 7, makeColour(255, 128, 64));
    for (std::size_t sample = 0; sample < 7; ++sample) {
        buffer.writeSample(12, 10, sample, makeColour(255, 255, 255));
    }
    buffer.writeSample(14, 10, 0, makeColour(3, 4, 5));

    resolve(buffer, frame);
    assertColour(frame.read(10, 10), makeColour(32, 16, 8));
    assertColour(frame.read(12, 10), makeColour(223, 223, 223));
    assertColour(frame.read(14, 10), makeColour(0, 1, 1));
    assertColour(frame.read(10, 11), Pixel{});
    assertColour(frame.read(11, 10), Pixel{});
}

int main() {
    testUniformSamples(1);
    testUniformSamples(4);
    testUniformSamples(8);
    testFourSampleMixtures();
    testEightSampleMixtures();
    return 0;
}
