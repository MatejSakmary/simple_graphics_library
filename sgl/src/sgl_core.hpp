#pragma once

#include <vector>
#include <memory>
#include <cstdint>

#include "sgl_context.hpp"
#include "sgl.h"

struct SglCore
{
    std::vector<SglContext> contexts;
    bool recording;

    SglCore();
    ~SglCore();

    auto create_context(int32_t width, int32_t height) -> int32_t;
    auto get_context() -> int32_t;
    void set_context(int32_t context_idx);
    void destroy_context(int32_t context_idx);

    sglEErrorCode get_error();
    void set_error(sglEErrorCode error_code);

    private:
        // set to true when sglBegin() is called and back to false when sglEnd() is called
        int32_t current_context;
        sglEErrorCode error;
};