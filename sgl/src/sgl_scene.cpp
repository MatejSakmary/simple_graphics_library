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

PointLight::PointLight(float x, float y, float z, float r, float g, float b) {
    this->source = f32vec4(x, y, z, 1.0f);
    this->color = {r,g,b};
}

PointLight::~PointLight() {}

// Find intersection point - from PBRT - www.pbrt.org
bool Polygon::intersection(const Ray &ray, float &t) const {
    // NOTE(Very cool, but also slower than normal) :(
    // f32vec3 vert0 {this->vertices[0].x, this->vertices[0].y, this->vertices[0].z};
    // f32vec3 vert1 {this->vertices[1].x, this->vertices[1].y, this->vertices[1].z};
    // f32vec3 vert2 {this->vertices[2].x, this->vertices[2].y, this->vertices[2].z};
    // const f32vec3 e1 = vert1 - vert0;
    // const f32vec3 e2 = vert2 - vert0;

    // f32vec3 s1 = cross(ray.direction, e2);
    // f32vec3 d = ray.origin - vert0;
    // f32vec3 s2 = cross(d, e1);

    // __m128 s1_s2_x = {s1.x, s1.x, s2.x, s2.x};
    // __m128 s1_s2_y = {s1.y, s1.y, s2.y, s2.y};
    // __m128 s1_s2_z = {s1.z, s1.z, s2.z, s2.z};
    // __m128 e1_d_e2_rd_x = {e1.x, d.x, e2.x, ray.direction.x};
    // __m128 e1_d_e2_rd_y = {e1.y, d.y, e2.y, ray.direction.y};
    // __m128 e1_d_e2_rd_z = {e1.z, d.z, e2.z, ray.direction.z};
    
    // __m128 tmp_1 = _mm_mul_ps(s1_s2_x, e1_d_e2_rd_x);
    // __m128 tmp_2 = _mm_mul_ps(s1_s2_y, e1_d_e2_rd_y);
    // __m128 tmp_3 = _mm_mul_ps(s1_s2_z, e1_d_e2_rd_z);

    // __m128 res = _mm_add_ps(tmp_1, tmp_2);
    // res = _mm_add_ps(res, tmp_3);

    // __m128 div = _mm_shuffle_ps(res, res, _MM_SHUFFLE(0,0,0,0));
    // res = _mm_div_ps(res, div);

    // std::array<float, 5> arr;
    // _mm_store1_ps(arr.data(), div);
    // _mm_store_ps(arr.data() + 1, res);
    // float divisor = arr[0];
    // auto b1 = arr[2];
    // auto tt = arr[3];
    // auto b2 = arr[4];

    // if ((b1 < 0.0f) | (b1 > 1.0f) | (b2 < 0.0f) | (b1 + b2 > 1.0f) | (divisor == 0.0f) | (tt < 0.0f))
    //     return false;
    // t = tt;
    // return true;
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
Primitive::Primitive(unsigned material_index) : material_index(material_index) {};
Sphere::Sphere(const f32vec4 &center, const float radius, const unsigned material_index) : Primitive(material_index), center(center), radius(radius) {};
Polygon::Polygon(const f32vec4 &v1, const f32vec4 &v2, const f32vec4 &v3, unsigned material_index) : Primitive(material_index), already_computed_normal(false), vertices{v1,v2,v3} {};

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

