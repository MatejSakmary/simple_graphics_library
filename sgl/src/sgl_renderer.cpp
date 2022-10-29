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

    if (state.element_type_mode == SGL_POINTS) {
        for(int i = - static_cast<int>(state.point_size / 2); i <= static_cast<int>(state.point_size / 2); ++i) {
            for(int j = - static_cast<int>(state.point_size / 2); j <= static_cast<int>(state.point_size / 2); ++j) {
                state.currentFramebuffer->set_pixel(static_cast<int>(vertex.at(0) + i), static_cast<int>(vertex.at(1) + j), state.draw_color);
            }
        }
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

// void SglRenderer::draw_polyline(int x_c, int y_c, int z_c, const SglVertex & start, const SglVertex & end, SglMatrix mat) {
//     int x0 = static_cast<int>(start.at(0));
//     int y0 = static_cast<int>(start.at(1));
//     int x1 = static_cast<int>(end.at(0));
//     int y1 = static_cast<int>(end.at(1));
//     int d_x = abs(x1 - x0);
//     int d_y = abs(y1 - y0);

//     if(d_y < d_x) {
//         if (x0 > x1) draw_line_low(x1, y1, x0, y0);
//         else draw_line_low(x0, y0, x1, y1);
//     }
//     else {
//         if(y0 > y1) draw_line_high(x1, y1, x0, y0);
//         else draw_line_high(x0, y0, x1, y1);
//     }
// }


void SglRenderer::draw_sym_pixels(int x_c, int y_c, int x, int y) {
    state.currentFramebuffer->set_pixel(x_c + x, y_c + y, state.draw_color);
    state.currentFramebuffer->set_pixel(x_c + x, y_c - y, state.draw_color);
    state.currentFramebuffer->set_pixel(x_c - x, y_c + y, state.draw_color);
    state.currentFramebuffer->set_pixel(x_c - x, y_c - y, state.draw_color);
}

void SglRenderer::draw_sym_pixels_rotated(int x_c, int y_c, int z_c, int x, int y, SglMatrix mat) {
    int xh, yh, x1, y1, x2, y2;
    for(int i = 0; i < 2; ++i) {
        for(int j = 0; j < 2; ++j) {
            SglVertex help = SglVertex(pow(-1, i) * x, pow(-1, j) * y, z_c, 1.0f);
            help = mat * help;
            x2 = x_c + help.at(0);
            y2 = y_c + help.at(1);
            state.currentFramebuffer->set_pixel(static_cast<int>(x2), static_cast<int>(y2), state.draw_color);
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
        draw_sym_pixels(x_c, y_c, y, x);
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

void SglRenderer::draw_ellipse(const SglVertex & center, int a, int b, SglMatrix mat) {
    int d_x, d_y, p, x, y, b2, a2;
    int x_c = static_cast<int>(center.at(0));
    int y_c = static_cast<int>(center.at(1));
    int z_c = static_cast<int>(center.at(2));

    // float cos_a = mat.at(0, 0);
    // float sin_a = mat.at(0, 1);

    x = 0;
    y = b;
    a2 = a * a;
    b2 = b * b;
    d_x = 2 * b2 * x;
    d_y = 2 * a2 * y;
    
    p = b2 - (a2 * b) + (0.25 * a2);
    
    while (d_x < d_y) {
        draw_sym_pixels_rotated(x_c, y_c, z_c, x, y, mat);
        if (p >= 0) {
            --y;
            d_y -= 2 * a2;
            p -= d_y;
        }
        ++x;
        d_x += 2 * b2;
        p += d_x + b2;
    }

    p = (b2 * ((x + 0.5) * (x + 0.5))) + (a2 * ((y - 1) * (y - 1))) - (a2 * b2);
    
    while (y >= 0) {
        draw_sym_pixels_rotated(x_c, y_c, z_c, x, y, mat);
        if (p <= 0)
        {
            x++;
            d_x += 2 * b2;
            p += d_x;
        }
        --y;
        d_y -= 2 * a2;
        p += a2 - d_y;
    }
}

void SglRenderer::draw_arc(const SglVertex & center, int radius, float from, float to, SglMatrix mat) {
    float x1, x2, y1, y2;
    int x_c, y_c, z_c;
    int x11, y11, x22, y22;

    float angle = to - from;
    // SGL_DEBUG_OUT("Angle is " + std::to_string(angle));
    int N = round(40 * angle / (2 * M_PI));
    // SGL_DEBUG_OUT("N is " + std::to_string(N));
    float alpha = angle / N;
    // SGL_DEBUG_OUT("Angle is " + std::to_string(alpha));
    float SA = sin(alpha);
    float CA = cos(alpha);
    // SGL_DEBUG_OUT("SA is " + std::to_string(SA));

    x_c = static_cast<int>(center.at(0));
    y_c = static_cast<int>(center.at(1));
    z_c = static_cast<int>(center.at(2));

    x1 = radius * cos(from);
    y1 = radius * sin(from);

    for (int i = 0; i < N; ++i) {
        x2 = CA * x1 - SA * y1;
        y2 = SA * x1 + CA * y1;
        
        SglVertex start = SglVertex(x1, y1, z_c, center.at(3));        
        start = mat * start;
        x11 = start.at(0) + x_c;
        y11 = start.at(1) + y_c;
        start = SglVertex(x11, y11, z_c, start.at(3));
        
        SglVertex end = SglVertex(x2, y2, z_c, center.at(3));        
        end = mat * end;
        x22 = end.at(0) + x_c;
        y22 = end.at(1) + y_c;
        end = SglVertex(x22, y22, z_c, end.at(3));
        
        SGL_DEBUG_OUT("START " + std::to_string(start.at(0)) + " " + std::to_string(start.at(1)));
        SGL_DEBUG_OUT("END " + std::to_string(end.at(0)) + " " + std::to_string(end.at(1)));
        
        draw_line(start, end);
        x1 = x2;
        y1 = y2;
    }
}

void SglRenderer::recording_start()
{
}

void SglRenderer::recording_end()
{
    if(state.element_type_mode == SGL_LINE_LOOP && vertices.size() == 2) {
        draw_line(vertices[0], vertices[1]);
    }
    vertices.clear();
}