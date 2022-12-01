#include "sgl_core.hpp"



SglCore::SglCore() : 
    recording{false},
    current_context{-1},
    error{sglEErrorCode::SGL_NO_ERROR}
    {}

SglCore::~SglCore() {}

auto SglCore::create_context(int32_t width, int32_t height) -> int32_t
{
    // try to find any free context slots
    for(int32_t i = 0; i < static_cast<int32_t>(contexts.size()); i++)
    {
        if(contexts.at(i).released) 
        {
            // SGL_DEBUG_OUT("[SglCore::create_context()] Found released context at index " + std::to_string(i));
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
    // SGL_DEBUG_OUT("[SglCore::create_context()] Creating new context at index " +
        // std::to_string(contexts.size() -1));
    return contexts.size() - 1;
}

void SglCore::set_context(int32_t context_idx)
{
    if(context_idx > static_cast<int32_t>(contexts.size()) || 
       contexts.at(context_idx).released ) 
    {
        // SGL_DEBUG_OUT("[SglCore::set_context()] Context at index " +
            // std::to_string(context_idx) + " is invalid"); 
        set_error(sglEErrorCode::SGL_INVALID_VALUE);
        return;
    }

    // SGL_DEBUG_OUT("[SglCore::set_context()] Setting context with index " +
    //     std::to_string(context_idx) + " as active context"); 
    current_context = context_idx;
    // TODO(msakmary) very error prone code -> think about better way of doing this
    renderer.state.currentFramebuffer = &contexts.at(current_context).framebuffer;
    renderer.state.clear_color = &contexts.at(current_context).clear_color;
    renderer.state.depth_test = contexts.at(current_context).capabilites;
}

auto SglCore::get_context() -> int32_t
{
    if(current_context == -1)
    {
        // SGL_DEBUG_OUT("[SglCore::get_context()] No context has yet been created");
        set_error(sglEErrorCode::SGL_INVALID_OPERATION);
    }
    return current_context;
}

void SglCore::destroy_context(int32_t context_idx)
{
    // check if context_idx is valid
    if(context_idx > static_cast<int32_t>(contexts.size()) || 
       contexts.at(context_idx).released ) 
    {
        // SGL_DEBUG_OUT("[SglCore::destroy_context()] Context at index " +
            // std::to_string(context_idx) + " is out of bounds or was previously destroyed");
        set_error(sglEErrorCode::SGL_INVALID_VALUE);
        return;
    } 
    // check if current context is in use
    else if (context_idx == current_context)
    {
        // SGL_DEBUG_OUT("[SglCore::destroy_context()] Context at index " +
            // std::to_string(context_idx) + " is currently in use"); 
        set_error(sglEErrorCode::SGL_INVALID_OPERATION);
        return;
    }
    // SGL_DEBUG_OUT("[SglCore::destroy_contex()] Destroy context at index " + 
        // std::to_string(context_idx));
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

void SglCore::push_vertex(f32vec4 vertex)
{
    // SGL_DEBUG_OUT("Got vertex " + vertex.to_string());
    vertex = contexts.at(current_context).matrix_stacks[sglEMatrixMode::SGL_MODELVIEW].top() * vertex;
    // SGL_DEBUG_OUT("MODELVIEW MATRIX IS\n" + contexts.at(current_context).matrix_stacks[sglEMatrixMode::SGL_MODELVIEW].top().to_string());
    // SGL_DEBUG_OUT("After MODELVIEW TRANSFORM " + vertex.to_string());
    vertex = contexts.at(current_context).matrix_stacks[sglEMatrixMode::SGL_PROJECTION].top() * vertex;
    // SGL_DEBUG_OUT("After PROJECTION TRANSFORM " + vertex.to_string());
    vertex = vertex / vertex.w;
    // SGL_DEBUG_OUT("After perspective division " + vertex.to_string());
    vertex = contexts.at(current_context).viewport_mat * vertex;
    // SGL_DEBUG_OUT("After viewport transform " + vertex.to_string());

    renderer.push_vertex(vertex);
}


void SglCore::draw_circle(f32vec4 center, float radius)
{
    center = contexts.at(current_context).matrix_stacks[sglEMatrixMode::SGL_MODELVIEW].top() * center;
    center = contexts.at(current_context).matrix_stacks[sglEMatrixMode::SGL_PROJECTION].top() * center;
    center = contexts.at(current_context).viewport_mat * center;

    radius = radius * std::sqrt(get_scaling_factor());

    renderer.draw_circle(center, radius);
}


void SglCore::draw_ellipse(f32vec4 center, float a, float b) {
    SglMatrix mat = SglMatrix(contexts.at(current_context).viewport_mat);
    mat = mat * contexts.at(current_context).matrix_stacks[sglEMatrixMode::SGL_PROJECTION].top(); 
    mat = mat * contexts.at(current_context).matrix_stacks[sglEMatrixMode::SGL_MODELVIEW].top();
    
    renderer.draw_ellipse(center, a, b, mat);
}


void SglCore::draw_arc(f32vec4 center, float radius, float from, float to) {
    SglMatrix mat = SglMatrix(contexts.at(current_context).viewport_mat);
    mat = mat * contexts.at(current_context).matrix_stacks[sglEMatrixMode::SGL_PROJECTION].top(); 
    mat = mat * contexts.at(current_context).matrix_stacks[sglEMatrixMode::SGL_MODELVIEW].top();
    
    renderer.draw_arc(center, radius, from, to, mat);
}


void SglCore::raytrace_scene() {
    SglMatrix mat = SglMatrix(contexts.at(current_context).viewport_mat);
    mat = mat * contexts.at(current_context).matrix_stacks[sglEMatrixMode::SGL_PROJECTION].top(); 
    mat = mat * contexts.at(current_context).matrix_stacks[sglEMatrixMode::SGL_MODELVIEW].top();
    
    renderer.raytrace_scene(contexts.at(current_context).matrix_stacks[sglEMatrixMode::SGL_MODELVIEW].top(),
                            contexts.at(current_context).matrix_stacks[sglEMatrixMode::SGL_PROJECTION].top(),
                            contexts.at(current_context).viewport_mat);
}