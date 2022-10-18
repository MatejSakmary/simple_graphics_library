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
    int32_t get_context();
    void set_context(int32_t context_idx);
    void destroy_context(int32_t context_idx);

    sglEErrorCode get_error();
    void set_error(sglEErrorCode error_code);

    private:
        sglEErrorCode error = sglEErrorCode::SGL_NO_ERROR;
        int32_t current_context = -1;
};