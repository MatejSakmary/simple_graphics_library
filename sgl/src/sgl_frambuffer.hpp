#pragma once
#include <vector>
#include <cstdint>

struct Pixel
{
    float r;
    float g;
    float b;
};

struct SglFramebuffer
{
    SglFramebuffer(uint32_t width, uint32_t height);
    ~SglFramebuffer();

    inline Pixel get_pixel(uint32_t x, uint32_t y) const { return pixels.at(y * width + x); }
    inline void set_pixel(uint32_t x, uint32_t y, const Pixel & pixel) { pixels.at(y * width + x) = pixel; }
    void clear_framebuffer(const Pixel & clear_color);
    uint32_t get_height() const;
    uint32_t get_width() const;
    float* get_framebuffer_pointer();

    private:
        friend struct Context;
        std::vector<Pixel> pixels; 
        uint32_t width;
        uint32_t height;
};