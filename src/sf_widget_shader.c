#include "blaze.h"
#include "sf_widget_shader.h"
#include "sf_dialog_data.h"
#include "sf_shader.h"

static int32_t sf_widget_shader_initialize(blz_widget_t* widget, sf_dialog_data_t* data)
{
	if (!widget || !data)
	{
		return -1;
	}

	sf_widget_shader_data_t* shader_data = &data->m_shader_data;

	return 0;
}

static void sf_widget_shader_uninitialize(blz_widget_t* widget, sf_dialog_data_t* data)
{
	if (!widget || !data)
	{
		return;
	}

	sf_widget_shader_data_t* shader_data = &data->m_shader_data;

	if (shader_data->m_shader)
	{
		al_destroy_shader(shader_data->m_shader);
		shader_data->m_shader = NULL;
	}
}

static void sf_widget_shader_start(blz_widget_t* widget, sf_dialog_data_t* data)
{
	(void)widget;
	(void)data;

	data->m_time_start = (float)al_get_time();

}

static void sf_widget_shader_stop(blz_widget_t* widget, sf_dialog_data_t* data)
{
	(void)widget;
	(void)data;
}

static void sf_widget_shader_update(blz_widget_t* widget, sf_dialog_data_t* data)
{
	if (!widget || !data)
	{
		return;
	}

	data->m_time_current = (float)al_get_time();

	sf_uniform_t* time_uniform = sf_uniform_get(data->m_uniform_data.m_uniforms, "u_time");

	sf_uniform_set_float(time_uniform, data->m_time_current - data->m_time_start);
}

static void sf_widget_shader_render(const blz_widget_t* widget, const sf_dialog_data_t* data)
{
	if (!widget || !data)
	{
		return;
	}

	if (data->m_shader_data.m_shader)
	{
		sf_shader_render(data->m_shader_data.m_shader, data->m_uniform_data.m_uniforms, widget->m_position, widget->m_size);
		blz_draw_frame_f(widget->m_position.m_x, widget->m_position.m_y, widget->m_size.m_width, widget->m_size.m_height, BLAZE_FRAME_TYPE_NONE, BLAZE_FRAME_BORDER_SUNKEN_ETCHED);
	}
	else
	{
		float line_padding = 10.0f;
		float line_thickness = 6.0f;

		al_draw_filled_rectangle(widget->m_position.m_x, widget->m_position.m_y, widget->m_position.m_x + widget->m_size.m_width, widget->m_position.m_y + widget->m_size.m_height, (ALLEGRO_COLOR) { 0.8f, 0.0f, 0.0f, 1.0f });
		al_draw_line(widget->m_position.m_x + line_padding, widget->m_position.m_y + line_padding, widget->m_position.m_x + widget->m_size.m_width - line_padding, widget->m_position.m_y + widget->m_size.m_height - line_padding, (ALLEGRO_COLOR) { 0.0f, 0.0f, 0.0f, 1.0f }, line_thickness);
		al_draw_line(widget->m_position.m_x + line_padding, widget->m_position.m_y + widget->m_size.m_height - line_padding, widget->m_position.m_x + widget->m_size.m_width - line_padding, widget->m_position.m_y + line_padding, (ALLEGRO_COLOR) { 0.0f, 0.0f, 0.0f, 1.0f }, line_thickness);
	}
}

static bool sf_widget_shader_on_event(blz_widget_t* widget, ALLEGRO_EVENT event, sf_dialog_data_t* data)
{
	(void)widget;
	(void)event;
	(void)data;

	return false;
}

const blz_widget_vtable_t* sf_widget_shader_get_vtable(void)
{
	static const blz_widget_vtable_t table =
	{
		.m_initialize = (blz_widget_initialize_func_t)sf_widget_shader_initialize,
		.m_uninitialize = (blz_widget_uninitialize_func_t)sf_widget_shader_uninitialize,
		.m_start = (blz_widget_start_func_t)sf_widget_shader_start,
		.m_stop = (blz_widget_stop_func_t)sf_widget_shader_stop,
		.m_update = (blz_widget_update_func_t)sf_widget_shader_update,
		.m_render = (blz_widget_render_func_t)sf_widget_shader_render,
		.m_on_event = (blz_widget_on_event_func_t)sf_widget_shader_on_event
	};

	return &table;
}

int32_t sf_widget_shader_build(const blz_text_t* text, const sf_uniform_t* uniforms, sf_widget_shader_data_t* data)
{
	if (!text || !uniforms || !data)
	{
		return -1;
	}

	ALLEGRO_USTR* shader_code = blz_text_to_ustr(text, "\n");

	if (!shader_code)
	{
		DO_LOG("ERROR: Failed to convert shader code to ustr");
		return -1;
	}

	data->m_shader = sf_shader_generate(shader_code, uniforms);
	if (!data->m_shader)
	{
		DO_LOG("ERROR: Failed to create shader");
		al_ustr_free(shader_code);
		return -1;
	}

	al_ustr_free(shader_code);
	return 0;
}

int32_t sf_widget_shader_rebuild(const blz_text_t* text, const sf_uniform_t* uniforms, sf_widget_shader_data_t* data)
{
	if (!text || !uniforms || !data)
	{
		return -1;
	}
	
	if (data->m_shader)
	{
		al_destroy_shader(data->m_shader);
		data->m_shader = NULL;
	}

	return sf_widget_shader_build(text, uniforms, data);
}
