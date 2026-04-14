#include "blaze.h"
#include "sf_widget_console.h"
#include "sf_dialog_data.h"

static int32_t sf_widget_console_initialize(blz_widget_t* widget, sf_dialog_data_t* data)
{
	if (!widget || !data)
	{
		return -1;
	}

	data->m_console_data.m_text = blz_text_create_empty();

	if (!data->m_console_data.m_text)
	{
		return -1;
	}


	data->m_console_data.m_info.m_cursor_line = 0;
	data->m_console_data.m_info.m_cursor_offset = 0;
	data->m_console_data.m_info.m_top_line = 0;
	data->m_console_data.m_info.m_horizontal_padding = 5.0f;
	data->m_console_data.m_info.m_vertical_padding = 5.0f;
	data->m_console_data.m_info.m_line_spacing = 2.0f;
	data->m_console_data.m_info.m_color = al_map_rgb(0, 0, 0);
	data->m_editor_data.m_info.m_selection = (blz_text_selection_t){ { 0, 0}, { 0, 0 }, BLAZE_TEXT_SELECTION_TYPE_NONE };

	return 0;
}

static void sf_widget_console_uninitialize(blz_widget_t* widget, sf_dialog_data_t* data)
{
	if (!widget || !data)
	{
		return;
	}

	blz_text_destroy(data->m_console_data.m_text);
}

static void sf_widget_console_start(blz_widget_t* widget, sf_dialog_data_t* data)
{
	(void)widget;
	(void)data;
}

static void sf_widget_console_stop(blz_widget_t* widget, sf_dialog_data_t* data)
{
	(void)widget;
	(void)data;
}

static void sf_widget_console_update(blz_widget_t* widget, sf_dialog_data_t* data)
{
	(void)widget;
	(void)data;
}

static void sf_widget_console_render(const blz_widget_t* widget, const sf_dialog_data_t* data)
{
	blz_draw_frame_f(widget->m_position.m_x, widget->m_position.m_y, widget->m_size.m_width - 1.0f, widget->m_size.m_height - 1.0f, BLAZE_FRAME_TYPE_WORKSPACE, BLAZE_FRAME_BORDER_SUNKEN_ETCHED);
}

static bool sf_widget_console_on_event(blz_widget_t* widget, ALLEGRO_EVENT event, sf_dialog_data_t* data)
{
	(void)widget;
	(void)event;
	(void)data;

	return false;
}

const blz_widget_vtable_t* sf_widget_console_get_vtable(void)
{
	static const blz_widget_vtable_t table =
	{
		.m_initialize = (blz_widget_initialize_func_t)sf_widget_console_initialize,
		.m_uninitialize = (blz_widget_uninitialize_func_t)sf_widget_console_uninitialize,
		.m_start = (blz_widget_start_func_t)sf_widget_console_start,
		.m_stop = (blz_widget_stop_func_t)sf_widget_console_stop,
		.m_update = (blz_widget_update_func_t)sf_widget_console_update,
		.m_render = (blz_widget_render_func_t)sf_widget_console_render,
		.m_on_event = (blz_widget_on_event_func_t)sf_widget_console_on_event
	};

	return &table;
}

