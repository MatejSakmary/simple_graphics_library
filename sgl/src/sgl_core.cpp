#include "sgl_core.hpp"


SglCore::SglCore()
{};

SglCore::~SglCore()
{};

int32_t SglCore::create_context(int32_t width, int32_t height)
{
    // try to find any free context slots
    for(int32_t i = 0; i < contexts.size(); i++)
    {
        if(contexts.at(i).released) 
        {
            contexts.at(i) = {{
                .width = static_cast<uint32_t>(width),
                .height = static_cast<uint32_t>(height)
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
    if(context_idx < contexts.size()) 
    {
        //TODO(msakmary) -> Figure out how to throw error here
    }
    current_context = context_idx;
}

int32_t SglCore::get_context()
{
    return current_context;
}