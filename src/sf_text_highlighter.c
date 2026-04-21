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
	{ 9, "attribute" }, { 4, "bool" }, { 5, "break" }, { 6, "buffer" }, { 5, "bvec2" },
	{ 5, "bvec3" }, { 5, "bvec4" }, { 4, "case" }, { 8, "centroid" }, { 8, "coherent" },
	{ 5, "const" }, { 8, "continue" }, { 7, "default" }, { 7, "discard" }, { 2, "do" },
	{ 6, "double" }, { 5, "dvec2" }, { 5, "dvec3" }, { 5, "dvec4" }, { 4, "else" },
	{ 5, "false" }, { 4, "flat" }, { 5, "float" }, { 3, "for" }, { 5, "highp" },
	{ 2, "if" }, { 7, "image1D" }, { 7, "image2D" }, { 7, "image3D" }, { 2, "in" },
	{ 5, "inout" }, { 3, "int" }, { 9, "invariant" }, { 10, "isampler2D" }, { 10, "isampler3D" },
	{ 5, "ivec2" }, { 5, "ivec3" }, { 5, "ivec4" }, { 6, "layout" }, { 5, "lowp" },
	{ 4, "mat2" }, { 4, "mat3" }, { 4, "mat4" }, { 8, "mediump" }, { 13, "noperspective" },
	{ 3, "out" }, { 5, "patch" }, { 7, "precise" }, { 9, "precision" }, { 8, "readonly" },
	{ 8, "restrict" }, { 6, "return" }, { 9, "sampler2D" }, { 9, "sampler3D" }, { 11, "samplerCube" },
	{ 6, "shared" }, { 6, "smooth" }, { 6, "struct" }, { 10, "subroutine" }, { 6, "switch" },
	{ 4, "true" }, { 8, "uimage2D" }, { 8, "uimage3D" }, { 4, "uint" }, { 7, "uniform" },
	{ 10, "usampler2D" }, { 10, "usampler3D" }, { 5, "uvec2" }, { 5, "uvec3" }, { 5, "uvec4" },
	{ 7, "varying" }, { 4, "vec2" }, { 4, "vec3" }, { 4, "vec4" }, { 4, "void" },
	{ 8, "volatile" }, { 5, "while" }, { 10, "writeonly" }
};

const blz_stringview_t glsl_function_name[BLAZE_TEXT_FUNCTION_NAME_COUNT] =
{
	{ 3, "abs" }, { 4, "acos" }, { 4, "asin" }, { 4, "atan" }, { 4, "ceil" },
	{ 5, "clamp" }, { 3, "cos" }, { 7, "degrees" }, { 3, "exp" }, { 4, "exp2" },
	{ 5, "floor" }, { 5, "fract" }, { 11, "inversesqrt" }, { 3, "log" }, { 4, "log2" },
	{ 4, "main" },{ 3, "max" }, { 3, "min" }, { 3, "mix" }, { 3, "mod" }, { 3, "pow" },
	{ 7, "radians" }, { 4, "sign" }, { 3, "sin" }, { 10, "smoothstep" }, { 4, "sqrt" },
	{ 4, "step" }, { 3, "tan" }
};

typedef struct blz_text_token_t
{
	blz_stringview_t m_value;
	int32_t m_type;
} blz_text_token_t;


static bool _blz_text_is_name(blz_stringview_t text, const blz_stringview_t* names, size_t count)
{
	if (!text.m_length || !names || !count)
	{
		return false;
	}

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

		if (c == BLAZE_NEWLINE_CHAR)
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

	return (blz_stringview_t) { line.m_length - 1, line.m_data + 1 };
}

static void _blz_text_parse_multiline_comment(const blz_font_t* font, blz_stringview_t* line, blz_vec2f_t position, int32_t* offset, const blz_text_info_t* info, bool* multiline_comment)
{
	size_t line_length = line->m_length;

	// this is an already commented line, we just need to find the end of the comment if any and draw it
	for (int32_t i = 0; i < line->m_length; ++i)
	{
		if (line->m_data[i] == '*' && i + 1 < (int32_t)line->m_length && line->m_data[i + 1] == '/')
		{
			*multiline_comment = false;
			line_length = (size_t)(i + 2);
			break;
		}
	}

	blz_text_draw_tab_delimited(font, *line, position, offset, SF_TEXT_COLOR_COMMENT);

	line->m_data += line_length;
	line->m_length -= line_length;
}


typedef struct sf_text_highlighter_data_tag_t
{
	bool m_multiline_comment;
} sf_text_highlighter_data_t;

static void sf_text_highlighter_begin(const blz_font_t* font, const blz_text_t* text, blz_vec2f_t position, blz_sizef_t size, const blz_text_info_t* info, void* data)
{
	if (!font || !text || !info || !data)
	{
		return;
	}

	sf_text_highlighter_data_t* hdata = (sf_text_highlighter_data_t*)data;

	hdata->m_multiline_comment = false;
}

static void sf_text_highlighter_finish(const blz_font_t* font, const blz_text_t* text, blz_vec2f_t position, blz_sizef_t size, const blz_text_info_t* info, void* data)
{
	if (!font || !text || !info || !data)
	{
		return;
	}

	sf_text_highlighter_data_t* hdata = (sf_text_highlighter_data_t*)data;

	hdata->m_multiline_comment = false;
}

static void sf_text_highlighter_per_line(const blz_font_t* font, blz_stringview_t line, blz_vec2f_t position, const blz_text_info_t* info, void* data)
{
	blz_text_token_t token = { 0 };
	sf_text_highlighter_data_t* hdata = (sf_text_highlighter_data_t*)data;
	int32_t offset = 0;

	if (hdata->m_multiline_comment)
	{
		_blz_text_parse_multiline_comment(font, &line, position, &offset, info, &hdata->m_multiline_comment);
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
			blz_text_draw_tab_delimited(font, token.m_value, position, &offset, color);
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
