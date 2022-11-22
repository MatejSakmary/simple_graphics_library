#pragma once
#pragma GCC optimize("O3,unroll-loops,fast-math")

#ifdef SGL_LOG_OUTPUT
#include <iostream>
#define SGL_DEBUG_OUT(x) (std::cout << (x) << std::endl)
#define SGL_DEBUG_VAR_OUT(x) (std::cout << #x << ": " << x << std::endl)
#else
#define SGL_DEBUG_OUT(x)
#define SGL_DEBUG_VAR_OUT(x)
#endif