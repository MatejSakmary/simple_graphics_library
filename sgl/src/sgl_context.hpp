#pragma once
#include <cstdint>
#include <stack>
#include <functional>

#include "sgl_frambuffer.hpp"
#include "sgl_matrix.hpp"
#include "sgl.h"

struct SglContextInitInfo
{
    uint32_t width;
    uint32_t height;
    std::function<void(sglEErrorCode)> error_cbf;
};

struct SglContext
{
    SglContextInitInfo info;
    SglFramebuffer framebuffer;

    SglContext(const SglContextInitInfo & info);
    ~SglContext();

    void pop_matrix();
    void push_matrix();
    void load_identity();
    void load_matrix(const SglMatrix matrix);
    void mult_matrix(const SglMatrix matrix);
    void translate(float x, float y, float z);
    void rotate_2d(float angle, float center_x, float center_y);
    void rotate_y(float angle);
    void scale();

    private:
        friend struct SglCore;
        bool released;
        sglEMatrixMode mode;
        std::function<void(sglEErrorCode)> error_cbf;
        std::array<std::stack<SglMatrix>, 2> matrix_stacks;
};