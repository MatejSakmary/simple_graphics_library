#pragma once

#include <array>
#include <string>
#include <cmath>

#include "sgl.h"

enum MatrixType
{
    IDENTITY,
    SCALE,
    TRANSLATE,
    ROTATE
};

struct MatrixCreateInfo
{
    MatrixType type = IDENTITY;
    float x = 0.0f;
    float y = 0.0f;
    float z = 0.0f;
};

struct SglMatrix
{
    SglMatrix(const std::array<float, 16> & mat);
    SglMatrix(const MatrixCreateInfo & info);
    ~SglMatrix();
    SglMatrix operator * (const SglMatrix & other );
    SglMatrix operator + (const SglMatrix & other );

    auto at(int x, int y) -> float&;
    auto to_string() const -> std::string;

    private:
        SglMatrix();
        std::array<float, 16> mat;
};