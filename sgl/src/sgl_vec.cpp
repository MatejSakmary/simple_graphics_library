#include "sgl_vec.hpp"

Vec4::Vec4(float x, float y, float z, float w) : vert{{x, y, z , w}} {}
Vec4::Vec4() : vert{{0.0f, 0.0f, 0.0f, 0.0f}} {}
Vec4::~Vec4() {}

auto Vec4::at(int idx) const -> float
{
    return vert.at(idx);
}

auto Vec4::at(int idx) -> float &
{
    return vert.at(idx);
}

void Vec4::persp_division()
{
    // foreach xyzw in vec divide by w
    std::for_each(vert.begin(), vert.end(), [this] (float & val) {val /= this->vert.at(3);});
}

auto Vec4::to_string() -> std::string
{
    std::stringstream s;
    for(int i = 0; i < 4; i++)
    {
        s << std::to_string(vert[i]) << " ";
    }
    s << "\n";
    return s.str();
}

Vec4 Vec4::normalize() {
    float norm = this->get_norm();
    
    return Vec4(
        this->at(0) / norm,
        this->at(1) / norm,
        this->at(2) / norm,
        this->at(3));
}


float Vec4::get_norm() {
    return sqrt(this->at(0) * this->at(0) + this->at(1) * this->at(1) + this->at(2) * this->at(2));
}

Vec4 Vec4::operator - (const Vec4 &rhs) const
{
    return {this->at(0) - rhs.at(0), this->at(1) - rhs.at(1),this->at(2) - rhs.at(2),this->at(3) - rhs.at(3)};
}

Vec4 Vec4::operator*(const float scalar) const {
    return {this->at(0) * scalar, this->at(1) * scalar,this->at(2) * scalar,this->at(3) * scalar};
}


Vec4 cross_product(const Vec4 &A, const Vec4 &B)
{
    return {
        A.at(1) * B.at(2) - A.at(2) * B.at(1), 
        A.at(2) * B.at(0) - A.at(0) * B.at(3), 
        A.at(0) * B.at(1) - A.at(1) * B.at(0), 
        1.0f};
}

float dot_product(const Vec4 &A, const Vec4 &B)
{
    return (A.at(0) * B.at(0)) + (A.at(1) * B.at(1)) + (A.at(2) * B.at(2));
}

//A must be normalized!!!
Vec4 reflect(const Vec4 &A, const Vec4 &normal) {
    return A - (normal * 2 * (dot_product(A, normal)));
}
