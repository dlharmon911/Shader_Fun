#include "blaze/blz_common.h"
#include "blaze/blz_string.h"
#include <ctype.h>

static const int32_t BLAZE_STRING_NUMBER_BASE = 10;

static const char* BLAZE_FORMAT_STRING[BLAZE_VALUE_TYPE_COUNT] =
{
	[BLAZE_VALUE_TYPE_BOOL] = "%s",
	[BLAZE_VALUE_TYPE_INT8] = "%d",
	[BLAZE_VALUE_TYPE_INT16] = "%d",
	[BLAZE_VALUE_TYPE_INT32] = "%d",
	[BLAZE_VALUE_TYPE_INT64] = "%lld",
	[BLAZE_VALUE_TYPE_UINT8] = "%u",
	[BLAZE_VALUE_TYPE_UINT16] = "%u",
	[BLAZE_VALUE_TYPE_UINT32] = "%u",
	[BLAZE_VALUE_TYPE_UINT64] = "%llu",
	[BLAZE_VALUE_TYPE_FLOAT] = "%f",
	[BLAZE_VALUE_TYPE_DOUBLE] = "%lf",
	[BLAZE_VALUE_TYPE_STRING] = "%s"
};


bool blz_string_is_valid(blz_string_t string)
{
	return (string.m_data != NULL) && (string.m_size > 0);
}

bool blz_string_is_empty(blz_string_t string)
{
	return !string.m_data || string.m_size == 0 || string.m_data[0] == 0;
}

bool blz_string_equals(blz_string_t string, blz_string_t other)
{
	if (!string.m_data || string.m_size == 0 || string.m_data[0] == 0)
	{
		return (other.m_size == 0);
	}
	size_t string_length = strnlen(string.m_data, string.m_size);
	if (string_length != other.m_size)
	{
		return false;
	}
	return (strncmp(string.m_data, other.m_data, string_length) == 0);
}

int32_t blz_string_compare(blz_string_t string, blz_string_t other)
{
	if (!string.m_data || string.m_size == 0 || string.m_data[0] == 0)
	{
		return (other.m_size == 0) ? 0 : -1;
	}
	size_t string_length = strnlen(string.m_data, string.m_size);
	int cmp = strncmp(string.m_data, other.m_data, (string_length < other.m_size) ? string_length : other.m_size);
	if (cmp == 0)
	{
		if (string_length < other.m_size)
		{
			return -1;
		}
		else if (string_length > other.m_size)
		{
			return 1;
		}
	}
	return cmp;
}

bool blz_string_get_as_bool(const char* string, size_t size)
{
	if (!string || size == 0 || string[0] == 0)
	{
		return false;
	}

	size_t index = 0;

	while (BLAZE_STRING_TRUE[index])
	{
		if (tolower(string[index]) != BLAZE_STRING_TRUE[index])
		{
			return false;
		}

		++index;
	}

	return true;
}

int8_t blz_string_get_as_int8(const char* string, size_t size)
{
	return (int8_t)blz_string_get_as_int32(string, size);
}

int16_t blz_string_get_as_int16(const char* string, size_t size)
{
	return (int16_t)blz_string_get_as_int32(string, size);
}

int32_t blz_string_get_as_int32(const char* string, size_t size)
{
	int32_t value = 0;
	char* endptr = NULL;

	if (!string || size == 0 || string[0] == 0)
	{
		return 0;
	}

	value = strtol(string, &endptr, BLAZE_STRING_NUMBER_BASE);

	if (endptr && *endptr != 0)
	{
		return 0;
	}

	return value;
}

int64_t blz_string_get_as_int64(const char* string, size_t size)
{
	int64_t value = 0;
	char* endptr = NULL;

	if (!string || size == 0 || string[0] == 0)
	{
		return 0;
	}

	value = strtoll(string, &endptr, BLAZE_STRING_NUMBER_BASE);

	if (endptr && *endptr != 0)
	{
		return 0;
	}

	return value;
}

uint8_t blz_string_get_as_uint8(const char* string, size_t size)
{
	return (uint8_t)blz_string_get_as_uint32(string, size);
}

uint16_t blz_string_get_as_uint16(const char* string, size_t size)
{
	return (uint8_t)blz_string_get_as_uint32(string, size);
}

uint32_t blz_string_get_as_uint32(const char* string, size_t size)
{
	uint32_t value = 0;
	char* endptr = NULL;

	if (!string || size == 0 || string[0] == 0)
	{
		return 0;
	}

	value = strtoul(string, &endptr, BLAZE_STRING_NUMBER_BASE);

	if (endptr && *endptr != 0)
	{
		return 0;
	}

	return value;
}

uint64_t blz_string_get_as_uint64(const char* string, size_t size)
{
	uint64_t value = 0;
	char* endptr = NULL;

	if (!string || size == 0 || string[0] == 0)
	{
		return 0;
	}

	value = strtoull(string, &endptr, BLAZE_STRING_NUMBER_BASE);

	if (endptr && *endptr != 0)
	{
		return 0;
	}

	return value;
}

float blz_string_get_as_float(const char* string, size_t size)
{
	float value = 0.0f;
	char* endptr = NULL;

	if (!string || size == 0 || string[0] == 0)
	{
		return 0;
	}

	value = strtof(string, &endptr);

	if (endptr && *endptr != 0)
	{
		return 0;
	}

	return value;
}

double blz_string_get_as_double(const char* string, size_t size)
{
	double value = 0.0;
	char* endptr = NULL;

	if (!string || size == 0 || string[0] == 0)
	{
		return 0;
	}

	value = strtod(string, &endptr);

	if (endptr && *endptr != 0)
	{
		return 0;
	}

	return value;
}

blz_stringview_t blz_string_get_as_string(const char* string, size_t size)
{
	blz_stringview_t result = { 0 };

	if (!string || size == 0 || string[0] == 0)
	{
		return (blz_stringview_t) { 0, NULL };
	}

	result.m_data = string;
	result.m_length = strnlen(string, size);
	return result;
}

bool blz_string_get_as_type(const char* string, size_t size, int32_t type, void* out_value)
{
	if (!out_value)
	{
		return false;
	}

	switch (type)
	{
	case BLAZE_VALUE_TYPE_BOOL:
		*(bool*)out_value = blz_string_get_as_bool(string, size);
		return true;
	case BLAZE_VALUE_TYPE_INT8:
		*(int8_t*)out_value = blz_string_get_as_int8(string, size);
		return true;
	case BLAZE_VALUE_TYPE_INT16:
		*(int16_t*)out_value = blz_string_get_as_int16(string, size);
		return true;
	case BLAZE_VALUE_TYPE_INT32:
		*(int32_t*)out_value = blz_string_get_as_int32(string, size);
		return true;
	case BLAZE_VALUE_TYPE_INT64:
		*(int64_t*)out_value = blz_string_get_as_int64(string, size);
		return true;
	case BLAZE_VALUE_TYPE_UINT8:
		*(uint8_t*)out_value = blz_string_get_as_uint8(string, size);
		return true;
	case BLAZE_VALUE_TYPE_UINT16:
		*(uint16_t*)out_value = blz_string_get_as_uint16(string, size);
		return true;
	case BLAZE_VALUE_TYPE_UINT32:
		*(uint32_t*)out_value = blz_string_get_as_uint32(string, size);
		return true;
	case BLAZE_VALUE_TYPE_UINT64:
		*(uint64_t*)out_value = blz_string_get_as_uint64(string, size);
		return true;
	case BLAZE_VALUE_TYPE_FLOAT:
		*(float*)out_value = blz_string_get_as_float(string, size);
		return true;
	case BLAZE_VALUE_TYPE_DOUBLE:
		*(double*)out_value = blz_string_get_as_double(string, size);
		return true;
	case BLAZE_VALUE_TYPE_STRING:
	default:
		return false;
	}

	return false;
}

void blz_string_set_as_bool(char* string, size_t size, bool value)
{
	if (!string || size == 0)
	{
		return;
	}

	const char* s = (value ? BLAZE_STRING_TRUE : BLAZE_STRING_FALSE);

	sprintf_s(string, size, BLAZE_FORMAT_STRING[BLAZE_VALUE_TYPE_BOOL], s);
}

void blz_string_set_as_int8(char* string, size_t size, int8_t value)
{
	if (!string || size == 0)
	{
		return;
	}

	const char* s = (value ? BLAZE_STRING_TRUE : BLAZE_STRING_FALSE);

	sprintf_s(string, size, BLAZE_FORMAT_STRING[BLAZE_VALUE_TYPE_INT8], s);
}

void blz_string_set_as_int16(char* string, size_t size, int16_t value)
{
	if (!string || size == 0)
	{
		return;
	}

	const char* s = (value ? BLAZE_STRING_TRUE : BLAZE_STRING_FALSE);

	sprintf_s(string, size, BLAZE_FORMAT_STRING[BLAZE_VALUE_TYPE_INT16], s);
}

void blz_string_set_as_int32(char* string, size_t size, int32_t value)
{
	if (!string || size == 0)
	{
		return;
	}

	const char* s = (value ? BLAZE_STRING_TRUE : BLAZE_STRING_FALSE);

	sprintf_s(string, size, BLAZE_FORMAT_STRING[BLAZE_VALUE_TYPE_INT32], s);
}

void blz_string_set_as_int64(char* string, size_t size, int64_t value)
{
	if (!string || size == 0)
	{
		return;
	}

	const char* s = (value ? BLAZE_STRING_TRUE : BLAZE_STRING_FALSE);

	sprintf_s(string, size, BLAZE_FORMAT_STRING[BLAZE_VALUE_TYPE_INT64], s);
}

void blz_string_set_as_uint8(char* string, size_t size, uint8_t value)
{
	if (!string || size == 0)
	{
		return;
	}

	const char* s = (value ? BLAZE_STRING_TRUE : BLAZE_STRING_FALSE);

	sprintf_s(string, size, BLAZE_FORMAT_STRING[BLAZE_VALUE_TYPE_UINT8], s);
}

void blz_string_set_as_uint16(char* string, size_t size, uint16_t value)
{
	if (!string || size == 0)
	{
		return;
	}

	const char* s = (value ? BLAZE_STRING_TRUE : BLAZE_STRING_FALSE);

	sprintf_s(string, size, BLAZE_FORMAT_STRING[BLAZE_VALUE_TYPE_UINT16], s);
}

void blz_string_set_as_uint32(char* string, size_t size, uint32_t value)
{
	if (!string || size == 0)
	{
		return;
	}

	const char* s = (value ? BLAZE_STRING_TRUE : BLAZE_STRING_FALSE);

	sprintf_s(string, size, BLAZE_FORMAT_STRING[BLAZE_VALUE_TYPE_UINT32], s);
}

void blz_string_set_as_uint64(char* string, size_t size, uint64_t value)
{
	if (!string || size == 0)
	{
		return;
	}

	const char* s = (value ? BLAZE_STRING_TRUE : BLAZE_STRING_FALSE);

	sprintf_s(string, size, BLAZE_FORMAT_STRING[BLAZE_VALUE_TYPE_UINT64], s);
}

void blz_string_set_as_float(char* string, size_t size, float value)
{
	if (!string || size == 0)
	{
		return;
	}

	const char* s = (value ? BLAZE_STRING_TRUE : BLAZE_STRING_FALSE);

	sprintf_s(string, size, BLAZE_FORMAT_STRING[BLAZE_VALUE_TYPE_FLOAT], s);
}

void blz_string_set_as_double(char* string, size_t size, double value)
{
	if (!string || size == 0)
	{
		return;
	}

	const char* s = (value ? BLAZE_STRING_TRUE : BLAZE_STRING_FALSE);

	sprintf_s(string, size, BLAZE_FORMAT_STRING[BLAZE_VALUE_TYPE_DOUBLE], s);
}

void blz_string_set_as_string(char* string, size_t size, blz_stringview_t value)
{
	if (!string || size == 0)
	{
		return;
	}
	size_t copy_size = (value.m_length < size) ? value.m_length : (size - 1);
	memcpy(string, value.m_data, copy_size);
	string[copy_size] = 0;
}

void blz_string_set_as_type(char* string, size_t size, int32_t type, const void* value)
{
	if (!string || size == 0 || !value)
	{
		return;
	}
	switch (type)
	{
	case BLAZE_VALUE_TYPE_BOOL:
		blz_string_set_as_bool(string, size, *(const bool*)value);
		break;
	case BLAZE_VALUE_TYPE_INT8:
		blz_string_set_as_int8(string, size, *(const int8_t*)value);
		break;
	case BLAZE_VALUE_TYPE_INT16:
		blz_string_set_as_int16(string, size, *(const int16_t*)value);
		break;
	case BLAZE_VALUE_TYPE_INT32:
		blz_string_set_as_int32(string, size, *(const int32_t*)value);
		break;
	case BLAZE_VALUE_TYPE_INT64:
		blz_string_set_as_int64(string, size, *(const int64_t*)value);
		break;
	case BLAZE_VALUE_TYPE_UINT8:
		blz_string_set_as_uint8(string, size, *(const uint8_t*)value);
		break;
	case BLAZE_VALUE_TYPE_UINT16:
		blz_string_set_as_uint16(string, size, *(const uint16_t*)value);
		break;
	case BLAZE_VALUE_TYPE_UINT32:
		blz_string_set_as_uint32(string, size, *(const uint32_t*)value);
		break;
	case BLAZE_VALUE_TYPE_UINT64:
		blz_string_set_as_uint64(string, size, *(const uint64_t*)value);
		break;
	case BLAZE_VALUE_TYPE_FLOAT:
		blz_string_set_as_float(string, size, *(const float*)value);
		break;
	case BLAZE_VALUE_TYPE_DOUBLE:
		blz_string_set_as_double(string, size, *(const double*)value);
		break;
	case BLAZE_VALUE_TYPE_STRING:
		blz_string_set_as_string(string, size, *(const blz_stringview_t*)value);
		break;
	default:
		break;
	}
}