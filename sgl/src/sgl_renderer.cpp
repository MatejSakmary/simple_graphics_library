#include "sgl_renderer.hpp"
#include <algorithm>
#include <list>
#include <numeric>

#pragma GCC optimize("O3,unroll-loops,fast-math")

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



void SglRenderer::push_vertex(const f32vec4 & vertex)
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
                draw_line(vertices[1], vertices[2]);
                vertices.erase(vertices.begin()+1);
            }
            break;

        case sglEElementType::SGL_POLYGON:
            vertices.push_back(vertex);
            // TODO sakacond add comment
            if (state.defining_scene && vertices.size() == 3) {
                if (!materials.empty()) {
                    Polygon *poly = new Polygon(vertices.at(0), vertices.at(1), vertices.at(2), materials.size() - 1);
                    scene.objects.push_back(poly);
                    vertices.clear();
                }
                else {
                    //TODO sakacond ERROR material not defined
                }
            }
            else {
                if ((state.area_mode == sglEAreaMode::SGL_LINE) && (vertices.size() >= 2)) {
                    if(vertices.size() == 2) {
                        draw_line(vertices[0], vertices[1]);
                    }
                    else if(vertices.size() == 3) {
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

void SglRenderer::push_sphere(const f32vec4 & center, float radius) {
    if (state.defining_scene) {
        if (!materials.empty()) {
            Sphere* sphere = new Sphere(center, radius, materials.size() - 1);
            scene.objects.emplace_back(sphere);
        }
    }
    // TODO ERRORS
}

void SglRenderer::push_light(float x, float y, float z, float r, float g, float b) {
    PointLight light = PointLight(x, y, z, r, g, b);
    scene.lights.push_back(light);
}


void SglRenderer::draw_point(const f32vec4 & point) {
    int half_size = static_cast<int>(state.point_size / 2);
            
    // If the point size is 1 the half size will be 0 therefore both
    // for loops will happen only once and draw the point itself
    // otherwise it will create a square point around the vertex
    for(int i = -half_size; i <= half_size; ++i) {
        for(int j = -half_size; j <= half_size; ++j) {
            int x = static_cast<int>(point.x + i);
            int y = static_cast<int>(point.y + j);
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

void SglRenderer::draw_line(const f32vec4 & start_v, const f32vec4 & end_v) {
    int x0 = static_cast<int>(start_v.x);
    int y0 = static_cast<int>(start_v.y);
    int x1 = static_cast<int>(end_v.x);
    int y1 = static_cast<int>(end_v.y);
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
        f32vec4 start1 =  f32vec4(x_c + x, y_c + y - 1, z_c, 1.0f);
        f32vec4 end1 =    f32vec4(x_c + x, y_c - y + 1, z_c, 1.0f);
        f32vec4 start2 =  f32vec4(x_c - x, y_c + y - 1, z_c, 1.0f);
        f32vec4 end2 =    f32vec4(x_c - x, y_c - y + 1, z_c, 1.0f);
        draw_line(start1, end1);
        draw_line(start2, end2);
    }
}

void SglRenderer::draw_sym_pixels_rotated(int x_c, int y_c, int z_c, int x, int y, SglMatrix mat) {
    int x1, y1;
    for(int i = 0; i < 2; ++i) {
        for(int j = 0; j < 2; ++j) {
            f32vec4 help = f32vec4(pow(-1, i) * x, pow(-1, j) * y, z_c, 1.0f);
            help = mat * help;
            x1 = x_c + help.x;
            y1 = y_c + help.y;
            state.currentFramebuffer->set_pixel(static_cast<int>(x1), static_cast<int>(y1), state.draw_color);
        }
    }
}

void SglRenderer::draw_circle(const f32vec4 & center, float radius) {
    if (state.area_mode == sglEAreaMode::SGL_POINT) {
        push_vertex(center);
        return;
    }

    int x, y, p, twoX, twoY;
    int x_c = static_cast<int>(center.x);
    int y_c = static_cast<int>(center.y);
    int z_c = static_cast<int>(center.z);

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

void SglRenderer::draw_ellipse(const f32vec4 & center, float a, float b, SglMatrix mat) {
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
        .x = center.x, .y = center.y, .z = center.z
    });

    for (int i = 1; i <= N; ++i) {      
        x2 = a * cos(i * alpha);
        y2 = b * sin(i * alpha);

        f32vec4 start = f32vec4(x1, y1, center.z, center.w);
        f32vec4 end = f32vec4(x2, y2, center.z, center.w);

        draw_line(mat * start, mat * end);
        
        if(i == 1) vertices.push_back(mat * start);
        vertices.push_back(mat * end);

        x1 = x2;
        y1 = y2;
    }

    if (state.area_mode == sglEAreaMode::SGL_FILL) draw_fill_object();
}

void SglRenderer::draw_arc(const f32vec4 & center, float radius, float from, float to, SglMatrix mat) {

    if (state.area_mode == sglEAreaMode::SGL_POINT) {
        push_vertex(mat * center);
        return;
    }

    float x1, y1;
    int z_c = center.z;

    float angle = to - from;
    int N = int((40.0f * angle) / (2.0f * M_PI));
    float alpha = angle / N;
    
    x1 = radius * cos(from);
    y1 = radius * sin(from);

    mat = mat * SglMatrix({
        .type = MatrixType::TRANSLATE,
        .x = center.x, .y = center.y, .z = center.z
    }); 
    
    auto rot_mat = SglMatrix({ .type = MatrixType::ROTATE, .x = 0.0f, .y = 0.0f, .z = alpha });

    f32vec4 start = f32vec4(x1, y1, z_c, center.w);

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
    auto insert_processed_edge = [&edges](const f32vec4 & from, const f32vec4 & to) -> bool
    {
        // 2) remove horizontal edges
        if(from.y == to.y) { return false; }
        // 1) orient edges top - bottom and shorten them by one pixel (this is in to.at(1) + 1.0f)
        // TODO(msakmary) CONTINUE HERE - store 1 - 1/z into the depth buffer
        if(from.y < to.y) 
        { 
            edges.emplace_back(SglEdge{
                .from_y  = static_cast<uint>(to.y),
                .to_y    = static_cast<uint>(from.y + 1.0f),
                .upper_x = to.x,
                .upper_z = to.z,
                // x_step = (x_from - x_to) / (y_from - y_to)
                .step_x  = (from.x - to.x) / ((from.y) - to.y),
                // x_step = (z_from - z_to) / (y_from - y_to)
                .step_z  = (from.z - to.z) / ((from.y) - to.y),
            });
        }
        else
        {
            edges.emplace_back(SglEdge{
                .from_y  = static_cast<uint>(from.y),
                .to_y    = static_cast<uint>(to.y + 1.0f),
                .upper_x = from.x,
                .upper_z = from.z,
                // x_step = (x_to - x_from) / (y_to - y_from)
                .step_x  = (to.x - from.x) / ((to.y) - from.y),
                // x_step = (z_to - z_from) / (y_to - y_from)
                .step_z  = (to.z - from.z) / ((to.y) - from.y),
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
    if (vertices.at(0).x > max_x) max_x = vertices.at(0).x;
    if (vertices.at(0).y > max_y) max_y = vertices.at(0).y;
    if (vertices.at(0).x < min_x) min_x = vertices.at(0).x;
    if (vertices.at(0).y < min_y) min_y = vertices.at(0).y;
    // each edge consists of two vertices so we start at one so we already have two vertices at the start of the loop
    for(size_t i = 1; i < vertices.size(); i++)
    {
        insert_processed_edge(vertices.at(i - 1), vertices.at(i));

        if (vertices.at(i).x > max_x) max_x = vertices.at(i).x;
        if (vertices.at(i).y > max_y) max_y = vertices.at(i).y;
        if (vertices.at(i).x < min_x) min_x = vertices.at(i).x;
        if (vertices.at(i).y < min_y) min_y = vertices.at(i).y;
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
            if(!state.defining_scene)
            {
                draw_fill_object();
            }
        }
    }

    vertices.clear();
}

void SglRenderer::draw_sphere(const Sphere & sphere) {
    draw_circle(sphere.center, sphere.radius);
}

void SglRenderer::draw_polygon(const Polygon & polygon) {
    for (int i = 0; i < 3; ++i) vertices.push_back(polygon.vertices.at(i));
    draw_fill_object();
}

void SglRenderer::rasterize_scene() {
    for(auto & object : scene.objects){
        Polygon * polygon = dynamic_cast<Polygon*>(object);
        if(polygon!= nullptr){
            draw_polygon(*polygon);
        }
        else{
            Sphere * sphere = dynamic_cast<Sphere*>(object);
            draw_sphere(*sphere);
        }
    }
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

f32vec3 SglRenderer::phong_color(const PointLight &light,const Ray &ray, f32vec3 &normal,const f32vec3 &intersection, unsigned material_index){
    f32vec3 to_light = (f32vec3(light.source) - intersection).normalize();
    f32vec3 reflected_light = reflect(-to_light, normal); // -to_light = from_light

    float cos_beta = std::max(dot(reflected_light, -ray.direction), 0.0f); // -ray.direction = to_view
    float cos_alpha = std::max(dot(normal, to_light), 0.0f);

    f32vec3 diffuse = materials[material_index].color * light.color * materials[material_index].kd * cos_alpha;
    f32vec3 specular = light.color * materials[material_index].ks * std::pow(cos_beta, materials[material_index].shine);

    return diffuse + specular;
}

bool SglRenderer::is_visible_from_light(const f32vec3 &light_position, const f32vec3 &intersection){
    const Ray shadow_ray = {intersection, (light_position - intersection).normalize()};
    float dist;
    for(auto &object : scene.objects)
    {
        if(object->intersection(shadow_ray, dist) && dist > 0.01f && dist < MAXFLOAT)
        {
            return false;
        }
    }
    return true;
}

Ray SglRenderer::gen_ray(const float width, const float height, const f32vec3 v00, 
    const f32vec3 v10, const f32vec3 v01, const f32vec3 origin)
{
    Ray ray;

    float t_x = float(width)/float(state.currentFramebuffer->get_width());
    float t_y = float(height)/float(state.currentFramebuffer->get_height());
    auto final_dir = v00 + t_x * (v10 - v00);
    final_dir = final_dir + t_y * (v01 - v00);
    final_dir = final_dir.normalize();


    ray.direction = f32vec3(final_dir.x, final_dir.y, final_dir.z);
    ray.origin = origin;

    return ray;
}

f32vec3 SglRenderer::trace_ray(const Ray &ray, const int depth, bool refracted)
{
    float best_dist = MAXFLOAT;
    Primitive * best_primitive = nullptr;
    f32vec3 intersection_point, intersection_normal;

    for(auto & object : scene.objects){
        float dist;
        if(object->intersection(ray, dist) && dist > 0.0f && dist < best_dist){
            auto tmp_point = ray.origin + (ray.direction * dist);
            auto tmp_normal = object->compute_normal_vector(tmp_point);// IS NORMALIZED

            //BACKFACE CULLING
            if(!refracted && dot(tmp_normal, ray.direction) > 0.0f)
            {
                continue;
            }
            intersection_point = tmp_point;
            intersection_normal = tmp_normal;
            best_dist = dist;
            best_primitive = object;
        }
    }

    if(best_dist < MAXFLOAT)
    {
        f32vec3 color; //  0.0f , 0.0f, 0.0f by default
        for(auto &l : scene.lights){
            f32vec3 light_position = {l.source.x, l.source.y, l.source.z};
            if(is_visible_from_light(light_position, intersection_point)){
                color = color + phong_color(l, ray, intersection_normal, intersection_point, best_primitive->material_index);
            }
        }
        // TRACE RAY
        int next_depth = depth + 1;
        if(next_depth < 8){ // DEFAULT 8
            if(materials[best_primitive->material_index].ks > 0.01f){// Can .ks be negative? If yes change to != 0.0f.
                // shift origin, se we dont accidentally intersect with self
                auto reflected_dir = reflect(ray.direction, intersection_normal);
                Ray reflected_ray = {intersection_point + (reflected_dir * 0.01f), reflected_dir};
                color = color +
                        trace_ray(reflected_ray, next_depth, false) * materials[best_primitive->material_index].ks;
            }
            if(materials[best_primitive->material_index].T > 0.01f){
                f32vec3 refracted_dir = refract(ray.direction, intersection_normal, materials[best_primitive->material_index].ior);
                Ray refracted_ray = {intersection_point + (refracted_dir * 0.01f), refracted_dir};
                color = color +
                        trace_ray(refracted_ray, next_depth, true) * materials[best_primitive->material_index].T;
            }

        }
        return color;

    }
    return {0.0, 0.0, 0.0}; // TODO We have to get clear color here!
}

void SglRenderer::raytrace_scene(const SglMatrix & modelview,
                                 const SglMatrix & projection,
                                 const SglMatrix & viewport) {

    const int num_threads = std::thread::hardware_concurrency() * 3;
    //const int num_threads = 1;
    std::vector<std::thread> threads;

    auto modelview_inverse = modelview;
    auto viewport_projection_inverse = viewport * projection;
    modelview_inverse.invert();
    viewport_projection_inverse.invert();
    viewport_projection_inverse = modelview_inverse * viewport_projection_inverse;

    auto origin_tmp = modelview_inverse * f32vec4(0.0, 0.0, 0.0, 1.0);
    auto orig = f32vec3(origin_tmp.x, origin_tmp.y, origin_tmp.z);

    auto horizontal_min = viewport_projection_inverse * f32vec4(0.5f, 0.5f, 1.0f, 1.0f);
    horizontal_min = (1.0f / horizontal_min.w) * horizontal_min;
    auto dir_00 = (horizontal_min - origin_tmp).normalize();
    auto d00 = f32vec3(dir_00.x, dir_00.y, dir_00.z);

    auto horizontal_max = viewport_projection_inverse * f32vec4(float(state.currentFramebuffer->get_width()) + 0.5f, 0.5f, 1.0f, 1.0f);
    horizontal_max = (1.0f / horizontal_max.w) * horizontal_max;
    auto dir_10 = (horizontal_max - origin_tmp).normalize();
    auto d10 = f32vec3(dir_10.x, dir_10.y, dir_10.z);

    auto vertical_max = viewport_projection_inverse * f32vec4(0.5f, float(state.currentFramebuffer->get_height()) + 0.5f, 1.0f, 1.0f);
    vertical_max = (1.0f / vertical_max.w) * vertical_max;
    auto dir_01 = (vertical_max - origin_tmp).normalize();
    auto d01 = f32vec3(dir_01.x, dir_01.y, dir_01.z);

    auto task = [&](int start, int end){
        for(int y = start; y < end; y++)
        {
            for(uint32_t x = 0; x < state.currentFramebuffer->get_width(); x++)
            {
                auto col = trace_ray(gen_ray(x, y, d00, d10, d01, orig), 0, false);
                //if(col.x != -420.0f)
                state.currentFramebuffer->set_pixel(x, y, Pixel{col.r, col.g, col.b});
            }
        }
    };
    threads.reserve(num_threads);
    int chunk = state.currentFramebuffer->get_height() / num_threads;
    int remainder = state.currentFramebuffer->get_height() % num_threads;
    for(int i = 0; i < num_threads; i++)
    {
        if(i != num_threads - 1)
        {
            threads.push_back(std::thread(task, i * chunk, (i+1) * chunk));
        } else 
        {
            threads.push_back(std::thread(task, i * chunk, ((i+1) * chunk) + remainder));
        }
    }
    for(int i = 0; i < num_threads; i++)
    {
        threads.at(i).join();
    }
}
