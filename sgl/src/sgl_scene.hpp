#pragma once

#include <vector>

#include "sgl_vertex.hpp"
#include "sgl_matrix.hpp"


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
    Material material;
    // Primitive(const Material & material);
    virtual SglVertex compute_normal_vector(const SglVertex & vector);
    virtual bool intersection(const SglVertex & vector);
};

struct Sphere : Primitive 
{
    SglVertex center;
    float radius;
    // Sphere(const Material & material, const SglVertex & center, float radius);
    // ~Sphere();
    SglVertex compute_normal_vector(const SglVertex & vector) override;
    bool intersection(const SglVertex & vector) override;
};

struct Polygon : Primitive 
{
    std::vector<SglVertex> vertices;
    bool computed;
    SglVertex norm;

    SglVertex compute_normal_vector(const SglVertex & vector) override;
    bool intersection(const SglVertex & vector) override;
    // Polygon(const Material & material, const SglVertex & v1, const SglVertex & v2, const SglVertex & v3);
    // ~Primitive();
};

struct Light 
{
    SglVertex source;
    float color[3] = {0.0f, 0.0f, 0.0f};
};

struct Scene
{
    std::vector<Sphere> spheres;
    std::vector<Polygon> polygons;

    Scene();
    ~Scene();
    // void add_sphere(const Material & material, const SglVertex & center, float radius);
    // void add_polygon(const Material & material, const SglVertex & v1, const SglVertex & v2, const SglVertex & v3);
};