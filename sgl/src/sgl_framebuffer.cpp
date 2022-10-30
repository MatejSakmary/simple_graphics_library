#include "sgl_frambuffer.hpp"

#include <cstring>

SglFramebuffer::SglFramebuffer(uint32_t width, uint32_t height) :
    width{width}, height{height}, pixels(width * height, {0,0,0}) 
{}

SglFramebuffer::~SglFramebuffer()
{}

void SglFramebuffer::clear_framebuffer(const Pixel & clear_color)
{
    for(auto & pixel : pixels) { pixel = clear_color; }
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