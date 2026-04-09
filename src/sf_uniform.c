#include <string.h>
#include "blaze.h"
#include "sf_uniform.h"

void sf_uniform_clear(sf_uniform_t* uniform)
{
	if (!uniform)
	{
		return;
	}

	if (uniform->m_name)
	{
		free(uniform->m_name);
		uniform->m_name = NULL;
	}

	uniform->m_type = SF_UNIFORM_TYPE_UNDEFINED;
}

size_t sf_uniform_size(const sf_uniform_t* uniform)
{
	return blz_darray_size(uniform);
}

bool sf_uniform_push(sf_uniform_t** uniform, const char* name, int32_t type, const void* value, size_t value_size)
{
	sf_uniform_t new_uniform = { 0 };

	if (!name || !value || value_size == 0)
	{
		return false;
	}
	
	size_t name_length = strlen(name);
	new_uniform.m_name = (char*)al_malloc(name_length + 1);
	
	if (!new_uniform.m_name)
	{
		return false;
	}

	memcpy(new_uniform.m_name, name, name_length);
	new_uniform.m_name[name_length] = '\0';

	new_uniform.m_type = type;

	memcpy(&new_uniform.m_value, value, value_size);

	if (!blz_darray_pushback(uniform, &new_uniform, sizeof(sf_uniform_t)))
	{
		al_free(new_uniform.m_name);
		return false;
	}

	return true;
}

bool sf_uniform_pop(sf_uniform_t* uniform)
{
	return blz_darray_pop(uniform);
}

void sf_uniform_destroy(sf_uniform_t* uniform)
{
	if (!uniform)
	{
		return;
	}

	size_t uniform_count = blz_darray_size(uniform);

	for (size_t i = 0; i < uniform_count; ++i)
	{
		sf_uniform_t* current_uniform = (sf_uniform_t*)blz_darray_at(uniform, i);

		if (current_uniform->m_name)
		{
			al_free(current_uniform->m_name);
			current_uniform->m_name = NULL;
		}
	}

	blz_darray_destroy(uniform);
}

sf_uniform_t* sf_uniform_get(sf_uniform_t* uniform, const char* name)
{
	if (!uniform || !name)
	{
		return NULL;
	}
	
	size_t uniform_count = blz_darray_size(uniform);
	
	for (size_t i = 0; i < uniform_count; ++i)
	{
		sf_uniform_t* current_uniform = (sf_uniform_t*)blz_darray_at(uniform, i);
	
		if (strcmp(current_uniform->m_name, name) == 0)
		{
			return current_uniform;
		}
	}

	return NULL;
}

const sf_uniform_t* sf_uniform_get_const(const sf_uniform_t* uniform, const char* name)
{
	if (!uniform || !name)
	{
		return NULL;
	}
	
	size_t uniform_count = blz_darray_size(uniform);
	
	for (size_t i = 0; i < uniform_count; ++i)
	{
		const sf_uniform_t* current_uniform = (const sf_uniform_t*)blz_darray_at_const(uniform, i);
	
		if (strcmp(current_uniform->m_name, name) == 0)
		{
			return current_uniform;
		}
	}

	return NULL;
}

static void sf_uniform_update_shader_type(int32_t type, ALLEGRO_SHADER* shader, const char* name, const void* value)
{
	ALLEGRO_SHADER* current_shader = al_get_current_shader();
	al_use_shader(shader);

	switch (type)
	{
	case SF_UNIFORM_TYPE_BOOL:
	{
		al_set_shader_bool(name, *(const bool*)value);
	} break;
	case SF_UNIFORM_TYPE_INT:
	{
		al_set_shader_int(name, *(const int32_t*)value);
	} break;
	case SF_UNIFORM_TYPE_FLOAT:
	{
		al_set_shader_float(name, *(const float*)value);
	} break;
	case SF_UNIFORM_TYPE_INT_VEC2:
	{
		al_set_shader_int_vector(name, 2, (const int32_t*)value, 1);
	} break;
	case SF_UNIFORM_TYPE_INT_VEC3:
	{
		al_set_shader_int_vector(name, 3, (const int32_t*)value, 1);
	} break;
	case SF_UNIFORM_TYPE_INT_VEC4:
	{
		al_set_shader_int_vector(name, 4, (const int32_t*)value, 1);
	} break;
	case SF_UNIFORM_TYPE_FLOAT_VEC2:
	{
		al_set_shader_float_vector(name, 2, (const float*)value, 1);
	} break;
	case SF_UNIFORM_TYPE_FLOAT_VEC3:
	{
		al_set_shader_float_vector(name, 3, (const float*)value, 1);
	} break;
	case SF_UNIFORM_TYPE_FLOAT_VEC4:
	{
		al_set_shader_float_vector(name, 4, (const float*)value, 1);
	} break;
	case SF_UNIFORM_TYPE_MATRIX:
	{
		al_set_shader_matrix(name, (const ALLEGRO_TRANSFORM*)value);
	} break;
	default: break;
	}

	al_use_shader(current_shader);
}

void sf_uniform_update_shader(const sf_uniform_t* uniform, ALLEGRO_SHADER* shader)
{
	if (!uniform || !shader)
	{
		return;
	}

	size_t uniform_count = blz_darray_size(uniform);

	for (size_t i = 0; i < uniform_count; ++i)
	{
		const sf_uniform_t* current_uniform = (const sf_uniform_t*)blz_darray_at_const(uniform, i);
		sf_uniform_update_shader_type(current_uniform->m_type, shader, current_uniform->m_name, &current_uniform->m_value);
	}	
}

void sf_uniform_set(sf_uniform_t* uniform, int32_t type, const void* value, size_t value_size)
{
	if (!uniform || !value || value_size == 0)
	{
		return;
	}
	memcpy(&uniform->m_value, value, value_size);
	uniform->m_type = type;
}

void sf_uniform_set_bool(sf_uniform_t* uniform, bool value)
{
	if (!uniform)
	{
		return;
	}

	uniform->m_value.m_bool = value;
	uniform->m_type = SF_UNIFORM_TYPE_BOOL;
}

void sf_uniform_set_int(sf_uniform_t* uniform, int32_t value)
{
	if (!uniform)
	{
		return;
	}

	uniform->m_value.m_int = value;
	uniform->m_type = SF_UNIFORM_TYPE_INT;
}

void sf_uniform_set_float(sf_uniform_t* uniform, float value)
{
	if (!uniform)
	{
		return;
	}

	uniform->m_value.m_float = value;
	uniform->m_type = SF_UNIFORM_TYPE_FLOAT;
}

void sf_uniform_set_int_vec(sf_uniform_t* uniform, const int32_t* value, size_t count)
{
	if (!uniform || !value || count == 0 || count > 4)
	{
		return;
	}
	
	memcpy(uniform->m_value.m_int_vec, value, sizeof(int32_t) * count);
	
	if (count == 2)
	{
		uniform->m_type = SF_UNIFORM_TYPE_INT_VEC2;
	}
	else if (count == 3)
	{
		uniform->m_type = SF_UNIFORM_TYPE_INT_VEC3;
	}
	else
	{
		uniform->m_type = SF_UNIFORM_TYPE_INT_VEC4;
	}
}

void sf_uniform_set_float_vec(sf_uniform_t* uniform, const float* value, size_t count)
{
	if (!uniform || !value || count == 0 || count > 4)
	{
		return;
	}

	memcpy(uniform->m_value.m_float_vec, value, sizeof(float) * count);

	if (count == 2)
	{
		uniform->m_type = SF_UNIFORM_TYPE_FLOAT_VEC2;
	}
	else if (count == 3)
	{
		uniform->m_type = SF_UNIFORM_TYPE_FLOAT_VEC3;
	}
	else
	{
		uniform->m_type = SF_UNIFORM_TYPE_FLOAT_VEC4;
	}
}

void sf_uniform_set_matrix(sf_uniform_t* uniform, ALLEGRO_TRANSFORM transform)
{
	if (!uniform)
	{
		return;
	}

	uniform->m_value.m_matrix = transform;
	uniform->m_type = SF_UNIFORM_TYPE_MATRIX;
}

void sf_uniform_to_string(const sf_uniform_t* uniform, char* buffer, size_t buffer_size)
{
	if (!uniform || !buffer || buffer_size == 0)
	{
		return;
	}
	switch (uniform->m_type)
	{
	case SF_UNIFORM_TYPE_BOOL:
	{
		snprintf(buffer, buffer_size, "%s: %s", uniform->m_name, uniform->m_value.m_bool ? "true" : "false");
	} break;
	case SF_UNIFORM_TYPE_INT:
	{
		snprintf(buffer, buffer_size, "%s: %d", uniform->m_name, uniform->m_value.m_int);
	} break;
	case SF_UNIFORM_TYPE_FLOAT:
	{
		snprintf(buffer, buffer_size, "%s: %0.2f", uniform->m_name, uniform->m_value.m_float);
	} break;
	case SF_UNIFORM_TYPE_INT_VEC2:
	{
		snprintf(buffer, buffer_size, "%s: <%d, %d>", uniform->m_name, uniform->m_value.m_int_vec[0], uniform->m_value.m_int_vec[1]);
	} break;
	case SF_UNIFORM_TYPE_INT_VEC3:
	{
		snprintf(buffer, buffer_size, "%s: <%d, %d, %d>", uniform->m_name, uniform->m_value.m_int_vec[0], uniform->m_value.m_int_vec[1], uniform->m_value.m_int_vec[2]);
	} break;
	case SF_UNIFORM_TYPE_INT_VEC4:
	{
		snprintf(buffer, buffer_size, "%s: <%d, %d, %d, %d>", uniform->m_name, uniform->m_value.m_int_vec[0], uniform->m_value.m_int_vec[1], uniform->m_value.m_int_vec[2], uniform->m_value.m_int_vec[3]);
	} break;
	case SF_UNIFORM_TYPE_FLOAT_VEC2:
	{
		snprintf(buffer, buffer_size, "%s: <%0.2f, %0.2f>", uniform->m_name, uniform->m_value.m_float_vec[0], uniform->m_value.m_float_vec[1]);
	} break;
	case SF_UNIFORM_TYPE_FLOAT_VEC3:
	{
		snprintf(buffer, buffer_size, "%s: <%0.2f, %0.2f, %0.2f>", uniform->m_name, uniform->m_value.m_float_vec[0], uniform->m_value.m_float_vec[1], uniform->m_value.m_float_vec[2]);
	} break;
	case SF_UNIFORM_TYPE_FLOAT_VEC4:
	{
		snprintf(buffer, buffer_size, "%s: <%0.2f, %0.2f, %0.2f, %0.2f>", uniform->m_name, uniform->m_value.m_float_vec[0], uniform->m_value.m_float_vec[1], uniform->m_value.m_float_vec[2], uniform->m_value.m_float_vec[3]);
	} break;
	case SF_UNIFORM_TYPE_MATRIX:
	{
		snprintf(buffer, buffer_size, "%s: [matrix]", uniform->m_name);
	} break;
	default:
	{
		snprintf(buffer, buffer_size, "%s: [unknown type]", uniform->m_name);
	} break;
	}
}

