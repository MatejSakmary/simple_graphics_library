#pragma once

#include <array>



struct SglMatrix
{
    SglMatrix(const std::array<float, 16> & mat);
    ~SglMatrix();
    float operator () (int x, int y);
    SglMatrix operator * (const SglMatrix & other );
    SglMatrix operator + (const SglMatrix & other );
    private:
        std::array<float, 16> mat;
};