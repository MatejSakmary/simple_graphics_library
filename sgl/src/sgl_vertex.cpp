#include "sgl_vertex.hpp"

#include <sstream>

SglVertex::SglVertex(float x, float y, float z, float w) : vert{{x, y, z ,w}} {}
SglVertex::SglVertex() : vert{{0.0f, 0.0f, 0.0f, 0.0f}} {}
SglVertex::~SglVertex() {}

SglVertex SglVertex::operator * (const SglMatrix & other )
{
    SglVertex res_vert = SglVertex();
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
            res_vert.vert[row] += other.mat[row * 4 + idx] * vert[idx]; 
        }
    }
    return res_vert;
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