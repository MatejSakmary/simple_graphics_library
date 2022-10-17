#include "sgl_context.hpp"

SglContext::SglContext(const SglContextInitInfo & info) :
    info{info}, framebuffer(info.width, info.height), released{false}
{}

SglContext::~SglContext()
{}