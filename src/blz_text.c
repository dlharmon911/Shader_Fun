#include "blz_text.h"

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

static const ALLEGRO_COLOR BLAZE_TEXT_COLOR_KEYWORD = { 0.0f, 0.0f, 1.0f, 1.0f };
static const ALLEGRO_COLOR BLAZE_TEXT_COLOR_FUNCTION = { 0.6f, 0.0f, 1.0f, 1.0f };
static const ALLEGRO_COLOR BLAZE_TEXT_COLOR_NUMBER = { 1.0f, 0.0f, 0.0f, 1.0f };
static const ALLEGRO_COLOR BLAZE_TEXT_COLOR_COMMENT = { 0.0f, 0.4f, 0.0f, 1.0f };
static const ALLEGRO_COLOR BLAZE_TEXT_COLOR_PREPROCESSOR = { 0.5f, 0.5f, 0.5f, 1.0f };
static const ALLEGRO_COLOR BLAZE_TEXT_COLOR_DEFAULT = { 0.25f, 0.25f, 0.25f, 1.0f };
static const ALLEGRO_COLOR BLAZE_TEXT_COLOR_SELECTED = { 0.75f, 0.85f, 1.0f, 1.0f };

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

static blz_stringview_t _blz_text_get_token(blz_stringview_t line, blz_text_token_t* token, bool *multiline_comment)
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

	al_draw_textf(font, BLAZE_TEXT_COLOR_COMMENT, position->m_x, position->m_y, 0, "%.*s", (int32_t)line_length, line->m_data);

	line->m_data += line_length;
	line->m_length -= line_length;

	position->m_x += (float)line_length * (float)al_get_text_width(font, "W");
}

static void _blz_text_hightlight_line(const ALLEGRO_FONT* font, blz_stringview_t line, blz_vec2f_t position, const blz_text_info_t* info, bool* multiline_comment)
{
	blz_text_token_t token = { 0 };

	if (*multiline_comment)
	{
		_blz_text_parse_multiline_comment(font, &line, &position, info, multiline_comment);
	}

	while (line.m_length)
	{
		ALLEGRO_COLOR color = { 0.0f, 0.0f, 0.0f, 1.0f };
		line = _blz_text_get_token(line, &token, multiline_comment);

		switch (token.m_type)
		{
		case GLSL_TOKEN_TYPE_IDENTIFIER:
		{
			if (_blz_text_is_name(token.m_value, glsl_keywords, BLAZE_TEXT_KEYWORD_COUNT))
			{
				color = BLAZE_TEXT_COLOR_KEYWORD;
			}
			else if (_blz_text_is_name(token.m_value, glsl_function_name, BLAZE_TEXT_FUNCTION_NAME_COUNT))
			{
				color = BLAZE_TEXT_COLOR_FUNCTION;
			}
			else
			{
				color = BLAZE_TEXT_COLOR_DEFAULT;
			}
		} break;
		case GLSL_TOKEN_TYPE_NUMBER:
		{
			color = BLAZE_TEXT_COLOR_NUMBER;
		} break;
		case GLSL_TOKEN_TYPE_COMMENT:
		{
			color = BLAZE_TEXT_COLOR_COMMENT;
		} break;
		case GLSL_TOKEN_TYPE_PREPROCESSOR:
		{
			color = BLAZE_TEXT_COLOR_PREPROCESSOR;
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

static blz_stringview_t _blz_text_get_line(blz_stringview_t line, blz_stringview_t* next)
{
	next->m_data = line.m_data;
	next->m_length = 0;

	while (line.m_length)
	{
		char c = line.m_data[0];
		line.m_data++;
		line.m_length--;
	
		if (c == '\n')
		{
			break;
		}
		
		next->m_length++;
	}

	return line;
}

static blz_text_node_t* _blz_text_create_node(blz_stringview_t line)
{
	blz_text_node_t* node = al_malloc(sizeof(blz_text_node_t));

	if (!node)
	{
		return NULL;
	}

	node->m_text = al_ustr_new_from_buffer(line.m_data, line.m_length);
	node->m_next = NULL;

	if (!node->m_text)
	{
		al_free(node);
		return NULL;
	}

	return node;
}

blz_text_node_t* blz_text_create_node()
{
	blz_text_node_t* node = al_malloc(sizeof(blz_text_node_t));
	if (!node)
	{
		return NULL;
	}
	node->m_text = al_ustr_new("");
	node->m_next = NULL;
	if (!node->m_text)
	{
		al_free(node);
		return NULL;
	}
	return node;
}

void blz_text_node_destroy(blz_text_node_t* node)
{
	if (node)
	{
		al_ustr_free(node->m_text);
		al_free(node);
	}
}

static int32_t _blz_text_create(blz_stringview_t line, blz_text_t** out)
{
	*out = al_malloc(sizeof(blz_text_t));

	if (!*out)
	{
		return -1;
	}

	(*out)->m_head = NULL;
	blz_text_node_t* current = NULL;
	blz_stringview_t next_line = { 0 };

	while (line.m_length)
	{
		line = _blz_text_get_line(line, &next_line);
		blz_text_node_t* node = _blz_text_create_node(next_line);

		if (!node)
		{
			return -1;
		}

		if (!current)
		{
			current = (*out)->m_head = node;
		}
		else
		{
			current->m_next = node;
			current = node;
		}
	}

	return 0;
}

blz_text_t* blz_text_create(const char* text)
{
	if (!text)
	{
		return NULL;
	}

	blz_stringview_t line = { text, strlen(text) };
	blz_text_t* result = NULL;

	if (_blz_text_create(line, &result) != 0)
	{
		blz_text_destroy(result);
		result = NULL;
	}

	return result;
}

void blz_text_destroy(blz_text_t* text)
{
	if (text)
	{
		blz_text_node_t* node = text->m_head;

		while (node)
		{
			blz_text_node_t* next = node->m_next;
			al_ustr_free(node->m_text);
			al_free(node);
			node = next;
		}

		al_free(text);
	}
}

size_t blz_text_get_line_count(const blz_text_t* text)
{
	size_t count = 0;
	
	if (text)
	{
		blz_text_node_t* node = text->m_head;
	
		while (node)
		{
			count++;
			node = node->m_next;
		}
	}

	return count;
}

blz_text_node_t* blz_text_get_line(const blz_text_t* text, size_t index)
{
	size_t count = 0;
	
	if (text)
	{
		blz_text_node_t* node = text->m_head;
	
		while (node)
		{
			if (count == index)
			{
				return node;
			}
			count++;
			node = node->m_next;
		}
	}

	return NULL;
}

ALLEGRO_USTR* blz_text_to_ustr(const blz_text_t* text)
{
	ALLEGRO_USTR* out = NULL;

	if (!text)
	{
		return NULL;
	}

	blz_text_node_t* node = text->m_head;

	out = al_ustr_new("");

	while (node)
	{
		al_ustr_append(out, node->m_text);
		al_ustr_append_cstr(out, "\n");

		node = node->m_next;
	}

	return out;
}

blz_text_node_t* blz_text_get_previous_line(const blz_text_t* text, const blz_text_node_t* current_line)
{
	if (!text || !current_line)
	{
		return NULL;
	}
	blz_text_node_t* node = text->m_head;

	if (node == current_line)
	{
		return NULL;
	}

	while (node->m_next)
	{
		if (node->m_next == current_line)
		{
			return node;
		}

		node = node->m_next;
	}

	return NULL;
}

blz_text_node_t* blz_text_get_next_line(const blz_text_t* text, const blz_text_node_t* current_line)
{
	if (!text || !current_line)
	{
		return NULL;
	}

	return current_line->m_next;
}

static void _blz_text_draw(const ALLEGRO_FONT* font, blz_text_node_t* node, blz_vec2f_t position, blz_sizef_t size, const blz_text_info_t* info, bool highlighted)
{
	if (!font || !node || !info)
	{
		return;
	}

	float line_height = (float)al_get_font_line_height(font) + info->m_line_spacing;
	float visible_height = size.m_height - info->m_vertical_padding * 2.0f;
	size_t visible_line_count = (size_t)(visible_height / line_height);
	bool multiline_comment = false;
	position.m_x += info->m_horizontal_padding;
	position.m_y += info->m_vertical_padding;

	blz_text_node_t* current = node;
	int32_t line_offset = 0;

	while (current && line_offset < info->m_top_line)
	{
		current = current->m_next;
		line_offset++;
	}

	if (!current)
	{
		return;
	}
	
	int32_t line_index = info->m_top_line;

	while (current && line_index < (info->m_top_line + visible_line_count))
	{
		if (info->m_selection.m_type != BLAZE_TEXT_SELECTION_TYPE_NONE)
		{
			const blz_text_selection_point_t* a = &info->m_selection.m_start;
			const blz_text_selection_point_t* b = &info->m_selection.m_end;

			if (b->m_line < a->m_line)
			{
				const blz_text_selection_point_t* temp = a;
				a = b;
				b = temp;
			}

			if (line_index >= a->m_line && line_index <= b->m_line)
			{
				float start_x = position.m_x;
				float end_x = position.m_x + (float)(al_get_text_width(font, "W") * (int32_t)al_ustr_size(current->m_text));

				if (line_index == a->m_line)
				{
					start_x += (float)al_get_text_width(font, "W") * (float)a->m_offset;
				}

				if (line_index == b->m_line)
				{
					end_x = position.m_x + (float)al_get_text_width(font, "W") * (float)b->m_offset;
				}

				al_draw_filled_rectangle(start_x, position.m_y, end_x, position.m_y + line_height, BLAZE_TEXT_COLOR_SELECTED);
			}
		}


		if (highlighted)
		{
			_blz_text_hightlight_line(font, (blz_stringview_t) { al_cstr(current->m_text), al_ustr_size(current->m_text) }, position, info, &multiline_comment);
		}
		else
		{
			al_draw_textf(font, info->m_color, position.m_x, position.m_y, 0, "%.*s", (int)al_ustr_size(current->m_text), al_cstr(current->m_text));
		}

		if (line_index == info->m_cursor_line)
		{
			float cursor_x = position.m_x + (float)al_get_text_width(font, "W") * (float)info->m_cursor_offset;

			ALLEGRO_COLOR color = { 1.0f, 1.0f, 1.0f, 1.0f };

			if (((int32_t)(al_get_time() * 4.0)) % 2)
			{
				color = info->m_color;
				color.a = 1.0f;
			}
			al_draw_line(cursor_x, position.m_y, cursor_x, position.m_y + line_height, color, 1.0f);
		}

		position.m_y += line_height;
		current = current->m_next;
		
		line_index++;
	}
}

void blz_text_draw(const ALLEGRO_FONT* font, const blz_text_t* text, blz_vec2f_t position, blz_sizef_t size, const blz_text_info_t* info)
{
	_blz_text_draw(font, text->m_head, position, size, info, false);
}

void blz_text_draw_highlighted(const ALLEGRO_FONT* font, const blz_text_t* text, blz_vec2f_t position, blz_sizef_t size, const blz_text_info_t* info)
{
	_blz_text_draw(font, text->m_head, position, size, info, true);
}

bool blz_text_merge(blz_text_node_t* a, blz_text_node_t** b)
{
	if (!a || !b || !*b)
	{
		return false;
	}
	
	size_t a_length = al_ustr_size(a->m_text);
	size_t b_length = al_ustr_size((*b)->m_text);
	
	if (a_length + b_length + 1 > 1024)
	{
		return false;
	}
	
	al_ustr_append(a->m_text, (*b)->m_text);

	a->m_next = (*b)->m_next;
	
	blz_text_node_destroy(*b);
	*b = NULL;

	return true;
}

bool blz_text_split(blz_text_node_t** node, int32_t offset)
{
	if (!node || !*node || offset < 0 || offset > (int32_t)al_ustr_size((*node)->m_text))
	{
		return false;
	}

	ALLEGRO_USTR* substr = al_ustr_dup_substr((*node)->m_text, offset, (int32_t)al_ustr_size((*node)->m_text));

	blz_text_node_t* new_node = blz_text_create_node();
	if (!new_node)
	{
		return false;
	}

	al_ustr_free(new_node->m_text);
	new_node->m_text = substr;

	al_ustr_truncate((*node)->m_text, offset);

	new_node->m_next = (*node)->m_next;
	(*node)->m_next = new_node;	

	return true;
}

ALLEGRO_USTR* blz_text_copy_selection(const blz_text_node_t* start, int32_t start_offset, const blz_text_node_t* end, int32_t end_offset)
{
	// todo: implement this
	return NULL;
}

int32_t blz_text_paste_selection(blz_text_node_t* target, int32_t offset, const ALLEGRO_USTR* clipboard)
{
	// todo: implement this
	return 0;
}
