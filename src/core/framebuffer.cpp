#include "core/framebuffer.hpp"

bool Framebuffer::inBounds(int x, int y) const{
    return x >= 0 && 
           x < static_cast<int>(kWidth) && 
           y >= 0 && 
           y < static_cast<int>(kHeight);
}

Pixel Framebuffer::read(int x, int y) const{
    if(!inBounds(x,y)){
        return Pixel{};
    }
    std::size_t index = static_cast<std::size_t>(y) * kWidth + static_cast<std::size_t>(x);
    
    return pixels_[index];
}

void Framebuffer::write(int x, int y, Pixel pixel){
    if(!inBounds(x,y)){
        return;
    }
    
    std::size_t index = static_cast<std::size_t>(y) * kWidth + static_cast<std::size_t>(x);
    pixels_[index] = pixel;
}

void Framebuffer::clear(){
    pixels_.fill(Pixel{});
    return;
}