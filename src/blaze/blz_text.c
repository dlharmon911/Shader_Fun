#include "blaze/blz_common.h"
#include "blaze/blz_stringview.h"
#include "blaze/blz_font.h"
#include "blaze/blz_shapes.h"
#include "blaze/blz_malloc.h"
#include "blaze/blz_text.h"

typedef int32_t(*blz_text_edit_func_t)(blz_text_t* text, blz_text_info_t* info, blz_text_undo_t* undo, const void* data);

int32_t _blz_text_edit_command_excise(blz_text_t* text, blz_text_info_t* info, blz_text_undo_t* undo, const void* data);
int32_t _blz_text_edit_command_paste(blz_text_t* text, blz_text_info_t* info, blz_text_undo_t* undo, const void* data);
int32_t _blz_text_edit_command_split(blz_text_t* text, blz_text_info_t* info, blz_text_undo_t* undo, const void* data);
int32_t _blz_text_edit_command_merge(blz_text_t* text, blz_text_info_t* info, blz_text_undo_t* undo, const void* data);
int32_t _blz_text_edit_command_undo(blz_text_t* text, blz_text_info_t* info, blz_text_undo_t* undo, const void* data);
int32_t _blz_text_edit_command_redo(blz_text_t* text, blz_text_info_t* info, blz_text_undo_t* undo, const void* data);

static blz_text_edit_func_t _blz_text_edit_funcs[] =
{
	[BLAZE_TEXT_EDIT_OPTION_EXCISE] = (blz_text_edit_func_t)_blz_text_edit_command_excise,
	[BLAZE_TEXT_EDIT_OPTION_PASTE] = (blz_text_edit_func_t)_blz_text_edit_command_paste,
	[BLAZE_TEXT_EDIT_OPTION_SPLIT_LINE] = (blz_text_edit_func_t)_blz_text_edit_command_split,
	[BLAZE_TEXT_EDIT_OPTION_MERGE_LINE] = (blz_text_edit_func_t)_blz_text_edit_command_merge,
	[BLAZE_TEXT_EDIT_OPTION_UNDO] = (blz_text_edit_func_t)_blz_text_edit_command_undo,
	[BLAZE_TEXT_EDIT_OPTION_REDO] = (blz_text_edit_func_t)_blz_text_edit_command_redo,
};

static blz_stringview_t _blz_text_get_line(blz_stringview_t line, blz_stringview_t* next)
{
	next->m_data = line.m_data;
	next->m_length = 0;

	while (line.m_length)
	{
		char c = line.m_data[0];
		line.m_data++;
		line.m_length--;

		if (c == BLAZE_NEWLINE_CHAR)
		{
			break;
		}
		
		next->m_length++;
	}

	char c = next->m_data[next->m_length - 1];
	if (c == BLAZE_CARRIAGE_RETURN_CHAR)
	{
		--next->m_length;
	}

	return line;
}

static blz_text_node_t* _blz_text_create_node(blz_stringview_t line)
{
	blz_text_node_t* node = blz_malloc_new(sizeof(blz_text_node_t));

	if (!node)
	{
		return NULL;
	}

	node->m_text = al_ustr_new_from_buffer(line.m_data, line.m_length);
	node->m_next = NULL;

	if (!node->m_text)
	{
		blz_malloc_delete(node);
		return NULL;
	}

	return node;
}

blz_text_node_t* blz_text_create_node()
{
	blz_text_node_t* node = blz_malloc_new(sizeof(blz_text_node_t));
	if (!node)
	{
		return NULL;
	}
	node->m_text = al_ustr_new("");
	node->m_next = NULL;
	if (!node->m_text)
	{
		blz_malloc_delete(node);
		return NULL;
	}
	return node;
}

void blz_text_node_destroy(blz_text_node_t* node)
{
	if (node)
	{
		al_ustr_free(node->m_text);
		blz_malloc_delete(node);
	}
}

static int32_t _blz_text_create(blz_stringview_t line, blz_text_t** out)
{
	*out = blz_malloc_new(sizeof(blz_text_t));

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

void blz_text_info_copy(blz_text_info_t* dest, const blz_text_info_t* src)
{
	if (!dest || !src)
	{
		return;
	}

	memcpy(dest, src, sizeof(blz_text_info_t));
}

blz_text_t* blz_text_create_empty()
{
	blz_text_t* result = blz_malloc_new(sizeof(blz_text_t));
	if (!result)
	{
		return NULL;
	}

	result->m_head = _blz_text_create_node((blz_stringview_t){ 0, "" });

	if (!result->m_head)
	{
		blz_malloc_delete(result);
		result = NULL;
	}

	return result;
}

blz_text_t* blz_text_create_from_cstr(const char* text)
{
	if (!text)
	{
		return NULL;
	}

	blz_stringview_t line = { strlen(text), text };
	blz_text_t* result = NULL;

	if (_blz_text_create(line, &result) != 0)
	{
		blz_text_destroy(result);
		result = NULL;
	}

	return result;
}

blz_text_t* blz_text_create_from_ustr(const ALLEGRO_USTR* text)
{
	if (!text)
	{
		return NULL;
	}
	
	return blz_text_create_from_cstr(al_cstr(text));
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
			blz_malloc_delete(node);
			node = next;
		}

		blz_malloc_delete(text);
	}
}

blz_text_t* blz_text_clone(const blz_text_t* text)
{
	blz_text_t* result = NULL;

	if (!text)
	{
		return NULL;
	}

	char* cstr = blz_text_to_cstr(text, BLAZE_NEWLINE_STR);

	if (!cstr)
	{
		return NULL;
	}

	result = blz_text_create_from_cstr(cstr);

	blz_malloc_delete(cstr);

	return result;
}

static int32_t _blz_parse_line(ALLEGRO_USTR* ustr, ALLEGRO_FILE* file, size_t* bytes_read)
{
	if (!ustr || !file || !bytes_read)
	{
		return -1;
	}

	ALLEGRO_USTR* line = al_fget_ustr(file);

	if (!line)
	{
		return -1;
	}

	size_t line_length = al_ustr_size(line);
	int32_t result = 0;
	int32_t pos = 0;
	
	while (pos < (int32_t)line_length)
	{
		int32_t c = al_ustr_get_next(line, &pos);

		if (c >= 127)
		{
			c = 127;
		}

		if (c == BLAZE_CARRIAGE_RETURN_CHAR)
		{
			continue;
		}

		if (al_ustr_append_chr(ustr, c) == 0)
		{
			result = -1;
			break;
		}
	}

	*bytes_read += line_length;
	
	al_ustr_free(line);

	return result;
}

static int32_t _blz_text_load_from_file_f(ALLEGRO_FILE* file, blz_text_t** out)
{
	ALLEGRO_USTR* ustr = NULL;

	if (!file)
	{
		return -1;
	}

	ustr = al_ustr_new("");

	if (!ustr)
	{
		return -1;
	}

	size_t file_size = al_fsize(file);

	if (file_size == 0)
	{
		return 0;
	}

	size_t index = 0;

	while (!al_feof(file) && index < file_size)
	{
		_blz_parse_line(ustr, file, &index);
	}

	*out = blz_text_create_from_cstr(al_cstr(ustr));

	al_ustr_free(ustr);

	if (!*out)
	{
		return -1;
	}

	return 0;
}

blz_text_t* blz_text_load_from_file(const char* filename)
{
	blz_text_t* result = NULL;

	if (!filename)
	{
		return NULL;
	}

	ALLEGRO_FILE* file = al_fopen(filename, "rb");

	if (!file)
	{
		return NULL;
	}

	int32_t load_result = _blz_text_load_from_file_f(file, &result);

	al_fclose(file);

	if (load_result != 0)
	{
		blz_text_destroy(result);
		result = NULL;
	}

	return result;
}

bool blz_text_save_to_file(const blz_text_t* text, const char* filename)
{
	if (!text || !filename)
	{
		return false;
	}
	ALLEGRO_FILE* file = al_fopen(filename, "wb");
	if (!file)
	{
		return false;
	}
	blz_text_node_t* node = text->m_head;
	while (node)
	{
		const char* line = al_cstr(node->m_text);
		size_t line_length = al_ustr_size(node->m_text);
		if (al_fwrite(file, line, line_length) != line_length || al_fputc(file, '\n') == EOF)
		{
			al_fclose(file);
			return false;
		}
		node = node->m_next;
	}
	al_fclose(file);
	return true;
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

ALLEGRO_USTR* blz_text_to_ustr(const blz_text_t* text, const char* new_line)
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
		if (new_line)
		{
			al_ustr_append_cstr(out, new_line);
		}

		node = node->m_next;
	}

	return out;
}

char* blz_text_to_cstr(const blz_text_t* text, const char* new_line)
{
	if (!text)
	{
		return NULL;
	}

	ALLEGRO_USTR* ustr = blz_text_to_ustr(text, new_line);
	
	if (!ustr)
	{
		return NULL;
	}

	size_t cstr_length = al_ustr_size(ustr);
	char* out = blz_malloc_new(cstr_length + 1);
	if (!out)
	{
		al_ustr_free(ustr);
		return NULL;
	}

	memcpy(out, al_cstr(ustr), cstr_length);
	out[cstr_length] = '\0';

	al_ustr_free(ustr);

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

static void _blz_text_draw_selection(const blz_font_t* font, const blz_text_node_t* current, blz_vec2f_t position, blz_sizef_t size, float line_height, int32_t line_index, ALLEGRO_COLOR color, const blz_text_info_t* info)
{
	if (info->m_selection.m_type != BLAZE_TEXT_SELECTION_TYPE_NONE)
	{
		const blz_cursor_t* a = &info->m_selection.m_start;
		const blz_cursor_t* b = &info->m_selection.m_end;

		if (b->m_line < a->m_line)
		{
			const blz_cursor_t* temp = a;
			a = b;
			b = temp;
		}

		if (line_index >= a->m_line && line_index <= b->m_line)
		{
			int32_t start_offset = 0;
			int32_t end_offset = (int32_t)al_ustr_size(current->m_text);

			if (line_index == a->m_line)
			{
				start_offset = a->m_offset;
			}

			if (line_index == b->m_line)
			{
				end_offset = b->m_offset;
			}

			blz_stringview_t line = { al_ustr_size(current->m_text), al_cstr(current->m_text) };

			start_offset = blz_text_calculate_tabbed_offset(line, start_offset);
			end_offset = blz_text_calculate_tabbed_offset(line, end_offset);

			float x1 = position.m_x + font->m_char_width * (float)start_offset;
			float x2 = position.m_x + font->m_char_width * (float)end_offset;
			float y1 = position.m_y + info->m_vertical_padding;
			float y2 = position.m_y + line_height + info->m_vertical_padding;

			al_draw_filled_rectangle(x1, y1, x2, y2, color);
		}
	}
}

static void _blz_text_draw_cursor(const blz_font_t* font, blz_stringview_t line, float line_height, blz_vec2f_t position, const blz_text_info_t* info)
{
	ALLEGRO_COLOR color = { 1.0f, 1.0f, 1.0f, 1.0f };
	int32_t offset = info->m_cursor.m_offset;
	offset = blz_text_calculate_tabbed_offset(line, offset);
	float x = position.m_x + font->m_char_width * (float)offset;
	float y1 = position.m_y + info->m_vertical_padding;
	float y2 = position.m_y + line_height + info->m_vertical_padding;

	if (((int32_t)(al_get_time() * 4.0)) % 2)
	{
		color = info->m_color;
		color.a = 1.0f;
	}

	al_draw_line(x, y1, x, y2, color, 1.0f);
}

void blz_text_draw(const blz_font_t* font,
	const blz_text_highlighter_t* highlighter, const blz_text_t* text, blz_vec2f_t position, blz_sizef_t size, const blz_text_info_t* info)
{
	if (!font || !text || !info)
	{
		return;
	}

	float line_height = font->m_line_height + info->m_line_spacing;
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
		static const ALLEGRO_COLOR BLAZE_TEXT_COLOR_SELECTED = { 0.75f, 0.85f, 1.0f, 1.0f };

		_blz_text_draw_selection(font, current, position, size, line_height, line_index, BLAZE_TEXT_COLOR_SELECTED, info);

		size_t line_length = al_ustr_size(current->m_text);
		const char* line_data = al_cstr(current->m_text);

		blz_stringview_t line_view = { .m_length = line_length, .m_data = line_data };

		if (highlighter && highlighter->m_per_line)
		{
			highlighter->m_per_line(font, line_view, position, info, highlighter->m_user_data);
		}
		else
		{
			int32_t temp_offset = 0;
			blz_text_draw_line_tab_delimited(font, line_view, position, &temp_offset, info->m_color);
		}

		if (line_index == info->m_cursor.m_line)
		{
			_blz_text_draw_cursor(font, line_view, line_height, position, info);
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

static int32_t _blz_text_edit_excise_end_of_a_line(blz_text_t* text, const blz_text_info_t* info, const blz_cursor_t* a)
{
	blz_text_node_t* line = blz_text_get_line(text, a->m_line);

	if (!line)
	{
		return -1;
	}

	int32_t line_length = (int32_t)al_ustr_size(line->m_text);

	if (a->m_offset >= line_length)
	{
		return 0;
	}

	al_ustr_truncate(line->m_text, a->m_offset);

	return 0;
}

static int32_t _blz_text_edit_excise_start_of_b_line(blz_text_t* text, const blz_text_info_t* info, const blz_cursor_t* b)
{
	blz_text_node_t* line = blz_text_get_line(text, b->m_line);
	
	if (!line)
	{
		return -1;
	}
	
	if (b->m_offset <= 0)
	{
		return 0;
	}

	if (b->m_offset >= (int32_t)al_ustr_size(line->m_text))
	{
		al_ustr_truncate(line->m_text, 0);
		return 0;
	}
	
	al_ustr_remove_range(line->m_text, 0, b->m_offset);

	return 0;
}

static int32_t _blz_text_edit_excise_same_line(blz_text_t* text, blz_text_info_t* info, const blz_cursor_t* a, const blz_cursor_t* b)
{
	if (a->m_offset == b->m_offset)
	{
		return 0;
	}

	blz_text_node_t* line = blz_text_get_line(text, a->m_line);

	if (!line)
	{
		return -1;
	}

	int32_t line_length = (int32_t)al_ustr_size(line->m_text);

	if (a->m_offset == 0 && b->m_offset >= line_length)
	{
		al_ustr_truncate(line->m_text, a->m_offset);
	}
	else
	{
		al_ustr_remove_range(line->m_text, a->m_offset, b->m_offset);
	}
	
	info->m_cursor.m_offset = a->m_offset;

	return 0;
}

static int32_t blz_text_merge_nodes(blz_text_node_t** head, blz_text_node_t* a, blz_text_node_t* b)
{
	if (!a || !b)
	{
		return -1;
	}

	al_ustr_append(a->m_text, b->m_text);
	a->m_next = b->m_next;
	blz_text_node_destroy(b);

	return 0;
}

static int32_t _blz_text_edit_excise_text(blz_text_t* text, blz_text_info_t* info)
{
	if (!text || !info)
	{
		return -1;
	}

	const blz_text_selection_t* selection = &info->m_selection;

	int32_t line_a = selection->m_start.m_line;
	int32_t offset_a = selection->m_start.m_offset;
	int32_t line_b = selection->m_end.m_line;
	int32_t offset_b = selection->m_end.m_offset;

	if (line_a > line_b)
	{
		int32_t temp_line = line_a;
		line_a = line_b;
		line_b = temp_line;

		int32_t temp_offset = offset_a;
		offset_a = offset_b;
		offset_b = temp_offset;
	}

	if (line_a == line_b && offset_a > offset_b)
	{
		int32_t temp_offset = offset_a;
		offset_a = offset_b;
		offset_b = temp_offset;
	}

	info->m_cursor.m_line = line_a;
	info->m_cursor.m_offset = offset_a;

	const blz_cursor_t a = { line_a, offset_a };
	const blz_cursor_t b = { line_b, offset_b };

	if (a.m_line == b.m_line)
	{
		return _blz_text_edit_excise_same_line(text, info, &a, &b);
	}
	else
	{
		if (_blz_text_edit_excise_end_of_a_line(text, info, &a) != 0)
		{
			return -1;
		}

		if (_blz_text_edit_excise_start_of_b_line(text, info, &b) != 0)
		{
			return -1;
		}

		blz_text_node_t* current = blz_text_get_line(text, a.m_line)->m_next;
		while (current && current != blz_text_get_line(text, b.m_line))
		{
			blz_text_node_t* next = current->m_next;
			al_ustr_free(current->m_text);
			blz_malloc_delete(current);
			current = next;
		}

		blz_text_node_t* a_line = blz_text_get_line(text, a.m_line);
		blz_text_node_t* b_line = blz_text_get_line(text, b.m_line);

		if (blz_text_merge_nodes(&text->m_head, a_line, b_line) < 0)
		{
			return -1;
		}
	}

	return 0;
}

int32_t blz_text_cut_to_clipboard(blz_text_t* text, blz_text_info_t* info, blz_text_undo_t* undo)
{
	if (!text)
	{
		return -1;
	}
	
	if (blz_text_copy_to_clipboard(text, info) != 0)
	{
		return -1;
	}

	int32_t edit_result = blz_text_edit(BLAZE_TEXT_EDIT_OPTION_EXCISE, text, info, undo, 0);

	if (edit_result != 0)
	{
		return -1;
	}

	info->m_selection.m_type = BLAZE_TEXT_SELECTION_TYPE_NONE;

	return 0;
}

static ALLEGRO_USTR* blz_text_copy_same_line(const blz_text_t* text, const blz_text_info_t* info, const blz_cursor_t* a, const blz_cursor_t* b)
{
	int32_t oa = a->m_offset;
	int32_t ob = b->m_offset;

	if (oa == ob)
	{
		return al_ustr_new("");
	}
	else if (oa > ob)
	{
		int32_t temp_offset = oa;
		oa = ob;
		ob = temp_offset;
	}

	const blz_text_node_t* line = blz_text_get_line(text, a->m_line);
	return al_ustr_dup_substr(line->m_text, oa, ob);
}

static int32_t blz_text_copy_end_of_a_line(ALLEGRO_USTR* ustr, const blz_text_t* text, const blz_text_info_t* info, const blz_cursor_t* a)
{
	const blz_text_node_t* line = blz_text_get_line(text, a->m_line);
	int32_t line_length = (int32_t)al_ustr_size(line->m_text);

	if (a->m_offset >= line_length)
	{
		return al_ustr_append_cstr(ustr, BLAZE_NEWLINE_STR) ? 0 : -1;
	}

	int32_t length = line_length - a->m_offset;
	const char* text_start = al_cstr(line->m_text) + a->m_offset;

	if (!al_ustr_appendf(ustr, "%.*s\n", length, text_start))
	{
		return -1;
	}

	return 0;
}

static int32_t blz_text_copy_start_of_b_line(ALLEGRO_USTR* ustr, const blz_text_t* text, const blz_text_info_t* info, const blz_cursor_t* b)
{
	if (b->m_offset <= 0)
	{
		return 0;
	}

	const blz_text_node_t* line = blz_text_get_line(text, b->m_line);
	int32_t length = b->m_offset;
	const char* text_start = al_cstr(line->m_text);
	
	if (!al_ustr_appendf(ustr, "%.*s\n", length, text_start))
	{
		return -1;
	}

	return 0;
}

static ALLEGRO_USTR* blz_text_copy_different_line(const blz_text_t* text, const blz_text_info_t* info, const blz_cursor_t* a, const blz_cursor_t* b)
{
	ALLEGRO_USTR* ustr = al_ustr_new("");

	if (!ustr)
	{
		return NULL;
	}

	if (blz_text_copy_end_of_a_line(ustr, text, info, a) != 0)
	{
		al_ustr_free(ustr);
		return NULL;
	}

	blz_text_node_t* current = blz_text_get_line(text, a->m_line)->m_next;

	while (current && current != blz_text_get_line(text, b->m_line))
	{
		int32_t length = (int32_t)al_ustr_size(current->m_text);
		const char* text_start = al_cstr(current->m_text);

		if (!al_ustr_appendf(ustr, "%.*s\n", length, text_start))
		{
			al_ustr_free(ustr);
			return NULL;
		}

		current = current->m_next;
	}

	if (blz_text_copy_start_of_b_line(ustr, text, info, b) != 0)
	{
		al_ustr_free(ustr);
		return NULL;
	}

	return ustr;
}

int32_t blz_text_copy_to_clipboard(const blz_text_t* text, const blz_text_info_t* info)
{
	ALLEGRO_USTR* ustr = blz_text_copy_to_ustr(text, info);
	
	if (!ustr)
	{
		return -1;
	}

	ALLEGRO_DISPLAY* current_display = al_get_current_display();

	if (current_display)
	{
		al_set_clipboard_text(current_display, al_cstr(ustr));
	}

	al_ustr_free(ustr);

	return 0;
}


ALLEGRO_USTR* blz_text_copy_to_ustr(const blz_text_t* text, const blz_text_info_t* info)
{
	ALLEGRO_USTR* ustr = NULL;

	if (!text || !info)
	{
		return NULL;
	}

	if (info->m_selection.m_type == BLAZE_TEXT_SELECTION_TYPE_NONE)
	{
		return al_ustr_new("");
	}

	const blz_text_selection_t* selection = &info->m_selection;

	const blz_cursor_t* a = &selection->m_start;
	const blz_cursor_t* b = &selection->m_end;

	if (a->m_line > b->m_line)
	{
		const blz_cursor_t* temp_point = a;
		a = b;
		b = temp_point;
	}

	if (a->m_line == b->m_line)
	{
		ustr = blz_text_copy_same_line(text, info, a, b);
	}
	else
	{
		ustr = blz_text_copy_different_line(text, info, a, b);
	}

	return ustr;
}

blz_text_t* blz_text_copy(const blz_text_t* text, const blz_text_info_t* info)
{
	if (!text || !info)
	{
		return NULL;
	}

	ALLEGRO_USTR* ustr = blz_text_copy_to_ustr(text, info);
	if (!ustr)
	{
		return NULL;
	}

	blz_text_t* result = blz_text_create_from_ustr(ustr);
	al_ustr_free(ustr);
	return result;
}

static blz_text_t* _blz_text_from_clipboard()
{
	ALLEGRO_DISPLAY* current_display = al_get_current_display();
	if (!current_display)
	{
		return NULL;
	}
	char* clipboard_text = al_get_clipboard_text(current_display);
	if (!clipboard_text)
	{
		return NULL;
	}
	blz_text_t* clipboard_blz_text = blz_text_create_from_cstr(clipboard_text);
	blz_malloc_delete((void*)clipboard_text);
	return clipboard_blz_text;
}

static int32_t _blz_text_extract_hbt(blz_text_t** text, blz_text_node_t** head, blz_text_node_t** body, blz_text_node_t** tail)
{
	if (!text || !*text || !head || !body || !tail)
	{
		return -1;
	}

	*head = (*text)->m_head;
	*body = (*text)->m_head->m_next;
	*tail = *body;

	while (*tail && (*tail)->m_next)
	{
		*tail = (*tail)->m_next;
	}

	if ((*head)->m_next == *tail)
	{
		*body = NULL;
	}

	(*text)->m_head = NULL;
	blz_malloc_delete(*text);

	return 0;
}

int32_t blz_text_paste_cursor(const blz_text_t* text, blz_text_info_t* info, const blz_text_t* paste_text)
{
	blz_text_node_t* head = NULL;
	blz_text_node_t* body = NULL;
	blz_text_node_t* tail = NULL;

	if (!text || !info)
	{
		return -1;
	}

	blz_text_t* text_copy = blz_text_clone(paste_text);

	if (_blz_text_extract_hbt(&text_copy, &head, &body, &tail) != 0)
	{
		return -1;
	}

	blz_text_node_t* current_line = blz_text_get_line(text, info->m_cursor.m_line);
	if (!current_line)
	{
		blz_text_node_destroy(head);
		if (body)
		{
			blz_text_node_t* node = body;
			while (node && node != tail)
			{
				blz_text_node_destroy(node);
				node = node->m_next;
			}
		}
		if (tail && tail != head)
		{
			blz_text_node_destroy(tail);
		}
		return -1;
	}

	if (!tail)
	{
		int32_t head_length = (int32_t)al_ustr_size(head->m_text);
		al_ustr_insert(current_line->m_text, info->m_cursor.m_offset, head->m_text);
		info->m_cursor.m_offset += head_length;
		blz_text_node_destroy(head);
		return 0;
	}

	if (!blz_text_split(&current_line, info->m_cursor.m_offset))
	{
		blz_text_node_destroy(head);
		if (body)
		{
			blz_text_node_t* node = body;
			while (node && node != tail)
			{
				blz_text_node_destroy(node);
				node = node->m_next;
			}
		}
		blz_text_node_destroy(tail);
		return -1;
	}

	blz_text_node_t* second_part = current_line->m_next;
	int32_t second_part_length = (int32_t)al_ustr_size(second_part->m_text);

	al_ustr_append(current_line->m_text, head->m_text);
	blz_text_node_destroy(head);

	if (body)
	{
		current_line->m_next = body;
		blz_text_node_t* last_body = body;
		while (last_body->m_next && last_body->m_next != tail)
		{
			last_body = last_body->m_next;
		}
		last_body->m_next = tail;
	}
	else
	{
		current_line->m_next = tail;
	}

	al_ustr_append(tail->m_text, second_part->m_text);
	tail->m_next = second_part->m_next;
	blz_text_node_destroy(second_part);

	int32_t lines_added = 0;
	blz_text_node_t* node = current_line->m_next;
	while (node && node != tail)
	{
		lines_added++;
		node = node->m_next;
	}
	if (node == tail)
	{
		lines_added++;
	}

	info->m_cursor.m_line += lines_added;
	info->m_cursor.m_offset = (int32_t)al_ustr_size(tail->m_text) - second_part_length;

	info->m_selection.m_type = BLAZE_TEXT_SELECTION_TYPE_NONE;

	return 0;
}

int32_t blz_text_select_all(const blz_text_t* text, blz_text_info_t* info)
{
	if (!text || !info)
	{
		return -1;
	}
	
	size_t line_count = blz_text_get_line_count(text);
	
	if (line_count == 0)
	{
		info->m_selection.m_type = BLAZE_TEXT_SELECTION_TYPE_NONE;
		return 0;
	}

	info->m_selection.m_start.m_line = 0;
	info->m_selection.m_start.m_offset = 0;
	info->m_selection.m_end.m_line = (int32_t)line_count - 1;
	const blz_text_node_t* last_line = blz_text_get_line(text, line_count - 1);
	info->m_selection.m_end.m_offset = (int32_t)al_ustr_size(last_line->m_text);
	info->m_selection.m_type = BLAZE_TEXT_SELECTION_TYPE_SELECTED;

	return 0;
}

static int32_t _blz_text_edit_split(blz_text_t* text, blz_text_info_t* info)
{
	if (!text || !info)
	{
		return -1;
	}

	blz_text_node_t* current_line = blz_text_get_line(text, info->m_cursor.m_line);
	int32_t size = (int32_t)al_ustr_size(current_line->m_text);

	if (info->m_cursor.m_offset == size)
	{
	    blz_text_node_t* next_line = blz_text_get_next_line(text, current_line);
	    current_line->m_next = blz_text_create_node();
	    current_line->m_next->m_next = next_line;
	}
	else
	{
	    blz_text_split(&current_line, info->m_cursor.m_offset);
	}

	++info->m_cursor.m_line;
	info->m_cursor.m_offset = 0;

	return 0;
}

static int32_t _blz_text_edit_merge(blz_text_t* text, blz_text_info_t* info)
{
	if (!text || !info)
	{
		return -1;
	}

	blz_text_node_t* current_line = blz_text_get_line(text, info->m_cursor.m_line);
	blz_text_node_t* previous_line = blz_text_get_previous_line(text, current_line);
	int32_t prev_length = (int32_t)al_ustr_size(previous_line->m_text);

	blz_text_merge(previous_line, &current_line);

	--info->m_cursor.m_line;
	info->m_cursor.m_offset = prev_length;

	return 0;
}

int32_t blz_text_edit(int32_t option, blz_text_t* text, blz_text_info_t* info, blz_text_undo_t* undo, const void* data)
{
	int32_t result = 0;

	if (!text || !info || option < 0 || option >= BLAZE_TEXT_EDIT_OPTION_COUNT)
	{
		return -1;
	}

	blz_text_edit_func_t func = _blz_text_edit_funcs[option];
	if (!func)
	{
		return -1;
	}

	result = func(text, info, undo, data);

	return result;
}

static void blz_text_draw_tab_section(const blz_font_t* font, blz_stringview_t section, blz_vec2f_t position, int32_t* offset, const ALLEGRO_COLOR color)
{
	float x = position.m_x + font->m_char_width * (float)(*offset);

	al_draw_textf(font->m_font, color, x, position.m_y, 0, "%.*s", section.m_length, section.m_data);

	(*offset) += (int32_t)section.m_length;
}

void blz_text_draw_line_tab_delimited(const blz_font_t* font, blz_stringview_t text, blz_vec2f_t position, int32_t* offset, const ALLEGRO_COLOR color)
{
	for (int32_t i = 0; i < text.m_length; ++i)
	{
		if (text.m_data[i] == BLAZE_TAB_CHAR)
		{
			if (i > 0)
			{
				blz_stringview_t section = { (size_t)i, text.m_data };
				blz_text_draw_tab_section(font, section, position, offset, color);

				i = -1;
				text.m_data += section.m_length;
				text.m_length -= section.m_length;
			}

			int32_t t = ((*offset) % 4);
			if (t == 0)
			{
				t = 4;
			}
			(*offset) += t;
			++text.m_data;
			--text.m_length;
		}
	}

	if (text.m_length > 0)
	{
		blz_text_draw_tab_section(font, text, position, offset, color);
	}
}

int32_t blz_text_calculate_tabbed_offset(blz_stringview_t line, int32_t offset)
{
	int32_t tabbed_offset = 0;

	offset = min(offset, (int32_t)line.m_length);

	for (int32_t i = 0; i < offset; ++i)
	{
		char c = line.m_data[i];

		if (c == BLAZE_TAB_CHAR)
		{
			int32_t t = (tabbed_offset % 4);
			if (t == 0)
			{
				t = 4;
			}

			tabbed_offset += (t - 1);
		}

		++tabbed_offset;
	}

	return tabbed_offset;
}

int32_t blz_text_calculate_untabbed_offset(blz_stringview_t line, int32_t tabbed_offset)
{
	int32_t untabbed_offset = 0;
	int32_t current_tabbed_offset = 0;
	for (size_t i = 0; i < line.m_length; ++i)
	{
		if (current_tabbed_offset >= tabbed_offset)
		{
			break;
		}
		char c = line.m_data[i];
		if (c == BLAZE_TAB_CHAR)
		{
			int32_t t = (current_tabbed_offset % 4);
			if (t == 0)
			{
				t = 4;
			}
			current_tabbed_offset += (t - 1);
		}
		++current_tabbed_offset;
		++untabbed_offset;
	}

	return untabbed_offset;
}

static void _blz_text_selection_ordered(const blz_text_selection_t* selection, blz_cursor_t* first, blz_cursor_t* last)
{
	blz_cursor_t a = selection->m_start;
	blz_cursor_t b = selection->m_end;

	if (a.m_line > b.m_line || (a.m_line == b.m_line && a.m_offset > b.m_offset))
	{
		blz_cursor_t temp = a;
		a = b;
		b = temp;
	}

	*first = a;
	*last = b;
}

static blz_text_undo_node_data_t _blz_text_undo_make_selection_data(const blz_text_info_t* info, blz_text_t* text)
{
	blz_text_undo_node_data_t node_data = { .m_info = *info, .m_text = text };

	_blz_text_selection_ordered(&info->m_selection, &node_data.m_info.m_selection.m_start, &node_data.m_info.m_selection.m_end);
	node_data.m_info.m_selection.m_type = BLAZE_TEXT_SELECTION_TYPE_SELECTED;
	node_data.m_info.m_cursor = node_data.m_info.m_selection.m_start;

	return node_data;
}

static blz_text_undo_node_data_t _blz_text_undo_make_cursor_data(const blz_text_info_t* info, blz_text_t* text)
{
	blz_text_undo_node_data_t node_data = { .m_info = *info, .m_text = text };

	return node_data;
}

static void _blz_text_undo_record(blz_text_undo_t* undo, int32_t type, blz_text_undo_node_data_t* data, blz_text_undo_node_data_t* extra)
{
	bool recorded = false;

	if (undo)
	{
		blz_text_undo_pop_to_current(undo);
		recorded = blz_text_undo_pushback(undo, type, data, extra);
	}

	if (!recorded)
	{
		blz_text_destroy(data->m_text);
		if (extra)
		{
			blz_text_destroy(extra->m_text);
		}
	}
}

static int32_t _blz_text_undo_excise_range(blz_text_t* text, blz_text_info_t* info, const blz_cursor_t* start, const blz_cursor_t* end)
{
	info->m_selection.m_start = *start;
	info->m_selection.m_end = *end;
	info->m_selection.m_type = BLAZE_TEXT_SELECTION_TYPE_SELECTED;

	int32_t result = _blz_text_edit_excise_text(text, info);

	info->m_selection.m_type = BLAZE_TEXT_SELECTION_TYPE_NONE;

	return result;
}

static int32_t _blz_text_edit_command_excise(blz_text_t* text, blz_text_info_t* info, blz_text_undo_t* undo, const void* data)
{
	if (!text || !info)
	{
		return -1;
	}

	blz_text_t* excised_text = blz_text_copy(text, info);
	if (!excised_text)
	{
		return -1;
	}

	blz_text_undo_node_data_t undo_node_data = _blz_text_undo_make_selection_data(info, excised_text);

	int32_t result = _blz_text_edit_excise_text(text, info);

	info->m_selection.m_type = BLAZE_TEXT_SELECTION_TYPE_NONE;

	_blz_text_undo_record(undo, BLAZE_TEXT_EDIT_OPTION_EXCISE, &undo_node_data, NULL);

	return result;
}

static int32_t _blz_text_edit_command_excise_paste(blz_text_t* text, blz_text_info_t* info, blz_text_undo_t* undo, const void* data)
{
	if (!text || !info || !data)
	{
		return -1;
	}

	blz_text_t* excised_text = blz_text_copy(text, info);
	if (!excised_text)
	{
		return -1;
	}

	blz_text_t* pasted_text = blz_text_create_from_cstr((const char*)data);
	if (!pasted_text)
	{
		blz_text_destroy(excised_text);
		return -1;
	}

	blz_text_undo_node_data_t undo_node_data = _blz_text_undo_make_selection_data(info, excised_text);

	int32_t excise_result = _blz_text_edit_excise_text(text, info);

	info->m_selection.m_type = BLAZE_TEXT_SELECTION_TYPE_NONE;

	int32_t paste_result = blz_text_paste_cursor(text, info, pasted_text);

	blz_text_undo_node_data_t undo_node_extra = _blz_text_undo_make_cursor_data(info, pasted_text);

	_blz_text_undo_record(undo, BLAZE_TEXT_EDIT_OPTION_EXCISE_AND_PASTE, &undo_node_data, &undo_node_extra);

	return (paste_result < 0 || excise_result < 0) ? -1 : 0;
}

static int32_t _blz_text_edit_command_paste(blz_text_t* text, blz_text_info_t* info, blz_text_undo_t* undo, const void* data)
{
	if (!text || !info || !data)
	{
		return -1;
	}

	if (info->m_selection.m_type != BLAZE_TEXT_SELECTION_TYPE_NONE)
	{
		return _blz_text_edit_command_excise_paste(text, info, undo, data);
	}

	blz_text_t* pasted_text = blz_text_create_from_cstr((const char*)data);
	if (!pasted_text)
	{
		return -1;
	}

	blz_text_undo_node_data_t undo_node_data = _blz_text_undo_make_cursor_data(info, pasted_text);

	int32_t paste_result = blz_text_paste_cursor(text, info, pasted_text);

	info->m_selection.m_type = BLAZE_TEXT_SELECTION_TYPE_NONE;

	blz_text_undo_node_data_t undo_node_extra = _blz_text_undo_make_cursor_data(info, NULL);

	_blz_text_undo_record(undo, BLAZE_TEXT_EDIT_OPTION_PASTE, &undo_node_data, &undo_node_extra);

	return paste_result;
}

static int32_t _blz_text_edit_command_split(blz_text_t* text, blz_text_info_t* info, blz_text_undo_t* undo, const void* data)
{
	if (!text || !info)
	{
		return -1;
	}

	blz_text_undo_node_data_t undo_node_data = _blz_text_undo_make_cursor_data(info, NULL);

	if (_blz_text_edit_split(text, info) != 0)
	{
		return -1;
	}

	blz_text_undo_node_data_t undo_node_extra = _blz_text_undo_make_cursor_data(info, NULL);

	_blz_text_undo_record(undo, BLAZE_TEXT_EDIT_OPTION_SPLIT_LINE, &undo_node_data, &undo_node_extra);

	return 0;
}

static int32_t _blz_text_edit_command_merge(blz_text_t* text, blz_text_info_t* info, blz_text_undo_t* undo, const void* data)
{
	if (!text || !info)
	{
		return -1;
	}

	blz_text_undo_node_data_t undo_node_data = _blz_text_undo_make_cursor_data(info, NULL);

	if (_blz_text_edit_merge(text, info) != 0)
	{
		return -1;
	}

	blz_text_undo_node_data_t undo_node_extra = _blz_text_undo_make_cursor_data(info, NULL);

	_blz_text_undo_record(undo, BLAZE_TEXT_EDIT_OPTION_MERGE_LINE, &undo_node_data, &undo_node_extra);

	return 0;
}


int32_t _blz_text_edit_command_undo(blz_text_t* text, blz_text_info_t* info, blz_text_undo_t* undo, const void* data)
{
	if (!text || !info || !undo)
	{
		return -1;
	}

	if (!blz_text_undo_can_undo(undo))
	{
		return -1;
	}

	int32_t current_node = blz_text_undo_get_current(undo);
	if (current_node < 0)
	{
		return -1;
	}

	blz_text_undo_node_t* node = blz_text_undo_get(undo, current_node);
	if (!node)
	{
		return -1;
	}

	if (blz_text_undo_dec_current(undo))
	{
		return -1;
	}

	const blz_text_undo_node_data_t* node_data = &node->m_data;
	const blz_text_undo_node_data_t* node_extra = &node->m_extra;
	const blz_cursor_t* cursor = &node_data->m_info.m_cursor;

	info->m_selection.m_type = BLAZE_TEXT_SELECTION_TYPE_NONE;

	switch (node->m_type)
	{
	case BLAZE_TEXT_EDIT_OPTION_EXCISE:
	{
		info->m_cursor = *cursor;
		blz_text_paste_cursor(text, info, node_data->m_text);
	} break;
	case BLAZE_TEXT_EDIT_OPTION_EXCISE_AND_PASTE:
	{
		_blz_text_undo_excise_range(text, info, cursor, &node_extra->m_info.m_cursor);
		info->m_cursor = *cursor;
		blz_text_paste_cursor(text, info, node_data->m_text);
	} break;
	case BLAZE_TEXT_EDIT_OPTION_PASTE:
	{
		_blz_text_undo_excise_range(text, info, cursor, &node_extra->m_info.m_cursor);
	} break;
	case BLAZE_TEXT_EDIT_OPTION_MERGE_LINE:
	{
		info->m_cursor = node_extra->m_info.m_cursor;
		_blz_text_edit_split(text, info);
	} break;
	case BLAZE_TEXT_EDIT_OPTION_SPLIT_LINE:
	{
		info->m_cursor = node_extra->m_info.m_cursor;
		_blz_text_edit_merge(text, info);
	} break;
	default: break;
	}

	info->m_selection.m_type = BLAZE_TEXT_SELECTION_TYPE_NONE;
	info->m_cursor = *cursor;

	return 0;
}

int32_t _blz_text_edit_command_redo(blz_text_t* text, blz_text_info_t* info, blz_text_undo_t* undo, const void* data)
{
	if (!text || !info || !undo)
	{
		return -1;
	}

	if (!blz_text_undo_can_redo(undo))
	{
		return -1;
	}

	if (blz_text_undo_inc_current(undo))
	{
		return -1;
	}

	int32_t current_node = blz_text_undo_get_current(undo);
	if (current_node < 0)
	{
		return -1;
	}

	blz_text_undo_node_t* node = blz_text_undo_get(undo, current_node);
	if (!node)
	{
		return -1;
	}

	const blz_text_undo_node_data_t* node_data = &node->m_data;
	const blz_text_undo_node_data_t* node_extra = &node->m_extra;
	const blz_cursor_t* cursor = &node_data->m_info.m_cursor;

	info->m_selection.m_type = BLAZE_TEXT_SELECTION_TYPE_NONE;

	switch (node->m_type)
	{
	case BLAZE_TEXT_EDIT_OPTION_EXCISE:
	{
		_blz_text_undo_excise_range(text, info, &node_data->m_info.m_selection.m_start, &node_data->m_info.m_selection.m_end);
		info->m_cursor = *cursor;
	} break;
	case BLAZE_TEXT_EDIT_OPTION_EXCISE_AND_PASTE:
	{
		_blz_text_undo_excise_range(text, info, &node_data->m_info.m_selection.m_start, &node_data->m_info.m_selection.m_end);
		info->m_cursor = *cursor;
		blz_text_paste_cursor(text, info, node_extra->m_text);
		cursor = &node_extra->m_info.m_cursor;
	} break;
	case BLAZE_TEXT_EDIT_OPTION_PASTE:
	{
		info->m_cursor = *cursor;
		blz_text_paste_cursor(text, info, node_data->m_text);
		cursor = &node_extra->m_info.m_cursor;
	} break;
	case BLAZE_TEXT_EDIT_OPTION_MERGE_LINE:
	{
		info->m_cursor = *cursor;
		_blz_text_edit_merge(text, info);
		cursor = &node_extra->m_info.m_cursor;
	} break;
	case BLAZE_TEXT_EDIT_OPTION_SPLIT_LINE:
	{
		info->m_cursor = *cursor;
		_blz_text_edit_split(text, info);
		cursor = &node_extra->m_info.m_cursor;
	} break;
	default: break;
	}

	info->m_selection.m_type = BLAZE_TEXT_SELECTION_TYPE_NONE;
	info->m_cursor = *cursor;

	return 0;
}