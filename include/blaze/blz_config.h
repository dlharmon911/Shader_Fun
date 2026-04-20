#ifndef _GUARD_BLAZE_CONFIG_H_
#define _GUARD_BLAZE_CONFIG_H_

#include "blaze/blz_common.h"
#include "blaze/blz_string.h"
#include "blaze/blz_stringview.h"

const char* blz_config_get_value_as_string(ALLEGRO_CONFIG* config, const char* section, const char* key, const char* default_value);
bool blz_config_get_value_as_bool(ALLEGRO_CONFIG* config, const char* section, const char* key, bool default_value);
int8_t blz_config_get_value_as_int8(ALLEGRO_CONFIG* config, const char* section, const char* key, int8_t default_value);
int16_t blz_config_get_value_as_int16(ALLEGRO_CONFIG* config, const char* section, const char* key, int16_t default_value);
int32_t blz_config_get_value_as_int32(ALLEGRO_CONFIG* config, const char* section, const char* key, int32_t default_value);
int64_t blz_config_get_value_as_int64(ALLEGRO_CONFIG* config, const char* section, const char* key, int64_t default_value);
uint8_t blz_config_get_value_as_uint8(ALLEGRO_CONFIG* config, const char* section, const char* key, uint8_t default_value);
uint16_t blz_config_get_value_as_uint16(ALLEGRO_CONFIG* config, const char* section, const char* key, uint16_t default_value);
uint32_t blz_config_get_value_as_uint32(ALLEGRO_CONFIG* config, const char* section, const char* key, uint32_t default_value);
uint64_t blz_config_get_value_as_uint64(ALLEGRO_CONFIG* config, const char* section, const char* key, uint64_t default_value);
float blz_config_get_value_as_float(ALLEGRO_CONFIG* config, const char* section, const char* key, float default_value);
double blz_config_get_value_as_double(ALLEGRO_CONFIG* config, const char* section, const char* key, double default_value);

void blz_config_set_value_as_string(ALLEGRO_CONFIG* config, const char* section, const char* key, const char* value);
void blz_config_set_value_as_bool(ALLEGRO_CONFIG* config, const char* section, const char* key, bool value);
void blz_config_set_value_as_int8(ALLEGRO_CONFIG* config, const char* section, const char* key, int8_t value);
void blz_config_set_value_as_int16(ALLEGRO_CONFIG* config, const char* section, const char* key, int16_t value);
void blz_config_set_value_as_int32(ALLEGRO_CONFIG* config, const char* section, const char* key, int32_t value);
void blz_config_set_value_as_int64(ALLEGRO_CONFIG* config, const char* section, const char* key, int64_t value);
void blz_config_set_value_as_uint8(ALLEGRO_CONFIG* config, const char* section, const char* key, uint8_t value);
void blz_config_set_value_as_uint16(ALLEGRO_CONFIG* config, const char* section, const char* key, uint16_t value);
void blz_config_set_value_as_uint32(ALLEGRO_CONFIG* config, const char* section, const char* key, uint32_t value);
void blz_config_set_value_as_uint64(ALLEGRO_CONFIG* config, const char* section, const char* key, uint64_t value);
void blz_config_set_value_as_float(ALLEGRO_CONFIG* config, const char* section, const char* key, float value);
void blz_config_set_value_as_double(ALLEGRO_CONFIG* config, const char* section, const char* key, double value);

#endif // !_GUARD_BLAZE_CONFIG_H_

