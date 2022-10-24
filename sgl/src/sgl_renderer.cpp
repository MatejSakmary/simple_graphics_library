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


void init(int c0, int c1, int p, int d_x, int d_y) {
    c0 = 2 * d_y;
    c1 = c0 - 2 * d_x;
    p = c0 - d_x;
}

void SglRenderer::draw_line_low(float x0, float y0, float x1, float y1) {
    int c0, c1, p;
    float d_x = x1 - x0;
    float d_y = y1 - y0;
    float y_step = 1;
    
    if(d_y < 0) {
        y_step = -1;
        d_y = -d_y;
    }

    init(c0, c1, p, d_x, d_y);

    state.currentFramebuffer->set_pixel(x0, y0, state.draw_color);
    
    for(int i=x0+1; i < x1; ++i) {
        if(p<0) {
            p += c0;
        } else {
            p += c1;
            y0 += y_step;
        }
    
        state.currentFramebuffer->set_pixel(i, y0, state.draw_color);
    }
}

void SglRenderer::draw_line_high(float x0, float y0, float x1, float y1) {
    int c0, c1, p;
    float d_x = x1 - x0;
    float d_y = y1 - y0;
    float x_step = 1;
    
    if(d_x < 0) {
        x_step = -1;
        d_x = -d_x;
    }

    init(c0, c1, p, d_y, d_x);

    state.currentFramebuffer->set_pixel(x0, y0, state.draw_color);
    
    for(int i=y0+1; i < y1; ++i) {
        if(p<0) {
            p += c0;
        } else {
            p += c1;
            x0 += x_step;
        }
    
        state.currentFramebuffer->set_pixel(x0, i, state.draw_color);
    }
}


void SglRenderer::draw_line(SglVertex & start_v, SglVertex & end_v) {
    float x0 = start_v.at(0);
    float y0 = start_v.at(1);
    float x1 = end_v.at(0);
    float y1 = end_v.at(1);
    float d_x = abs(x1 - x0);
    float d_y = abs(y1 - y0);
    // float k = d_y / d_x;

    if(d_y < d_x) {
        if (x0 > x1) draw_line_low(x1, y1, x0, y0);
        else draw_line_low(x0, y0, x1, y1);
    }
    else {
        if(y0 > y1) draw_line_high(x1, y1, x0, y0);
        else draw_line_high(x0, y0, x1, y1);
    }
}


void SglRenderer::push_vertex(const SglVertex & vertex)
{
    vertices.push_back(vertex);

    state.currentFramebuffer->set_pixel(static_cast<int>(vertex.at(0)), static_cast<int>(vertex.at(1)), state.draw_color);

    if ((state.element_type_mode == SGL_LINES) & (vertices.size() == 2)) {
        draw_line(vertices[0], vertices[1]);
        vertices.clear();
    }
    else if ((state.element_type_mode == SGL_LINE_STRIP) & vertices.size() == 2) {
        draw_line(vertices[0], vertices[1]);
        vertices.clear();
        vertices.push_back(vertex);
    }
    else if ((state.element_type_mode == SGL_LINE_LOOP) & vertices.size() >= 2) {
        if(vertices.size() == 2) {
            draw_line(vertices[0], vertices[1]);
        }
        if(vertices.size() == 3) {
            SglVertex first = vertices[0];
            draw_line(vertices[1], vertices[2]);
            vertices.clear();
            vertices.push_back(first);
            vertices.push_back(vertex);
        }
    }
}

void SglRenderer::draw_circle(const SglVertex & center, float radius) {
    // TODO Sakaci Midpoint alg
    int x, y, p, twoX, twoY;
    x = 0;
    y = radius;
    p = 1 - radius;
    twoX = 0;
    twoY = 2 * radius;

    while (x <= y) {
        //set_sym_pixel(x,y);
        if (p > 0) {
            p = p - twoY + 2;
            twoY -= 2;
            y -= 1; 
        }
        p = p + twoX + 3;
        twoX += 2;
        x += 1;
    }
}

void SglRenderer::draw_ellipse(const SglVertex & center, float a, float b) {
    // TODO Sakaci
}

void SglRenderer::draw_arc(const SglVertex & center, float radius, float from, float to) {
    // TODO Sakaci
}

void SglRenderer::recording_start()
{
    // init vertices?
    // init frame_buffer?
}

void SglRenderer::recording_end()
{
    if(state.element_type_mode == SGL_LINE_LOOP && vertices.size() == 2) {
        draw_line(vertices[0], vertices[1]);
    }
    vertices.clear();
}