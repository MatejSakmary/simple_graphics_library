#include "sgl_scene.hpp"


#include <iostream>
#include <immintrin.h>

Material::Material() :
    color{0.0f, 0.0f, 0.0f},
    kd{0.0f},
    ks{0.0f},
    shine{0.0f},
    T{0.0f},
    ior{0.0f}
{
}

Material::Material(
    float r,
    float g,
    float b,
    float kd,
    float ks,
    float shine,
    float T,
    float ior) :
    color{r,g,b},
    kd{kd},
    ks{ks},
    shine{shine},
    T{T},
    ior{ior}
{
}

Material::~Material() {}

// Intersection and raytrace needed functions
f32vec3 Polygon::compute_normal_vector(const f32vec3 & vector){
    if (already_computed_normal) {
        return norm;
    }
    f32vec3 vert0 = f32vec3(vertices.at(0));
    f32vec3 vert1 = f32vec3(vertices.at(1));
    f32vec3 vert2 = f32vec3(vertices.at(2));
    auto tmp_norm = cross(vert1 - vert0, vert2 - vert0);
    tmp_norm = tmp_norm.normalize();
    norm = tmp_norm;

    already_computed_normal = true;

    return norm;
}

f32vec3 Polygon::get_sampled_point( const float r1, const float r2) const
{
    const f32vec4 e0 = this->vertices[1] - this->vertices[0];
    const f32vec4 e1 = this->vertices[2] - this->vertices[0];
    float u, v;

    if(r1 + r2 > 1.0f)
    {
        u = 1 - r1;
        v = 1 - r2;
    } else {
        u = r1;
        v = r2;
    }
    const auto sample = this->vertices[0] + u * e0 + v * e1;
    return {sample.x, sample.y, sample.z};
}

float Polygon::get_area() const
{
    f32vec3 vert0 {this->vertices[0].x, this->vertices[0].y, this->vertices[0].z};
    f32vec3 vert1 {this->vertices[1].x, this->vertices[1].y, this->vertices[1].z};
    f32vec3 vert2 {this->vertices[2].x, this->vertices[2].y, this->vertices[2].z};
    const f32vec3 e0 = vert1 - vert0;
    const f32vec3 e1 = vert2 - vert0;
    
    const auto res = cross(e0, e1);
    const f32vec3 res_ = {res.x, res.y, res.z};
    const float A = 0.5f * res_.get_norm();
    return A;
}

PointLight::PointLight(float x, float y, float z, float r, float g, float b) {
    this->source = f32vec4(x, y, z, 1.0f);
    this->color = {r,g,b};
}

PointLight::~PointLight() {}

// Find intersection point - from PBRT - www.pbrt.org
bool Polygon::intersection(const Ray &ray, float &t) const {
    f32vec3 vert0 {this->vertices[0].x, this->vertices[0].y, this->vertices[0].z};
    f32vec3 vert1 {this->vertices[1].x, this->vertices[1].y, this->vertices[1].z};
    f32vec3 vert2 {this->vertices[2].x, this->vertices[2].y, this->vertices[2].z};
    const f32vec3 e1 = vert1 - vert0;
    const f32vec3 e2 = vert2 - vert0;

    f32vec3 s1 = cross(ray.direction, e2);
    float divisor = dot(s1, e1);
    if (divisor == 0.0)
        return false;

    float invDivisor = 1.0f / divisor;
    // Compute first barycentric coordinate
    f32vec3 d = ray.origin - vert0;
    float b1 = dot(d, s1) * invDivisor;
    if (b1 < 0.0f || b1 > 1.0f)
        return false;
    // Compute second barycentric coordinate
    f32vec3 s2 = cross(d, e1);
    float b2 = dot(ray.direction, s2) * invDivisor;
    if (b2 < 0.0f || b1 + b2 > 1.0f)
        return false;
    // Compute _t_ to intersection point
    float tt = dot(e2, s2) * invDivisor;
    if (tt < 0.0f) // In original algo there is tt < 0 || tt > maxT
        return false;
    t = tt;
    return true;   
}

f32vec3 Sphere::compute_normal_vector(const f32vec3 & vector){
    f32vec3 normal = {vector.x - this->center.x,
                      vector.y - this->center.y,
                      vector.z - this->center.z};
    return normal.normalize();
}

// source: http://www.devmaster.net/wiki/Ray-sphere_intersection
bool Sphere::intersection(const Ray &ray, float &t) const {
    const f32vec3 dst = ray.origin - f32vec3(center.x, center.y, center.z);
    const float b = dot(dst, ray.direction);
    const float c = dot(dst, dst) - radius*radius;
    const float d = b*b - c;

    if(d > 0) {
        t = -b - sqrtf(d);
        if (t < 0.0f)
            t = -b + sqrtf(d);

        return true;
    }
    return false;

}
Primitive::Primitive(unsigned material_index, bool is_emissive) : material_index(material_index), is_emissive(is_emissive) {};
Sphere::Sphere(const f32vec4 &center, const float radius, const unsigned material_index) : Primitive(material_index), center(center), radius(radius) {};
Polygon::Polygon(const f32vec4 &v1, const f32vec4 &v2, const f32vec4 &v3, unsigned material_index, bool is_emissive)
     : Primitive(material_index, is_emissive), vertices{v1,v2,v3}, already_computed_normal(false) {};

Scene::Scene() : objects{} {};

Scene::~Scene() {
    clear();
}

void Scene::clear() {
    for(auto &o : objects){
        delete o;
    }
    objects.clear();
    lights.clear();
}

