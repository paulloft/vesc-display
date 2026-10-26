#include <lvgl.h>
#include "UiMemory.h"
#include <string.h>

/* Small, frequently used LVGL objects stay in internal RAM; large drawing
 * buffers go to PSRAM. Same allocator is used by the host renderer. */
void lv_mem_init(void) {}
void lv_mem_deinit(void) {}
lv_mem_pool_t lv_mem_add_pool(void *p,size_t n) {(void)p;(void)n;return NULL;}
void lv_mem_remove_pool(lv_mem_pool_t p) {(void)p;}
void *lv_malloc_core(size_t n) {return uiMalloc(n);}
void *lv_realloc_core(void *p,size_t n) {return uiRealloc(p,n);}
void lv_free_core(void *p) {free(p);}
void lv_mem_monitor_core(lv_mem_monitor_t *m) {memset(m,0,sizeof(*m));}
lv_result_t lv_mem_test_core(void) {return LV_RESULT_OK;}
