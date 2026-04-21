#include <allegro5/allegro_memfile.h>
#include "blaze/blz_common.h"
#include "blaze/blz_font.h"
#include "blaze/blz_file.h"

#define _BUILD_HEADER_FILES

#ifndef BUILD_HEADER_FILES
#include "blaze/blz_font_regular.h"
#include "blaze/blz_font_bold.h"
#endif

enum
{
	BLAZE_FONT_MIN_SIZE = 10,
	BLAZE_FONT_MAX_SIZE = 72,
};

void* BLAZE_FONT_DATA[BLAZE_FONT_ID_COUNT] =
{
	blz_font_regular,
	blz_font_regular,
	blz_font_regular
};

const size_t BLAZE_FONT_DATA_SIZE[BLAZE_FONT_ID_COUNT] =
{
	BLAZE_FONT_REGULAR_SIZE,
	BLAZE_FONT_REGULAR_SIZE,
	BLAZE_FONT_REGULAR_SIZE
};

int32_t BLAZE_DEFAULT_FONT_HEIGHTS[BLAZE_FONT_ID_COUNT] =
{
	14,
	14,
	10
};

static ALLEGRO_FONT* _blz_load_font_from_memory(void* data, size_t data_size, int32_t font_size)
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

static int32_t _blz_create_font(blz_font_t* font_entry, int32_t height, int32_t i)
{
	font_entry->m_font = _blz_load_font_from_memory(BLAZE_FONT_DATA[i], BLAZE_FONT_DATA_SIZE[i], height);

	if (!font_entry->m_font)
	{
		return -1;
	}
	
	font_entry->m_line_height = (float)height;
	font_entry->m_char_width = (float)al_get_text_width(font_entry->m_font, "W");

	return 0;
}

static void _blz_destroy_font(blz_font_t* font_entry)
{
	if (font_entry->m_font)
	{
		al_destroy_font(font_entry->m_font);
		font_entry->m_font = NULL;
		font_entry->m_line_height = 0.0f;
		font_entry->m_char_width = 0.0f;
	}
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

	for (int32_t id = 0; id < BLAZE_FONT_ID_COUNT; ++id)
	{
		(*cache)[id].m_font = NULL;
		(*cache)[id].m_line_height = 0.0f;
		(*cache)[id].m_char_width = 0.0f;
	}

	for (int32_t id = 0; id < BLAZE_FONT_ID_COUNT; ++id)
	{
		blz_font_t* font_entry = &((*cache)[id]);
		int32_t height = BLAZE_DEFAULT_FONT_HEIGHTS[id];

		if (_blz_create_font(font_entry, height, id) != 0)
		{
			DO_LOG(BLAZE_LOG_LEVEL_ERROR, "Failed to load font with ID %zu", id);
			return -1;
		}
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
	
	for (int32_t id = 0; id < BLAZE_FONT_ID_COUNT; ++id)
	{
		_blz_destroy_font(&((*cache)[id]));
	}
}

void blz_font_increment_size(blz_font_cache_t* cache, int32_t id)
{
	if (!cache || id < 0 || id >= BLAZE_FONT_ID_COUNT)
	{
		return;
	}

	blz_font_t* font_entry = &((*cache)[id]);
	int32_t new_height = (int32_t)font_entry->m_line_height;
	
	if (font_entry->m_line_height < BLAZE_FONT_MAX_SIZE)
	{
		if (font_entry->m_line_height < 12)
		{
			++new_height;
		}
		else
		{
			new_height += 2;
		}

		_blz_destroy_font(font_entry);

		if (_blz_create_font(font_entry, new_height, id) != 0)
		{
			DO_LOG(BLAZE_LOG_LEVEL_ERROR, "Failed to load font with ID %zu", id);
			font_entry->m_line_height = 0.0f;
			font_entry->m_char_width = 0.0f;
			return;
		}
	}
}

void blz_font_decrement_size(blz_font_cache_t* cache, int32_t id)
{
	if (!cache || id < 0 || id >= BLAZE_FONT_ID_COUNT)
	{
		return;
	}
	blz_font_t* font_entry = &((*cache)[id]);
	int32_t new_height = (int32_t)font_entry->m_line_height;
	
	if (font_entry->m_line_height > BLAZE_FONT_MIN_SIZE)
	{
		if (font_entry->m_line_height <= 12)
		{ 
			--new_height;
		}
		else
		{
			new_height -= 2;
		}

		_blz_destroy_font(font_entry);

		if (_blz_create_font(font_entry, new_height, id) != 0)
		{
			DO_LOG(BLAZE_LOG_LEVEL_ERROR, "Failed to load font with ID %zu", id);
			font_entry->m_line_height = 0;
			return;
		}
	}
}
