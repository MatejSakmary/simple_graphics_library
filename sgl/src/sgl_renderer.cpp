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
       .depth_test = false,
       .currentFramebuffer = nullptr,
       .defining_scene = false
    },
    scene{ Scene() },
    materials{},
    vertices{} {}

SglRenderer::~SglRenderer() {}



void SglRenderer::push_vertex(const SglVertex & vertex)
{
    // TODO(osakaci) This function is in great need of rewrite -> I recommend look up table which describes what to draw
    // READY FOR REVIEW

    switch (state.element_type_mode) {
        case sglEElementType::SGL_POINTS:
            draw_point(vertex);
            break;
        
        case sglEElementType::SGL_LINES:
            vertices.push_back(vertex);
            // If there are 2 vertices already in vertex vector we will draw them
            // and clear the vector.
            if (vertices.size() == 2) {
                draw_line(vertices[0], vertices[1]);
                vertices.clear();
            }
            break;
        
        case sglEElementType::SGL_LINE_STRIP:
            vertices.push_back(vertex);
            // If there are 2 vertices in the vertex vector we will draw them
            // and then we erase the older vector.
            if (vertices.size() == 2) {
                draw_line(vertices[0], vertices[1]);
                vertices.erase(vertices.begin());
            }
            break;
        
        case sglEElementType::SGL_LINE_LOOP:
            vertices.push_back(vertex);
            // TODO sakacond add comment
            if(vertices.size() == 2) {
                draw_line(vertices[0], vertices[1]);
            }
            else if(vertices.size() == 3) {
                SglVertex first = vertices[0];
                draw_line(vertices[1], vertices[2]);
                vertices.erase(vertices.begin()+1);
            }
            break;

        case sglEElementType::SGL_POLYGON:
            vertices.push_back(vertex);
            // TODO sakacond add comment
            if (state.defining_scene && vertices.size() == 3) {
                if (materials.size() != 0) { 
                    Polygon poly;
                    poly.material = materials.back();
                    for (int i = 0; i < 3; ++i)
                        poly.vertices.push_back(vertices.at(i));
                    scene.primitives.push_back(poly);
                    vertices.clear();
                }
                else {
                    //TODO sakacond ERROR 
                }
            }
            else {
                if ((state.area_mode == sglEAreaMode::SGL_LINE) && (vertices.size() >= 2)) {
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
            break;
        
        case sglEElementType::SGL_TRIANGLES:
            vertices.push_back(vertex);
            if (vertices.size() % 3 == 0) {
                int last_index = static_cast<int>(vertices.size()) - 1;
                draw_line(vertices.at(last_index), vertices.at(last_index -1));
                draw_line(vertices.at(last_index - 1), vertices.at(last_index - 2));
                draw_line(vertices.at(last_index - 2), vertices.at(last_index));
            }
        default:
            break;
    }
}

void SglRenderer::push_sphere(const SglVertex & center, float radius) {
    if (state.defining_scene) {
        Sphere sphere;
        sphere.material = materials.back();
        sphere.center = center;
        sphere.radius = radius;
        if (materials.size() != 0) scene.primitives.push_back(sphere);
    }
    // TODO ERRORS
}

void SglRenderer::draw_point(const SglVertex & point) {
    int half_size = static_cast<int>(state.point_size / 2);
            
    // If the point size is 1 the half size will be 0 therefore both
    // for loops will happen only once and draw the point itself
    // otherwise it will create a square point around the vertex
    for(int i = -half_size; i <= half_size; ++i) {
        for(int j = -half_size; j <= half_size; ++j) {
            int x = static_cast<int>(point.at(0) + i);
            int y = static_cast<int>(point.at(1) + j);
            state.currentFramebuffer->set_pixel(x, y, state.draw_color);
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


void SglRenderer::draw_sym_pixels(int x_c, int y_c, int z_c, int x, int y) {

    if(state.area_mode != sglEAreaMode::SGL_FILL) {
        state.currentFramebuffer->set_pixel(x_c + x, y_c + y, state.draw_color);
        state.currentFramebuffer->set_pixel(x_c + x, y_c - y, state.draw_color);
        state.currentFramebuffer->set_pixel(x_c - x, y_c + y, state.draw_color);
        state.currentFramebuffer->set_pixel(x_c - x, y_c - y, state.draw_color);
    }
    
    else {
        SglVertex start1 =  SglVertex(x_c + x, y_c + y - 1, z_c, 1.0f);
        SglVertex end1 =    SglVertex(x_c + x, y_c - y + 1, z_c, 1.0f);
        SglVertex start2 =  SglVertex(x_c - x, y_c + y - 1, z_c, 1.0f);
        SglVertex end2 =    SglVertex(x_c - x, y_c - y + 1, z_c, 1.0f);
        draw_line(start1, end1);
        draw_line(start2, end2);
    }
}

void SglRenderer::draw_sym_pixels_rotated(int x_c, int y_c, int z_c, int x, int y, SglMatrix mat) {
    int x1, y1;
    for(int i = 0; i < 2; ++i) {
        for(int j = 0; j < 2; ++j) {
            SglVertex help = SglVertex(pow(-1, i) * x, pow(-1, j) * y, z_c, 1.0f);
            help = mat * help;
            x1 = x_c + help.at(0);
            y1 = y_c + help.at(1);
            state.currentFramebuffer->set_pixel(static_cast<int>(x1), static_cast<int>(y1), state.draw_color);
        }
    }
}

void SglRenderer::draw_circle(const SglVertex & center, float radius) {
    if (state.area_mode == sglEAreaMode::SGL_POINT) {
        push_vertex(center);
        return;
    }

    int x, y, p, twoX, twoY;
    int x_c = static_cast<int>(center.at(0));
    int y_c = static_cast<int>(center.at(1));
    int z_c = static_cast<int>(center.at(2));

    x = 0;
    y = radius;
    p = 1 - radius;
    twoX = 0;
    twoY = 2 * radius;

    while (x <= y) {
        // SGL_DEBUG_OUT("[draw_circle] offsets are " + std::to_string(x) + " " + std::to_string(y));
        if (abs(x) < radius || state.area_mode != SGL_FILL) {
            draw_sym_pixels(x_c, y_c, z_c, x, y);
            draw_sym_pixels(x_c, y_c, z_c, y, x);
        }
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
    if (state.area_mode == sglEAreaMode::SGL_POINT) {
        push_vertex(mat * center);
        return;
    }
    float x1, x2, y1, y2;

    int N = 40;
    float alpha = 2 * M_PI / N;

    x1 = a;
    y1 = 0;

    mat = mat * SglMatrix({
        .type = MatrixType::TRANSLATE,
        .x = center.at(0), .y = center.at(1), .z = center.at(2)
    });

    for (int i = 1; i <= N; ++i) {      
        x2 = a * cos(i * alpha);
        y2 = b * sin(i * alpha);

        SglVertex start = SglVertex(x1, y1, center.at(2), center.at(3));  
        SglVertex end = SglVertex(x2, y2, center.at(2), center.at(3));

        draw_line(mat * start, mat * end);
        
        if(i == 1) vertices.push_back(mat * start);
        vertices.push_back(mat * end);

        x1 = x2;
        y1 = y2;
    }

    if (state.area_mode == sglEAreaMode::SGL_FILL) draw_fill_object();
}

void SglRenderer::draw_arc(const SglVertex & center, float radius, float from, float to, SglMatrix mat) {

    if (state.area_mode == sglEAreaMode::SGL_POINT) {
        push_vertex(mat * center);
        return;
    }

    float x1, y1;
    int z_c = center.at(2);

    float angle = to - from;
    int N = int((40.0f * angle) / (2.0f * M_PI));
    float alpha = angle / N;
    
    x1 = radius * cos(from);
    y1 = radius * sin(from);

    mat = mat * SglMatrix({
        .type = MatrixType::TRANSLATE,
        .x = center.at(0), .y = center.at(1), .z = center.at(2)
    }); 
    
    auto rot_mat = SglMatrix({ .type = MatrixType::ROTATE, .x = 0.0f, .y = 0.0f, .z = alpha });

    SglVertex start = SglVertex(x1, y1, z_c, center.at(3));

    vertices.push_back(mat * start);

    for (int i = 0; i < N; ++i) {
        auto end = rot_mat * start;
        
        draw_line(mat * start, mat * end);
        
        vertices.push_back(mat * end);
        
        start = end;
    }

    vertices.push_back(mat * center);

    if (state.area_mode == sglEAreaMode::SGL_FILL) draw_fill_object();
}

void SglRenderer::draw_fill_object()
{
    struct SglEdge
    {
        // each edge is oriented in such a way so that *from* is the upper points
        // and *to* is the lower point (from.second is always bigger than to.second)
        // TODO(msakmary) remove x coordinates from *from* and *to* they are not needed
        uint from_y;
        uint to_y;
        float upper_x;
        float upper_z;
        float step_x;
        float step_z;
    };

    struct AABB
    {
        int upRight[2];
        int downLeft[2];

        AABB(int max_x, int max_y, int min_x, int min_y) {
            this->upRight[0] = max_x;
            this->upRight[1] = max_y;
            this->downLeft[0] = min_x;
            this->downLeft[1] = min_y;
        }
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
        // TODO(msakmary) CONTINUE HERE - store 1 - 1/z into the depth buffer
        if(from.at(1) < to.at(1)) 
        { 
            edges.emplace_back(SglEdge{
                .from_y  = static_cast<uint>(to.at(1)),
                .to_y    = static_cast<uint>(from.at(1) + 1.0f),
                .upper_x = to.at(0),
                .upper_z = to.at(2),
                // x_step = (x_from - x_to) / (y_from - y_to)
                .step_x  = (from.at(0) - to.at(0)) / ((from.at(1)) - to.at(1)),
                // x_step = (z_from - z_to) / (y_from - y_to)
                .step_z  = (from.at(2) - to.at(2)) / ((from.at(1)) - to.at(1)),
            });
        }
        else
        {
            edges.emplace_back(SglEdge{
                .from_y  = static_cast<uint>(from.at(1)),
                .to_y    = static_cast<uint>(to.at(1) + 1.0f),
                .upper_x = from.at(0),
                .upper_z = from.at(2),
                // x_step = (x_to - x_from) / (y_to - y_from)
                .step_x  = (to.at(0) - from.at(0)) / ((to.at(1)) - from.at(1)),
                // x_step = (z_to - z_from) / (y_to - y_from)
                .step_z  = (to.at(2) - from.at(2)) / ((to.at(1)) - from.at(1)),
            });
        } 
        return true;
    };

    int max_x = 0;
    int max_y = 0;
    int min_x = state.currentFramebuffer->get_width();
    int min_y = state.currentFramebuffer->get_height();
    // add the first vertex again into vertices - this is because we need to add an edge connecting the
    // last vertex to the first vertex and this makes it a simple forloop later
    vertices.push_back(vertices.at(0));
    if (vertices.at(0).at(0) > max_x) max_x = vertices.at(0).at(0);
    if (vertices.at(0).at(1) > max_y) max_y = vertices.at(0).at(1);
    if (vertices.at(0).at(0) < min_x) min_x = vertices.at(0).at(0);
    if (vertices.at(0).at(1) < min_y) min_y = vertices.at(0).at(1);
    // each edge consists of two vertices so we start at one so we already have two vertices at the start of the loop
    for(size_t i = 1; i < vertices.size(); i++)
    {
        SGL_DEBUG_OUT("1/z of current vertex is " + std::to_string(vertices.at(i-1).at(2)) + " giving depth of " + std::to_string(1.0f / vertices.at(i-1).at(2)));
        insert_processed_edge(vertices.at(i - 1), vertices.at(i));


        if (vertices.at(i).at(0) > max_x) max_x = vertices.at(i).at(0);
        if (vertices.at(i).at(1) > max_y) max_y = vertices.at(i).at(1);
        if (vertices.at(i).at(0) < min_x) min_x = vertices.at(i).at(0);
        if (vertices.at(i).at(1) < min_y) min_y = vertices.at(i).at(1);
    }

    AABB boundingBox = AABB(max_x, max_y, min_x, min_y);

    // sort inactive edges by y_upper so that later I can stop inactive checking early
    std::sort(edges.begin(), edges.end(), [](const SglEdge & first, const SglEdge & second) -> bool
        { return first.from_y > second.from_y; });

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
            if(edges.at(edge).from_y == y) 
            { 
                // SGL_DEBUG_OUT("edge at idx: " + std::to_string(edge) + " is hit at y: " + std::to_string(y) + " adding to active");
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
            if(y < edges.at(edge).to_y) 
            { 
                // SGL_DEBUG_OUT("edge at idx: " + std::to_string(edge) + " is finished at y: " + std::to_string(y) + " deleting from active");
                to_remove_edges.push_back(edge);
            }
        }
        std::for_each(to_remove_edges.begin(), to_remove_edges.end(), [&active_edges](const uint remove_edge) { active_edges.remove(remove_edge); });
    };

    // sort active edges by x intersection
    auto shake_sort_active_edges = [&edges, &active_edges](){
        // cocktail sort copied from https://www.geeksforgeeks.org/cocktail-sort/ and modified for lists using iterators
        bool swapped = true;
 
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
                if (edges.at(*(--it_)).upper_x > edges.at(*it).upper_x) {
                    std::swap(*it, *it_);
                    swapped = true;
                }
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
                if(this->state.depth_test)
                {
                    // lerp between start.z and end.z based on the x parameter
                    float t = (static_cast<float>(x) - start.upper_x) / (end.upper_x - start.upper_x);
                    if(x == static_cast<uint>(start.upper_x)) { t = 0.0f; }

                    float depth = ((1.0f - t) * start.upper_z) + (t * end.upper_z); 
                    // there is something closer in the depth buffer -> ignore this write
                    if(depth > this->state.currentFramebuffer->get_depth(x, y)) { continue; }
                    this->state.currentFramebuffer->set_pixel(x, y, this->state.draw_color);
                    this->state.currentFramebuffer->set_depth(x, y, depth);
                } else {
                    this->state.currentFramebuffer->set_pixel(x, y, this->state.draw_color);
                }
            }
            start.upper_x -= start.step_x;
            start.upper_z -= start.step_z;
            end.upper_x -= end.step_x;
            end.upper_z -= end.step_z;
        }
    };

    // ============================ SCANLINE MAIN LOOP =========================================
    // TODO(msakmary) Add Bounding box to polygon so that I don't iterate over the entire screen
    for(int y = boundingBox.upRight[1]; y >= boundingBox.downLeft[1] ; y--)
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

    SGL_DEBUG_OUT("Number of vertices: " + std::to_string(vertices.size()));
    // TODO(msakmary) Add other objects which are allowed with SGL_FILL - triangle, arc etc...
    if (state.area_mode == sglEAreaMode::SGL_FILL) {
        if (state.element_type_mode == sglEElementType::SGL_TRIANGLES || 
            state.element_type_mode == sglEElementType::SGL_POLYGON) 
        {
            draw_fill_object();
        }
    }

    vertices.clear();
}


// Raytrace functions

void SglRenderer::push_material(
        float r,
        float g,
        float b,
        float kd,
        float ks,
        float shine,
        float T,
        float ior) 
{
    materials.push_back(Material(r, g, b, kd, ks, shine, T, ior));
}

void SglRenderer::raytrace_sphere(const Sphere & sphere) {
    // TODO sakacond with raytracing
}


void SglRenderer::raytrace_polygon(const Polygon & polygon) {
    // TODO sakacond with raytracing
}


void SglRenderer::raytrace_scene() {
    for (Sphere s: scene.spheres) raytrace_sphere(s);
    for (Polygon p: scene.polygons) raytrace_polygon(p);
}


void SglRenderer::draw_sphere(const Sphere & sphere) {

}

void SglRenderer::draw_polygon(const Polygon & polygon) {

}


void SglRenderer::rasterize_scene() {
    for (Sphere s: scene.spheres) draw_sphere(s);
    for (Polygon p: scene.polygons) draw_polygon(p);
}