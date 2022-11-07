#include "sgl_scene.hpp"

Material::Material() {
    this->r     = 0.0f;
    this->g     = 0.0f;
    this->b     = 0.0f;
    this->kd    = 0.0f;
    this->ks    = 0.0f;
    this->shine = 0.0f;
    this->T     = 0.0f;
    this->ior   = 0.0f;
}

Material::Material(
        float r,
        float g,
        float b,
        float kd,
        float ks,
        float shine,
        float T,
        float ior
        ) 
{
    this->r     = r;
    this->g     = g;
    this->b     = b;
    this->kd    = kd;
    this->ks    = ks;
    this->shine = shine;
    this->T     = T;
    this->ior   = ior;
}

Material::~Material() {}

// These virtual functions will never be used.
SglVertex Primitive::compute_normal_vector(const SglVertex & vector) { return SglVertex(0.0f, 0.0f, 0.0f, 1.0f); }
bool Primitive::intersection(const SglVertex & vector) { return false; }


// Intersection and raytrace needed functions
SglVertex Polygon::compute_normal_vector(const SglVertex & vector) {
     // TODO sakacond compute from vertices, set computed to true, set norm, return norm
    if (computed) {
        return norm;
    }

    float x = 0.0f;
    float y = 0.0f;
    float z = 0.0f;

    computed = true;
    norm = SglVertex(x, y, z, 1.0f);

    return norm;
}

bool Polygon::intersection(const SglVertex & vector) {
    // TODO sakacond
    return false;
}

SglVertex Sphere::compute_normal_vector(const SglVertex & vector) {
    // TODO sakacond
    return SglVertex(0.0f, 0.0f, 0.0f, 1.0f);
}

bool Sphere::intersection(const SglVertex & vector) {
    // TODO sakacond
    return false;
}

Scene::Scene() :
    spheres{},
    polygons{} {}

Scene::~Scene() {}
