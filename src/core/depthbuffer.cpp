#include <limits>
#include <stdexcept>

#include "core/depthbuffer.hpp"

DepthBuffer::DepthBuffer(std::size_t sampleCount)
    : sampleCount_(sampleCount)
{
    if (sampleCount == 0 || sampleCount > SampleMask::kMaxSamples) {
        throw std::invalid_argument("Sample count must be between 1 and 8");
    }

    clear();
}

bool DepthBuffer::inBounds(int x, int y) const{
    return x >= 0 &&
           x < static_cast<int>(kWidth) &&
           y >= 0 &&
           y < static_cast<int>(kHeight);
}

Depth DepthBuffer::read(int x, int y, std::size_t sample) const{
    if(!inBounds(x,y) || sample >= sampleCount_){
        return std::numeric_limits<Depth>::max();
    }
    std::size_t index = static_cast<std::size_t>(y) * kWidth + static_cast<std::size_t>(x);

    return depths_[index][sample];
}

void DepthBuffer::write(int x, int y, std::size_t sample, Depth depth){
    if (!inBounds(x, y) || sample >= sampleCount_) {
        return;
    }

    std::size_t index = static_cast<std::size_t>(y) * kWidth + static_cast<std::size_t>(x);
    depths_[index][sample] = depth;
}

void DepthBuffer::clear(){
    std::array<Depth, SampleMask::kMaxSamples> max_array;
    max_array.fill(std::numeric_limits<Depth>::max());
    depths_.fill(max_array);
    return;
}

std::size_t DepthBuffer::sampleCount() const{
    return sampleCount_;
}
