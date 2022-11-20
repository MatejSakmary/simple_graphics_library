#include "sgl_matrix.hpp"

#include <cmath>
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
        case MatrixType::PERSPECTIVE:
        {
            float right = info.right; float left = info.left;
            float top = info.top; float bottom = info.bottom;
            float near = info.near; float far = info.far;
            mat = {(2.0f * near) / (right - left)  ,               0.0f                , (right + left) / (right - left) ,                0.0f                 ,
                                0.0f               ,  (2.0f * near) / (top - bottom )  , (top + bottom) / (top - bottom) ,                0.0f                 ,
                                0.0f               ,               0.0f                ,  -(far + near) / (far - near)   ,  -(2.0f * far * near) / (far - near),
                                0.0f               ,               0.0f                ,            -1.0f                ,                0.0f                 };
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


template <typename T>
vec<4, T> SglMatrix::operator * (const vec<4, T> & other ) const
{
    vec<4, T> res_vert = vec<4, T>();
    // RET = OTHER_MAT * THIS_VERT
    //                            
    // #      row > # # # #       # 
    // #   =        # # # #   *   # 
    // #            # # # #       # 
    // #            # # # #       # 
    // row is the selected row in the first matrix
    // idx is just the position in respective selected row / col of the vector
    for(int row = 0; row < 4; row++)
    {
        for(int idx = 0; idx < 4; idx++)
        {
            res_vert[row] += mat[row * 4 + idx] * other[idx]; 
        }
    }
    return res_vert;
}

template vec<4, float> SglMatrix::operator * (const vec<4, float> & other ) const;
template vec<4, int> SglMatrix::operator * (const vec<4, int> & other ) const;
template vec<4, uint> SglMatrix::operator * (const vec<4, uint> & other ) const;
template vec<4, double> SglMatrix::operator * (const vec<4, double> & other ) const;

SglMatrix SglMatrix::operator * (const SglMatrix & other ) const
{
    SglMatrix ret_mat = SglMatrix();
    // RET = THIS * OTHER
    //                            
    // # # # #      row > # # # #       # # # #
    // # # # #   =        # # # #   *   # # # #
    // # # # #            # # # #       # # # #
    // # # # #            # # # #       # # # #
    //                                  ^
    //                              other col
    // row is the selected row in the first matrix
    // other_col is the selected column in the second matrix
    // idx is just the position in respective selected row / column
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

SglMatrix SglMatrix::operator + (const SglMatrix & other ) const
{
    SglMatrix ret_mat = SglMatrix(other);
    for(int i = 0; i < 16; i++)
    {
        ret_mat.mat[i] += mat[i];
    }
    return ret_mat;
}
// inverse matrix computation using gauss_jacobi method
// source: N.R. in C
// if matrix is regular = computatation successfull = returns true
// in case of singular matrix returns false
bool SglMatrix::invert() {
    int indxc[4],indxr[4],ipiv[4];
    int i,icol,irow,j,k,l,ll,n;
    float big,dum,pivinv,temp;

    // satisfy the compiler
    icol = irow = 0;
    // the size of the matrix
    n = 4;

    for ( j = 0 ; j < n ; j++) /* zero pivots */
        ipiv[j] = 0;
    for ( i = 0; i < n; i++)
    {
        big = 0.0;
        for (j = 0 ; j < n ; j++)
            if (ipiv[j] != 1)
                for ( k = 0 ; k<n ; k++)
                {
                    if (ipiv[k] == 0)
                    {
                        if (std::fabs(this->at(k,j)) >= big)
                        {
                            big = std::fabs(this->at(k,j));
                            irow = j;
                            icol = k;
                        }
                    }
                    else
                    if (ipiv[k] > 1)
                        return false; /* singular matrix */
                }
        ++(ipiv[icol]);
        if (irow != icol)
        {
            for ( l = 0 ; l<n ; l++)
            {
                temp = this->at(l,icol);
                this->at(l,icol) = this->at(l,irow);
                this->at(l,irow) = temp;
            }
        }
        indxr[i] = irow;
        indxc[i] = icol;
        if (this->at(icol,icol) == 0.0f)
            return false; /* singular matrix */

        pivinv = 1.0f / this->at(icol, icol);
        this->at(icol, icol) = 1.0 ;
        for ( l = 0 ; l<n ; l++)
            this->at(l, icol) = this->at(l, icol) * pivinv ;
        for (ll = 0 ; ll < n ; ll++)
            if (ll != icol)
            {
                dum = this->at(icol,ll);
                this->at(icol,ll) = 0.0;
                for ( l = 0 ; l<n ; l++)
                    this->at(l,ll) = this->at(l, ll) - this->at(l, icol) * dum ;
            }
    }
    for ( l = n; l--; )
    {
        if (indxr[l] != indxc[l])
            for ( k = 0; k<n ; k++)
            {
                temp = this->at(indxr[l],k);
                this->at(indxr[l], k) = this->at(indxc[l], k);
                this->at(indxc[l], k) = temp;
            }
    }

    return true ; // matrix is regular .. inversion has been succesfull
}
