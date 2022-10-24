#pragma once

#include <array>
#include <string>

#include "sgl.h"

// forward declare matrix so we can friend it
struct SglMatrix;

struct SglVertex
{
    SglVertex(float x, float y, float z, float w);
    ~SglVertex();

    auto to_string() -> std::string;
    auto at(int idx) const -> float;

    private:
        friend struct SglMatrix;
        SglVertex();
        std::array<float, 4> vert;
};