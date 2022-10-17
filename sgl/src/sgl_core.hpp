#pragma once

#include <vector>
#include <memory>
#include <cstdint>

#include "sgl_context.hpp"
#include "sgl.h"

struct SglCore
{
    std::vector<SglContext> contexts;

    SglCore();
    ~SglCore();

    int32_t create_context(int32_t width, int32_t height);
    void set_context(int32_t context_idx);
    int32_t get_context();

    private:
        int32_t current_context = -1;
};