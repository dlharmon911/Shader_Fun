#ifndef _GUARD_BLAZE_FRAME_H
#define _GUARD_BLAZE_FRAME_H

#include "blaze/blz_common.h"
#include "blaze/blz_shapes.h"

enum BLAZE_FRAME_BORDER
{
	BLAZE_FRAME_BORDER_NONE = -1,
	BLAZE_FRAME_BORDER_RAISED_BEVEL,
	BLAZE_FRAME_BORDER_SUNKEN_BEVEL,
	BLAZE_FRAME_BORDER_RAISED_ETCHED,
	BLAZE_FRAME_BORDER_SUNKEN_ETCHED,
	BLAZE_FRAME_BORDER_LINE_NORMAL,
	BLAZE_FRAME_BORDER_LINE_DARK,
	BLAZE_FRAME_BORDER_COUNT
};

enum BLAZE_FRAME_TYPE
{
	BLAZE_FRAME_TYPE_NONE = -1,
	BLAZE_FRAME_TYPE_NORMAL,
	BLAZE_FRAME_TYPE_WORKSPACE,
	BLAZE_FRAME_TYPE_COUNT
};

enum BLAZE_FRAME_THEME_COLOR
{
	BLAZE_FRAME_THEME_COLOR_BACKGROUND,
	BLAZE_FRAME_THEME_COLOR_FOREGROUND,
	BLAZE_FRAME_THEME_COLOR_WORKSPACE,
	BLAZE_FRAME_THEME_COLOR_BORDER,
	BLAZE_FRAME_THEME_COLOR_BORDER_LIGHT,
	BLAZE_FRAME_THEME_COLOR_BORDER_DARK,
	BLAZE_FRAME_THEME_COLOR_TEXT,
	BLAZE_FRAME_THEME_COLOR_TEXT_SHADOW,
	BLAZE_FRAME_THEME_COLOR_COUNT
};

typedef struct blz_frame_theme_tag_t
{
	uint32_t m_background;
	uint32_t m_foreground;
	uint32_t m_workspace;
	uint32_t m_border;
	uint32_t m_border_light;
	uint32_t m_border_dark;
	uint32_t m_text;
	uint32_t m_text_shadow;
} blz_frame_theme_t;

void blz_frame_change_theme(const blz_frame_theme_t* theme);
const blz_frame_theme_t* blz_frame_get_default_theme(void);
const blz_frame_theme_t* blz_frame_get_current_theme(void);
uint32_t blz_frame_get_theme_color_index(size_t index);
void blz_frame_set_theme_color_index(size_t index, uint32_t color);
void blz_frame_set_theme_color(uint32_t workspace_color, uint32_t border_color);
void blz_frame_set_theme_border_color(uint32_t border_color);

void blz_draw_frame_f(float x, float y, float w, float h, int32_t frame_type, int32_t border_type);

void blz_draw_h_line_f(float x1, float x2, float y, uint32_t color, float thickness);
void blz_draw_v_line_f(float x, float y1, float y2, uint32_t color, float thickness);
void blz_draw_line_f(float x1, float y1, float x2, float y2, uint32_t color, float thickness);
void blz_draw_rectangle_f(float x, float y, float width, float height, uint32_t color, float thickness);
void blz_draw_filled_rectangle_f(float x, float y, float width, float height, uint32_t color);
void blz_draw_box_f(float x1, float y1, float x2, float y2, uint32_t color, float thickness);
void blz_draw_filled_box_f(float x1, float y1, float x2, float y2, uint32_t color);

void blz_draw_rectangle(blz_rectangle_t rect, uint32_t color, float thickness);
void blz_draw_filled_rectangle(blz_rectangle_t rect, uint32_t color);
void blz_draw_box(blz_box_t box, uint32_t color, float thickness);
void blz_draw_filled_box(blz_box_t box, uint32_t color);

#endif // !_GUARD_BLAZE_FRAME_H
