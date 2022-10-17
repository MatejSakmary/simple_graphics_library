#pragma once
#include <cstdint>
#include <vector>

#include "sgl_frambuffer.hpp"
struct SglContextInitInfo
{
    uint32_t width;
    uint32_t height;
};

struct SglContext
{
    SglContextInitInfo info;
    SglFramebuffer framebuffer;

    SglContext(const SglContextInitInfo & info);
    ~SglContext();

    private:
        friend struct SglCore;
        bool released;
};