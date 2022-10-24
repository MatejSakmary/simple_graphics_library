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
    SGL_DEBUG_OUT("[SglCore::push_vertex()] Transforming vertex: \n" + vertex.to_string());
    vertex = contexts.at(current_context).matrix_stacks[sglEMatrixMode::SGL_MODELVIEW].top() * vertex;
    SGL_DEBUG_OUT("[SglCore::push_vertex()] vertex transformed by modelview: \n" + vertex.to_string());
    vertex = contexts.at(current_context).matrix_stacks[sglEMatrixMode::SGL_PROJECTION].top() * vertex;
    SGL_DEBUG_OUT("[SglCore::push_vertex()] vertex transformed by modelview and projection: \n" + vertex.to_string());
    vertex = contexts.at(current_context).viewport_mat * vertex;
    SGL_DEBUG_OUT("[SglCore::push_vertex()] vertex transformed by modelview, projection and viewport: \n" + vertex.to_string());
    renderer.push_vertex(vertex);
}

void SglCore::draw_circle(SglVertex center, float radius) {
    //add transforms
    renderer.draw_circle(center, radius);
}


void SglCore::draw_ellipse(SglVertex center, float a, float b) {
    //add transforms
    renderer.draw_ellipse(center, a, b);
}


void SglCore::draw_arc(SglVertex center, float radius, float from, float to) {
    //add transforms
    renderer.draw_arc(center, radius, from, to);
}