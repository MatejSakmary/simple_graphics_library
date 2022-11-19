#pragma once

#include <array>
#include <string>
#include <cmath>

#include <algorithm>
#include <sstream>

#include "sgl.h"

// forward declare matrix so we can friend it
struct SglMatrix;

template<int size, typename T> struct vec;

using b32vec3 = vec<3, bool>;
using b32vec4 = vec<4, bool>;

using i32vec3 = vec<3, int>;
using i32vec4 = vec<4, int>;
using i64vec3 = vec<3, long>;
using i64vec4 = vec<4, long>;

using u32vec3 = vec<3, unsigned>;
using u32vec4 = vec<4, unsigned>;
using u64vec3 = vec<3, unsigned long>;
using u64vec4 = vec<4, unsigned long>;

using f32vec3 = vec<3, float>;
using f32vec4 = vec<4, float>;
using f64vec3 = vec<3, double>;
using f64vec4 = vec<4, double>;

template<typename T>
struct vec<3, T>
{
    union { T x, r; };
    union { T y, g; };
    union { T z, b; };

    vec<3, T>();
    vec<3, T>(const T x, const T y, const T z);
    vec<3, T>(const vec<3, T> & other);

    auto to_string() const -> std::string;
    auto get_norm() const -> float;
    auto normalize() const -> vec<3, T>;

    vec<3, T> operator - (const vec<3, T> & rhs) const;
    vec<3, T> operator + (const vec<3, T> & rhs) const;
    vec<3, T> operator * (const vec<3, T> & rhs) const;
    vec<3, T> operator / (const vec<3, T> & rhs) const;

    vec<3, T> operator * (float scalar) const;
    vec<3, T> operator / (float scalar) const;
    private:
        friend SglMatrix;
};

template<typename T>
struct vec<4, T>
{
    union { T x, r; };
    union { T y, g; };
    union { T z, b; };
    union { T w, a; };

    vec<4, T>();
    vec<4, T>(const T x, const T y, const T z, const T w);
    vec<4, T>(const vec<4, T> & other);

    auto to_string() const -> std::string;
    auto get_norm() const -> float;
    auto normalize() const -> vec<4, T>;

    vec<4, T> operator - (const vec<4, T> & rhs) const;
    vec<4, T> operator + (const vec<4, T> & rhs) const;
    vec<4, T> operator * (const vec<4, T> & rhs) const;
    vec<4, T> operator / (const vec<4, T> & rhs) const;

    vec<4, T> operator * (float scalar) const;
    vec<4, T> operator / (float scalar) const;
    private:
        friend SglMatrix;
};

template<int size, typename T>
auto operator * (T scalar, const vec<size, T> & vector) -> vec<size, T>
{
    return vector * scalar;
} 

template<int size, typename T>
auto operator / (T scalar , const vec<size, T> & vector) -> vec<size, T>
{
    return vector / scalar;
} 

template<int size, typename T>
auto cross(const vec<size, T> & first, const vec<size, T> & second) -> vec<size, T>
{
    throw std::runtime_error("[sgl_vec.hpp::cross] ERROR cross on types other than vec3 is not implemented");
}

template<int size, typename T>
auto reflect(const vec<size, T> & first, const vec<size, T> &second) -> vec<size, T>
{
    throw std::runtime_error("[sgl_vec.hpp::reflect] ERROR reflect on types other than vec3 is not implemented");
}

template<int size, typename T>
auto dot(const vec<size, T> & vector, const vec<size, T> & normal) -> float
{
    throw std::runtime_error("[sgl_vec.hpp::dot] ERROR dot product on types other than vec3 is not implemented");
};

#include "sgl_vec.inl"

struct Vec4
{
    Vec4();
    Vec4(float x, float y, float z, float w);
    ~Vec4();

    auto to_string() -> std::string;
    auto at(int idx) const -> float;
    auto at(int idx) -> float &;
    // Divide vertex by w
    void persp_division();
    
    float get_norm();

    Vec4 normalize();


    Vec4 operator - (const Vec4 &rhs) const;
    Vec4 operator * (float scalar) const;

    private:
        friend SglMatrix;
        std::array<float, 4> vert;
};

Vec4 cross_product(const Vec4 &A, const Vec4 &B);
float dot_product(const Vec4 &A, const Vec4 &B);
Vec4 reflect(const Vec4 &A, const Vec4 &normal);