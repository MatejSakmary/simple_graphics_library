#include "sgl_matrix.hpp"

#include <sstream>

SglMatrix::SglMatrix(const std::array<float, 16> & mat) : mat{mat} {};
SglMatrix::SglMatrix() : mat{ {1.0f, 0.0f, 0.0f, 0.0f,
                               0.0f, 1.0f, 0.0f, 0.0f,
                               0.0f, 0.0f, 1.0f, 0.0f,
                               0.0f, 0.0f, 0.0f, 1.0f}} {};

SglMatrix::SglMatrix(bool emtpy) : mat{} {};
SglMatrix::~SglMatrix(){};

auto SglMatrix::to_string() -> std::string
{
    std::stringstream s;
    for(int i = 0; i < 16; i++)
    {
        s << std::to_string(mat[i]);
        if(!(i % 4)) { s << "\n"; }
        else 		 { s << " ";  } 
    }
    return s.str();
}

float SglMatrix::operator () (int x, int y)
{
    return mat[y * 4 + x];
};

SglMatrix SglMatrix::operator * (const SglMatrix & other )
{
    SglMatrix ret_mat = SglMatrix();
    for(int row = 0; row < 4; row++)
    {
        for(int other_col = 0; other_col < 4; other_col++)
        {
            for(int idx = 0; idx < 4; idx++)
            {
                ret_mat.mat[row * 4 + other_col] += mat[row * 4 + idx] * other.mat[idx * 4 + other_col];
            }
        } 
    }
    return ret_mat;
}

SglMatrix SglMatrix::operator + (const SglMatrix & other )
{
    SglMatrix ret_mat = SglMatrix(other);
    for(int i = 0; i < 16; i++)
    {
        ret_mat.mat[i] += mat[i];
    }
    return ret_mat;
}