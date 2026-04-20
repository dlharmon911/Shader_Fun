#include "blaze/blz_common.h"
#include "blaze/blz_string.h"
#include "blaze/blz_stringview.h"
#include "blaze/blz_config.h"

const char* blz_config_get_value_as_string(ALLEGRO_CONFIG* config, const char* section, const char* key, const char* default_value)
{
	if (!config || !section || !key || !default_value)
	{
		return NULL;
	}

	const char* value = al_get_config_value(config, section, key);

	if (!value)
	{
		al_set_config_value(config, section, key, default_value);
		value = default_value;
	}

	return value;
}

bool blz_config_get_value_as_bool(ALLEGRO_CONFIG* config, const char* section, const char* key, bool default_value)
{
	const char* default_value_str = default_value ? BLAZE_STRING_TRUE : BLAZE_STRING_FALSE;
	
	const char* value_str = blz_config_get_value_as_string(config, section, key, default_value_str);

	return blz_string_get_as_bool(value_str, strlen(value_str));
}
int8_t blz_config_get_value_as_int8(ALLEGRO_CONFIG* config, const char* section, const char* key, int8_t default_value)
{
	char default_value_str[256] = { 0 };
	blz_string_set_as_int8(default_value_str, sizeof(default_value_str), default_value);

	const char* value_str = blz_config_get_value_as_string(config, section, key, default_value_str);

	return blz_string_get_as_int8(value_str, strlen(value_str));
}

int16_t blz_config_get_value_as_int16(ALLEGRO_CONFIG* config, const char* section, const char* key, int16_t default_value)
{
	char default_value_str[256] = { 0 };
	blz_string_set_as_int16(default_value_str, sizeof(default_value_str), default_value);

	const char* value_str = blz_config_get_value_as_string(config, section, key, default_value_str);

	return blz_string_get_as_int16(value_str, strlen(value_str));
}

int32_t blz_config_get_value_as_int32(ALLEGRO_CONFIG* config, const char* section, const char* key, int32_t default_value)
{
	char default_value_str[256] = { 0 };
	blz_string_set_as_int32(default_value_str, sizeof(default_value_str), default_value);

	const char* value_str = blz_config_get_value_as_string(config, section, key, default_value_str);

	return blz_string_get_as_int32(value_str, strlen(value_str));
}

int64_t blz_config_get_value_as_int64(ALLEGRO_CONFIG* config, const char* section, const char* key, int64_t default_value)
{
	char default_value_str[256] = { 0 };
	blz_string_set_as_int64(default_value_str, sizeof(default_value_str), default_value);

	const char* value_str = blz_config_get_value_as_string(config, section, key, default_value_str);

	return blz_string_get_as_int64(value_str, strlen(value_str));
}

uint8_t blz_config_get_value_as_uint8(ALLEGRO_CONFIG* config, const char* section, const char* key, uint8_t default_value)
{
	char default_value_str[256] = { 0 };
	blz_string_set_as_uint8(default_value_str, sizeof(default_value_str), default_value);

	const char* value_str = blz_config_get_value_as_string(config, section, key, default_value_str);

	return blz_string_get_as_uint8(value_str, strlen(value_str));
}

uint16_t blz_config_get_value_as_uint16(ALLEGRO_CONFIG* config, const char* section, const char* key, uint16_t default_value)
{
	char default_value_str[256] = { 0 };
	blz_string_set_as_uint16(default_value_str, sizeof(default_value_str), default_value);

	const char* value_str = blz_config_get_value_as_string(config, section, key, default_value_str);

	return blz_string_get_as_uint16(value_str, strlen(value_str));
}

uint32_t blz_config_get_value_as_uint32(ALLEGRO_CONFIG* config, const char* section, const char* key, uint32_t default_value)
{
	char default_value_str[256] = { 0 };
	blz_string_set_as_uint32(default_value_str, sizeof(default_value_str), default_value);

	const char* value_str = blz_config_get_value_as_string(config, section, key, default_value_str);

	return blz_string_get_as_uint32(value_str, strlen(value_str));
}

uint64_t blz_config_get_value_as_uint64(ALLEGRO_CONFIG* config, const char* section, const char* key, uint64_t default_value)
{
	char default_value_str[256] = { 0 };
	blz_string_set_as_uint64(default_value_str, sizeof(default_value_str), default_value);

	const char* value_str = blz_config_get_value_as_string(config, section, key, default_value_str);

	return blz_string_get_as_uint64(value_str, strlen(value_str));
}

float blz_config_get_value_as_float(ALLEGRO_CONFIG* config, const char* section, const char* key, float default_value)
{
	char default_value_str[256] = { 0 };
	blz_string_set_as_float(default_value_str, sizeof(default_value_str), default_value);

	const char* value_str = blz_config_get_value_as_string(config, section, key, default_value_str);

	return blz_string_get_as_float(value_str, strlen(value_str));
}

double blz_config_get_value_as_double(ALLEGRO_CONFIG* config, const char* section, const char* key, double default_value)
{
	char default_value_str[256] = { 0 };
	blz_string_set_as_double(default_value_str, sizeof(default_value_str), default_value);

	const char* value_str = blz_config_get_value_as_string(config, section, key, default_value_str);

	return blz_string_get_as_double(value_str, strlen(value_str));
}

void blz_config_set_value_as_string(ALLEGRO_CONFIG* config, const char* section, const char* key, const char* value)
{
	if (!config || !section || !key)
	{
		return;
	}

	al_set_config_value(config, section, key, value ? value : "");
}

void blz_config_set_value_as_bool(ALLEGRO_CONFIG* config, const char* section, const char* key, bool value)
{
	char value_str[256] = { 0 };
	blz_string_set_as_bool(value_str, sizeof(value_str), value);
	blz_config_set_value_as_string(config, section, key, value_str);
}


void blz_config_set_value_as_int8(ALLEGRO_CONFIG* config, const char* section, const char* key, int8_t value)
{
	char value_str[256] = { 0 };
	blz_string_set_as_int8(value_str, sizeof(value_str), value);
	blz_config_set_value_as_string(config, section, key, value_str);
}

void blz_config_set_value_as_int16(ALLEGRO_CONFIG* config, const char* section, const char* key, int16_t value)
{
	char value_str[256] = { 0 };
	blz_string_set_as_int16(value_str, sizeof(value_str), value);
	blz_config_set_value_as_string(config, section, key, value_str);
}

void blz_config_set_value_as_int32(ALLEGRO_CONFIG* config, const char* section, const char* key, int32_t value)
{
	char value_str[256] = { 0 };
	blz_string_set_as_int32(value_str, sizeof(value_str), value);
	blz_config_set_value_as_string(config, section, key, value_str);
}

void blz_config_set_value_as_int64(ALLEGRO_CONFIG* config, const char* section, const char* key, int64_t value)
{
	char value_str[256] = { 0 };
	blz_string_set_as_int64(value_str, sizeof(value_str), value);
	blz_config_set_value_as_string(config, section, key, value_str);
}

void blz_config_set_value_as_uint8(ALLEGRO_CONFIG* config, const char* section, const char* key, uint8_t value)
{
	char value_str[256] = { 0 };
	blz_string_set_as_uint8(value_str, sizeof(value_str), value);
	blz_config_set_value_as_string(config, section, key, value_str);
}

void blz_config_set_value_as_uint16(ALLEGRO_CONFIG* config, const char* section, const char* key, uint16_t value)
{
	char value_str[256] = { 0 };
	blz_string_set_as_uint16(value_str, sizeof(value_str), value);
	blz_config_set_value_as_string(config, section, key, value_str);
}

void blz_config_set_value_as_uint32(ALLEGRO_CONFIG* config, const char* section, const char* key, uint32_t value)
{
	char value_str[256] = { 0 };
	blz_string_set_as_uint32(value_str, sizeof(value_str), value);
	blz_config_set_value_as_string(config, section, key, value_str);
}

void blz_config_set_value_as_uint64(ALLEGRO_CONFIG* config, const char* section, const char* key, uint64_t value)
{
	char value_str[256] = { 0 };
	blz_string_set_as_uint64(value_str, sizeof(value_str), value);
	blz_config_set_value_as_string(config, section, key, value_str);
}

void blz_config_set_value_as_float(ALLEGRO_CONFIG* config, const char* section, const char* key, float value)
{
	char value_str[256] = { 0 };
	blz_string_set_as_float(value_str, sizeof(value_str), value);
	blz_config_set_value_as_string(config, section, key, value_str);
}

void blz_config_set_value_as_double(ALLEGRO_CONFIG* config, const char* section, const char* key, double value)
{
	char value_str[256] = { 0 };
	blz_string_set_as_double(value_str, sizeof(value_str), value);
	blz_config_set_value_as_string(config, section, key, value_str);
}
