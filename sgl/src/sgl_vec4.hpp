#pragma once

#include <array>
#include <string>
#include <cmath>

#include "sgl.h"

// forward declare matrix so we can friend it
struct SglMatrix;

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
        friend struct SglMatrix;
        std::array<float, 4> vert;
};


Vec4 cross_product(const Vec4 &A, const Vec4 &B);
float dot_product(const Vec4 &A, const Vec4 &B);
Vec4 reflect(const Vec4 &A, const Vec4 &normal);