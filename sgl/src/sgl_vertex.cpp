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

SglVertex SglVertex::normalize() {
    float norm = this->get_norm();
    
    return SglVertex(
        this->at(0) / norm,
        this->at(1) / norm,
        this->at(2) / norm,
        this->at(3));
}


SglVertex & SglVertex::operator+(const SglVertex & B) {
    return SglVertex(
        this->at(0) + B.at(0),
        this->at(1) + B.at(1),
        this->at(2) + B.at(2),
        this->at(3) + B.at(3)
        );
}


float SglVertex::get_norm() {
    return sqrt(this->at(0) * this->at(0) + this->at(1) * this->at(1) + this->at(2) * this->at(2));
}

SglVertex cross_product(const SglVertex &A, const SglVertex &B)
{
    return SglVertex(
        A.at(1) * B.at(2) - A.at(2) * B.at(1), 
        A.at(2) * B.at(0) - A.at(0) * B.at(3), 
        A.at(0) * B.at(1) - A.at(1) * B.at(0), 
        1.0f);
}

float dot_product(const SglVertex &A, const SglVertex &B)
{
    return (A.at(0) * B.at(0)) + (A.at(1) * B.at(1)) + (A.at(2) * B.at(2));
}
