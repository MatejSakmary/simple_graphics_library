#include "sgl_vertex.hpp"

#include <sstream>

SglVertex::SglVertex(float x, float y, float z, float w) : vert{{x, y, z ,w}} {}
SglVertex::SglVertex() : vert{{0.0f, 0.0f, 0.0f, 0.0f}} {}
SglVertex::~SglVertex() {}

auto SglVertex::at(int idx) const -> float
{
    return vert.at(idx);
}

auto SglVertex::to_string() -> std::string
{
    std::stringstream s;
    for(int i = 0; i < 4; i++)
    {
        s << std::to_string(vert[i]) << " ";
    }
    s << "\n";
    return s.str();
}