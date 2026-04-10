#include "blaze/blz_text.h"

static const ALLEGRO_COLOR BLAZE_TEXT_COLOR_SELECTED = { 0.75f, 0.85f, 1.0f, 1.0f };

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

void blz_text_draw(const ALLEGRO_FONT* font, const blz_text_t* text, blz_vec2f_t position, blz_sizef_t size, const blz_text_info_t* info)
{
	blz_text_draw_highlighted(font, text, position, size, info, NULL);
}

void blz_text_draw_highlighted(const ALLEGRO_FONT* font, const blz_text_t* text, blz_vec2f_t position, blz_sizef_t size, const blz_text_info_t* info, const blz_text_highlighter_t* highlighter)
{
	if (!font || !text || !info)
	{
		return;
	}

	float line_height = (float)al_get_font_line_height(font) + info->m_line_spacing;
	float visible_height = size.m_height - info->m_vertical_padding * 2.0f;
	size_t visible_line_count = (size_t)(visible_height / line_height);
	position.m_x += info->m_horizontal_padding;
	position.m_y += info->m_vertical_padding;

	blz_text_node_t* current = text->m_head;
	int32_t line_offset = 0;

	if (highlighter && highlighter->m_begin)
	{
		highlighter->m_begin(font, text, position, size, info, highlighter->m_user_data);
	}

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


		if (highlighter && highlighter->m_per_line)
		{
			highlighter->m_per_line(font, (blz_stringview_t) { al_cstr(current->m_text), al_ustr_size(current->m_text) }, position, info, highlighter->m_user_data);
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

	if (highlighter && highlighter->m_finish)
	{
		highlighter->m_finish(font, text, position, size, info, highlighter->m_user_data);
	}
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


int32_t blz_text_excise_selection(blz_text_t* text, const blz_text_selection_t* selection)
{
	// TODO: implement this function
	return 0;
}

int32_t blz_text_cut_selection(const blz_text_t* text, const blz_text_selection_t* selection, ALLEGRO_USTR** clipboard)
{
	// TODO: implement this function
	return 0;
}

int32_t blz_text_copy_selection(const blz_text_t* text, const blz_text_selection_t* selection, ALLEGRO_USTR** clipboard)
{
	// TODO: implement this function
	return 0;
}

int32_t blz_text_paste_selection(blz_text_t* text, const blz_text_selection_t* selection, const ALLEGRO_USTR* clipboard)
{
	// TODO: implement this function
	return 0;
}

