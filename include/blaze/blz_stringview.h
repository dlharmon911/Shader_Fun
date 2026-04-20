#ifndef _GUARD_BLAZE_STRINGVIEW_H_
#define _GUARD_BLAZE_STRINGVIEW_H_

#include "blaze/blz_common.h"

typedef struct blz_stringview_tag_t
{
	size_t m_length;
	const char* m_data;
} blz_stringview_t;

size_t blz_string_length(const char* cstr);
blz_stringview_t blz_stringview(blz_stringview_t str);
blz_stringview_t blz_stringview_from_cstr(const char* cstr);
blz_stringview_t blz_stringview_from_buffer(const char* buffer, size_t n);
blz_stringview_t blz_stringview_ltrim(blz_stringview_t str, size_t n);
blz_stringview_t blz_stringview_rtrim(blz_stringview_t str, size_t n);
bool blz_stringview_equals(blz_stringview_t str1, blz_stringview_t str2);
int32_t blz_stringview_compare(blz_stringview_t str1, blz_stringview_t str2);
bool blz_stringview_iequals(blz_stringview_t str1, blz_stringview_t str2);
int32_t blz_stringview_icompare(blz_stringview_t str1, blz_stringview_t str2);

#endif // !_GUARD_BLAZE_STRINGVIEW_H_

