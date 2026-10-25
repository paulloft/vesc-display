#pragma once
#include <stdlib.h>
#define MALLOC_CAP_INTERNAL 0
#define MALLOC_CAP_8BIT 0
inline void* heap_caps_malloc(size_t n,int){return malloc(n);}
