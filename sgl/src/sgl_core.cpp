#include "sgl_core.hpp"


SglCore::SglCore() {}

SglCore::~SglCore() {}

int32_t SglCore::create_context(int32_t width, int32_t height)
{
    // try to find any free context slots
    for(int32_t i = 0; i < contexts.size(); i++)
    {
        if(contexts.at(i).released) 
        {
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
        .height = static_cast<uint32_t>(height)
    });
    return contexts.size() - 1;
}

void SglCore::set_context(int32_t context_idx)
{
    if(context_idx < contexts.size() || 
       contexts.at(context_idx).released ) 
    {
        set_error(sglEErrorCode::SGL_INVALID_VALUE);
        return;
    } else if (context_idx == current_context)
    {
        set_error(sglEErrorCode::SGL_INVALID_OPERATION);
        return;
    }
    current_context = context_idx;
}

int32_t SglCore::get_context()
{
    if(current_context == -1)
    {
        set_error(sglEErrorCode::SGL_INVALID_OPERATION);
    }
    return current_context;
}

void SglCore::destroy_context(int32_t context_idx)
{
    // check if context_idx is valid
    if(context_idx < contexts.size() || 
       contexts.at(context_idx).released ) 
    {
        set_error(sglEErrorCode::SGL_INVALID_VALUE);
        return;
    } 
    // check if current context is in use
    else if (context_idx == current_context)
    {
        set_error(sglEErrorCode::SGL_INVALID_OPERATION);
        return;
    }
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
