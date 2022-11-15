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

bool Primitive::intersection(const SglVertex & vector) { 
    return false; 
}


// Intersection and raytrace needed functions
SglVertex Polygon::compute_normal_vector(const SglVertex & vector) {
    if (computed) {
        return norm;
    }

    norm = norm.cross_product(this->vertices.at(0), this->vertices.at(1));
    norm = norm.normalize();

    computed = true;

    return norm;
}

PointLight::PointLight(float x, float y, float z, float r, float g, float b) {
    this->source = SglVertex(x, y, z, 1.0f);
    this->color[0] = r;
    this->color[1] = g;
    this->color[2] = b;
}

PointLight::~PointLight() {}

bool Polygon::intersection(const SglVertex & vector) {
    // TODO sakacond
    return false;
}

SglVertex Sphere::compute_normal_vector(const SglVertex & vector) {
    
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
