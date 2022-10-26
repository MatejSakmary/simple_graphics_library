#pragma once

#include <vector>

#include "sgl.h"
#include "sgl_vertex.hpp"
#include "sgl_matrix.hpp"
#include "sgl_frambuffer.hpp"

// Forward decl vertex so we can friend it 
struct SglCore;

struct DrawState
{
    Pixel draw_color;
    float point_size;
    sglEAreaMode area_mode;
    sglEElementType element_type_mode;
    SglFramebuffer* currentFramebuffer;
};

struct SglRenderer
{
    DrawState state;
    SglRenderer();
    ~SglRenderer();

    void push_vertex(const SglVertex & vertex);
    void draw_circle(const SglVertex & center, int radius);
    void draw_ellipse(const SglVertex & center, int a, int b, SglMatrix mat);
    void draw_arc(const SglVertex & center, int radius, int from, int to);

    private:
        friend struct SglCore;
        std::vector<SglVertex> vertices;

        void draw_line_low(int x0, int y0, int x1, int y1);
        void draw_line_high(int x0, int y0, int x1, int y1);
        void draw_line(const SglVertex & start_v, const SglVertex & end_v);
        void draw_sym_pixels(int x_c, int y_c, int x, int y);
        void draw_sym_pixels_rotated(int x_c, int y_c, int z_c, int x, int y, SglMatrix mat);

        void recording_start();
        void recording_end();
};