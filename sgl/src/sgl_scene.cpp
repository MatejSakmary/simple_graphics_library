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
Vec4 Primitive::compute_normal_vector(const Vec4 & vector) { return Vec4(0.0f, 0.0f, 0.0f, 1.0f); }

bool Primitive::intersection(const Ray &ray, float &t) {
    return false;
}


// Intersection and raytrace needed functions
Vec4 Polygon::compute_normal_vector(const Vec4 & vector) {
    if (computed) {
        return norm;
    }

    norm = cross_product(this->vertices.at(0), this->vertices.at(1));
    norm = norm.normalize();

    computed = true;

    return norm;
}

PointLight::PointLight(float x, float y, float z, float r, float g, float b) {
    this->source = Vec4(x, y, z, 1.0f);
    this->color[0] = r;
    this->color[1] = g;
    this->color[2] = b;
}

PointLight::~PointLight() {}

// Find intersection point - from PBRT - www.pbrt.org
bool Polygon::intersection(const Ray &ray, float &t) {
    const Vec4 e1 = this->vertices[1] - this->vertices[0];
    const Vec4 e2 = this->vertices[2] - this->vertices[0];

    Vec4 s1 = cross_product(e1, e2);
    float divisor = dot_product(s1, e1);
    float invDivisor = 1.0f / divisor;
    // Compute first barycentric coordinate
    Vec4 d = ray.origin - this->vertices[0];
    float b1 = dot_product(d, s1) * invDivisor;
    if (b1 < 0. || b1 > 1.)
        return false;
    // Compute second barycentric coordinate
    Vec4 s2 = cross_product(d, e1);
    float b2 = dot_product(ray.direction, s2) * invDivisor;
    if (b2 < 0. || b1 + b2 > 1.)
        return false;
    // Compute _t_ to intersection point
    float tt = dot_product(e2, s2) * invDivisor;
    if (tt < 0) // In original algo there is tt < 0 || tt > maxT
        return false;
    t = tt;
    return true;
}

Vec4 Sphere::compute_normal_vector(const Vec4 & vector) {
    Vec4 normal = Vec4(
        vector.at(0) - this->center.at(0),
        vector.at(1) - this->center.at(1),
        vector.at(2) - this->center.at(2),
        0.0f
    );

    return normal.normalize();
}

// source: http://www.devmaster.net/wiki/Ray-sphere_intersection
bool Sphere::intersection(const Ray &ray, float &t) {
    const Vec4 dst = ray.origin - this->center;
    const float b = dot_product(dst, ray.direction);
    const float c = dot_product(dst, dst) - radius*radius;
    const float d = b*b - c;

    if(d > 0) {
        t = -b - sqrtf(d);
        if (t < 0.0f)
            t = -b + sqrtf(d);

        return true;
    }
    return false;

}

Scene::Scene() :
    spheres{},
    polygons{} {}

Scene::~Scene() {}
