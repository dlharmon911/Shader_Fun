#ifndef _GUARD_BLAZE_DYNAMIC_ARRAY_H_
#define _GUARD_BLAZE_DYNAMIC_ARRAY_H_

#include "blaze/blz_common.h"

size_t blz_darray_size(const void* array);
void blz_darray_destroy(void* array);
void* blz_darray_at(void* array, size_t index);
const void* blz_darray_at_const(const void* array, size_t index);
bool blz_darray_pushback(void** array, const void* element, size_t element_size);
bool blz_darray_pop(void* array);


#endif // !_GUARD_BLAZE_DYNAMIC_ARRAY_H_

