#include <allegro5/allegro_memfile.h>
#include "blaze/blz_common.h"
#include "blaze/blz_font.h"
#include "blaze/blz_file.h"

#define _BUILD_HEADER_FILES

#ifndef BUILD_HEADER_FILES
#include "blaze/blz_font_regular.h"
#include "blaze/blz_font_bold.h"
#endif

ALLEGRO_FONT* _blz_load_font_from_memory(void* data, size_t data_size, int32_t font_size)
{
	ALLEGRO_FILE* file = al_open_memfile(data, data_size, "r");

	if (!file)
	{
		DO_LOG(BLAZE_LOG_LEVEL_ERROR, "Failed to open memory file for font data");
		return NULL;
	}

	ALLEGRO_FONT* font = al_load_ttf_font_f(file, NULL, font_size, ALLEGRO_TTF_NO_KERNING);

	if (!font)
	{
		DO_LOG(BLAZE_LOG_LEVEL_ERROR, "Failed to load font from memory file");
		return NULL;
	}

	return font;
}

int32_t blz_font_cache_load(blz_font_cache_t* cache)
{
	if (!cache)
	{
		return -1;
	}

#ifdef BUILD_HEADER_FILES
	blz_convert_file_to_c_array("assets/nm_regular.ttf", "include/blaze/blz_font_regular.h", "blz_font_regular");
	blz_convert_file_to_c_array("assets/nm_bold.ttf", "include/blaze/blz_font_bold.h", "blz_font_bold");
	return -1;
#else
	
	void* font_data[BLAZE_FONT_ID_COUNT] =
	{
		blz_font_regular,
		blz_font_bold,
		blz_font_regular
	};

	size_t font_data_size[BLAZE_FONT_ID_COUNT] =
	{
		blz_font_regular_size,
		blz_font_bold_size,
		blz_font_regular_size
	};

	int32_t font_heights[BLAZE_FONT_ID_COUNT] =
	{
		14,
		14,
		10
	};

	for (size_t i = 0; i < BLAZE_FONT_ID_COUNT; ++i)
	{
		(*cache)[i].m_font = NULL;
		(*cache)[i].m_size = 0;
	}

	for (size_t i = 0; i < BLAZE_FONT_ID_COUNT; ++i)
	{
		blz_font_t* font_entry = &((*cache)[i]);

		font_entry->m_font = _blz_load_font_from_memory(font_data[i], font_data_size[i], font_heights[i]);
		
		if (!font_entry->m_font)
		{
			return -1;
		}
		font_entry->m_size = font_heights[i];
	}

	return 0;
#endif
}

void blz_font_cache_unload(blz_font_cache_t* cache)
{
	if (!cache)
	{
		return;
	}
	
	for (size_t i = 0; i < BLAZE_FONT_ID_COUNT; ++i)
	{
		blz_font_t* font_entry = &((*cache)[i]);

		if (font_entry->m_font)
		{
			al_destroy_font(font_entry->m_font);
			font_entry->m_font = NULL;
			font_entry->m_size = 0;
		}
	}
}

