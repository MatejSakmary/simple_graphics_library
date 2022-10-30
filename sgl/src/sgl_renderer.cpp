#include "sgl_renderer.hpp"
#include <algorithm>
#include <list>
#include <numeric>

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

    if (state.area_mode == sglEAreaMode::SGL_FILL && state.element_type_mode == sglEElementType::SGL_POLYGON)
    {
        vertices.push_back(vertex);
        return;
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
    
    else if (((state.element_type_mode == SGL_LINE_LOOP) || 
              (state.element_type_mode == SGL_POLYGON && state.area_mode == SGL_LINE)) &&
            (vertices.size() >= 2)) {
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
    
    for(int i=x0+1; i <= x1; ++i) {
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
    
    for(int i=y0+1; i <= y1; ++i) {
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

void SglRenderer::draw_circle(const SglVertex & center, float radius) {
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

void SglRenderer::draw_ellipse(const SglVertex & center, float a, float b, SglMatrix mat) {
    float x1, x2, y1, y2;

    int N = 40;
    float alpha = 2 * M_PI / N;

    x1 = a;
    y1 = 0;

    mat = mat * SglMatrix({
        .type = MatrixType::TRANSLATE,
        .x = center.at(0), .y = center.at(1), .z = center.at(2)
    });
    
    auto rot_mat = SglMatrix({ .type = MatrixType::ROTATE, .x = 0.0f, .y = 0.0f, .z = alpha });


    for (int i = 1; i <= N; ++i) {      
        x2 = a * cos(i * alpha);
        y2 = b * sin(i * alpha);

        SglVertex start = SglVertex(x1, y1, center.at(2), center.at(3));  
        SglVertex end = SglVertex(x2, y2, center.at(2), center.at(3));

        // SGL_DEBUG_OUT("START " + std::to_string(start.at(0)) + " " + std::to_string(start.at(1)));
        // SGL_DEBUG_OUT("END " + std::to_string(end.at(0)) + " " + std::to_string(end.at(1)));
        
        draw_line(mat * start, mat * end);

        x1 = x2;
        y1 = y2;
    }
}

void SglRenderer::draw_arc(const SglVertex & center, float radius, float from, float to, SglMatrix mat) {
    float x1, x2, y1, y2;
    int x_c, y_c, z_c;
    int x11, y11, x22, y22;

    float angle = to - from;
    // SGL_DEBUG_OUT("Angle is " + std::to_string(angle));
    int N = int((40.0f * angle) / (2.0f * M_PI));
    // SGL_DEBUG_OUT("N is " + std::to_string(N));
    float alpha = angle / N;
    // SGL_DEBUG_OUT("Angle is " + std::to_string(alpha));
    float SA = sin(alpha);
    float CA = cos(alpha);
    // SGL_DEBUG_OUT("SA is " + std::to_string(SA));

    x1 = radius * cos(from);
    y1 = radius * sin(from);

    mat = mat * SglMatrix({
        .type = MatrixType::TRANSLATE,
        .x = center.at(0), .y = center.at(1), .z = center.at(2)
    }); 
    
    auto rot_mat = SglMatrix({ .type = MatrixType::ROTATE, .x = 0.0f, .y = 0.0f, .z = alpha });

    SglVertex start = SglVertex(x1, y1, z_c, center.at(3));        

    for (int i = 0; i < N; ++i) {
        auto end = rot_mat * start;
        
        // SGL_DEBUG_OUT("START " + std::to_string(start.at(0)) + " " + std::to_string(start.at(1)));
        // SGL_DEBUG_OUT("END " + std::to_string(end.at(0)) + " " + std::to_string(end.at(1)));
        
        draw_line(mat * start, mat * end);

        start = end;
    }
}

void SglRenderer::draw_fill_object()
{
    struct SglEdge
    {
        // each edge is oriented in such a way so that *from* is the upper points
        // and *to* is the lower point (from.second is always bigger than to.second)
        // TODO(msakmary) remove x coordinates from *from* and *to* they are not needed
        std::pair<uint, uint> from;
        std::pair<uint, uint> to;
        float upper_x;
        float step;
    };

    std::vector<SglEdge> edges;
    edges.reserve(vertices.size());
    // vectors of indices into edges
    std::list<uint> active_edges;
    std::list<uint> inactive_edges;

    // NOTE(msakmary) framebuffer (0,0) is bottom left corner
    auto insert_processed_edge = [&edges](const SglVertex & from, const SglVertex & to) -> bool
    {
        // 2) remove horizontal edges
        if(from.at(1) == to.at(1)) { return false; }
        // 1) orient edges top - bottom and shorten them by one pixel (this is in to.at(1) + 1.0f)
        if(from.at(1) < to.at(1)) 
        { 
            edges.emplace_back(SglEdge{
                .from{to.at(0), to.at(1)},
                .to{from.at(0), from.at(1) + 1.0f}
            });
        }
        else
        {
            edges.emplace_back(SglEdge{
                .from{from.at(0), from.at(1)},
                .to{to.at(0), to.at(1) + 1.0f}
            });
        } 
        // x_step = (x_to - x_from) / (y_to - y_from)
        auto & curr_edge = edges.back();
        curr_edge.step = (static_cast<float>(curr_edge.to.first) - static_cast<float>(curr_edge.from.first)) / 
                         (static_cast<float>(curr_edge.to.second - 1.0f) - static_cast<float>(curr_edge.from.second));
        curr_edge.upper_x = curr_edge.from.first;
        return true;
    };

    // add the first vertex again into vertices - this is because we need to add an edge connecting the
    // last vertex to the first vertex and this makes it a simple forloop later
    vertices.push_back(vertices.at(0));
    // each edge consists of two vertices so we start at one so we already have two vertices at the start of the loop
    for(size_t i = 1; i < vertices.size(); i++)
    {
        insert_processed_edge(vertices.at(i - 1), vertices.at(i));
    }

    // sort inactive edges by y_upper so that later I can stop inactive checking early
    std::sort(edges.begin(), edges.end(), [](const SglEdge & first, const SglEdge & second) -> bool
        { return first.from.second > second.from.second; });

    // fill inactive edges with indices
    inactive_edges.resize(edges.size());
    std::iota(inactive_edges.begin(), inactive_edges.end(), 0);

    // check inactive edges and add new active edges if there are some
    auto check_inactive_edges = [&edges, &inactive_edges, &active_edges](uint y){
        std::vector<uint> to_remove_edges;
        for(const auto & edge : inactive_edges)
        {
            // if the edge start is equal to y we hit it and we should 
            // move it to the active lists 
            if(edges.at(edge).from.second == y) 
            { 
                SGL_DEBUG_OUT("edge at idx: " + std::to_string(edge) + " is hit at y: " + std::to_string(y) + " adding to active");
                active_edges.push_back(edge);
                to_remove_edges.push_back(edge);
            }
        }
        std::for_each(to_remove_edges.begin(), to_remove_edges.end(), [&inactive_edges](const uint remove_edge) { inactive_edges.remove(remove_edge); });
    };

    // check if any active edge is done processing and should be removed
    auto check_active_edges = [&edges, &active_edges](uint y){
        std::vector<uint> to_remove_edges;
        for(const auto & edge : active_edges)
        {
            // if the edge start is equal to y we hit it and we should 
            // move it to the active lists 
            if(y < edges.at(edge).to.second) 
            { 
                SGL_DEBUG_OUT("edge at idx: " + std::to_string(edge) + " is finished at y: " + std::to_string(y) + " deleting from active");
                to_remove_edges.push_back(edge);
            }
        }
        std::for_each(to_remove_edges.begin(), to_remove_edges.end(), [&active_edges](const uint remove_edge) { active_edges.remove(remove_edge); });
    };

    // sort active edges by x intersection
    auto shake_sort_active_edges = [&edges, &active_edges](){
        // cocktail sort copied from https://www.geeksforgeeks.org/cocktail-sort/ and modified for lists using iterators
        bool swapped = true;
        int start = 0;
 
        while (swapped) {
            swapped = false;
 
            auto one_before_end_it = --active_edges.end();
            for (auto it = active_edges.begin(); it != one_before_end_it; ++it) 
            {
                // list iterators don't have + operator defined thus we must only use ++ or -- operations
                // which would modify the current iterator and the forloop condition would then cause it to
                // increase/decrease twice. So we need to create a copy of the existing iterator and 
                // move that copy to access next element

                // TODO(msakmary) This is very cursed, improve if there is time
                auto it_ = it;
                if (edges.at(*it).upper_x > edges.at(*(++it_)).upper_x) 
                {
                    std::swap(*it, *it_);
                    swapped = true;
                }
            }
 
            if (!swapped){ break; }
            swapped = false;
 
            for (auto it = --active_edges.end(); it != active_edges.begin(); --it) 
            {
                auto it_ = it;
                // TODO(msakmary) This is very cursed, improve if there is time
                if (edges.at(*(--it_)).upper_x > edges.at(*it).upper_x) 
                    std::swap(*it, *it_);
                    swapped = true;
            }
        }
    };

    auto draw_active_edges = [this, &edges, &active_edges](uint y){
        for(auto it = active_edges.begin(); it != active_edges.end(); )
        {
            auto & start = edges.at(*(it++));
            auto & end = edges.at(*(it++));
            // SGL_DEBUG_OUT("Filling row of pixels at " + std::to_string(y) + " bounded (" + std::to_string(start.upper_x) + "," + std::to_string(end.upper_x) + ")"); 
            for(uint x = static_cast<uint>(start.upper_x); x <= static_cast<uint>(end.upper_x); x++) 
            {
                this->state.currentFramebuffer->set_pixel(x, y, this->state.draw_color);
            }
            start.upper_x -= start.step;
            end.upper_x -= end.step;
        }
    };

    // TODO(msakmary) Add Bounding box to polygon so that I don't iterate over the entire screen
    for(int y = state.currentFramebuffer->get_height(); y >= 0 ; y--)
    {
        // add hit inactive edges to active list (when y == edge.from.y)
        check_inactive_edges(y);
        // remove processed edges from active list (when y < edge.to.y)
        check_active_edges(y);
        // sort based on x intersections
        shake_sort_active_edges();

        //draw active edges + update x value
        draw_active_edges(y);
    }
    vertices.clear();
}

void SglRenderer::recording_start()
{
}

void SglRenderer::recording_end()
{
    if(state.element_type_mode == SGL_LINE_LOOP && vertices.size() == 2) {
        draw_line(vertices[0], vertices[1]);
    }
    // TODO(msakmary) Add other objects which are allowed with SGL_FILL - triangle, arc etc...
    if(state.element_type_mode == SGL_POLYGON && state.area_mode == SGL_FILL)
    {
        draw_fill_object();
    }
    vertices.clear();
}