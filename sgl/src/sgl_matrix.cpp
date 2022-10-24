#include "sgl_matrix.hpp"

#include <sstream>

SglMatrix::SglMatrix(const std::array<float, 16> & mat) : mat{mat} {};
SglMatrix::SglMatrix(const MatrixCreateInfo & info) :
    mat{ }
{
    switch(info.type)
    {
        case MatrixType::IDENTITY:
        {
            mat = {1.0f, 0.0f, 0.0f, 0.0f,
                   0.0f, 1.0f, 0.0f, 0.0f,
                   0.0f, 0.0f, 1.0f, 0.0f,
                   0.0f, 0.0f, 0.0f, 1.0f };
            return;
        }
        case MatrixType::TRANSLATE:
        {
            mat = {1.0f, 0.0f, 0.0f, info.x,
                   0.0f, 1.0f, 0.0f, info.y,
                   0.0f, 0.0f, 1.0f, info.z,
                   0.0f, 0.0f, 0.0f, 1.0f };
            return;
        }
        case MatrixType::SCALE:
        {
            mat = {info.x,  0.0f,   0.0f,  0.0f,
                    0.0f,  info.y,  0.0f,  0.0f,
                    0.0f,   0.0f,  info.z, 0.0f,
                    0.0f,   0.0f,   0.0f,  1.0f };
            return;
        }
        case MatrixType::ROTATE:
        {
            using std::cos;
            using std::sin;
            // General rotation matrix in the order x-y-z euler angles
            float x = info.x; float y = info.y; float z = info.z;
            mat = {cos(y) * cos(z)  ,  sin(x) * sin(y) * cos(z) - cos(x) * sin(z)  , cos(x) * sin(y) * cos(z) + sin(x) * sin(z)  , 0.0f,
                   cos(y) * sin(z)  ,  sin(x) * sin(y) * sin(z) + cos(x) * cos(z)  , cos(x) * sin(y) * sin(z) - sin(x) * cos(z)  , 0.0f,
                        -sin(y)     ,                sin(x) * cos(y)               ,                cos(x) * cos(y)              , 0.0f,
                         0.0f       ,                      0.0f                    ,                      0.0f                   , 1.0f };
            return;
        }
        case MatrixType::ORTHOGRAPHIC:
        {
            float right = info.right; float left = info.left;
            float top = info.top; float bottom = info.bottom;
            float near = info.near; float far = info.far;
            mat = {2.0f / (right - left)  ,            0.0f          ,            0.0f       ,  -(right + left) / (right - left),
                           0.0f           ,  2.0f / (top - bottom )  ,            0.0f       ,  -(top + bottom) / (top - bottom),
                           0.0f           ,            0.0f          ,  2.0f / (far - near)  ,  -(far + near)   / (far - near)  ,
                           0.0f           ,            0.0f          ,            0.0f       ,                 1.0f              };
            return;
        }
        case MatrixType::VIEWPORT:
        {
            mat = { info.left / 2.0f ,        0.0f        ,  0.0f  ,  info.x + (info.left  / 2.0f),
                       0.0f          , info.right / 2.0f  ,  0.0f  ,  info.y + (info.right / 2.0f),
                       0.0f          ,        0.0f        ,  0.5f  ,               0.5f           ,
                       0.0f          ,        0.0f        ,  0.0f  ,               1.0f            };
            return;
        }
        default:
            SGL_DEBUG_OUT("[SglMatrix::SglMatrix()] Unknown input matrix type");
            return;
    }
}

SglMatrix::SglMatrix() : mat{} {};
SglMatrix::~SglMatrix(){};

auto SglMatrix::to_string() const -> std::string
{
    std::stringstream s;
    for(int row = 0; row < 4; row++)
    {
        for(int col = 0; col < 4; col++)
        {
            s << std::to_string(mat[row * 4 + col]) << " ";

        }
        s << "\n";
    }
    return s.str();
}

auto SglMatrix::at (int x, int y) -> float&
{
    return mat[y * 4 + x];
};

SglMatrix SglMatrix::operator * (const SglMatrix & other )
{
    SglMatrix ret_mat = SglMatrix();
    // NOTE(msakmary) This is possibly wrong recheck - but since in sgl.h they use column major matrix
    // and we use row major matrix the matrix multiplication has to be reversed
    // TODO(msakmary) if this is corect I probably should reverse the order of multiplication on the
    // caller side (SglContext) and leave the order unchanged here (RET = THIS * OTHER)
    // RET = OTHER * THIS
    //                            
    // # # # #   other row > # # # #       # # # #
    // # # # #   =           # # # #   *   # # # #
    // # # # #               # # # #       # # # #
    // # # # #               # # # #       # # # #
    //                                     ^
    //                                    col
    // other_row is the selected row in the first matrix
    // col is the selected column in the second matrix
    // idx is just the position in respective selected row / column
    for(int other_row = 0; other_row < 4; other_row++)
    {
        for(int col = 0; col < 4; col++)
        {
            for(int idx = 0; idx < 4; idx++)
            {
                ret_mat.mat[other_row * 4 + col] += other.mat[other_row * 4 + idx] * mat[idx * 4 + col];
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