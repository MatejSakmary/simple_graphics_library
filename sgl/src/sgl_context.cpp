#include "sgl_context.hpp"

SglContext::SglContext(const SglContextInitInfo & info) :
    info{info},
    framebuffer(info.width, info.height),
    released{false},
    mode{sglEMatrixMode::SGL_MODELVIEW},
    error_cbf{info.error_cbf},
    matrix_stacks{
        std::stack<SglMatrix>({SglMatrix()}),
        std::stack<SglMatrix>({SglMatrix()}) 
    }  {}

SglContext::~SglContext() {}


void SglContext::pop_matrix()
{
    if(matrix_stacks[mode].size() < 1) 
    {
        error_cbf(sglEErrorCode::SGL_STACK_UNDERFLOW); 
        return;
    } 
    matrix_stacks[mode].pop();
}

void SglContext::push_matrix()
{

}

void SglContext::load_identity()
{

}

void SglContext::load_matrix(const SglMatrix matrix)
{

}