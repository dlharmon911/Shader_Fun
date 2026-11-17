#ifndef _GUARD_BLAZE_TEXT_H_
#define _GUARD_BLAZE_TEXT_H_

#include "blaze/blz_common.h"
#include "blaze/blz_stringview.h"
#include "blaze/blz_font.h"
#include "blaze/blz_shapes.h"
#include "blaze/blz_darray.h"

enum BLAZE_TEXT_SELECTION_TYPE
{
	BLAZE_TEXT_SELECTION_TYPE_NONE,
	BLAZE_TEXT_SELECTION_TYPE_START_GRABBED,
	BLAZE_TEXT_SELECTION_TYPE_SELECTED
};

static const int32_t BLAZE_MAX_UNDO_STACK_SIZE = 100;

enum BLAZE_TEXT_EDIT_OPTIONS
{
	BLAZE_TEXT_EDIT_OPTION_NONE,
	BLAZE_TEXT_EDIT_OPTION_EXCISE_SELECTION,
	BLAZE_TEXT_EDIT_OPTION_EXCISE_CHAR,
	BLAZE_TEXT_EDIT_OPTION_PASTE_CLIPBOARD,
	BLAZE_TEXT_EDIT_OPTION_INSERT_CHAR,
	BLAZE_TEXT_EDIT_OPTION_SPLIT_LINE,
	BLAZE_TEXT_EDIT_OPTION_MERGE_LINE,
	BLAZE_TEXT_EDIT_OPTION_COUNT
};

#define BLAZE_NEWLINE_STR "\n"
#define BLAZE_TAB_STR "\t"

static const int32_t BLAZE_TAB_CHAR = '\t';
static const int32_t BLAZE_NEWLINE_CHAR = '\n';
static const int32_t BLAZE_ESCAPE_CHAR = '\\';
static const int32_t BLAZE_ESCAPE_CHAR_SUFFIX_TAB = 't';
static const int32_t BLAZE_ESCAPE_CHAR_SUFFIX_NEWLINE = 'n';

typedef struct blz_text_node_tag_t
{
	ALLEGRO_USTR* m_text;
	struct blz_text_node_tag_t* m_next;
} blz_text_node_t;

typedef struct blz_text_tag_t
{
	blz_text_node_t* m_head;
} blz_text_t;

typedef struct blz_cursor_tag_t
{
	int32_t m_line;
	int32_t m_offset;
} blz_cursor_t;

typedef struct blz_text_selection_tag_t
{
	blz_cursor_t m_start;
	blz_cursor_t m_end;
	int32_t m_type;
} blz_text_selection_t;

typedef struct blz_text_info_tag_t blz_text_info_t;

typedef void (*blz_text_highlight_begin_func_t)(const blz_font_t* font, const blz_text_t* text, blz_vec2f_t position, blz_sizef_t size, const blz_text_info_t* info, void* user_data);
typedef void (*blz_text_highlight_finish_func_t)(const blz_font_t* font, const blz_text_t* text, blz_vec2f_t position, blz_sizef_t size, const blz_text_info_t* info, void* user_data);
typedef void (*blz_text_highlight_per_line_func_t)(const blz_font_t* font, blz_stringview_t line, blz_vec2f_t position, const blz_text_info_t* info, void* user_data);

typedef struct blz_text_highlighter_tag_t
{
	blz_text_highlight_begin_func_t m_begin;
	blz_text_highlight_per_line_func_t m_per_line;
	blz_text_highlight_finish_func_t m_finish;
	void* m_user_data;
} blz_text_highlighter_t;


typedef struct blz_text_undo_entry_tag_t
{
	int32_t m_type;
	blz_cursor_t m_cursor;
	union
	{
		int32_t m_char;
		char* m_clipboard_data;
		blz_text_t* m_text;
		blz_text_selection_t m_selection;
	};
	struct blz_text_undo_entry_tag_t* m_sibling;
} blz_text_undo_entry_t;

typedef struct blz_text_undo_tag_t
{
	blz_text_undo_entry_t* m_entries;
	size_t m_current_index;
} blz_text_undo_t;

blz_text_undo_t* blz_text_undo_create();
void blz_text_undo_destroy(blz_text_undo_t* undo);
size_t blz_text_undo_size(const blz_text_undo_t* undo);
blz_text_undo_entry_t* blz_text_undo_create_entry(int32_t type, blz_cursor_t cursor, int32_t c);
void blz_text_undo_push(blz_text_undo_t* undo, blz_text_undo_entry_t* entry);
void blz_text_undo_pop(blz_text_undo_t* undo);

typedef struct blz_text_info_tag_t
{
	blz_text_selection_t m_selection;
	const blz_text_highlighter_t* m_highlighter;
	blz_text_undo_t* m_undo;
	int32_t m_top_line;
	blz_cursor_t m_cursor;
	float m_horizontal_padding;
	float m_vertical_padding;
	float m_line_spacing;
	ALLEGRO_COLOR m_color;
} blz_text_info_t;

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
void blz_text_draw(const blz_font_t* font, const blz_text_t* text, blz_vec2f_t position, blz_sizef_t size, const blz_text_info_t* info);
void blz_text_draw_line_tab_delimited(const blz_font_t* font, blz_stringview_t text, blz_vec2f_t position, int32_t* offset, const ALLEGRO_COLOR color);
int32_t blz_text_cut_to_clipboard(blz_text_t* text, blz_text_info_t* info);
int32_t blz_text_copy_to_clipboard(const blz_text_t* text, const blz_text_info_t* info);
int32_t blz_text_edit(int32_t option, blz_text_t* text, blz_text_info_t* info, int32_t c);
int32_t blz_text_calculate_tabbed_offset(blz_stringview_t line, int32_t offset);
int32_t blz_text_calculate_untabbed_offset(blz_stringview_t line, int32_t tabbed_offset);

#endif // !_GUARD_BLAZE_TEXT_H_

