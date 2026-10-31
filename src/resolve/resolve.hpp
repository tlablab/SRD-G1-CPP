#pragma once
#include "core/coveragebuffer.hpp"
#include "core/framebuffer.hpp"

void resolve(
    const CoverageBuffer& buffer,
    Framebuffer& frame,
    std::size_t sampleCount
);
