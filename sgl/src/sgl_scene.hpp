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
    unsigned material_index;
    // Primitive(const Material & material);
    virtual f32vec3 compute_normal_vector(const f32vec3 & vector);
    virtual bool intersection(const Ray &ray, float &t) const = 0;
    virtual ~Primitive() = default;

    protected:
        Primitive(){}
};

struct Sphere : public Primitive 
{
    f32vec4 center;
    float radius;
    Sphere() = default;

    f32vec3 compute_normal_vector(const f32vec3 & vector) override;
    bool intersection(const Ray &ray, float &t) const override;
};

struct Polygon : public Primitive 
{
    std::vector<f32vec4> vertices;
    bool computed;
    f32vec3 norm;

    Polygon();
    f32vec3 compute_normal_vector(const f32vec3 & vector) override;
    bool intersection(const Ray &ray, float &t) const override;
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