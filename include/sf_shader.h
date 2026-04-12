#ifndef _GUARD_SHADER_FUN_SHADER_H_
#define _GUARD_SHADER_FUN_SHADER_H_

#include "blaze.h"
#include "sf_uniform.h"

ALLEGRO_SHADER* sf_shader_generate(const ALLEGRO_USTR* text_str, const sf_uniform_t* uniform);
void sf_shader_render(ALLEGRO_SHADER* shader, const sf_uniform_t* uniform, blz_vec2f_t position, blz_sizef_t resolution);

#endif // !_GUARD_SHADER_FUN_SHADER_H_

