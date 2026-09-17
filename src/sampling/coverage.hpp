#pragma once
#include <cassert>
#include <cstddef>
#include <cstdint>

struct SampleMask {
    static constexpr std::size_t kMaxSamples = 8;
    std::uint32_t bits = 0;
    
    bool any() const {
        return bits != 0;
    }
    
    bool test(std::size_t sample) const {
        assert(sample < kMaxSamples);
        return (bits & (std::uint32_t(1) << sample)) != 0;
    }
     
     bool set(std::size_t sample) {
        assert(sample < kMaxSamples);
        bits |= std::uint32_t(1) << sample;
    }
     
    void clear() {
        bits = 0;
    }
};