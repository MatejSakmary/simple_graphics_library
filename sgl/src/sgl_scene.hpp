#pragma once

#include <vector>

#include "sgl_vec.hpp"
#include "sgl_matrix.hpp"

struct Ray 
{
    f32vec3 origin;
    f32vec3 direction;
};


struct Material
{
    float r;
    float g;
    float b;
    float kd;
    float ks;
    float shine;
    float T;
    float ior;

    Material();
    Material(
        float r,
        float g,
        float b,
        float kd,
        float ks,
        float shine,
        float T,
        float ior
    );
    ~Material();
};


struct Primitive 
{
    unsigned materialIndex;
    // Primitive(const Material & material);
    virtual f32vec4 compute_normal_vector(const f32vec4 & vector);
    virtual bool intersection(const Ray &ray, float &t);
};

struct Sphere : Primitive 
{
    f32vec4 center;
    float radius;
    // Sphere(const Material & material, const Vec4 & center, float radius);
    // ~Sphere();
    f32vec4 compute_normal_vector(const f32vec4 & vector) override;
    bool intersection(const Ray &ray, float &t) override;
};

struct Polygon : Primitive 
{
    std::vector<f32vec4> vertices;
    bool computed;
    f32vec4 norm;

    f32vec4 compute_normal_vector(const f32vec4 & vector) override;
    bool intersection(const Ray &ray, float &t) override;
    // Polygon(const Material & material, const Vec4 & v1, const Vec4 & v2, const Vec4 & v3);
    // ~Primitive();
};

struct PointLight 
{
    f32vec4 source;
    float color[3] = {0.0f, 0.0f, 0.0f};

    PointLight(float x, float y, float z, float r, float g, float b);
    ~PointLight();
};

struct Scene
{
    std::vector<Sphere> spheres;
    std::vector<Polygon> polygons;
    std::vector<PointLight> lights;

    Scene();
    ~Scene();
    // void add_sphere(const Material & material, const Vec4 & center, float radius);
    // void add_polygon(const Material & material, const Vec4 & v1, const Vec4 & v2, const Vec4 & v3);
};