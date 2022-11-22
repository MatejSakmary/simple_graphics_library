#pragma once

#pragma GCC optimize("O3,unroll-loops,fast-math")
#include <array>
#include <string>
#include <cmath>

#include "sgl.h"
#include "macros.hpp"
#include "sgl_vec.hpp"

enum MatrixType
{
    IDENTITY,
    SCALE,
    TRANSLATE,
    ROTATE,
    ORTHOGRAPHIC,
    PERSPECTIVE,
    VIEWPORT
};

struct MatrixCreateInfo
{
    MatrixType type = IDENTITY;
    float x = 0.0f;
    float y = 0.0f;
    float z = 0.0f;

    // TODO(msakmary) Figure out a better way to pass them
    // Params for orthographic and viewport matrix
    float left = 0.0;   // width in the case of viewport matrix
    float right = 0.0;  // height in the case of viewport matrix
    float bottom = 0.0;
    float top = 0.0;
    float near = 0.0;
    float far = 0.0;
};

struct SglMatrix
{
    SglMatrix(const std::array<float, 16> & mat);
    SglMatrix(const MatrixCreateInfo & info);
    ~SglMatrix();
    SglMatrix operator * (const SglMatrix & other ) const;
    SglMatrix operator + (const SglMatrix & other ) const;

    template<typename T>
    vec<4, T> operator * (const vec<4, T> & other ) const;

    auto at(int x, int y) -> float&;
    auto to_string() const -> std::string;
    bool invert();

    private:
        template<int size,typename T>
        friend struct vec;
        SglMatrix();
        std::array<float, 16> mat;
};