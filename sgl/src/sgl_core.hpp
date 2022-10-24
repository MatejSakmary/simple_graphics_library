#pragma once

#include <vector>
#include <memory>
#include <cstdint>

#include "sgl.h"
#include "sgl_context.hpp"
#include "sgl_frambuffer.hpp"
#include "sgl_vertex.hpp"
#include "sgl_renderer.hpp"

struct SglCore
{
    SglRenderer renderer;
    std::vector<SglContext> contexts;

    SglCore();
    ~SglCore();

    auto create_context(int32_t width, int32_t height) -> int32_t;
    auto get_context() -> int32_t;
    void set_context(int32_t context_idx);
    void destroy_context(int32_t context_idx);
    void push_vertex(SglVertex vertex);
    void set_recording(bool new_recording);
    auto get_recording() -> bool;

    sglEErrorCode get_error();
    void set_error(sglEErrorCode error_code);

    private:
        // set to true when sglBegin() is called and back to false when sglEnd() is called
        bool recording;
        int32_t current_context;
        sglEErrorCode error;
};