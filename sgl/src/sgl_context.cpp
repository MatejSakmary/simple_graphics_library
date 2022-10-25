#include "sgl_context.hpp"

SglContext::SglContext(const SglContextInitInfo & info) :
    info{info},
    framebuffer(info.width, info.height),
    clear_color{Pixel{.r = 0.0f, .g = 0.0f, .b = 0.0f}},
    released{false},
    mode{sglEMatrixMode::SGL_MODELVIEW},
    error_cbf{info.error_cbf},
    matrix_stacks{
        std::stack<SglMatrix>({SglMatrix({.type = MatrixType::IDENTITY, .x = 0.0f, .y = 0.0f, .z = 0.0f})}),
        std::stack<SglMatrix>({SglMatrix({.type = MatrixType::IDENTITY, .x = 0.0f, .y = 0.0f, .z = 0.0f})})},
    viewport_mat{SglMatrix({.type = MatrixType::IDENTITY, .x = 0.0f, .y = 0.0f, .z = 0.0f})}
    {}

SglContext::~SglContext() {}

void SglContext::set_matrix_mode(sglEMatrixMode new_mode)
{
    SGL_DEBUG_OUT("[SglContext::set_matrix_mode()] setting new mode");
    mode = new_mode;
}

void SglContext::pop_matrix()
{
    if(matrix_stacks[mode].size() == 1) 
    {
        SGL_DEBUG_OUT("[SglContext::pop_matrix()] there is only a single matrix on the selected stack");
        error_cbf(sglEErrorCode::SGL_STACK_UNDERFLOW); 
        return;
    } 
    matrix_stacks[mode].pop();
    SGL_DEBUG_OUT("[SglContext::pop_matrix()] poping matrix from stack current top matrix is: \n" 
        + matrix_stacks[mode].top().to_string());
}

void SglContext::push_matrix()
{
    SGL_DEBUG_OUT("[SglContext::push_matrix()] pushing new matrix to stack");
    matrix_stacks[mode].push(matrix_stacks[mode].top());
}

void SglContext::load_identity()
{
    SGL_DEBUG_OUT("[SglContext::load_identity()] replacing top of the selected matrix stack with identity matrix");
    matrix_stacks[mode].top() = SglMatrix({
        .type = MatrixType::IDENTITY,
        .x = 0.0f,
        .y = 0.0f,
        .z = 0.0f
    });
}

void SglContext::load_matrix(const SglMatrix matrix)
{
    SGL_DEBUG_OUT("[SglContext::load_matrix()] replacing top of the selected matrix stack with new matrix: \n" 
        + matrix.to_string());
    matrix_stacks[mode].top() = matrix;
}

void SglContext::mult_matrix(const SglMatrix matrix)
{
    auto& curr_mat = matrix_stacks[mode].top();
    SGL_DEBUG_OUT("[SglContext::mult_matrix()] multiplying matrix: \n" + curr_mat.to_string() + " with: \n" + matrix.to_string());
    curr_mat = curr_mat * matrix;
    SGL_DEBUG_OUT("[SglContext::mult_matrix()] result is \n" + matrix_stacks[mode].top().to_string());
}

void SglContext::translate(float x, float y, float z)
{
    SGL_DEBUG_OUT("[SglContext::traslate_matrix()] translating current matrix: \n" + matrix_stacks[mode].top().to_string() +
         " by x: " + std::to_string(x) + " y: " + std::to_string(y) + " z: " + std::to_string(z));
    matrix_stacks[mode].top() = matrix_stacks[mode].top() * SglMatrix({
        .type = MatrixType::TRANSLATE,
        .x = x, .y = y, .z = z
    });
    SGL_DEBUG_OUT("[SglContext::traslate_matrix()] result is: \n" + matrix_stacks[mode].top().to_string());
}

void SglContext::rotate_2d(float angle, float center_x, float center_y)
{
    SGL_DEBUG_OUT("[SglContext::rotate_2d()] 2d rotating current matrix: \n" + matrix_stacks[mode].top().to_string() +
         " by angle: " + std::to_string(angle) + " and with origin: " + std::to_string(center_x) + "," + std::to_string(center_y));
        
    // Translate back so that origin is again 0,0
    matrix_stacks[mode].top() = matrix_stacks[mode].top() * SglMatrix({
        .type = MatrixType::TRANSLATE,
        .x = center_x, .y = center_y, .z = 0.0f
    });
   // Rotate by the z axis (xy plane)
    matrix_stacks[mode].top() = matrix_stacks[mode].top() * SglMatrix({
        .type = MatrixType::ROTATE,
        .x = 0.0f, .y = 0.0f, .z = angle
    });
    // Translate so that center_x, center_y is now in the coordinate origin
    matrix_stacks[mode].top() = matrix_stacks[mode].top() * SglMatrix({
        .type = MatrixType::TRANSLATE,
        .x = -center_x, .y = -center_y, .z = 0.0f
    });
    SGL_DEBUG_OUT("[SglContext::traslate_matrix()] result is: \n" + matrix_stacks[mode].top().to_string());
}

void SglContext::rotate_y(float angle)
{
    SGL_DEBUG_OUT("[SglContext::rotate_y()] rotating current matrix: \n" + matrix_stacks[mode].top().to_string() +
         " by angle: " + std::to_string(angle));
    matrix_stacks[mode].top() = matrix_stacks[mode].top() * SglMatrix({
        .type = MatrixType::ROTATE,
        .x = 0.0f, .y = angle, .z = 0.0f
    });
    SGL_DEBUG_OUT("[SglContext::rotate_y()] result is: \n" + matrix_stacks[mode].top().to_string());
}

void SglContext::scale(float scale_x, float scale_y, float scale_z)
{
    SGL_DEBUG_OUT("[SglContext::scale()] scaling current matrix: \n" + matrix_stacks[mode].top().to_string() +
         " by x: " + std::to_string(scale_x) + " y: " + std::to_string(scale_y) + " z: " + std::to_string(scale_z));
    matrix_stacks[mode].top() = matrix_stacks[mode].top() * SglMatrix({
        .type = MatrixType::SCALE,
        .x = scale_x, .y = scale_y, .z = scale_z
    });
    SGL_DEBUG_OUT("[SglContext::scale()] result is: \n" + matrix_stacks[mode].top().to_string());
}

void SglContext::ortho(float left, float right, float bottom, float top, float near, float far)
{
    SGL_DEBUG_OUT("[SglContext::ortho()] multiplying top of the selected matrix stack: \n" 
        + matrix_stacks[mode].top().to_string() + "with orthographic projection matrix");
    matrix_stacks[mode].top() = matrix_stacks[mode].top() * SglMatrix({
        .type = MatrixType::ORTHOGRAPHIC, .x = 0.0f, .y = 0.0f, .z = 0.0f,
        .left = left, .right = right, .bottom = bottom, .top = top, .near = near, .far = far
    });
    SGL_DEBUG_OUT("[SglContext::ortho()] result is: \n" + matrix_stacks[mode].top().to_string());
}

void SglContext::viewport(int x, int y, int width, int height)
{
    viewport_mat = SglMatrix({
        .type = MatrixType::VIEWPORT, .x = static_cast<float>(x), .y = static_cast<float>(y), .z = 0.0f,
        .left = static_cast<float>(width), .right = static_cast<float>(height), .bottom = 0.0f, .top = 0.0f, .near = 0.0f, .far = 0.0f
    });
    SGL_DEBUG_OUT("[SglContext::viewport()] setting viewport mat \n" + viewport_mat.to_string());
}

void SglContext::clear(unsigned mask)
{
    framebuffer.clear_framebuffer(clear_color);
}