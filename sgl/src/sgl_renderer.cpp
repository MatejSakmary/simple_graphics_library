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

template<typename Base, typename T>
inline bool instanceof(const T *ptr) {
   return dynamic_cast<const Base*>(ptr) != nullptr;
}

void draw_line(const SglVertex & start_v, const SglVertex & end_v) {
    float x_s = start_v.vert[0];
    float y_s = start_v.vert[1];
    float x_e = end_v.
}


void SglRenderer::push_vertex(const SglVertex & vertex)
{
    vertices.push_back(vertex);
    
    if ((state.element_type_mode == SGL_LINE) & (vertices.size() == 2)) {
        draw_line(vertices[0], vertices[1]);
        vertices.clear();
    }
    // else if ((state.element_type_mode ==SGL_LINE_STRIP ))
}

void SglRenderer::recording_start()
{
    // init vertices?
}

void SglRenderer::recording_end()
{
    vertices.clear();
}