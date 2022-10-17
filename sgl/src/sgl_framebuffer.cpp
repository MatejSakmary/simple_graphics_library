#include "sgl_frambuffer.hpp"

#include <cstring>

SglFramebuffer::SglFramebuffer(uint32_t width, uint32_t height) :
    width{width}, height{height}, pixels(width * height, {0,0,0}) 
{}

SglFramebuffer::~SglFramebuffer()
{}

Pixel SglFramebuffer::get_pixel(uint32_t x, uint32_t y)
{
    return pixels.at(y * width + x);
}

void SglFramebuffer::set_pixel(uint32_t x, uint32_t y, const Pixel & pixel)
{
    pixels.at(y * width + x) = pixel;
}

void SglFramebuffer::clear_framebuffer(const Pixel & clear_color)
{
    for(auto & pixel : pixels) { pixel = clear_color; }
}