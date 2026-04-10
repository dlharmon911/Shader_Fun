#include "blaze.h"
#include "sf_text_highlighter.h"

enum glsl_token_type
{
	GLSL_TOKEN_TYPE_IDENTIFIER,
	GLSL_TOKEN_TYPE_NUMBER,
	GLSL_TOKEN_TYPE_COMMENT,
	GLSL_TOKEN_TYPE_PREPROCESSOR,
	GLSL_TOKEN_TYPE_OTHER
};

enum
{
	BLAZE_TEXT_KEYWORD_COUNT = 78,
	BLAZE_TEXT_FUNCTION_NAME_COUNT = 28
};

const blz_stringview_t glsl_keywords[BLAZE_TEXT_KEYWORD_COUNT] =
{
	{ "attribute", 9 }, { "bool", 4 }, { "break", 5 }, { "buffer", 6 }, { "bvec2", 5 },
	{ "bvec3", 5 }, { "bvec4", 5 }, { "case", 4 }, { "centroid", 8 }, { "coherent", 8 },
	{ "const", 5 }, { "continue", 8 }, { "default", 7 }, { "discard", 7 }, { "do", 2 },
	{ "double", 6 }, { "dvec2", 5 }, { "dvec3", 5 }, { "dvec4", 5 }, { "else", 4 },
	{ "false", 5 }, { "flat", 4 }, { "float", 5 }, { "for", 3 }, { "highp", 5 },
	{ "if", 2 }, { "image1D", 7 }, { "image2D", 7 }, { "image3D", 7 }, { "in", 2 },
	{ "inout", 5 }, { "int", 3 }, { "invariant", 9 }, { "isampler2D", 10 }, { "isampler3D", 10 },
	{ "ivec2", 5 }, { "ivec3", 5 }, { "ivec4", 5 }, { "layout", 6 }, { "lowp", 5 },
	{ "mat2", 4 }, { "mat3", 4 }, { "mat4", 4 }, { "mediump", 8 }, { "noperspective", 13 },
	{ "out", 3 }, { "patch", 5 }, { "precise", 7 }, { "precision", 9 }, { "readonly", 8 },
	{ "restrict", 8 }, { "return", 6 }, { "sampler2D", 9 }, { "sampler3D", 9 }, { "samplerCube", 11 },
	{ "shared", 6 }, { "smooth", 6 }, { "struct", 6 }, { "subroutine", 10 }, { "switch", 6 },
	{ "true", 4 }, { "uimage2D", 8 }, { "uimage3D", 8 }, { "uint", 4 }, { "uniform", 7 },
	{ "usampler2D", 10 }, { "usampler3D", 10 }, { "uvec2", 5 }, { "uvec3", 5 }, { "uvec4", 5 },
	{ "varying", 7 }, { "vec2", 4 }, { "vec3", 4 }, { "vec4", 4 }, { "void", 4 },
	{ "volatile", 8 }, { "while", 5 }, { "writeonly", 10 }
};

const blz_stringview_t glsl_function_name[BLAZE_TEXT_FUNCTION_NAME_COUNT] =
{
	{ "abs", 3 }, { "acos", 4 }, { "asin", 4 }, { "atan", 4 }, { "ceil", 4 },
	{ "clamp", 5 }, { "cos", 3 }, { "degrees", 7 }, { "exp", 3 }, { "exp2", 4 },
	{ "floor", 5 }, { "fract", 5 }, { "inversesqrt", 11 }, { "log", 3 }, { "log2", 4 },
	{ "main", 4 },{ "max", 3 }, { "min", 3 }, { "mix", 3 }, { "mod", 3 }, { "pow", 3 },
	{ "radians", 7 }, { "sign", 4 }, { "sin", 3 }, { "smoothstep", 10 }, { "sqrt", 4 },
	{ "step", 4 }, { "tan", 3 }
};

typedef struct blz_text_token_t
{
	blz_stringview_t m_value;
	int32_t m_type;
} blz_text_token_t;

static bool _blz_text_is_name(blz_stringview_t text, const blz_stringview_t* names, size_t count)
{
	for (size_t i = 0; i < count && text.m_data[0] >= names[i].m_data[0]; ++i)
	{
		if (blz_stringview_equals(names[i], text))
		{
			return true;
		}
	}

	return false;
}

static blz_stringview_t _blz_text_parse_identifier(blz_stringview_t line, blz_text_token_t* token)
{
	token->m_type = GLSL_TOKEN_TYPE_IDENTIFIER;
	token->m_value.m_data = line.m_data;
	token->m_value.m_length = 0;

	while (true)
	{
		char c = line.m_data[0];

		if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9') || c == '_')
		{
			line.m_data++;
			line.m_length--;
			token->m_value.m_length++;
		}
		else
		{
			break;
		}
	}

	return line;
}

static blz_stringview_t _blz_text_parse_number(blz_stringview_t line, blz_text_token_t* token)
{
	token->m_type = GLSL_TOKEN_TYPE_NUMBER;
	token->m_value.m_data = line.m_data;
	token->m_value.m_length = 0;

	char* endptr = NULL;

	float f = strtof(line.m_data, &endptr);

	(void)f;

	int32_t count = (int32_t)(endptr - line.m_data);

	line.m_data += count;
	line.m_length -= count;
	token->m_value.m_length = (size_t)count;

	return line;
}

static blz_stringview_t _blz_text_parse_comment(blz_stringview_t line, blz_text_token_t* token, bool* multiline_comment)
{
	token->m_type = GLSL_TOKEN_TYPE_COMMENT;
	token->m_value.m_data = line.m_data;
	token->m_value.m_length = 0;

	if (*multiline_comment)
	{
		// we are already in a multiline comment, just find the end of it
		for (int32_t i = 0; i < line.m_length; ++i)
		{
			if (line.m_data[i] == '*' && i + 1 < line.m_length && line.m_data[i + 1] == '/')
			{
				*multiline_comment = false;
				line.m_data += i + 2;
				line.m_length -= i + 2;
				token->m_value.m_length = (size_t)(i + 2);
				return line;
			}
		}
	}

	token->m_value.m_length = line.m_length;
	line.m_data += line.m_length;
	line.m_length = 0;

	return line;
}

static blz_stringview_t _blz_text_parse_preprocessor(blz_stringview_t line, blz_text_token_t* token)
{
	token->m_type = GLSL_TOKEN_TYPE_PREPROCESSOR;
	token->m_value.m_data = line.m_data;
	token->m_value.m_length = 0;
	while (line.m_length)
	{
		char c = line.m_data[0];

		if (c == '/' && line.m_length > 0 && (line.m_data[0] == '/' || line.m_data[0] == '*'))
		{
			break;
		}

		line.m_data++;
		line.m_length--;

		if (c == '\n')
		{
			break;
		}
		token->m_value.m_length++;
	}

	return line;
}

static blz_stringview_t _blz_text_get_token(blz_stringview_t line, blz_text_token_t* token, bool* multiline_comment)
{
	if (!token || !line.m_data || !line.m_length)
	{
		return line;
	}

	char c = line.m_data[0];

	if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_')
	{
		return _blz_text_parse_identifier(line, token);
	}
	else if ((c >= '0' && c <= '9') || (c == '.' && line.m_length > 1 && (line.m_data[1] >= '0' && line.m_data[1] <= '9')))
	{
		return _blz_text_parse_number(line, token);
	}
	else if (c == '/' && line.m_length > 1 && (line.m_data[1] == '/' || line.m_data[1] == '*'))
	{
		if (line.m_data[1] == '*')
		{
			*multiline_comment = true;
		}

		return _blz_text_parse_comment(line, token, multiline_comment);
	}
	else if (c == '#' && line.m_length > 1)
	{
		return _blz_text_parse_preprocessor(line, token);
	}

	token->m_type = GLSL_TOKEN_TYPE_OTHER;
	token->m_value.m_data = line.m_data;
	token->m_value.m_length = 1;

	return (blz_stringview_t) { line.m_data + 1, line.m_length - 1 };
}

static void _blz_text_parse_multiline_comment(const ALLEGRO_FONT* font, blz_stringview_t* line, blz_vec2f_t* position, const blz_text_info_t* info, bool* multiline_comment)
{
	size_t line_length = line->m_length;

	// this is an already commented line, we just need to find the end of the comment if any and draw it
	for (int32_t i = 0; i < line->m_length; ++i)
	{
		if (line->m_data[i] == '*' && i + 1 < line->m_length && line->m_data[i + 1] == '/')
		{
			*multiline_comment = false;
			line_length = (size_t)(i + 2);
			break;
		}
	}

	al_draw_textf(font, SF_TEXT_COLOR_COMMENT, position->m_x, position->m_y, 0, "%.*s", (int32_t)line_length, line->m_data);

	line->m_data += line_length;
	line->m_length -= line_length;

	position->m_x += (float)line_length * (float)al_get_text_width(font, "W");
}


typedef struct sf_text_highlighter_data_tag_t
{
	bool m_multiline_comment;
} sf_text_highlighter_data_t;

void sf_text_highlighter_begin(const ALLEGRO_FONT* font, const blz_text_t* text, blz_vec2f_t position, blz_sizef_t size, const blz_text_info_t* info, void* data)
{
	static sf_text_highlighter_data_t _sf_text_highlighter_data = { false };

	if (!font || !text || !info || !data)
	{
		return;
	}

	sf_text_highlighter_data_t* hdata = (sf_text_highlighter_data_t*)data;

	hdata->m_multiline_comment = false;
}

void sf_text_highlighter_finish(const ALLEGRO_FONT* font, const blz_text_t* text, blz_vec2f_t position, blz_sizef_t size, const blz_text_info_t* info, void* data)
{
	if (!font || !text || !info || !data)
	{
		return;
	}

	sf_text_highlighter_data_t* hdata = (sf_text_highlighter_data_t*)data;

	hdata->m_multiline_comment = false;
}

void sf_text_highlighter_per_line(const ALLEGRO_FONT* font, blz_stringview_t line, blz_vec2f_t position, const blz_text_info_t* info, void* data)
{
	blz_text_token_t token = { 0 };
	sf_text_highlighter_data_t* hdata = (sf_text_highlighter_data_t*)data;

	if (hdata->m_multiline_comment)
	{
		_blz_text_parse_multiline_comment(font, &line, &position, info, &hdata->m_multiline_comment);
	}

	while (line.m_length)
	{
		ALLEGRO_COLOR color = { 0.0f, 0.0f, 0.0f, 1.0f };
		line = _blz_text_get_token(line, &token, &hdata->m_multiline_comment);

		switch (token.m_type)
		{
		case GLSL_TOKEN_TYPE_IDENTIFIER:
		{
			if (_blz_text_is_name(token.m_value, glsl_keywords, BLAZE_TEXT_KEYWORD_COUNT))
			{
				color = SF_TEXT_COLOR_KEYWORD;
			}
			else if (_blz_text_is_name(token.m_value, glsl_function_name, BLAZE_TEXT_FUNCTION_NAME_COUNT))
			{
				color = SF_TEXT_COLOR_FUNCTION;
			}
			else
			{
				color = SF_TEXT_COLOR_DEFAULT;
			}
		} break;
		case GLSL_TOKEN_TYPE_NUMBER:
		{
			color = SF_TEXT_COLOR_NUMBER;
		} break;
		case GLSL_TOKEN_TYPE_COMMENT:
		{
			color = SF_TEXT_COLOR_COMMENT;
		} break;
		case GLSL_TOKEN_TYPE_PREPROCESSOR:
		{
			color = SF_TEXT_COLOR_PREPROCESSOR;
		} break;
		default: break;
		}

		if (token.m_value.m_length)
		{
			al_draw_textf(font, color, position.m_x, position.m_y, 0, "%.*s", (int)token.m_value.m_length, token.m_value.m_data);
			position.m_x += (float)token.m_value.m_length * (float)al_get_text_width(font, "W");
		}
	}
}

const blz_text_highlighter_t* sf_text_highlighter_get(void)
{
	static sf_text_highlighter_data_t _sf_text_highlighter_data = 
	{
		.m_multiline_comment = false
	};

	static blz_text_highlighter_t sf_text_highlighter =
	{
		.m_begin = sf_text_highlighter_begin,
		.m_per_line = sf_text_highlighter_per_line,
		.m_finish = sf_text_highlighter_finish,
		.m_user_data = &_sf_text_highlighter_data
	};

	return &sf_text_highlighter;
}
