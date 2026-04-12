#include "blaze.h"
#include "sf_uniform.h"
#include "sf_shader.h"
#include "sf_dialog_data.h"
#include "sf_dialog_main.h"
#include "sf_widget_editor.h"
#include "sf_widget_console.h"
#include "sf_widget_shader.h"
#include "sf_widget_uniforms.h"

static int32_t sf_dialog_main_initialize(blz_dialog_t* dialog, sf_dialog_data_t* data)
{
	const float padding = 2.0f;
	blz_vec4f_t ed_rect = { 0 };
	blz_vec4f_t sh_rect = { 0 };
	blz_vec4f_t un_rect = { 0 };
	blz_vec4f_t con_rect = { 0 };
	const blz_widget_vtable_t* table = NULL;

	if (!dialog)
	{
		return -1;
	}

	ed_rect.m_x = padding;
	ed_rect.m_y = padding;
	ed_rect.m_z = ((blz_widget_t*)dialog)->m_size.m_width - SF_WIDGET_SHADER_WIDTH - padding * 2.0f;
	ed_rect.m_w = ((blz_widget_t*)dialog)->m_size.m_height - padding * 3.0f - SF_WIDGET_CONSOLE_HEIGHT;
	table = sf_widget_editor_get_vtable();

	blz_widget_t* child = blz_widget_create(ed_rect.m_x, ed_rect.m_y, ed_rect.m_z, ed_rect.m_w, data, table);

	if (!child)
	{
		return -1;
	}

	data->m_focus_widget = child;

	const blz_widget_t* w = blz_dialog_add_child(dialog, SF_DIALOG_MAIN_CHILD_ID_EDITOR, child, data);

	if (!w)
	{
		return -1;
	}

	sh_rect.m_x = ((blz_widget_t*)dialog)->m_size.m_width - SF_WIDGET_SHADER_WIDTH - padding;
	sh_rect.m_y = padding;
	sh_rect.m_z = SF_WIDGET_SHADER_WIDTH;
	sh_rect.m_w = SF_WIDGET_SHADER_HEIGHT;
	table = sf_widget_shader_get_vtable();

	child = blz_widget_create(sh_rect.m_x, sh_rect.m_y, sh_rect.m_z, sh_rect.m_w, data, table);

	if (!child)
	{
		return -1;
	}

	w = blz_dialog_add_child(dialog, SF_DIALOG_MAIN_CHILD_ID_SHADER, child, data);

	if (!w)
	{
		return -1;
	}

	un_rect.m_x = sh_rect.m_x;
	un_rect.m_y = sh_rect.m_y + sh_rect.m_w + padding;
	un_rect.m_z = sh_rect.m_z;
	un_rect.m_w = ed_rect.m_w - padding - sh_rect.m_w;
	table = sf_widget_uniforms_get_vtable();

	child = blz_widget_create(un_rect.m_x, un_rect.m_y, un_rect.m_z, un_rect.m_w, data, table);

	if (!child)
	{
		return -1;
	}

	w = blz_dialog_add_child(dialog, SF_DIALOG_MAIN_CHILD_ID_UNIFORMS, child, data);

	if (!w)
	{
		return -1;
	}

	con_rect.m_x = ed_rect.m_x;
	con_rect.m_y = ed_rect.m_y + ed_rect.m_w + padding;
	con_rect.m_z = ((blz_widget_t*)dialog)->m_size.m_width - padding * 2.0f;
	con_rect.m_w = SF_WIDGET_CONSOLE_HEIGHT;

	table = sf_widget_console_get_vtable();
	child = blz_widget_create(con_rect.m_x, con_rect.m_y, con_rect.m_z, con_rect.m_w, data, table);

	if (!child)
	{
		return -1;
	}

	w = blz_dialog_add_child(dialog, SF_DIALOG_MAIN_CHILD_ID_CONSOLE, child, data);

	if (!w)
	{
		return -1;
	}

	if (sf_widget_shader_build(data->m_editor_data.m_text, data->m_uniform_data.m_uniforms, &data->m_shader_data) != 0)
	{
		data->m_text_flags |= SF_TEXT_FLAG_ERROR;
	}

	return 0;
}

static void sf_dialog_main_uninitialize(blz_dialog_t* dialog, sf_dialog_data_t* data)
{
	if (!dialog)
	{
		return;
	}

	if (dialog->m_children)
	{
		size_t count = blz_darray_size(dialog->m_children);

		for (size_t i = 0; i < count; ++i)
		{
			blz_dialog_child_t* child_node = blz_darray_at(dialog->m_children, i);

			if (child_node && child_node->m_widget->m_table && child_node->m_widget->m_table->m_uninitialize)
			{
				child_node->m_widget->m_table->m_uninitialize(child_node->m_widget, data);
			}
		}
	}
}

static void sf_dialog_main_start(blz_dialog_t* dialog, sf_dialog_data_t* data)
{
	if (dialog->m_children)
	{
		size_t count = blz_darray_size(dialog->m_children);

		for (size_t i = 0; i < count; ++i)
		{
			blz_dialog_child_t* child_node = blz_darray_at(dialog->m_children, i);

			if (child_node && child_node->m_widget->m_table && child_node->m_widget->m_table->m_start)
			{
				child_node->m_widget->m_table->m_start(child_node->m_widget, data);
			}
		}
	}
}

static void sf_dialog_main_stop(blz_dialog_t* dialog, sf_dialog_data_t* data)
{
	if (dialog->m_children)
	{
		size_t count = blz_darray_size(dialog->m_children);

		for (size_t i = 0; i < count; ++i)
		{
			blz_dialog_child_t* child_node = blz_darray_at(dialog->m_children, i);

			if (child_node && child_node->m_widget->m_table && child_node->m_widget->m_table->m_stop)
			{
				child_node->m_widget->m_table->m_stop(child_node->m_widget, data);
			}
		}
	}
}

static void sf_dialog_main_update(blz_dialog_t* dialog, sf_dialog_data_t* data)
{
	if (!dialog)
	{
		return;
	}

	if (dialog->m_children)
	{
		size_t count = blz_darray_size(dialog->m_children);

		for (size_t i = 0; i < count; ++i)
		{
			blz_dialog_child_t* child_node = blz_darray_at(dialog->m_children, i);

			if (child_node && child_node->m_widget->m_table && child_node->m_widget->m_table->m_update)
			{
				child_node->m_widget->m_table->m_update(child_node->m_widget, data);
			}
		}
	}

	if (data->m_code != SF_DIALOG_CODE_NONE)
	{
		switch (data->m_code)
		{
		case SF_DIALOG_CODE_QUIT:
		{
			// todo - ask to save if there are unsaved changes
			// todo - close dialog
			data->m_code = SF_DIALOG_CODE_QUIT_NOASK;
		} break;
		case SF_DIALOG_CODE_QUIT_NOASK:
		{
			// todo - close dialog without asking to save
		} break;
		case SF_DIALOG_CODE_ESCAPE:
		{
			// todo - ask to save if there are unsaved changes
			data->m_code = SF_DIALOG_CODE_QUIT_NOASK;
		} break;
		case SF_DIALOG_CODE_SAVE:
		{
			// todo - only save shader, not entire dialog
		} break;
		default: break;
		}
	}

	if (data->m_text_flags & (SF_TEXT_FLAG_NEEDS_REBUILD | SF_TEXT_FLAG_ERROR))
	{
		data->m_text_flags = SF_TEXT_FLAG_CLEAN;

		if (sf_widget_shader_rebuild(data->m_editor_data.m_text, data->m_uniform_data.m_uniforms, &data->m_shader_data) != 0)
		{
			data->m_text_flags |= (SF_TEXT_FLAG_ERROR | SF_TEXT_FLAG_NEEDS_REBUILD);
			data->m_time_start = (float)al_get_time();
		}
	}
}

static void sf_dialog_main_render(const blz_dialog_t* dialog, const sf_dialog_data_t* data)
{
	if (!dialog)
	{
		return;
	}

	if (data->m_fullscreen)
	{
		if (data->m_shader_data.m_shader)
		{
			ALLEGRO_BITMAP* target = al_get_target_bitmap();
			blz_vec2f_t target_pos = { 0.0f, 0.0f };
			blz_sizef_t target_size = { (float)al_get_bitmap_width(target), (float)al_get_bitmap_height(target) };

			sf_shader_render(data->m_shader_data.m_shader, data->m_uniform_data.m_uniforms, target_pos, target_size);
		}
		else
		{
			float line_padding = 10.0f;
			float line_thickness = 6.0f;

			al_draw_filled_rectangle(dialog->m_self.m_position.m_x, dialog->m_self.m_position.m_y, dialog->m_self.m_position.m_x + dialog->m_self.m_size.m_width, dialog->m_self.m_position.m_y + dialog->m_self.m_size.m_height, (ALLEGRO_COLOR) { 0.8f, 0.0f, 0.0f, 1.0f });
			al_draw_line(dialog->m_self.m_position.m_x + line_padding, dialog->m_self.m_position.m_y + line_padding, dialog->m_self.m_position.m_x + dialog->m_self.m_size.m_width - line_padding, dialog->m_self.m_position.m_y + dialog->m_self.m_size.m_height - line_padding, (ALLEGRO_COLOR) { 0.0f, 0.0f, 0.0f, 1.0f }, line_thickness);
			al_draw_line(dialog->m_self.m_position.m_x + line_padding, dialog->m_self.m_position.m_y + dialog->m_self.m_size.m_height - line_padding, dialog->m_self.m_position.m_x + dialog->m_self.m_size.m_width - line_padding, dialog->m_self.m_position.m_y + line_padding, (ALLEGRO_COLOR) { 0.0f, 0.0f, 0.0f, 1.0f }, line_thickness);
		}
		return;
	}

	const blz_widget_t* widget = (const blz_widget_t*)dialog;

	blz_draw_frame_f(widget->m_position.m_x, widget->m_position.m_y, widget->m_size.m_width - 1.0f, widget->m_size.m_height - 1.0f, BLAZE_FRAME_TYPE_NORMAL, BLAZE_FRAME_BORDER_SUNKEN_ETCHED);

	if (dialog->m_children)
	{
		size_t count = blz_darray_size(dialog->m_children);

		for (size_t i = 0; i < count; ++i)
		{
			const blz_dialog_child_t* child_node = blz_darray_at_const(dialog->m_children, i);

			if (child_node && child_node->m_widget->m_table && child_node->m_widget->m_table->m_render)
			{
				child_node->m_widget->m_table->m_render(child_node->m_widget, data);
			}
		}
	}
}

static bool sf_dialog_main_on_event(blz_dialog_t* dialog, ALLEGRO_EVENT event, sf_dialog_data_t* data)
{
	if (!dialog || !data)
	{
		return false;
	}

	switch (event.type)
	{
	case ALLEGRO_EVENT_MOUSE_AXES:
	{
		const blz_widget_t* shader_widget = blz_dialog_get_child_const(dialog, SF_DIALOG_MAIN_CHILD_ID_SHADER);

		if (!shader_widget)
		{
			return false;
		}

		sf_uniform_t* mouse_uniform = sf_uniform_get(data->m_uniform_data.m_uniforms, "u_mouse");
		blz_vec2f_t mouse_vec = { (float)event.mouse.x - shader_widget->m_position.m_x, shader_widget->m_size.m_height - ((float)event.mouse.y - shader_widget->m_position.m_y) };
		sf_uniform_set_float_vec(mouse_uniform, &mouse_vec.m_x, 2);
	} break;
	default: break;
	}

	if (data->m_focus_widget && data->m_focus_widget->m_table && data->m_focus_widget->m_table->m_on_event &&
		data->m_focus_widget->m_table->m_on_event(data->m_focus_widget, event, data))
	{
		return true;
	}

	return false;
}

const blz_dialog_vtable_t* sf_dialog_main_get_vtable(void)
{
	static const blz_dialog_vtable_t sf_dialog_main_vtable =
	{
		.m_initialize = (blz_widget_initialize_func_t)sf_dialog_main_initialize,
		.m_uninitialize = (blz_widget_uninitialize_func_t)sf_dialog_main_uninitialize,
		.m_start = (blz_widget_start_func_t)sf_dialog_main_start,
		.m_stop = (blz_widget_stop_func_t)sf_dialog_main_stop,
		.m_update = (blz_widget_update_func_t)sf_dialog_main_update,
		.m_render = (blz_widget_render_func_t)sf_dialog_main_render,
		.m_on_event = (blz_widget_on_event_func_t)sf_dialog_main_on_event
	};

	return &sf_dialog_main_vtable;
}

