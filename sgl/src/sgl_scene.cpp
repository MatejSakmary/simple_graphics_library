#include "sgl_scene.hpp"

Material::Material() : 
    r{0.0f},
    g{0.0f},
    b{0.0f},
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
    r{r},
    g{g},
    b{b},
    kd{kd},
    ks{ks},
    shine{shine},
    T{T},
    ior{ior}
{
}

Material::~Material() {}

// These virtual functions will never be used.
f32vec4 Primitive::compute_normal_vector(const f32vec4 & vector) { return f32vec4(0.0f, 0.0f, 0.0f, 1.0f); }

bool Primitive::intersection(const Ray &ray, float &t) {
    return false;
}


// Intersection and raytrace needed functions
f32vec4 Polygon::compute_normal_vector(const f32vec4 & vector) {
    if (computed) {
        return norm;
    }

    f32vec3 vert0{vertices.at(0).x, vertices.at(0).y, vertices.at(0).z};
    f32vec3 vert1{vertices.at(1).x, vertices.at(1).y, vertices.at(1).z};
    auto tmp_norm = cross(vert0, vert1);
    tmp_norm = tmp_norm.normalize();
    norm = f32vec4(tmp_norm.x, tmp_norm.y, tmp_norm.z, 1.0);

    computed = true;

    return norm;
}

PointLight::PointLight(float x, float y, float z, float r, float g, float b) {
    this->source = f32vec4(x, y, z, 1.0f);
    this->color[0] = r;
    this->color[1] = g;
    this->color[2] = b;
}

PointLight::~PointLight() {}

// Find intersection point - from PBRT - www.pbrt.org
bool Polygon::intersection(const Ray &ray, float &t) {
    // TODO(msakmary) this is not very clean
    f32vec3 vert0 {this->vertices[0].x, this->vertices[0].y, this->vertices[0].z};
    f32vec3 vert1 {this->vertices[1].x, this->vertices[1].y, this->vertices[1].z};
    f32vec3 vert2 {this->vertices[2].x, this->vertices[2].y, this->vertices[2].z};
    const f32vec3 e1 = vert1 - vert0;
    const f32vec3 e2 = vert2 - vert0;

    f32vec3 s1 = cross(e1, e2);
    float divisor = dot(s1, e1);
    float invDivisor = 1.0f / divisor;
    // Compute first barycentric coordinate
    f32vec3 d = ray.origin - vert0;
    float b1 = dot(d, s1) * invDivisor;
    if (b1 < 0. || b1 > 1.)
        return false;
    // Compute second barycentric coordinate
    f32vec3 s2 = cross(d, e1);
    float b2 = dot(ray.direction, s2) * invDivisor;
    if (b2 < 0. || b1 + b2 > 1.)
        return false;
    // Compute _t_ to intersection point
    float tt = dot(e2, s2) * invDivisor;
    if (tt < 0) // In original algo there is tt < 0 || tt > maxT
        return false;
    t = tt;
    return true;
}

f32vec4 Sphere::compute_normal_vector(const f32vec4 & vector) {
    f32vec4 normal = f32vec4(
        vector.x - this->center.x,
        vector.y - this->center.y,
        vector.z - this->center.z,
        0.0f
    );

    return normal.normalize();
}

// source: http://www.devmaster.net/wiki/Ray-sphere_intersection
bool Sphere::intersection(const Ray &ray, float &t) {
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

Scene::Scene() :
    spheres{},
    polygons{} {}

Scene::~Scene() {}
