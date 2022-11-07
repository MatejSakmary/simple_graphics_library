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

// Primitive::Primitive(const Material & material) {
//     this->material = material;
// }

// Primitive::~Primitive() {}

// Sphere::Sphere(const Material & material, const SglVertex & center, float radius):Primitive(material) 
// {
//     // this->material = material;
//     this->center = center;
//     this->radius = radius;
// }

// Sphere::~Sphere() {}

// Polygon::Polygon(const Material & material, const SglVertex & v1, const SglVertex & v2, const SglVertex & v3) :Primitive(material) 
// {
//     // this->material = material;
//     this->vertices.push_back(v1);
//     this->vertices.push_back(v2);
//     this->vertices.push_back(v3);
// }

Scene::Scene() :
    spheres{},
    polygons{} {}

Scene::~Scene() {}
