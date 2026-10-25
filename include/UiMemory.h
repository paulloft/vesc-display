#pragma once
#include <stdlib.h>
#ifdef ARDUINO_ARCH_ESP32
#include <esp_heap_caps.h>
static inline void* uiMalloc(size_t n) { return heap_caps_malloc(n,(n<=4096?MALLOC_CAP_INTERNAL:MALLOC_CAP_SPIRAM)|MALLOC_CAP_8BIT); }
static inline void* uiRealloc(void* p,size_t n) { return heap_caps_realloc(p,n,(n<=4096?MALLOC_CAP_INTERNAL:MALLOC_CAP_SPIRAM)|MALLOC_CAP_8BIT); }
#else
#define uiMalloc malloc
#define uiRealloc realloc
#endif
