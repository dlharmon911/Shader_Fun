#include "blaze.h"
#include "sf_widget_editor.h"
#include "sf_dialog_data.h"

static void sf_set_line_selection_point(const blz_widget_t* widget, const sf_dialog_data_t* data, ALLEGRO_EVENT event, blz_text_selection_point_t* point)
{
    // set cursor position based on mouse click

    event.mouse.x = max(0, event.mouse.x);

    int32_t line_count = (int32_t)blz_text_get_line_count(data->m_editor_data.m_text);

	if (line_count == 0)
	{
		point->m_line = 0;
		point->m_offset = 0;
		return;
	}

	int32_t mouse_line_pos = (int32_t)(((float)event.mouse.y - widget->m_position.m_y - data->m_editor_data.m_info.m_vertical_padding) / ((float)al_get_font_line_height(data->m_fonts[SF_FONT_ID_EDITOR_REGULAR].m_font) + data->m_editor_data.m_info.m_line_spacing)) + data->m_editor_data.m_info.m_top_line;
	int32_t mouse_offset_pos = (int32_t)(((float) event.mouse.x - widget->m_position.m_x - data->m_editor_data.m_info.m_horizontal_padding + data->m_editor_data.m_text_x_offset) / (float)al_get_text_width(data->m_fonts[SF_FONT_ID_EDITOR_REGULAR].m_font, "W"));

    mouse_line_pos = max(0, min(mouse_line_pos, line_count - 1));

    const blz_text_node_t* line = blz_text_get_line(data->m_editor_data.m_text, mouse_line_pos);

    mouse_offset_pos = max(0, min(mouse_offset_pos, (int32_t)al_ustr_size(line->m_text)));

    point->m_line = mouse_line_pos;
    point->m_offset = mouse_offset_pos;
}

static bool sf_widget_editor_mouse_up(blz_widget_t* widget, ALLEGRO_EVENT event, sf_dialog_data_t* data)
{
    if (data->m_editor_data.m_info.m_selection.m_type == BLAZE_TEXT_SELECTION_TYPE_START_GRABBED)
    {
        sf_set_line_selection_point(widget, data, event, &data->m_editor_data.m_info.m_selection.m_end);
		data->m_editor_data.m_info.m_selection.m_type = BLAZE_TEXT_SELECTION_TYPE_SELECTED;
	}

    return true;
}

static bool sf_widget_editor_mouse_down(blz_widget_t* widget, ALLEGRO_EVENT event, sf_dialog_data_t* data)
{
    sf_set_line_selection_point(widget, data, event, &data->m_editor_data.m_info.m_selection.m_start);
    data->m_editor_data.m_info.m_selection.m_end.m_line = data->m_editor_data.m_info.m_selection.m_start.m_line;
    data->m_editor_data.m_info.m_selection.m_end.m_offset = data->m_editor_data.m_info.m_selection.m_start.m_offset;
    data->m_editor_data.m_info.m_cursor_line = data->m_editor_data.m_info.m_selection.m_end.m_line;
    data->m_editor_data.m_info.m_cursor_offset = data->m_editor_data.m_info.m_selection.m_end.m_offset;
    data->m_editor_data.m_info.m_selection.m_type = BLAZE_TEXT_SELECTION_TYPE_START_GRABBED;

	return true;
}

static bool sf_widget_editor_mouse_axes(blz_widget_t* widget, ALLEGRO_EVENT event, sf_dialog_data_t* data)
{
    if (data->m_editor_data.m_info.m_selection.m_type == BLAZE_TEXT_SELECTION_TYPE_START_GRABBED)
    {
        sf_set_line_selection_point(widget, data, event, &data->m_editor_data.m_info.m_selection.m_end);

        data->m_editor_data.m_info.m_cursor_line = data->m_editor_data.m_info.m_selection.m_end.m_line;
        data->m_editor_data.m_info.m_cursor_offset = data->m_editor_data.m_info.m_selection.m_end.m_offset;
    }

    if (event.mouse.dz)
    {
        if (event.mouse.dz > 0)
        {
            data->m_editor_data.m_info.m_top_line = max(0, data->m_editor_data.m_info.m_top_line - 1);
        }
        else
        {
            int32_t line_count = (int32_t)blz_text_get_line_count(data->m_editor_data.m_text);
            data->m_editor_data.m_info.m_top_line = min(line_count - 1, data->m_editor_data.m_info.m_top_line + 1);
		}
    }

    return true;
}

typedef bool (*sf_widget_editor_process_mouse_func_t)(blz_widget_t* widget, ALLEGRO_EVENT event, sf_dialog_data_t* data);

static const sf_widget_editor_process_mouse_func_t sf_widget_editor_process_mouse_funcs[] =
{
	[ALLEGRO_EVENT_MOUSE_BUTTON_UP] = sf_widget_editor_mouse_up,
	[ALLEGRO_EVENT_MOUSE_BUTTON_DOWN] = sf_widget_editor_mouse_down,
	[ALLEGRO_EVENT_MOUSE_AXES] = sf_widget_editor_mouse_axes
};

bool sf_widget_editor_mouse_func(blz_widget_t* widget, const ALLEGRO_EVENT* event, void* data)
{
    if (!widget || !event || !data)
    {
        return false;
	}

	const sf_widget_editor_process_mouse_func_t func = sf_widget_editor_process_mouse_funcs[event->type];

    if (func && func(widget, *event, (sf_dialog_data_t*)data))
    {
        return true;
    }

    return false;
}
