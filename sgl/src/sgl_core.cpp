#include "sgl_core.hpp"

SglCore::SglCore() : 
    current_context{-1},
    error{sglEErrorCode::SGL_NO_ERROR},
    recording{false}
    {}

SglCore::~SglCore() {}

auto SglCore::create_context(int32_t width, int32_t height) -> int32_t
{
    // try to find any free context slots
    for(int32_t i = 0; i < contexts.size(); i++)
    {
        if(contexts.at(i).released) 
        {
            SGL_DEBUG_OUT("[SglCore::create_context()] Found released context at index " + std::to_string(i));
            contexts.at(i) = {{
                .width = static_cast<uint32_t>(width),
                .height = static_cast<uint32_t>(height),
                .error_cbf = std::bind(&SglCore::set_error, this, std::placeholders::_1)
            }};
            return i;
        }
    }

    // if we find no free context slots create new one at the end
    contexts.emplace_back(SglContextInitInfo{
        .width = static_cast<uint32_t>(width),
        .height = static_cast<uint32_t>(height),
        .error_cbf = std::bind(&SglCore::set_error, this, std::placeholders::_1)
    });
    SGL_DEBUG_OUT("[SglCore::create_context()] Creating new context at index " +
        std::to_string(contexts.size() -1));
    return contexts.size() - 1;
}

void SglCore::set_context(int32_t context_idx)
{
    if(context_idx > contexts.size() || 
       contexts.at(context_idx).released ) 
    {
        SGL_DEBUG_OUT("[SglCore::set_context()] Context at index " +
            std::to_string(context_idx) + " is invalid"); 
        set_error(sglEErrorCode::SGL_INVALID_VALUE);
        return;
    }

    SGL_DEBUG_OUT("[SglCore::set_context()] Setting context with index " +
        std::to_string(context_idx) + " as active context"); 
    current_context = context_idx;
    renderer.state.currentFramebuffer = &contexts.at(current_context).framebuffer;
}

auto SglCore::get_context() -> int32_t
{
    if(current_context == -1)
    {
        SGL_DEBUG_OUT("[SglCore::get_context()] No context has yet been created");
        set_error(sglEErrorCode::SGL_INVALID_OPERATION);
    }
    return current_context;
}

void SglCore::destroy_context(int32_t context_idx)
{
    // check if context_idx is valid
    if(context_idx > contexts.size() || 
       contexts.at(context_idx).released ) 
    {
        SGL_DEBUG_OUT("[SglCore::destroy_context()] Context at index " +
            std::to_string(context_idx) + " is out of bounds or was previously destroyed");
        set_error(sglEErrorCode::SGL_INVALID_VALUE);
        return;
    } 
    // check if current context is in use
    else if (context_idx == current_context)
    {
        SGL_DEBUG_OUT("[SglCore::destroy_context()] Context at index " +
            std::to_string(context_idx) + " is currently in use"); 
        set_error(sglEErrorCode::SGL_INVALID_OPERATION);
        return;
    }
    SGL_DEBUG_OUT("[SglCore::destroy_contex()] Destroy context at index " + 
        std::to_string(context_idx));
    contexts.at(context_idx).released = true;
}

void SglCore::set_error(sglEErrorCode error_code)
{
    if(error == sglEErrorCode::SGL_NO_ERROR) { error = error_code; }
}

sglEErrorCode SglCore::get_error()
{
    sglEErrorCode error_ret = error;
    error = sglEErrorCode::SGL_NO_ERROR;
    return error_ret;
}

float SglCore::get_scaling_factor() {
    SglMatrix mat = SglMatrix(contexts.at(current_context).viewport_mat);
    mat = mat * contexts.at(current_context).matrix_stacks[sglEMatrixMode::SGL_PROJECTION].top(); 
    mat = mat * contexts.at(current_context).matrix_stacks[sglEMatrixMode::SGL_MODELVIEW].top();
    return  mat.at(0,0) * mat.at(1,1) - mat.at(0, 1) * mat.at(1, 0);
}

SglMatrix SglCore::get_matrix() {
    SglMatrix mat = SglMatrix(contexts.at(current_context).viewport_mat);
    mat = mat * contexts.at(current_context).matrix_stacks[sglEMatrixMode::SGL_PROJECTION].top(); 
    mat = mat * contexts.at(current_context).matrix_stacks[sglEMatrixMode::SGL_MODELVIEW].top();
    return mat;
}

void SglCore::set_recording(bool new_recording)
{
    recording = new_recording;
    if(recording == true) { renderer.recording_start(); }
    else                  { renderer.recording_end(); }
}

auto SglCore::get_recording() -> bool
{
    return recording;
}

void SglCore::push_vertex(SglVertex vertex)
{
    // SGL_DEBUG_OUT("[SglCore::push_vertex()] Transforming vertex: \n" + vertex.to_string());
    vertex = contexts.at(current_context).matrix_stacks[sglEMatrixMode::SGL_MODELVIEW].top() * vertex;
    // SGL_DEBUG_OUT("[SglCore::push_vertex()] vertex transformed by modelview: \n" + vertex.to_string());
    vertex = contexts.at(current_context).matrix_stacks[sglEMatrixMode::SGL_PROJECTION].top() * vertex;
    // SGL_DEBUG_OUT("[SglCore::push_vertex()] vertex transformed by modelview and projection: \n" + vertex.to_string());
    vertex = contexts.at(current_context).viewport_mat * vertex;
    // SGL_DEBUG_OUT("[SglCore::push_vertex()] vertex transformed by modelview, projection and viewport: \n" + vertex.to_string());
    renderer.push_vertex(vertex);
}

void SglCore::push_sym_vertices(int x_c, int y_c, SglVertex vert) {
    for (int i = 0; i < 2; ++i) {
        for (int j = 0; j < 2; ++j) {
            SglVertex a = SglVertex(x_c + (pow(-1, i) * vert.at(0)), y_c + (pow(-1, j) * vert.at(1)), vert.at(2), vert.at(3));
            renderer.push_vertex(a);
        }
    }
}

void SglCore::draw_circle(SglVertex center, float radius) 
{
    center = contexts.at(current_context).matrix_stacks[sglEMatrixMode::SGL_MODELVIEW].top() * center;
    center = contexts.at(current_context).matrix_stacks[sglEMatrixMode::SGL_PROJECTION].top() * center;
    center = contexts.at(current_context).viewport_mat * center;

    radius = radius * std::sqrt(get_scaling_factor());

    renderer.draw_circle(center, static_cast<int>(radius));
}


void SglCore::draw_ellipse(SglVertex center, float a, float b) {
    int x_c = static_cast<int>(center.at(0));
    int y_c = static_cast<int>(center.at(1));
    int z_c = static_cast<int>(center.at(2));
    
    // SGL_DEBUG_OUT("[SglCore::push_vertex()] Transforming vertex: \n" + vertex.to_string());
    center = contexts.at(current_context).matrix_stacks[sglEMatrixMode::SGL_MODELVIEW].top() * center;
    // SGL_DEBUG_OUT("[SglCore::push_vertex()] vertex transformed by modelview: \n" + vertex.to_string());
    center = contexts.at(current_context).matrix_stacks[sglEMatrixMode::SGL_PROJECTION].top() * center;
    // SGL_DEBUG_OUT("[SglCore::push_vertex()] vertex transformed by modelview and projection: \n" + vertex.to_string());
    center = contexts.at(current_context).viewport_mat * center;

    a = a * std::sqrt(get_scaling_factor());
    b = b * std::sqrt(get_scaling_factor());

    // renderer.draw_ellipse(center, static_cast<int>(a), static_cast<int>(b), get_matrix());
    // SglMatrix mat = get_matrix();
    int d_x, d_y, p, x, y, b2, a2;
    int x_c_t = static_cast<int>(center.at(0));
    int y_c_t = static_cast<int>(center.at(1));

    x = 0;
    y = b;
    a2 = a * a;
    b2 = b * b;
    d_x = 2 * b2 * x;
    d_y = 2 * a2 * y;
    
    p = b2 - (a2 * b) + (0.25 * a2);
    
    while (d_x < d_y) {
        SglVertex vert = SglVertex(x, y, z_c, 1.0f);
        SGL_DEBUG_OUT("Vertex is " + std::to_string(vert.at(0)) + " " + std::to_string(vert.at(1)));
        SglVertex vert_t = (contexts.at(current_context).matrix_stacks[sglEMatrixMode::SGL_MODELVIEW].top() * vert);
        SGL_DEBUG_OUT("Transformed vertex is " + std::to_string(vert_t.at(0)) + " " + std::to_string(vert_t.at(1)));
        push_sym_vertices(x_c_t, y_c_t, vert_t);
        if (p >= 0) {
            --y;
            d_y -= 2 * a2;
            p -= d_y;
        }
        ++x;
        d_x += 2 * b2;
        p += d_x + b2;
    }

    p = (b2 * ((x + 0.5) * (x + 0.5))) + (a2 * ((y - 1) * (y - 1))) - (a2 * b2);
    
    while (y >= 0) {
        SglVertex vert = SglVertex(x, y, z_c, 1.0f);
        SglVertex vert_t = (contexts.at(current_context).matrix_stacks[sglEMatrixMode::SGL_MODELVIEW].top() * vert);
        push_sym_vertices(x_c_t, y_c_t, vert_t);
        if (p <= 0)
        {
            x++;
            d_x += 2 * b2;
            p += d_x;
        }
        --y;
        d_y -= 2 * a2;
        p += a2 - d_y;
    }
}


void SglCore::draw_arc(SglVertex center, float radius, float from, float to) {
    // SGL_DEBUG_OUT("[SglCore::push_vertex()] Transforming vertex: \n" + vertex.to_string());
    center = contexts.at(current_context).matrix_stacks[sglEMatrixMode::SGL_MODELVIEW].top() * center;
    // SGL_DEBUG_OUT("[SglCore::push_vertex()] vertex transformed by modelview: \n" + vertex.to_string());
    center = contexts.at(current_context).matrix_stacks[sglEMatrixMode::SGL_PROJECTION].top() * center;
    // SGL_DEBUG_OUT("[SglCore::push_vertex()] vertex transformed by modelview and projection: \n" + vertex.to_string());
    center = contexts.at(current_context).viewport_mat * center;

    radius = radius * std::sqrt(get_scaling_factor());

    renderer.draw_arc(center, static_cast<int>(radius), static_cast<int>(from), static_cast<int>(to));
}