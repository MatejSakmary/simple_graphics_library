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
};

struct Sphere : Primitive 
{
    SglVertex center;
    float radius;
    // Sphere(const Material & material, const SglVertex & center, float radius);
    // ~Sphere();
};

struct Polygon : Primitive 
{
    std::vector<SglVertex> vertices;
    // Polygon(const Material & material, const SglVertex & v1, const SglVertex & v2, const SglVertex & v3);
    // ~Primitive();
};

struct Scene
{
    std::vector<Primitive> primitives;

    Scene();
    ~Scene();
    // void add_sphere(const Material & material, const SglVertex & center, float radius);
    // void add_polygon(const Material & material, const SglVertex & v1, const SglVertex & v2, const SglVertex & v3);
};