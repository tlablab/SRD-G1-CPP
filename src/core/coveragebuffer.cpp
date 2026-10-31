#include "core/coveragebuffer.hpp"
#include "sampling/coverage.hpp"

bool Coveragebuffer::inBounds(int x, int y) const{
    return x >= 0 &&
           x < static_cast<int>(kWidth) &&
           y >= 0 &&
           y < static_cast<int>(kHeight);
}

SampleMask Coveragebuffer::read(int x, int y) const{
    if(!inBounds(x,y)){
        return SampleMask{};
    }
    std::size_t index = static_cast<std::size_t>(y) * kWidth + static_cast<std::size_t>(x);

    return masks_[index];
}

void Coveragebuffer::write(int x, int y, SampleMask mask){
    if(!inBounds(x,y)){
        return;
    }

    std::size_t index = static_cast<std::size_t>(y) * kWidth + static_cast<std::size_t>(x);
    masks_[index].bits |= mask.bits;
}

void Coveragebuffer::clear(){
    masks_.fill(SampleMask{});
    return;
}
