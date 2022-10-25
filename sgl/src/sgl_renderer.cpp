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


void SglRenderer::draw_line_low(int x0, int y0, int x1, int y1) {
    int c0, c1, p;
    int d_x = x1 - x0;
    int d_y = y1 - y0;
    int y_step = 1;
    
    if(d_y < 0) {
        y_step = -1;
        d_y = -d_y;
    }
    
    c0 = 2 * d_y;
    c1 = c0 - 2 * d_x;
    p = c0 - d_x;

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

void SglRenderer::draw_line_high(int x0, int y0, int x1, int y1) {
    int c0, c1, p;
    int d_x = x1 - x0;
    int d_y = y1 - y0;
    int x_step = 1;
    
    if(d_x < 0) {
        x_step = -1;
        d_x = -d_x;
    }

    c0 = 2 * d_x;
    c1 = c0 - 2 * d_y;
    p = c0 - d_y;

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


void SglRenderer::draw_line(const SglVertex & start_v, const SglVertex & end_v) {
    int x0 = static_cast<int>(start_v.at(0));
    int y0 = static_cast<int>(start_v.at(1));
    int x1 = static_cast<int>(end_v.at(0));
    int y1 = static_cast<int>(end_v.at(1));
    int d_x = abs(x1 - x0);
    int d_y = abs(y1 - y0);

    if(d_y < d_x) {
        if (x0 > x1) draw_line_low(x1, y1, x0, y0);
        else draw_line_low(x0, y0, x1, y1);
    }
    else {
        if(y0 > y1) draw_line_high(x1, y1, x0, y0);
        else draw_line_high(x0, y0, x1, y1);
    }
}

void SglRenderer::draw_sym_pixels(int x_c, int y_c, int x, int y) {
    state.currentFramebuffer->set_pixel(x_c + x, y_c + y, state.draw_color);
    state.currentFramebuffer->set_pixel(x_c + x, y_c + y, state.draw_color);
    state.currentFramebuffer->set_pixel(x_c - x, y_c + y, state.draw_color);
    state.currentFramebuffer->set_pixel(x_c - x, y_c - y, state.draw_color);
    state.currentFramebuffer->set_pixel(x_c + y, y_c + x, state.draw_color);
    state.currentFramebuffer->set_pixel(x_c + y, y_c - x, state.draw_color);
    state.currentFramebuffer->set_pixel(x_c - y, y_c + x, state.draw_color);
    state.currentFramebuffer->set_pixel(x_c - y, y_c - x, state.draw_color);
}


void SglRenderer::push_vertex(const SglVertex & vertex)
{

    if (state.element_type_mode == SGL_POINTS) {
        state.currentFramebuffer->set_pixel(static_cast<int>(vertex.at(0)), static_cast<int>(vertex.at(1)), state.draw_color);
    }
    
    vertices.push_back(vertex);

    // SGL_DEBUG_OUT("[Vertices] are " + std::to_string(vertices.size()));
    if ((state.element_type_mode == SGL_LINES) && (vertices.size() == 2)) {
        draw_line(vertices[0], vertices[1]);
        vertices.clear();
    }

    else if ((state.element_type_mode == SGL_LINE_STRIP) && (vertices.size() == 2)) {
        draw_line(vertices[0], vertices[1]);
        vertices.erase(vertices.begin());
    }
    
    else if ((state.element_type_mode == SGL_LINE_LOOP) && (vertices.size() >= 2)) {
        if(vertices.size() == 2) {
            draw_line(vertices[0], vertices[1]);
        }
        else if(vertices.size() == 3) {
            SglVertex first = vertices[0];
            draw_line(vertices[1], vertices[2]);
            vertices.erase(vertices.begin()+1);
        }
    }
}

void SglRenderer::draw_circle(const SglVertex & center, int radius) {
    int x, y, p, twoX, twoY;
    int x_c = static_cast<int>(center.at(0));
    int y_c = static_cast<int>(center.at(1));

    x = 0;
    y = radius;
    p = 1 - radius;
    twoX = 0;
    twoY = 2 * radius;

    while (x <= y) {
        // SGL_DEBUG_OUT("[draw_circle] offsets are " + std::to_string(x) + " " + std::to_string(y));
        draw_sym_pixels(x_c, y_c, x, y);
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

void SglRenderer::draw_ellipse(const SglVertex & center, int a, int b) {
    // TODO Sakaci
}

void SglRenderer::draw_arc(const SglVertex & center, int radius, int from, int to) {
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