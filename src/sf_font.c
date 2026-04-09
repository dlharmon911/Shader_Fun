#include "sf_font.h"

int32_t sf_font_cache_load(sf_font_cache_t* cache)
{
	if (!cache)
	{
		return -1;
	}

	const char* font_filenames[SF_FONT_ID_COUNT] =
	{
		"assets/nm_regular.ttf",
		"assets/nm_bold.ttf",
		"assets/nm_regular.ttf"
	};

	int32_t font_heights[SF_FONT_ID_COUNT] =
	{
		14,
		14,
		10
	};

	for (size_t i = 0; i < SF_FONT_ID_COUNT; ++i)
	{
		(*cache)[i].m_font = NULL;
		(*cache)[i].m_size = 0;
	}

	for (size_t i = 0; i < SF_FONT_ID_COUNT; ++i)
	{
		ALLEGRO_FONT* font = al_load_ttf_font(font_filenames[i], font_heights[i], ALLEGRO_TTF_NO_KERNING);

		if (!font)
		{
			sf_font_cache_unload(cache);
			return -1;
		}

		(*cache)[i].m_font = font;
		(*cache)[i].m_size = font_heights[i];
	}

	return 0;
}

void sf_font_cache_unload(sf_font_cache_t* cache)
{
	if (!cache)
	{
		return;
	}

	for (size_t i = 0; i < SF_FONT_ID_COUNT; ++i)
	{
		if ((*cache)[i].m_font)
		{
			al_destroy_font((*cache)[i].m_font);
			(*cache)[i].m_font = NULL;
			(*cache)[i].m_size = 0;
		}
	}
}