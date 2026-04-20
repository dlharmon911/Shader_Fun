#include "blaze.h"
#include "sf_widget_editor.h"
#include "sf_dialog_data.h"
#include "sf_text_highlighter.h"
#include "sf_shader_snippets.h"


static int32_t sf_widget_editor_initialize(blz_widget_t* widget, sf_dialog_data_t* data)
{
    if (!widget || !data)
    {
        return -1;
    }

	const char* bubble_code = sf_get_shader_snippet(SF_SHADER_SNIPPET_ID_BUBBLE);

    data->m_editor_data.m_text = blz_text_create(bubble_code);
    
    if (!data->m_editor_data.m_text)
    {
        return -1;
    }   
    
    data->m_editor_data.m_info.m_cursor.m_line = 0;
    data->m_editor_data.m_info.m_cursor.m_offset = 0;
    data->m_editor_data.m_info.m_top_line = 0;
    data->m_editor_data.m_info.m_horizontal_padding = 5.0f;
    data->m_editor_data.m_info.m_vertical_padding = 5.0f;
    data->m_editor_data.m_info.m_line_spacing = 2.0f;
    data->m_editor_data.m_info.m_color = al_map_rgb(0, 0, 0);
    data->m_editor_data.m_info.m_selection = (blz_text_selection_t){ { 0, 0}, { 0, 0 }, BLAZE_TEXT_SELECTION_TYPE_NONE };
    

    return 0;
}

static void sf_widget_editor_uninitialize(blz_widget_t* widget, sf_dialog_data_t* data)
{
    if (!widget || !data)
    {
        return;
    }

    blz_text_destroy(data->m_editor_data.m_text);
}

static void sf_widget_editor_start(blz_widget_t* widget, sf_dialog_data_t* data)
{
    (void)widget;
    (void)data;
}

static void sf_widget_editor_stop(blz_widget_t* widget, sf_dialog_data_t* data)
{
    (void)widget;
    (void)data;
}

static void sf_widget_editor_update(blz_widget_t* widget, sf_dialog_data_t* data)
{
    (void)widget;
    (void)data;
}

static void sf_widget_editor_render(const blz_widget_t* widget, const sf_dialog_data_t* data)
{
    const blz_font_t* font = &data->m_fonts[BLAZE_FONT_ID_EDITOR];
    const blz_text_t* text_data = data->m_editor_data.m_text;
    blz_vec2f_t position = { widget->m_position.m_x - data->m_editor_data.m_text_x_offset, widget->m_position.m_y };

    blz_draw_frame_f(widget->m_position.m_x, widget->m_position.m_y, widget->m_size.m_width - 1.0f, widget->m_size.m_height - 1.0f, BLAZE_FRAME_TYPE_WORKSPACE, BLAZE_FRAME_BORDER_SUNKEN_ETCHED);

    blz_text_draw_highlighted(font, text_data, position, widget->m_size, &data->m_editor_data.m_info, sf_text_highlighter_get());
}

static void sf_widget_editor_scroll_to_cursor(const blz_widget_t* widget, sf_dialog_data_t* data)
{
    // Vertical scrolling
    const blz_text_node_t* current_line = blz_text_get_line(data->m_editor_data.m_text, data->m_editor_data.m_info.m_cursor.m_line);
    if (!current_line)
    {
        return;
    }
    float line_height = (float)al_get_font_line_height(data->m_fonts[BLAZE_FONT_ID_EDITOR].m_font) + data->m_editor_data.m_info.m_line_spacing;
    float cursor_y = widget->m_position.m_y + data->m_editor_data.m_info.m_vertical_padding + (line_height * (float)data->m_editor_data.m_info.m_cursor.m_line) - (line_height * (float)data->m_editor_data.m_info.m_top_line);


    if (cursor_y < widget->m_position.m_y + data->m_editor_data.m_info.m_vertical_padding)
    {
        data->m_editor_data.m_info.m_top_line = data->m_editor_data.m_info.m_cursor.m_line;
    }
    else if (cursor_y > widget->m_position.m_y + widget->m_size.m_height - data->m_editor_data.m_info.m_vertical_padding - line_height)
    {
        data->m_editor_data.m_info.m_top_line = data->m_editor_data.m_info.m_cursor.m_line - (int32_t)(widget->m_size.m_height / line_height) + 1;
    }

    // Horizontal scrolling

    float cursor_x = widget->m_position.m_x + data->m_editor_data.m_info.m_horizontal_padding + (float)al_get_text_width(data->m_fonts[BLAZE_FONT_ID_EDITOR].m_font, "W") * (float)data->m_editor_data.m_info.m_cursor.m_offset - data->m_editor_data.m_text_x_offset;

    if (cursor_x < widget->m_position.m_x + data->m_editor_data.m_info.m_horizontal_padding)
    {
        data->m_editor_data.m_text_x_offset -= (widget->m_position.m_x + data->m_editor_data.m_info.m_horizontal_padding) - cursor_x;
    }
    else if (cursor_x > widget->m_position.m_x + widget->m_size.m_width - data->m_editor_data.m_info.m_horizontal_padding)
    {
        data->m_editor_data.m_text_x_offset += cursor_x - (widget->m_position.m_x + widget->m_size.m_width - data->m_editor_data.m_info.m_horizontal_padding);
    }
}

static bool sf_widget_editor_on_event(blz_widget_t* widget, ALLEGRO_EVENT event, sf_dialog_data_t* data)
{
    if (!widget || !data)
    {
        return false;
    }

    switch (event.type)
    {
        case ALLEGRO_EVENT_KEY_CHAR:
        {
            if (event.keyboard.modifiers & ALLEGRO_KEYMOD_CTRL)
            {
                if (event.keyboard.keycode >= ALLEGRO_KEY_A &&
                    event.keyboard.keycode <= ALLEGRO_KEY_Z &&
                    sf_widget_editor_control_key_func(widget, &event, data))
                {
                    sf_widget_editor_scroll_to_cursor(widget, data);
                    return true;
                }
            }
            else
            {
                if (sf_widget_editor_key_func(widget, &event, data))
                {
                    sf_widget_editor_scroll_to_cursor(widget, data);
                    return true;
                }
            }
        } break;
        case ALLEGRO_EVENT_MOUSE_BUTTON_DOWN:
        case ALLEGRO_EVENT_MOUSE_BUTTON_UP:
        case ALLEGRO_EVENT_MOUSE_AXES:
        {
			int32_t current_cursor_line = data->m_editor_data.m_info.m_cursor.m_line;
			int32_t current_cursor_offset = data->m_editor_data.m_info.m_cursor.m_offset;

            if (sf_widget_editor_mouse_func(widget, &event, data))
            {
                if (current_cursor_line != data->m_editor_data.m_info.m_cursor.m_line ||
                    current_cursor_offset != data->m_editor_data.m_info.m_cursor.m_offset)
                {
                    sf_widget_editor_scroll_to_cursor(widget, data);
                }

                return true;
			}


        } break;        
        default: break;
	}

    return false;
}

const blz_widget_vtable_t* sf_widget_editor_get_vtable(void)
{
    static const blz_widget_vtable_t table =
    {
        .m_initialize = (blz_widget_initialize_func_t)sf_widget_editor_initialize,
        .m_uninitialize = (blz_widget_uninitialize_func_t)sf_widget_editor_uninitialize,
        .m_start = (blz_widget_start_func_t)sf_widget_editor_start,
        .m_stop = (blz_widget_stop_func_t)sf_widget_editor_stop,
        .m_update = (blz_widget_update_func_t)sf_widget_editor_update,
        .m_render = (blz_widget_render_func_t)sf_widget_editor_render,
        .m_on_event = (blz_widget_on_event_func_t)sf_widget_editor_on_event
    };

    return &table;
}

