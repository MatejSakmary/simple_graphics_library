#pragma once

#include <array>
#include <string>

#include "sgl.h"

// forward declare matrix so we can friend it
struct SglMatrix;

struct SglVertex
{
    SglVertex();
    SglVertex(float x, float y, float z, float w);
    ~SglVertex();

    auto to_string() -> std::string;
    auto at(int idx) const -> float;
    auto at(int idx) -> float &;
    // Divide vertex by w
    void persp_division();

    private:
        friend struct SglMatrix;
        std::array<float, 4> vert;
};