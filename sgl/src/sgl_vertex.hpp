#pragma once

#include <array>
#include <string>

#include "sgl.h"
#include "sgl_matrix.hpp"

struct SglVertex
{
    SglVertex(float x, float y, float z, float w);
    ~SglVertex();

    SglVertex operator * (const SglMatrix & other );

    auto to_string() -> std::string;

    private:
        SglVertex();
        std::array<float, 4> vert;
};