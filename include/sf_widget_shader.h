#ifndef _GUARD_SHADER_FUN_WIDGET_SHADER_H_
#define _GUARD_SHADER_FUN_WIDGET_SHADER_H_

#include "blaze.h"
#include "sf_uniform.h"

typedef struct sf_widget_shader_data_tag_t
{
	ALLEGRO_SHADER* m_shader;
	ALLEGRO_BITMAP* m_buffer;
} sf_widget_shader_data_t;

int32_t sf_widget_shader_build(const blz_text_t* text, const sf_uniform_t* uniforms, sf_widget_shader_data_t* data);
int32_t sf_widget_shader_rebuild(const blz_text_t* text, const sf_uniform_t* uniforms, sf_widget_shader_data_t* data);
const blz_widget_vtable_t* sf_widget_shader_get_vtable(void);

#endif // !_GUARD_SHADER_FUN_WIDGET_SHADER_H_

