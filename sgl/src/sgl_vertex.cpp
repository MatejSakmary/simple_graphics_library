#include "sgl_vertex.hpp"

#include <sstream>
#include <algorithm>

SglVertex::SglVertex(float x, float y, float z, float w) : vert{{x, y, z ,w}} {}
SglVertex::SglVertex() : vert{{0.0f, 0.0f, 0.0f, 0.0f}} {}
SglVertex::~SglVertex() {}

auto SglVertex::at(int idx) const -> float
{
    return vert.at(idx);
}

auto SglVertex::at(int idx) -> float &
{
    return vert.at(idx);
}

void SglVertex::persp_division()
{
    // foreach xyzw in vec divide by w
    std::for_each(vert.begin(), vert.end(), [this] (float & val) {val /= this->vert.at(3);});
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