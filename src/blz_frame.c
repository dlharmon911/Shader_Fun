#include "blaze/blz_frame.h"
#include "blaze/blz_color.h"
#include "blaze/blz_byte.h"
#include "blaze/blz_math.h"
#include "blaze/blz_color.h"

static const blz_frame_theme_t _BLAZE_DEFAULT_THEME =
{
	0xd6d3ceffU,
	0xffffffffU,
	0xffffffffU,
	0x828182ffU,
	0xffffffffU,
	0x424142ffU,
	0x000000ffU,
	0x828182ffU
};

static blz_frame_theme_t _BLAZE_FRAME_THEME =
{
	0xd6d3ceffU,
	0xffffffffU,
	0xffffffffU,
	0x828182ffU,
	0xffffffffU,
	0x424142ffU,
	0x000000ffU,
	0x828182ffU
};

void blz_frame_change_theme(const blz_frame_theme_t* theme)
{
	if (!theme)
	{
		memcpy(&_BLAZE_FRAME_THEME, &_BLAZE_DEFAULT_THEME, sizeof(blz_frame_theme_t));
		return;
	}

	memcpy(&_BLAZE_FRAME_THEME, theme, sizeof(blz_frame_theme_t));
}

const blz_frame_theme_t* blz_frame_get_default_theme(void)
{
	return &_BLAZE_DEFAULT_THEME;
}

const blz_frame_theme_t* blz_frame_get_current_theme(void)
{
	return &_BLAZE_FRAME_THEME;
}

uint32_t blz_frame_get_theme_color_index(size_t index)
{
	if (index >= BLAZE_FRAME_THEME_COLOR_COUNT)
	{
		return 0;
	}

	return ((const uint32_t*)&_BLAZE_FRAME_THEME)[index];
}

void blz_frame_set_theme_color_index(size_t index, uint32_t color)
{
	if (index >= BLAZE_FRAME_THEME_COLOR_COUNT)
	{
		return;
	}

	((uint32_t*)&_BLAZE_FRAME_THEME)[index] = color;
}

void blz_frame_set_theme_color(uint32_t workspace_color, uint32_t border_color)
{
	blz_frame_set_theme_color_index(BLAZE_FRAME_THEME_COLOR_WORKSPACE, workspace_color);
	blz_frame_set_theme_border_color(border_color);
}

void blz_frame_set_theme_border_color(uint32_t border_color)
{
	uint32_t carray[4] = { 0 };

	blz_color_get_gradient(border_color, carray);

	blz_frame_set_theme_color_index(BLAZE_FRAME_THEME_COLOR_BORDER_LIGHT, carray[0]);
	blz_frame_set_theme_color_index(BLAZE_FRAME_THEME_COLOR_BACKGROUND, carray[1]);
	blz_frame_set_theme_color_index(BLAZE_FRAME_THEME_COLOR_BORDER, carray[2]);
	blz_frame_set_theme_color_index(BLAZE_FRAME_THEME_COLOR_BORDER_DARK, carray[3]);
}

static void blz_draw_border_f(float x, float y, float w, float h, int32_t type)
{
	float x2 = x + w - 1.0f;
	float y2 = y + h - 1.0f;
	const blz_frame_theme_t* theme = blz_frame_get_current_theme();
	uint32_t border_light = ((const uint32_t*)theme)[BLAZE_FRAME_THEME_COLOR_BORDER_LIGHT];
	uint32_t border_normal = ((const uint32_t*)theme)[BLAZE_FRAME_THEME_COLOR_BORDER];
	uint32_t border_dark = ((const uint32_t*)theme)[BLAZE_FRAME_THEME_COLOR_BORDER_DARK];

	switch (type)
	{
	case BLAZE_FRAME_BORDER_NONE:
	{
		// No border to draw
	} break;
	case BLAZE_FRAME_BORDER_RAISED_BEVEL:
	{
		blz_draw_h_line_f(x, x2, y, border_light, 1.0f);
		blz_draw_v_line_f(x, y, y2, border_light, 1.0f);
		blz_draw_h_line_f(x, x2 - 1.0f, y + 1.0f, border_light, 1.0f);
		blz_draw_v_line_f(x + 1.0f, y + 1.0f, y2 - 1.0f, border_light, 1.0f);
		blz_draw_h_line_f(x + 1.0f, x2, y2 - 1.0f, border_normal, 1.0f);
		blz_draw_v_line_f(x2 - 1.0f, y + 1.0f, y2 - 1.0f, border_normal, 1.0f);
		blz_draw_h_line_f(x, x2 + 1.0f, y2, border_dark, 1.0f);
		blz_draw_v_line_f(x2, y, y2, border_dark, 1.0f);

	} break;
	case BLAZE_FRAME_BORDER_SUNKEN_BEVEL:
	{
		blz_draw_h_line_f(x, x2, y, border_normal, 1.0f);
		blz_draw_v_line_f(x, y, y2, border_normal, 1.0f);
		blz_draw_h_line_f(x + 1.0f, x2 - 1.0f, y + 1.0f, border_dark, 1.0f);
		blz_draw_v_line_f(x + 1.0f, y + 1.0f, y2 - 1.0f, border_dark, 1.0f);
		blz_draw_h_line_f(x + 1.0f, x2, y2 - 1.0f, border_light, 1.0f);
		blz_draw_v_line_f(x2 - 1.0f, y + 1.0f, y2 - 1.0f, border_light, 1.0f);
		blz_draw_h_line_f(x, x2 + 1.0f, y2, border_light, 1.0f);
		blz_draw_v_line_f(x2, y, y2, border_light, 1.0f);

	} break;
	case BLAZE_FRAME_BORDER_RAISED_ETCHED:
	{
		blz_draw_h_line_f(x, x2 - 1.0f, y + 1.0f, border_dark, 1.0f);
		blz_draw_v_line_f(x + 1.0f, y, y2 - 1.0f, border_dark, 1.0f);
		blz_draw_v_line_f(x2 - 1.0f, y, y2, border_dark, 1.0f);
		blz_draw_h_line_f(x, x2, y2, border_dark, 1.0f);
		blz_draw_box_f(x, y, x2 - 2.0f, y2 - 1.0f, border_light, 1.0f);
	} break;
	case BLAZE_FRAME_BORDER_SUNKEN_ETCHED:
	{
		blz_draw_h_line_f(x, x2, y, border_dark, 1.0f);
		blz_draw_v_line_f(x, y + 1.0f, y2, border_dark, 1.0f);
		blz_draw_v_line_f(x2 - 1.0f, y, y2 - 1.0f, border_dark, 1.0f);
		blz_draw_h_line_f(x + 2.0f, x2 - 1.0f, y2 - 1.0f, border_dark, 1.0f);
		blz_draw_box_f(x + 1.0f, y + 1.0f, x2, y2, border_light, 1.0f);
	} break;
	case BLAZE_FRAME_BORDER_LINE_NORMAL:
	{
		blz_draw_box_f(x, y, x2, y2, border_normal, 1.0f);
		blz_draw_box_f(x + 1.0f, y + 1.0f, x2 - 1.0f, y2 - 1.0f, border_dark, 1.0f);
	} break;
	case BLAZE_FRAME_BORDER_LINE_DARK:
	{
		blz_draw_box_f(x, y, x2, y2, border_dark, 1.0f);
		blz_draw_box_f(x + 1.0f, y + 1.0f, x2 - 1.0f, y2 - 1.0f, border_dark, 1.0f);
	} break;
	default: break;
	}
}

void blz_draw_frame_f(float x, float y, float w, float h, int32_t frame_type, int32_t border_type)
{
	if (frame_type > BLAZE_FRAME_TYPE_NONE && frame_type < BLAZE_FRAME_TYPE_COUNT)
	{
		const blz_frame_theme_t* theme = blz_frame_get_current_theme();
		uint32_t background = ((const uint32_t*)theme)[BLAZE_FRAME_THEME_COLOR_BACKGROUND];

		if (frame_type == BLAZE_FRAME_TYPE_WORKSPACE)
		{
			background = ((const uint32_t*)theme)[BLAZE_FRAME_THEME_COLOR_WORKSPACE];
		}

		blz_draw_filled_rectangle_f(x, y, w, h, background);
	}

	if (border_type > BLAZE_FRAME_BORDER_NONE && border_type < BLAZE_FRAME_BORDER_COUNT)
	{
		blz_draw_border_f(x, y, w, h, border_type);
	}
}

void blz_draw_h_line_f(float x1, float x2, float y, uint32_t color, float thickness)
{
	if (blz_math_is_equal_f(x1, x2))
	{
		return;
	}

	if (x2 < x1)
	{
		blz_byte_swap_array(&x1, &x2, sizeof(float));
	}

	al_draw_line(x1 + 0.5f, y + 0.5f, x2 + 0.5f, y + 0.5f, blz_color_map_rgba_to_acolor(color), thickness);
}

void blz_draw_v_line_f(float x, float y1, float y2, uint32_t color, float thickness)
{
	if (blz_math_is_equal_f(y1, y2))
	{
		return;
	}

	if (y2 < y1)
	{
		blz_byte_swap_array(&y1, &y2, sizeof(float));
	}

	al_draw_line(x + 0.5f, y1 + 0.5f, x + 0.5f, y2 + 0.5f, blz_color_map_rgba_to_acolor(color), thickness);
}

void blz_draw_line_f(float x1, float y1, float x2, float y2, uint32_t color, float thickness)
{
	if (!blz_math_is_equal_f(x1, x2) && !blz_math_is_equal_f(y1, y2))
	{
		if (x2 < x1)
		{
			blz_byte_swap_array(&x1, &x2, sizeof(float));
		}

		if (y2 < y1)
		{
			blz_byte_swap_array(&y1, &y2, sizeof(float));
		}


		al_draw_line(x1 + 0.5f, y1 + 0.5f, x2 + 1.5f, y2 + 1.5f, blz_color_map_rgba_to_acolor(color), thickness);
		return;
	}

	if (blz_math_is_equal_f(x1, x2))
	{
		blz_draw_v_line_f(x1, y1, y2, color, thickness);
		return;
	}

	blz_draw_h_line_f(x1, x2, y1, color, thickness);
}

void blz_draw_rectangle_f(float x, float y, float width, float height, uint32_t color, float thickness)
{
	blz_draw_box_f(x, y, x + width - 1.0f, y + height - 1.0f, color, thickness);
}

void blz_draw_filled_rectangle_f(float x, float y, float width, float height, uint32_t color)
{
	blz_draw_filled_box_f(x, y, x + width - 1.0f, y + height - 1.0f, color);
}

void blz_draw_box_f(float x1, float y1, float x2, float y2, uint32_t color, float thickness)
{
	if (blz_math_is_equal_f(x1, x2) || blz_math_is_equal_f(y1, y2))
	{
		return;
	}

	if (x2 < x1)
	{
		blz_byte_swap_array(&x1, &x2, sizeof(float));
	}

	if (y2 < y1)
	{
		blz_byte_swap_array(&y1, &y2, sizeof(float));
	}

	al_draw_rectangle(x1 + 0.5f, y1 + 0.5f, x2 + 0.5f, y2 + 0.5f, blz_color_map_rgba_to_acolor(color), thickness);
}

void blz_draw_filled_box_f(float x1, float y1, float x2, float y2, uint32_t color)
{
	if (blz_math_is_equal_f(x1, x2) || blz_math_is_equal_f(y1, y2))
	{
		return;
	}

	if (x2 < x1)
	{
		blz_byte_swap_array(&x1, &x2, sizeof(float));
	}

	if (y2 < y1)
	{
		blz_byte_swap_array(&y1, &y2, sizeof(float));
	}

	al_draw_filled_rectangle(x1 + 0.5f, y1 + 0.5f, x2 + 1.5f, y2 + 1.5f, blz_color_map_rgba_to_acolor(color));
}

void blz_draw_rectangle(blz_rectangle_t rect, uint32_t color, float thickness)
{
	blz_draw_rectangle_f(rect.m_position.m_x, rect.m_position.m_y, rect.m_size.m_width, rect.m_size.m_height, color, thickness);
}

void blz_draw_filled_rectangle(blz_rectangle_t rect, uint32_t color)
{
	blz_draw_filled_rectangle_f(rect.m_position.m_x, rect.m_position.m_y, rect.m_size.m_width, rect.m_size.m_height, color);
}

void blz_draw_box(blz_box_t box, uint32_t color, float thickness)
{
	blz_draw_box_f(box.m_top_left.m_x, box.m_top_left.m_y, box.m_bottom_right.m_x, box.m_bottom_right.m_y, color, thickness);
}

void blz_draw_filled_box(blz_box_t box, uint32_t color)
{
	blz_draw_filled_box_f(box.m_top_left.m_x, box.m_top_left.m_y, box.m_bottom_right.m_x, box.m_bottom_right.m_y, color);
}
