#ifndef _GUARD_TEXT_SHADER_FUNTEXT_HIGHLIGHTER_H_
#define _GUARD_TEXT_SHADER_FUNTEXT_HIGHLIGHTER_H_

#include "blaze.h"

static const ALLEGRO_COLOR SF_TEXT_COLOR_KEYWORD = { 0.0f, 0.0f, 1.0f, 1.0f };
static const ALLEGRO_COLOR SF_TEXT_COLOR_FUNCTION = { 0.6f, 0.0f, 1.0f, 1.0f };
static const ALLEGRO_COLOR SF_TEXT_COLOR_NUMBER = { 1.0f, 0.0f, 0.0f, 1.0f };
static const ALLEGRO_COLOR SF_TEXT_COLOR_COMMENT = { 0.0f, 0.4f, 0.0f, 1.0f };
static const ALLEGRO_COLOR SF_TEXT_COLOR_PREPROCESSOR = { 0.5f, 0.5f, 0.5f, 1.0f };
static const ALLEGRO_COLOR SF_TEXT_COLOR_DEFAULT = { 0.25f, 0.25f, 0.25f, 1.0f };

const blz_text_highlighter_t* sf_text_highlighter_get(void);


#endif // !_GUARD_TEXT_SHADER_FUNTEXT_HIGHLIGHTER_H_

