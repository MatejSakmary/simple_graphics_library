#pragma once

#include <vector>

#include "sgl.h"
#include "sgl_vertex.hpp"
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

    private:
        friend struct SglCore;
        std::vector<SglVertex> vertices;

        void recording_start();
        void recording_end();
};