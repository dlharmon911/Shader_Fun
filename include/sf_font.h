#ifndef _GUARD_SHADER_FUN_FONT_H_
#define _GUARD_SHADER_FUN_FONT_H_

#include "blaze.h"

enum SF_FONT_ID
{
	SF_FONT_ID_EDITOR_REGULAR,
	SF_FONT_ID_EDITOR_BOLD,
	SF_FONT_ID_UNIFORM,
	SF_FONT_ID_COUNT
};

typedef struct sf_font_tag_t
{
	ALLEGRO_FONT* m_font;
	int32_t m_size;
} sf_font_t;

typedef sf_font_t sf_font_cache_t[SF_FONT_ID_COUNT];

int32_t sf_font_cache_load(sf_font_cache_t* cache);
void sf_font_cache_unload(sf_font_cache_t* cache);

#endif // !_GUARD_SHADER_FUN_FONT_H_

