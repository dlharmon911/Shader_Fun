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

blz_text_t* blz_text_create_empty();
blz_text_t* blz_text_create(const char* text);
void blz_text_destroy(blz_text_t* text);

blz_text_t* blz_text_clone(const blz_text_t* text);
blz_text_t* blz_text_load_from_file(const char* filename);
bool blz_text_save_to_file(const blz_text_t* text, const char* filename);

size_t blz_text_get_line_count(const blz_text_t* text);
blz_text_node_t* blz_text_get_line(const blz_text_t* text, size_t index);
ALLEGRO_USTR* blz_text_to_ustr(const blz_text_t* text, const char* new_line);
char* blz_text_to_cstr(const blz_text_t* text, const char* new_line);
int32_t blz_text_select_all(const blz_text_t* text, blz_text_info_t* info);
blz_text_node_t* blz_text_create_node();
void blz_text_node_destroy(blz_text_node_t* node);
blz_text_node_t* blz_text_get_previous_line(const blz_text_t* text, const blz_text_node_t* current_line);
blz_text_node_t* blz_text_get_next_line(const blz_text_t* text, const blz_text_node_t* current_line);
bool blz_text_merge(blz_text_node_t* a, blz_text_node_t** b);
bool blz_text_split(blz_text_node_t** node, int32_t offset);
void blz_text_draw(const ALLEGRO_FONT* font, const blz_text_t* text, blz_vec2f_t position, blz_sizef_t size, const blz_text_info_t* info);
void blz_text_draw_highlighted(const ALLEGRO_FONT* font, const blz_text_t* text, blz_vec2f_t position, blz_sizef_t size, const blz_text_info_t* info, const blz_text_highlighter_t* highlighter);


int32_t blz_text_cut_to_clipboard(blz_text_t* text, blz_text_info_t* info);
int32_t blz_text_copy_to_clipboard(const blz_text_t* text, const blz_text_info_t* info);

enum BLAZE_TEXT_EDIT_OPTIONS
{
	BLAZE_TEXT_EDIT_OPTION_NONE,
	BLAZE_TEXT_EDIT_OPTION_EXCISE_SELECTION,
	BLAZE_TEXT_EDIT_OPTION_EXCISE_CHAR,
	BLAZE_TEXT_EDIT_OPTION_PASTE_CLIPBOARD,
	BLAZE_TEXT_EDIT_OPTION_INSERT_CHAR,
	BLAZE_TEXT_EDIT_OPTION_INSERT_TEXT,
	BLAZE_TEXT_EDIT_OPTION_SPLIT_LINE,
	BLAZE_TEXT_EDIT_OPTION_MERGE_LINE,
	BLAZE_TEXT_EDIT_OPTION_COUNT
};

int32_t blz_text_edit(blz_text_t* text, blz_text_info_t* info, int32_t option, const void* value);



#endif // !_GUARD_BLAZE_TEXT_H_

