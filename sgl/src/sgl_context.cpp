#include "sgl_context.hpp"




SglContext::SglContext(const SglContextInitInfo & info) :
    info{info},
    framebuffer(info.width, info.height),
    clear_color{0.0f,0.0f,0.0f},
    released{false},
    mode{sglEMatrixMode::SGL_MODELVIEW},
    error_cbf{info.error_cbf},
    matrix_stacks{
        std::stack<SglMatrix>({SglMatrix({.type = MatrixType::IDENTITY, .x = 0.0f, .y = 0.0f, .z = 0.0f})}),
        std::stack<SglMatrix>({SglMatrix({.type = MatrixType::IDENTITY, .x = 0.0f, .y = 0.0f, .z = 0.0f})})},
    viewport_mat{SglMatrix({.type = MatrixType::IDENTITY, .x = 0.0f, .y = 0.0f, .z = 0.0f})},
    environment_map{false, 0,0,{}}
    {}

SglContext::~SglContext() {}

void SglContext::set_matrix_mode(sglEMatrixMode new_mode)
{
    mode = new_mode;
}

void SglContext::pop_matrix()
{
    if(matrix_stacks[mode].size() == 1) 
    {
        error_cbf(sglEErrorCode::SGL_STACK_UNDERFLOW); 
        return;
    } 
    matrix_stacks[mode].pop();
}

void SglContext::push_matrix()
{
    matrix_stacks[mode].push(matrix_stacks[mode].top());
}

void SglContext::load_identity()
{
    matrix_stacks[mode].top() = SglMatrix({
        .type = MatrixType::IDENTITY,
        .x = 0.0f,
        .y = 0.0f,
        .z = 0.0f
    });
}

void SglContext::load_matrix(const SglMatrix matrix)
{
    matrix_stacks[mode].top() = matrix;
}

void SglContext::mult_matrix(const SglMatrix matrix)
{
    auto& curr_mat = matrix_stacks[mode].top();
    curr_mat = curr_mat * matrix;
}

void SglContext::translate(float x, float y, float z)
{
    matrix_stacks[mode].top() = matrix_stacks[mode].top() * SglMatrix({
        .type = MatrixType::TRANSLATE,
        .x = x, .y = y, .z = z
    });
}

void SglContext::rotate_2d(float angle, float center_x, float center_y)
{
    matrix_stacks[mode].top() = matrix_stacks[mode].top() * SglMatrix({
        .type = MatrixType::TRANSLATE,
        .x = center_x, .y = center_y, .z = 0.0f
    });
    matrix_stacks[mode].top() = matrix_stacks[mode].top() * SglMatrix({
        .type = MatrixType::ROTATE,
        .x = 0.0f, .y = 0.0f, .z = angle
    });
    matrix_stacks[mode].top() = matrix_stacks[mode].top() * SglMatrix({
        .type = MatrixType::TRANSLATE,
        .x = -center_x, .y = -center_y, .z = 0.0f
    });
}

void SglContext::rotate_y(float angle)
{
    matrix_stacks[mode].top() = matrix_stacks[mode].top() * SglMatrix({
        .type = MatrixType::ROTATE,
        .x = 0.0f, .y = angle, .z = 0.0f
    });
}

void SglContext::scale(float scale_x, float scale_y, float scale_z)
{
    matrix_stacks[mode].top() = matrix_stacks[mode].top() * SglMatrix({
        .type = MatrixType::SCALE,
        .x = scale_x, .y = scale_y, .z = scale_z
    });
}

void SglContext::ortho(float left, float right, float bottom, float top, float near, float far)
{
    matrix_stacks[mode].top() = matrix_stacks[mode].top() * SglMatrix({
        .type = MatrixType::ORTHOGRAPHIC, .x = 0.0f, .y = 0.0f, .z = 0.0f,
        .left = left, .right = right, .bottom = bottom, .top = top, .near = near, .far = far
    });
}

void SglContext::perspective(float left, float right, float bottom, float top, float near, float far)
{
    matrix_stacks[mode].top() = matrix_stacks[mode].top() * SglMatrix({
        .type = MatrixType::PERSPECTIVE, .x = 0.0f, .y = 0.0f, .z = 0.0f,
        .left = left, .right = right, .bottom = bottom, .top = top, .near = near, .far = far
    });
}

void SglContext::viewport(int x, int y, int width, int height)
{
    viewport_mat = SglMatrix({
        .type = MatrixType::VIEWPORT, .x = static_cast<float>(x), .y = static_cast<float>(y), .z = 0.0f,
        .left = static_cast<float>(width), .right = static_cast<float>(height), .bottom = 0.0f, .top = 0.0f, .near = 0.0f, .far = 0.0f
    });
}

void SglContext::clear(unsigned mask)
{
    framebuffer.clear_framebuffer(clear_color, mask);
}

f32vec3 SglEnvMap::at(const f32vec3 &dir) const {
    if(!this->set){
        return {0.0f, 0.0f, 0.0f};
    }
    float d = std::sqrt(dir.x*dir.x + dir.y*dir.y);
    float r = d>0 ? acos(dir.z)/(2*M_PI*d) : 0.0f;
    float u = 0.5f + dir.x * r;
    float v = 0.5f + dir.y * r;
    uint32_t x = (uint32_t) (u * width);
    uint32_t y = (uint32_t) (v * height);
    unsigned flipY_index = (y * width + x);
    return env_map[flipY_index];
}
