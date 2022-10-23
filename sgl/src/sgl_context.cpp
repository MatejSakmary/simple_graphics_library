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
        SGL_DEBUG_OUT("[SglContext::pop_matrix()] there is only a single matrix on the selected stack");
        error_cbf(sglEErrorCode::SGL_STACK_UNDERFLOW); 
        return;
    } 
    SGL_DEBUG_OUT("[SglContext::pop_matrix()] poping matrix from stack");
    matrix_stacks[mode].pop();
}

void SglContext::push_matrix()
{
    SGL_DEBUG_OUT("[SglContext::push_matrix()] pushing new matrix to stack");
    matrix_stacks[mode].push(matrix_stacks[mode].top());
}

void SglContext::load_identity()
{
    SGL_DEBUG_OUT("[SglContext::load_identity()] replacing top of the selected matrix stack with identity matrix");
    matrix_stacks[mode].pop();
    matrix_stacks[mode].push(SglMatrix());
}

void SglContext::load_matrix(const SglMatrix matrix)
{
    SGL_DEBUG_OUT("[SglContext::load_matrix()] replacing top of the selected matrix stack with new matrix: \n" 
        + matrix.to_string());
}