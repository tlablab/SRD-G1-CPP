#include "sampling/coverage.hpp"
#include "core/coveragebuffer.hpp"

bool CoverageBuffer::inBounds(int x, int y) const{
    return x >= 0 &&
           x < static_cast<int>(kWidth) &&
           y >= 0 &&
           y < static_cast<int>(kHeight);
}

SampleMask CoverageBuffer::read(int x, int y) const{
    if(!inBounds(x,y)){
        return SampleMask{};
    }
    std::size_t index = static_cast<std::size_t>(y) * kWidth + static_cast<std::size_t>(x);

    return masks_[index];
}

void CoverageBuffer::merge(int x, int y, SampleMask mask){
    if(!inBounds(x,y)){
        return;
    }

    std::size_t index = static_cast<std::size_t>(y) * kWidth + static_cast<std::size_t>(x);
    masks_[index].bits |= mask.bits;
}

void CoverageBuffer::clear(){
    masks_.fill(SampleMask{});
    return;
}
