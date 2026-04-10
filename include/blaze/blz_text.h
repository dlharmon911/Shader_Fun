#ifndef _GUARD_BLAZE_TEXT_H_
#define _GUARD_BLAZE_TEXT_H_

#include "blaze.h"

typedef struct blz_text_node_t
{
	ALLEGRO_USTR* m_text;
	struct blz_text_node_t* m_next;
} blz_text_node_t;

typedef struct blz_text_t
{
	blz_text_node_t* m_head;
} blz_text_t;

typedef struct blz_text_selection_point_t
{
	int32_t m_line;
	int32_t m_offset;
} blz_text_selection_point_t;

enum BLAZE_TEXT_SELECTION_TYPE
{
	BLAZE_TEXT_SELECTION_TYPE_NONE,
	BLAZE_TEXT_SELECTION_TYPE_START_GRABBED,
	BLAZE_TEXT_SELECTION_TYPE_SELECTED
};

typedef struct blz_text_selection_t
{
	blz_text_selection_point_t m_start;
	blz_text_selection_point_t m_end;
	int32_t m_type;
} blz_text_selection_t;

typedef struct blz_text_info_tag_t
{
	blz_text_selection_t m_selection;
	int32_t m_top_line;
	int32_t m_cursor_line;
	int32_t m_cursor_offset;
	float m_horizontal_padding;
	float m_vertical_padding;
	float m_line_spacing;
	ALLEGRO_COLOR m_color;
} blz_text_info_t;

typedef void (*blz_text_highlight_begin_func_t)(const ALLEGRO_FONT* font, const blz_text_t* text, blz_vec2f_t position, blz_sizef_t size, const blz_text_info_t* info, void* user_data);
typedef void (*blz_text_highlight_finish_func_t)(const ALLEGRO_FONT* font, const blz_text_t* text, blz_vec2f_t position, blz_sizef_t size, const blz_text_info_t* info, void* user_data);
typedef void (*blz_text_highlight_per_line_func_t)(const ALLEGRO_FONT* font, blz_stringview_t line, blz_vec2f_t position, const blz_text_info_t* info, void* user_data);

typedef struct blz_text_highlighter_t
{
	blz_text_highlight_begin_func_t m_begin;
	blz_text_highlight_per_line_func_t m_per_line;
	blz_text_highlight_finish_func_t m_finish;
	void* m_user_data;
} blz_text_highlighter_t;

blz_text_t* blz_text_create(const char* text);
void blz_text_destroy(blz_text_t* text);
size_t blz_text_get_line_count(const blz_text_t* text);
blz_text_node_t* blz_text_get_line(const blz_text_t* text, size_t index);
ALLEGRO_USTR* blz_text_to_ustr(const blz_text_t* text);
int32_t blz_text_excise_selection(blz_text_t* text, const blz_text_selection_t* selection);
int32_t blz_text_cut_selection(const blz_text_t* text, const blz_text_selection_t* selection, ALLEGRO_USTR** clipboard);
int32_t blz_text_copy_selection(const blz_text_t* text, const blz_text_selection_t* selection, ALLEGRO_USTR** clipboard);
int32_t blz_text_paste_selection(blz_text_t* text, const blz_text_selection_t* selection, const ALLEGRO_USTR* clipboard);
blz_text_node_t* blz_text_create_node();
void blz_text_node_destroy(blz_text_node_t* node);
blz_text_node_t* blz_text_get_previous_line(const blz_text_t* text, const blz_text_node_t* current_line);
blz_text_node_t* blz_text_get_next_line(const blz_text_t* text, const blz_text_node_t* current_line);
bool blz_text_merge(blz_text_node_t* a, blz_text_node_t** b);
bool blz_text_split(blz_text_node_t** node, int32_t offset);
void blz_text_draw(const ALLEGRO_FONT* font, const blz_text_t* text, blz_vec2f_t position, blz_sizef_t size, const blz_text_info_t* info);
void blz_text_draw_highlighted(const ALLEGRO_FONT* font, const blz_text_t* text, blz_vec2f_t position, blz_sizef_t size, const blz_text_info_t* info, const blz_text_highlighter_t* highlighter);


#endif // !_GUARD_BLAZE_TEXT_H_

