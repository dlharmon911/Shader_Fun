#ifndef _GUARD_BLAZE_STRING_H_
#define _GUARD_BLAZE_STRING_H_

#include "blaze/blz_common.h"
#include "blaze/blz_stringview.h"

static const char* BLAZE_STRING_TRUE = "true";
static const char* BLAZE_STRING_FALSE = "false";

enum BLAZE_VALUE_TYPE
{
	BLAZE_VALUE_TYPE_BOOL,
	BLAZE_VALUE_TYPE_INT8,
	BLAZE_VALUE_TYPE_INT16,
	BLAZE_VALUE_TYPE_INT32,
	BLAZE_VALUE_TYPE_INT64,
	BLAZE_VALUE_TYPE_UINT8,
	BLAZE_VALUE_TYPE_UINT16,
	BLAZE_VALUE_TYPE_UINT32,
	BLAZE_VALUE_TYPE_UINT64,
	BLAZE_VALUE_TYPE_FLOAT,
	BLAZE_VALUE_TYPE_DOUBLE,
	BLAZE_VALUE_TYPE_STRING,
	BLAZE_VALUE_TYPE_COUNT
};

typedef struct blz_string_t
{
	char* m_data;
	size_t m_size;
} blz_string_t;

bool blz_string_is_valid(blz_string_t string);
bool blz_string_is_empty(blz_string_t string);
bool blz_string_equals(blz_string_t string, blz_string_t other);
int32_t blz_string_compare(blz_string_t string, blz_string_t other);

bool blz_string_get_as_bool(const char* string, size_t size);
int8_t blz_string_get_as_int8(const char* string, size_t size);
int16_t blz_string_get_as_int16(const char* string, size_t size);
int32_t blz_string_get_as_int32(const char* string, size_t size);
int64_t blz_string_get_as_int64(const char* string, size_t size);
uint8_t blz_string_get_as_uint8(const char* string, size_t size);
uint16_t blz_string_get_as_uint16(const char* string, size_t size);
uint32_t blz_string_get_as_uint32(const char* string, size_t size);
uint64_t blz_string_get_as_uint64(const char* string, size_t size);
float blz_string_get_as_float(const char* string, size_t size);
double blz_string_get_as_double(const char* string, size_t size);
blz_stringview_t blz_string_get_as_string(const char* string, size_t size);
bool blz_string_get_as_type(const char* string, size_t size, int32_t type, void* out_value);

void blz_string_set_as_bool(char* string, size_t size, bool value);
void blz_string_set_as_int8(char* string, size_t size, int8_t value);
void blz_string_set_as_int16(char* string, size_t size, int16_t value);
void blz_string_set_as_int32(char* string, size_t size, int32_t value);
void blz_string_set_as_int64(char* string, size_t size, int64_t value);
void blz_string_set_as_uint8(char* string, size_t size, uint8_t value);
void blz_string_set_as_uint16(char* string, size_t size, uint16_t value);
void blz_string_set_as_uint32(char* string, size_t size, uint32_t value);
void blz_string_set_as_uint64(char* string, size_t size, uint64_t value);
void blz_string_set_as_float(char* string, size_t size, float value);
void blz_string_set_as_double(char* string, size_t size, double value);
void blz_string_set_as_string(char* string, size_t size, blz_stringview_t value);
void blz_string_set_as_type(char* string, size_t size, int32_t type, const void* value);

#endif // !_GUARD_BLAZE_STRING_H_

