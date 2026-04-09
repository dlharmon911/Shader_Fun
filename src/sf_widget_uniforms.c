#include "blaze.h"
#include "sf_widget_uniforms.h"
#include "sf_dialog_data.h"

static int32_t sf_widget_uniforms_initialize(blz_widget_t* widget, sf_dialog_data_t* data)
{
	if (!widget)
	{
		return -1;
	}

	sf_widget_uniform_data_t* uniform_data = &data->m_uniform_data;
	
	blz_sizef_t resolution = { SF_WIDGET_SHADER_WIDTH, SF_WIDGET_SHADER_HEIGHT };
	blz_vec2f_t mouse = { 0.0f, 0.0f };
	float time = 0.0f;

	sf_uniform_push(&uniform_data->m_uniforms, "u_resolution", SF_UNIFORM_TYPE_FLOAT_VEC2, &resolution.m_width, sizeof(blz_vec2f_t));
	sf_uniform_push(&uniform_data->m_uniforms, "u_mouse", SF_UNIFORM_TYPE_FLOAT_VEC2, &mouse.m_x, sizeof(blz_vec2f_t));
	sf_uniform_push(&uniform_data->m_uniforms, "u_time", SF_UNIFORM_TYPE_FLOAT, &time, sizeof(float));

	return 0;
}

static void sf_widget_uniforms_uninitialize(blz_widget_t* widget, sf_dialog_data_t* data)
{
	sf_widget_uniform_data_t* uniform_data = &data->m_uniform_data;

	if (uniform_data->m_uniforms)
	{
		sf_uniform_destroy(uniform_data->m_uniforms);
		uniform_data->m_uniforms = NULL;
	}
}

static void sf_widget_uniforms_start(blz_widget_t* widget, sf_dialog_data_t* data)
{
	(void)widget;
	(void)data;
}

static void sf_widget_uniforms_stop(blz_widget_t* widget, sf_dialog_data_t* data)
{
	(void)widget;
	(void)data;
}

static void sf_widget_uniforms_update(blz_widget_t* widget, sf_dialog_data_t* data)
{
	(void)widget;
	(void)data;
}

static void sf_widget_uniforms_render(const blz_widget_t* widget, const sf_dialog_data_t* data)
{
	const ALLEGRO_FONT* font = data->m_fonts[SF_FONT_ID_UNIFORM].m_font;
	float padding = 4.0f;
	float glyph_height = (float)al_get_font_line_height(font) + padding;
	char text[256];
	blz_draw_frame_f(widget->m_position.m_x, widget->m_position.m_y, widget->m_size.m_width - 1.0f, widget->m_size.m_height - 1.0f, BLAZE_FRAME_TYPE_WORKSPACE, BLAZE_FRAME_BORDER_SUNKEN_ETCHED);

	size_t uniform_count = sf_uniform_size(data->m_uniform_data.m_uniforms);
	for (size_t i = 0; i < uniform_count; ++i)
	{
		const sf_uniform_t* uniform = &data->m_uniform_data.m_uniforms[i];
		sf_uniform_to_string(uniform, text, sizeof(text));

		float y_offset = widget->m_position.m_y + padding + (float)i * glyph_height;
		al_draw_textf(font, al_map_rgb(0, 0, 0), widget->m_position.m_x + padding, y_offset, 0, "%s", text);
	}

}

static bool sf_widget_uniforms_on_event(blz_widget_t* widget, ALLEGRO_EVENT event, sf_dialog_data_t* data)
{
	(void)widget;
	(void)event;
	(void)data;

	return false;
}

const blz_widget_vtable_t* sf_widget_uniforms_get_vtable(void)
{
	static const blz_widget_vtable_t table =
	{
		.m_initialize = (blz_widget_initialize_func_t)sf_widget_uniforms_initialize,
		.m_uninitialize = (blz_widget_uninitialize_func_t)sf_widget_uniforms_uninitialize,
		.m_start = (blz_widget_start_func_t)sf_widget_uniforms_start,
		.m_stop = (blz_widget_stop_func_t)sf_widget_uniforms_stop,
		.m_update = (blz_widget_update_func_t)sf_widget_uniforms_update,
		.m_render = (blz_widget_render_func_t)sf_widget_uniforms_render,
		.m_on_event = (blz_widget_on_event_func_t)sf_widget_uniforms_on_event
	};

	return &table;
}
