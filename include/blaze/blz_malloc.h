#ifndef _GUARD_BLAZE_MALLOC_H_
#define _GUARD_BLAZE_MALLOC_H_

#include "blaze/blz_common.h"

typedef int32_t (*blz_malloc_init_t)(void*, void*);
typedef void (*blz_malloc_uninit_t)(void*);

void* blz_malloc_new(size_t size);
void* blz_malloc_new_realloc(void* ptr, size_t size);
void* blz_malloc_new_initialized(size_t size, blz_malloc_init_t init, blz_malloc_uninit_t uninit, void* data);
void blz_malloc_delete(void* ptr);
size_t blz_malloc_size(void* ptr);

#endif // !_GUARD_BLAZE_MALLOC_H_

