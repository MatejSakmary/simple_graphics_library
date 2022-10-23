#pragma once

#include <array>
#include <string>



struct SglMatrix
{
    SglMatrix(const std::array<float, 16> & mat);
    SglMatrix();
    ~SglMatrix();
    float operator () (int x, int y);
    SglMatrix operator * (const SglMatrix & other );
    SglMatrix operator + (const SglMatrix & other );

    auto to_string() const -> std::string;

    private:
        SglMatrix(bool emtpy);
        std::array<float, 16> mat;
};