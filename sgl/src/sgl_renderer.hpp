#pragma once

#include <vector>

#include "sgl.h"
#include "sgl_vertex.hpp"
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

    void push_vertex(const SglVertex & vertex);
    void draw_circle(const SglVertex & center, float radius);
    void draw_ellipse(const SglVertex & center, float a, float b, SglMatrix mat);
    void draw_arc(const SglVertex & center, float radius, float from, float to, SglMatrix mat);
    void push_sphere(const SglVertex & center, float radius);

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
        std::vector<SglVertex> vertices;

        void draw_point(const SglVertex & point);
        void draw_line_low(int x0, int y0, int x1, int y1);
        void draw_line_high(int x0, int y0, int x1, int y1);
        void draw_line(const SglVertex & start_v, const SglVertex & end_v);
        void draw_sym_pixels(int x_c, int y_c, int z_c, int x, int y);
        void draw_sym_pixels_rotated(int x_c, int y_c, int z_c, int x, int y, SglMatrix mat);

        // Reads *vertices* and draws fill of the polygon specified by vertices
        void draw_fill_object();
        
        // TODO sakacond (was not enough time before deadline, will finish later)
        // void draw_fill_triangles();

        // RayTracing functions
        void draw_primitive(const Sphere & sphere);
        void draw_primitive(const Polygon & polygon);
        void render_scene();

        void recording_start();
        void recording_end();
};