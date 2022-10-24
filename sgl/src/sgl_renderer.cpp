#include "sgl_renderer.hpp"

SglRenderer::SglRenderer() : 
    state{
       .draw_color = Pixel{.r = 0.0f, .g = 0.0f, .b = 0.0f} ,
       .point_size = 1.0f,
       .area_mode = sglEAreaMode::SGL_FILL,
       .element_type_mode = sglEElementType::SGL_POINTS,
       .currentFramebuffer = nullptr
    },
    vertices{} {}

SglRenderer::~SglRenderer() {}

void SglRenderer::push_vertex(const SglVertex & vertex)
{
}

void SglRenderer::recording_start()
{
}

void SglRenderer::recording_end()
{
    vertices.clear();
}