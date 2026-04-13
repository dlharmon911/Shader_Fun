#include <ctype.h>
#include "blaze/blz_stringview.h"

size_t blz_string_length(const char* cstr)
{
	size_t length = 0;

	while (cstr[length] != '\0')
	{
		length++;
	}

	return length;
}

blz_stringview_t blz_stringview(blz_stringview_t str)
{
	return str;
}

blz_stringview_t blz_stringview_from_cstr(const char* cstr)
{
	return (blz_stringview_t)
	{
		.m_data = cstr,
		.m_length= blz_string_length(cstr)
	};
}

blz_stringview_t blz_stringview_from_buffer(const char* buffer, size_t n)
{
	return (blz_stringview_t)
	{
		.m_data = buffer,
		.m_length= n
	};
}

blz_stringview_t blz_stringview_ltrim(blz_stringview_t str, size_t trim_length)
{
	if (trim_length > str.m_length)
	{
		trim_length = str.m_length;
	}

	return (blz_stringview_t) { str.m_data + trim_length, str.m_length - trim_length };
}

blz_stringview_t blz_stringview_rtrim(blz_stringview_t str, size_t trim_length)
{
	if (trim_length > str.m_length)
	{
		trim_length = str.m_length;
	}

	return (blz_stringview_t) { str.m_data, str.m_length - trim_length };
}

bool blz_stringview_equals(blz_stringview_t str1, blz_stringview_t str2)
{
	return 0 == blz_stringview_compare(str1, str2);
}

int32_t blz_stringview_compare(blz_stringview_t str1, blz_stringview_t str2)
{
	size_t min_length = str1.m_length < str2.m_length ? str1.m_length : str2.m_length;

	for (size_t i = 0; i < min_length; i++)
	{
		if (str1.m_data[i] != str2.m_data[i])
		{
			return str1.m_data[i] - str2.m_data[i];
		}
	}

	return (int32_t)(str1.m_length - str2.m_length);

}

bool blz_stringview_iequals(blz_stringview_t str1, blz_stringview_t str2)
{
	return 0 == blz_stringview_icompare(str1, str2);
}

int32_t blz_stringview_icompare(blz_stringview_t str1, blz_stringview_t str2)
{
	size_t min_length = str1.m_length < str2.m_length ? str1.m_length : str2.m_length;

	for (size_t i = 0; i < min_length; i++)
	{
		int c1 = tolower((int)str1.m_data[i]);
		int c2 = tolower((int)str2.m_data[i]);

		if (c1 != c2)
		{
			return c1 - c2;
		}
	}

	return (int32_t)(str1.m_length - str2.m_length);
}
