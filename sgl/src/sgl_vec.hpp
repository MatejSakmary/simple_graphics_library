#pragma once


#include <array>
#include <string>
#include <cmath>

#include <algorithm>
#include <sstream>
#include <immintrin.h>

#include "../include/sgl.h"

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

    vec();
    vec(const T x, const T y, const T z);
    vec(const vec<3, T> & other);
    vec(const vec<4, T> & other); // DISCARDS W COORDINATE!

    auto to_string() const -> std::string;
    auto get_norm() const -> float;
    auto normalize() const -> vec<3, T>;

    vec<3, T> operator - (const vec<3, T> & rhs) const;
    vec<3, T> operator - () const;
    vec<3, T> operator + (const vec<3, T> & rhs) const;
    vec<3, T> operator * (const vec<3, T> & rhs) const;
    vec<3, T> operator / (const vec<3, T> & rhs) const;

    vec<3, T> operator * (float scalar) const;
    vec<3, T> operator / (float scalar) const;
    T & operator [](int index);
    T operator [](int index) const;
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

    vec();
    vec(const T x, const T y, const T z, const T w);
    vec(const vec<4, T> & other);

    auto to_string() const -> std::string;
    auto get_norm() const -> float;
    auto normalize() const -> vec<4, T>;

    vec<4, T> operator - (const vec<4, T> & rhs) const;
    vec<4, T> operator + (const vec<4, T> & rhs) const;
    vec<4, T> operator * (const vec<4, T> & rhs) const;
    vec<4, T> operator / (const vec<4, T> & rhs) const;

    vec<4, T> operator * (float scalar) const;
    vec<4, T> operator / (float scalar) const;

    T & operator [](int index);
    T operator [](int index) const;
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

/// @brief reflect *vector* along *normal*
///              normal
///  reflected      x       vector
///                xxx
///       xxxxxx  xxxxx  xxxxxx
///       xxxx     xxx     xxxx
///       x xxx    xxx    xxx x
///       x  xxx   xxx   xxx  x
///           xxx  xxx  xxx
///            xxx xxx xxx
///             xxxxxxxxx
///              xxxxxxx
/// @param vector - outgoing direction from the point of reflection
/// @param normal - outgoing normal from the point of reflection
/// @return reflected vector outgoing from the point of reflection
template<int size, typename T>
auto reflect(const vec<size, T> & vector, const vec<size, T> &normal) -> vec<size, T>
{
    throw std::runtime_error("[sgl_vec.hpp::reflect] ERROR reflect on types other than vec3 is not implemented");
}

template<int size, typename T>
auto refract(const vec<size, T> & vector, const vec<size, T> &normal, float ior) -> vec<size, T>
{
    throw std::runtime_error("[sgl_vec.hpp::refract] ERROR refract on types other than vec3 is not implemented");
}

template<int size, typename T>
auto dot(const vec<size, T> & first, const vec<size, T> & second) -> float
{
    throw std::runtime_error("[sgl_vec.hpp::dot] ERROR dot product on types other than vec3 is not implemented");
};

#include "sgl_vec.inl"