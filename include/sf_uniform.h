#ifndef _GUARD_SHADER_FUN_UNIFORM_H_
#define _GUARD_SHADER_FUN_UNIFORM_H_

#include "blaze.h"

enum SF_UNIFORM_TYPE
{
	SF_UNIFORM_TYPE_UNDEFINED = -1,
	SF_UNIFORM_TYPE_BOOL,
	SF_UNIFORM_TYPE_INT,
	SF_UNIFORM_TYPE_FLOAT,
	SF_UNIFORM_TYPE_INT_VEC2,
	SF_UNIFORM_TYPE_INT_VEC3,
	SF_UNIFORM_TYPE_INT_VEC4,
	SF_UNIFORM_TYPE_FLOAT_VEC2,
	SF_UNIFORM_TYPE_FLOAT_VEC3,
	SF_UNIFORM_TYPE_FLOAT_VEC4,
	SF_UNIFORM_TYPE_MATRIX,
	SF_UNIFORM_TYPE_SAMPLER2D,
	SF_UNIFORM_TYPE_COUNT
};

static const char* SF_UNIFORM_NAME_STRS[SF_UNIFORM_TYPE_COUNT] =
{
	"bool",
	"int",
	"float",
	"ivec2",
	"ivec3",
	"ivec4",
	"vec2",
	"vec3",
	"vec4",
	"mat4",
	"sampler2D"
};

typedef struct sf_uniform_sampler2d_tag_t
{
	ALLEGRO_BITMAP* m_texture;
	int32_t m_unit;
} sf_uniform_sampler2d_t;

typedef struct sf_uniform_tag_t
{
	char* m_name;
	int32_t m_type;
	union
	{
		bool m_bool;
		int32_t m_int;
		float m_float;
		int32_t m_int_vec[4];
		float m_float_vec[4];
		ALLEGRO_TRANSFORM m_matrix;
		sf_uniform_sampler2d_t m_sampler2d;
	} m_value;
	bool m_visible;
} sf_uniform_t;

void sf_uniform_to_string(const sf_uniform_t* uniform, char* buffer, size_t buffer_size);

void sf_uniform_clear(sf_uniform_t* uniform);
size_t sf_uniform_size(const sf_uniform_t* uniform);
bool sf_uniform_push(sf_uniform_t** uniform, const char* name, int32_t type, const void* value, size_t value_size, bool visible);
bool sf_uniform_pop(sf_uniform_t* uniform);
void sf_uniform_destroy(sf_uniform_t* uniform);
sf_uniform_t* sf_uniform_get(sf_uniform_t* uniform, const char* name);
const sf_uniform_t* sf_uniform_get_const(const sf_uniform_t* uniform, const char* name);
void sf_uniform_update_shader(const sf_uniform_t* uniform, ALLEGRO_SHADER* shader);

void sf_uniform_set(sf_uniform_t* uniform, int32_t type, const void* value, size_t value_size);
void sf_uniform_set_bool(sf_uniform_t* uniform, bool value);
void sf_uniform_set_int(sf_uniform_t* uniform, int32_t value);
void sf_uniform_set_float(sf_uniform_t* uniform, float value);
void sf_uniform_set_int_vec(sf_uniform_t* uniform, const int32_t* value, size_t count);
void sf_uniform_set_float_vec(sf_uniform_t* uniform, const float* value, size_t count);
void sf_uniform_set_matrix(sf_uniform_t* uniform, ALLEGRO_TRANSFORM transform);
void sf_uniform_set_sampler2d(sf_uniform_t* uniform, ALLEGRO_BITMAP* texture, int32_t unit);

#endif // !_GUARD_SHADER_FUN_UNIFORM_H_

