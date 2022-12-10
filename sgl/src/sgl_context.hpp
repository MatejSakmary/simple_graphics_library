#pragma once

#include <cstdint>
#include <stack>
#include <functional>

#include "sgl_framebuffer.hpp"
#include "sgl_matrix.hpp"
#include "sgl.h"

struct SglContextInitInfo
{
    uint32_t width;
    uint32_t height;
    std::function<void(sglEErrorCode)> error_cbf;
};

struct SglEnvMap
{
    bool set;
    uint32_t width;
    uint32_t height;
    std::vector<f32vec3> env_map;

    f32vec3 at(const f32vec3 &dir) const;
};

struct SglContext
{
    SglContextInitInfo info;
    SglFramebuffer framebuffer;
    f32vec3 clear_color;
    sglEEnableFlags capabilites;
    SglEnvMap environment_map;

    SglContext(const SglContextInitInfo & info);
    ~SglContext();

    void set_matrix_mode(sglEMatrixMode mode);

    void pop_matrix();
    void push_matrix();
    void load_identity();
    void load_matrix(const SglMatrix matrix);
    void mult_matrix(const SglMatrix matrix);
    void translate(float x, float y, float z);
    void rotate_2d(float angle, float center_x, float center_y);
    void rotate_y(float angle);
    void scale(float scale_x, float scale_y, float scale_z);
	void ortho(float left, float right, float bottom, float top, float near, float far);
	void perspective(float left, float right, float bottom, float top, float near, float far);
	void viewport(int x, int y, int width, int height);
    void clear(unsigned mask);

    private:
        friend struct SglCore;
        bool released;
        sglEMatrixMode mode;
        std::function<void(sglEErrorCode)> error_cbf;
        std::array<std::stack<SglMatrix>, 2> matrix_stacks;
        SglMatrix viewport_mat;
};