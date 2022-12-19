#pragma once

#include <vector>


#include "sgl_vec.hpp"
#include "sgl_matrix.hpp"

struct Ray 
{
    f32vec3 origin;
    f32vec3 direction;
};

struct EmissiveMaterial{
    float r;
    float g;
    float b;
    float c0;
    float c1;
    float c2;

    EmissiveMaterial();
    EmissiveMaterial( float r, float g, float b, float c0, float c1, float c2 ) :
        r{r}, g{g}, b{b}, c0{c0}, c1{c1}, c2{c2} {}
};

struct Material
{
    f32vec3 color;
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
    bool is_emissive;
    // Primitive(const Material & material);
    virtual f32vec3 compute_normal_vector(const f32vec3 & vector) = 0;
    virtual bool intersection(const Ray &ray, float &t) const = 0;
    virtual ~Primitive() = default;

    protected:
        Primitive(unsigned material_index, bool is_emmisive = false);
};

struct Sphere : public Primitive 
{
    f32vec4 center;
    float radius;
    Sphere(const f32vec4 &center, float radius, unsigned material_index);

    f32vec3 compute_normal_vector(const f32vec3 & vector) override;
    bool intersection(const Ray &ray, float &t) const override;
};

// Polygon is considered as triangle
struct Polygon : public Primitive 
{
    std::vector<f32vec4> vertices;
    bool already_computed_normal;
    f32vec3 norm;

    f32vec3 compute_normal_vector(const f32vec3 & vector) override;
    f32vec3 get_sampled_point( const float r1, const float r2) const;
    float get_area() const;
    bool intersection(const Ray &ray, float &t) const override;

    Polygon(const f32vec4 &v1, const f32vec4 &v2, const f32vec4 &v3, unsigned material_index, bool is_emissive = false);
};

struct PointLight 
{
    f32vec4 source;
    f32vec3 color;

    PointLight(float x, float y, float z, float r, float g, float b);
    ~PointLight();
};

struct Scene
{
    std::vector<Primitive*> objects;
    std::vector<PointLight> lights;

    void clear();

    Scene();
    ~Scene();
};