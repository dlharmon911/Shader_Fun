#ifndef _GUARD_SHADER_FUN_WIDGET_UNIFORMS_H_
#define _GUARD_SHADER_FUN_WIDGET_UNIFORMS_H_

#include "blaze.h"
#include "sf_uniform.h"

typedef struct sf_widget_uniform_data_tag_t
{
	sf_uniform_t* m_uniforms;
} sf_widget_uniform_data_t;

const blz_widget_vtable_t* sf_widget_uniforms_get_vtable(void);

#endif // !_GUARD_SHADER_FUN_WIDGET_UNIFORMS_H_

