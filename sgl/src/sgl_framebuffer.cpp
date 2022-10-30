#include "sgl_framebuffer.hpp"

#include <cstring>

SglFramebuffer::SglFramebuffer(uint32_t width, uint32_t height) :
    width{width}, height{height}, pixels(width * height, {0,0,0}), depth(width * height, 0.0f) 
{}

SglFramebuffer::~SglFramebuffer()
{}

void SglFramebuffer::clear_framebuffer(const Pixel & clear_color, uint32_t mask)
{
    if(mask & sglEClearBit::SGL_COLOR_BUFFER_BIT)
    {
        SGL_DEBUG_OUT("[SglFramebuffer::clear_framebuffer()] Clearing pixel buffer");
        for(auto & pixel : pixels) { pixel = clear_color; }
    }
    if(mask & sglEClearBit::SGL_DEPTH_BUFFER_BIT)
    {
        SGL_DEBUG_OUT("[SglFramebuffer::clear_framebuffer()] Clearing depth buffer");
        for(auto & pixel : depth) { pixel = 0.0f; }
    }
}

float* SglFramebuffer::get_framebuffer_pointer() {
    return reinterpret_cast<float*>(pixels.data());
}

uint32_t SglFramebuffer::get_height() const
{
    return height;
}
uint32_t SglFramebuffer::get_width() const
{
    return width;
}