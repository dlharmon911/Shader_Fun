#ifndef _GUARD_BLAZE_FONT_H_
#define _GUARD_BLAZE_FONT_H_

#include "blaze/blz_common.h"

enum BLAZE_FONT_ID
{
	BLAZE_FONT_ID_EDITOR_REGULAR,
	BLAZE_FONT_ID_EDITOR_BOLD,
	BLAZE_FONT_ID_UNIFORM,
	BLAZE_FONT_ID_COUNT
};

typedef struct blz_font_tag_t
{
	ALLEGRO_FONT* m_font;
	int32_t m_size;
} blz_font_t;

typedef blz_font_t blz_font_cache_t[BLAZE_FONT_ID_COUNT];

int32_t blz_font_cache_load(blz_font_cache_t* cache);
void blz_font_cache_unload(blz_font_cache_t* cache);

#endif // !_GUARD_BLAZE_FONT_H_
