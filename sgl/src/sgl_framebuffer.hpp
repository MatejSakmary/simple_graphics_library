#pragma once
#include <vector>
#include <cstdint>
#include <limits>

#include "../include/sgl.h"
#include "macros.hpp"
#include "sgl_vec.hpp"

struct SglFramebuffer
{
    SglFramebuffer(uint32_t width, uint32_t height);
    ~SglFramebuffer();

    inline f32vec3 get_pixel(uint32_t x, uint32_t y) const               { return pixels.at(y * width + x);    }
    inline float get_depth(uint32_t x, uint32_t y) const               { return depth.at(y * width + x);     }
    inline void set_pixel(uint32_t x, uint32_t y, const f32vec3 & pixel) { pixels.at(y * width + x) = pixel;   }
    inline void set_depth(uint32_t x, uint32_t y, float new_depth)     { depth.at(y * width + x) = new_depth;} 
    void clear_framebuffer(const f32vec3 & clear_color, uint32_t clear_depth);
    uint32_t get_height() const;
    uint32_t get_width() const;
    float* get_framebuffer_pointer();
    // DEBUG ONLY
    float* get_depthbuffer_pointer();

    private:
        friend struct Context;
        std::vector<f32vec3> pixels;
        std::vector<float> depth;
        uint32_t width;
        uint32_t height;
};