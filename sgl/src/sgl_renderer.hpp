#pragma once

#pragma GCC optimize("O3,unroll-loops,fast-math")
#include <vector>
#include <thread>

#include "sgl.h"
#include "sgl_vec.hpp"
#include "sgl_matrix.hpp"
#include "sgl_framebuffer.hpp"
#include "sgl_scene.hpp"
#include "macros.hpp"

// Forward decl vertex so we can friend it 
struct SglCore;

struct DrawState
{
    Pixel draw_color;
    float point_size;
    sglEAreaMode area_mode;
    sglEElementType element_type_mode;
    bool depth_test;
    SglFramebuffer* currentFramebuffer;
    bool defining_scene;
};

struct SglRenderer
{
    DrawState state;
    Scene scene;
    std::vector<Material> materials;

    SglRenderer();
    ~SglRenderer();

    void push_vertex(const f32vec4 & vertex);
    void draw_circle(const f32vec4 & center, float radius);
    void draw_ellipse(const f32vec4 & center, float a, float b, SglMatrix mat);
    void draw_arc(const f32vec4 & center, float radius, float from, float to, SglMatrix mat);
    void push_sphere(const f32vec4 & center, float radius);
    void push_light(float x, float y, float z, float r, float g, float b);

    void push_material(
        float r,
        float g,
        float b,
        float kd,
        float ks,
        float shine,
        float T,
        float ior
    );

    private:
        friend struct SglCore;
        std::vector<f32vec4> vertices;

        void draw_point(const f32vec4 & point);
        void draw_line_low(int x0, int y0, int x1, int y1);
        void draw_line_high(int x0, int y0, int x1, int y1);
        void draw_line(const f32vec4 & start_v, const f32vec4 & end_v);
        void draw_sym_pixels(int x_c, int y_c, int z_c, int x, int y);
        void draw_sym_pixels_rotated(int x_c, int y_c, int z_c, int x, int y, SglMatrix mat);

        // Reads *vertices* and draws fill of the polygon specified by vertices
        void draw_fill_object();
        
        // TODO sakacond (was not enough time before deadline, will finish later)
        // void draw_fill_triangles();

        // RayTracing functions
        Ray gen_ray(const float width, const float height, const f32vec3 v00, 
            const f32vec3 v10, const f32vec3 v01, const f32vec3 origin);
        f32vec3 trace_ray(const Ray &ray, const int depth);
        //f32vec3 phong_color(f32vec3 &point, unsigned materialId, f32vec3 &normal, PointLight &pl, Ray &r);
        f32vec3 phong_color(const PointLight &light,const Ray &ray, f32vec3 &normal,const f32vec3 &intersection, unsigned material_index);
        bool is_visible_from_light(const f32vec3 &light_position, const f32vec3 &intersection);

        // Rasterizing primitives functions
        void draw_sphere(const Sphere & sphere);
        void draw_polygon(const Polygon & polygon);
        void rasterize_scene();

        void recording_start();
        void recording_end();

    public:
        void raytrace_scene(const SglMatrix & modelview,
                            const SglMatrix & projection,
                            const SglMatrix & viewport);
};