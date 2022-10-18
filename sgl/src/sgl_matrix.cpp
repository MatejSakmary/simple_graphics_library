#include "sgl_matrix.hpp"

SglMatrix::SglMatrix(const std::array<float, 16> & mat) : mat{mat} {};
float SglMatrix::operator () (int x, int y)
{
    return mat[y * 4 + x];
};

SglMatrix SglMatrix::operator * (const SglMatrix & other )
{

}

SglMatrix SglMatrix::operator + (const SglMatrix & other )
{

}