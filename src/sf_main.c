#include "blaze.h"
#include "sf_shader.h"
#include "sf_dialog_data.h"
#include "sf_dialog_main.h"

static const float SF_DIPLAY_WIDTH = 1200.0f;
static const float SF_DIPLAY_HEIGHT = 800.0f;

typedef struct  sf_data_tag_t
{
	blz_input_t* m_input;
	ALLEGRO_DISPLAY* m_display;
	ALLEGRO_EVENT_QUEUE* m_event_queue;
	ALLEGRO_TIMER* m_timer;
	blz_dialog_t* m_dialog;
	sf_dialog_data_t* m_dialog_data;
	bool m_running;
	bool m_update;
} sf_data_t;

static bool _sf_init_addon(const char* name, bool (*init_func)(void))
{
	if (!init_func())
	{
		DO_LOG("ERROR: Failed to initialize %s addon", name);
		return false;
	}

	DO_LOG("INFO: Initialized %s addon", name);
	return true;
}

static int32_t sf_init( sf_data_t* data)
{
	if (!data)
	{
		return -1;
	}

	if (!al_init())
	{
		return -1;
	}

#ifdef _DEBUG
	blz_log_open("output_log.txt");
#endif

	if (!_sf_init_addon("al_init_primitives_addon", al_init_primitives_addon) ||
		!_sf_init_addon("al_init_font_addon", al_init_font_addon) ||
		!_sf_init_addon("al_init_ttf_addon", al_init_ttf_addon) ||
		!_sf_init_addon("al_init_image_addon", al_init_image_addon))
	{
		return -1;
	}

	if (!al_install_keyboard())
	{
		DO_LOG("ERROR: Failed to initialize keyboard");
		return -1;
	}
	DO_LOG("INFO: Initialized keyboard");

	if (!al_install_mouse())
	{
		DO_LOG("ERROR: Failed to initialize mouse");
		return -1;
	}
	DO_LOG("INFO: Initialized mouse");

	data->m_input->m_keyboard.m_buffer = al_ustr_new("");
	if (!data->m_input->m_keyboard.m_buffer)
	{
		DO_LOG("ERROR: Failed to create keyboard buffer");
		return -1;
	}
	DO_LOG("INFO: Created keyboard buffer");


	int32_t flags = al_get_new_display_flags();

	flags |= (ALLEGRO_WINDOWED | ALLEGRO_OPENGL | ALLEGRO_PROGRAMMABLE_PIPELINE);

	al_set_new_display_flags(flags);
	al_set_new_display_option(ALLEGRO_DEPTH_SIZE, 24, ALLEGRO_SUGGEST);
	al_set_new_window_title("Shader Fun");
	data->m_display = al_create_display((int32_t)SF_DIPLAY_WIDTH, (int32_t)SF_DIPLAY_HEIGHT);
	if (!data->m_display)
	{
		DO_LOG("ERROR: Failed to create display");
		return -1;
	}
	DO_LOG("INFO: Created display");

	al_clear_to_color(al_map_rgb(0x16, 0x16, 0x21));
	al_flip_display();

	data->m_event_queue = al_create_event_queue();
	if (!data->m_event_queue)
	{
		DO_LOG("ERROR: Failed to create event queue");
		return -1;
	}
	DO_LOG("INFO: Created event queue");

	data->m_timer = al_create_timer(1.0 / 60.0);
	if (!data->m_timer)
	{
		DO_LOG("ERROR: Failed to create timer");
		return -1;
	}
	DO_LOG("INFO: Created timer");

	al_register_event_source(data->m_event_queue, al_get_display_event_source(data->m_display));
	al_register_event_source(data->m_event_queue, al_get_timer_event_source(data->m_timer));
	al_register_event_source(data->m_event_queue, al_get_keyboard_event_source());
	al_register_event_source(data->m_event_queue, al_get_mouse_event_source());

	if (blz_font_cache_load(&data->m_dialog_data->m_fonts) != 0)
	{
		DO_LOG("ERROR: Failed to load font cache");
		return -1;
	}

	uint32_t dark_color = blz_color_get_rgba(BLAZE_COLOR_NAVAJO_WHITE);
	blz_frame_set_theme_color(0xffffffff, dark_color);

	data->m_dialog = blz_dialog_create(0.0f, 0.0f, (float)al_get_display_width(data->m_display), (float)al_get_display_height(data->m_display), data->m_dialog_data, sf_dialog_main_get_vtable());
	if (!data->m_dialog)
	{
		DO_LOG("ERROR: Failed to create dialog");
		return -1;
	}

	return 0;
}

static void sf_deinit( sf_data_t* data)
{
	if (!data)
	{
		return;
	}

	if (data->m_dialog)
	{		
		blz_dialog_destroy(data->m_dialog, data->m_dialog_data);
		data->m_dialog = NULL;
		DO_LOG("INFO: Destroyed dialog");
	}

	blz_font_cache_unload(&data->m_dialog_data->m_fonts);

	if (data->m_timer)
	{
		al_stop_timer(data->m_timer);
		al_destroy_timer(data->m_timer);
		data->m_timer = NULL;
		DO_LOG("INFO: Destroyed timer");
	}

	if (data->m_event_queue)
	{
		al_destroy_event_queue(data->m_event_queue);
		data->m_event_queue = NULL;
		DO_LOG("INFO: Destroyed event queue");
	}

	if (data->m_display)
	{
		al_destroy_display(data->m_display);
		data->m_display = NULL;
		DO_LOG("INFO: Destroyed display");
	}

	if (data->m_input->m_keyboard.m_buffer)
	{
		al_ustr_free(data->m_input->m_keyboard.m_buffer);
		data->m_input->m_keyboard.m_buffer = NULL;
		DO_LOG("INFO: Destroyed keyboard buffer");
	}

	if (al_is_image_addon_initialized())
	{
		al_shutdown_image_addon();
		DO_LOG("INFO: Shutdown image addon");
	}
	if (al_is_ttf_addon_initialized())
	{
		al_shutdown_ttf_addon();
		DO_LOG("INFO: Shutdown ttf addon");
	}
	if (al_is_font_addon_initialized())
	{
		al_shutdown_font_addon();
		DO_LOG("INFO: Shutdown font addon");
	}
	if (al_is_primitives_addon_initialized())
	{
		al_shutdown_primitives_addon();
		DO_LOG("INFO: Shutdown primitives addon");
	}

#ifdef _DEBUG
	blz_log_close();
#endif

	al_uninstall_system();
}

static void sf_input(sf_data_t* data)
{
	if (!data)
	{
		return;
	}

	while (!al_is_event_queue_empty(data->m_event_queue))
	{
		static ALLEGRO_EVENT event;

		al_get_next_event(data->m_event_queue, &event);

		switch (event.type)
		{
		case ALLEGRO_EVENT_DISPLAY_CLOSE:
		{
			data->m_dialog_data->m_code = SF_DIALOG_CODE_ESCAPE;
		} break;
		case ALLEGRO_EVENT_KEY_UP:
		{
			data->m_input->m_keyboard.m_button[event.keyboard.keycode] = BLAZE_INPUT_BUTTON_CHANGED;
			data->m_input->m_keyboard.m_state = BLAZE_INPUT_BUTTON_CHANGED;
		} break;
		case ALLEGRO_EVENT_KEY_DOWN:
		{
			if (event.keyboard.keycode == ALLEGRO_KEY_F2)
			{
				data->m_dialog_data->m_fullscreen = !data->m_dialog_data->m_fullscreen;
			}

			data->m_input->m_keyboard.m_button[event.keyboard.keycode] = BLAZE_INPUT_BUTTON_PRESSED | BLAZE_INPUT_BUTTON_CHANGED;
			data->m_input->m_keyboard.m_state = BLAZE_INPUT_BUTTON_PRESSED | BLAZE_INPUT_BUTTON_CHANGED;
		} break;
		case ALLEGRO_EVENT_KEY_CHAR:
		{
			if (event.keyboard.unichar >= 32)
			{
				al_ustr_append_chr(data->m_input->m_keyboard.m_buffer, event.keyboard.unichar);
			}
			else
			{
				// todo: handle backspace, delete, etc
			}
		} break;
		case ALLEGRO_EVENT_MOUSE_AXES:
		{
			data->m_input->m_mouse.m_position.m_x = (float)event.mouse.x;
			data->m_input->m_mouse.m_position.m_y = (float)event.mouse.y;
			data->m_input->m_mouse.m_position.m_z = (float)event.mouse.z;
			data->m_input->m_mouse.m_position.m_w = (float)event.mouse.w;
			data->m_input->m_mouse.m_delta.m_x = (float)event.mouse.dx;
			data->m_input->m_mouse.m_delta.m_y = (float)event.mouse.dy;
			data->m_input->m_mouse.m_delta.m_z = (float)event.mouse.dz;
			data->m_input->m_mouse.m_delta.m_w = (float)event.mouse.dw;
		} break;
		case ALLEGRO_EVENT_MOUSE_BUTTON_DOWN:
		{
			data->m_input->m_mouse.m_position.m_x = (float)event.mouse.x;
			data->m_input->m_mouse.m_position.m_y = (float)event.mouse.y;
			data->m_input->m_mouse.m_position.m_z = (float)event.mouse.z;
			data->m_input->m_mouse.m_position.m_w = (float)event.mouse.w;
			data->m_input->m_mouse.m_button[event.mouse.button - 1] = BLAZE_INPUT_BUTTON_PRESSED | BLAZE_INPUT_BUTTON_CHANGED;
			data->m_input->m_mouse.m_state = BLAZE_INPUT_BUTTON_PRESSED | BLAZE_INPUT_BUTTON_CHANGED;
		} break;
		case ALLEGRO_EVENT_MOUSE_BUTTON_UP:
		{
			data->m_input->m_mouse.m_position.m_x = (float)event.mouse.x;
			data->m_input->m_mouse.m_position.m_y = (float)event.mouse.y;
			data->m_input->m_mouse.m_position.m_z = (float)event.mouse.z;
			data->m_input->m_mouse.m_position.m_w = (float)event.mouse.w;
			data->m_input->m_mouse.m_button[event.mouse.button - 1] = BLAZE_INPUT_BUTTON_CHANGED;
			data->m_input->m_mouse.m_state = BLAZE_INPUT_BUTTON_CHANGED;
		} break;
		default: break;
		}

		if (((blz_widget_t*)data->m_dialog)->m_table && ((blz_widget_t*)data->m_dialog)->m_table->m_on_event)
		{
			((blz_widget_t*)data->m_dialog)->m_table->m_on_event((blz_widget_t*)data->m_dialog, event, data->m_dialog_data);
		}
	}
}

static void sf_update(sf_data_t* data)
{
	if (!data)
	{
		return;
	}

	if (((blz_widget_t*)data->m_dialog)->m_table && ((blz_widget_t*)data->m_dialog)->m_table->m_update)
	{
		((blz_widget_t*)data->m_dialog)->m_table->m_update((blz_widget_t*)data->m_dialog, data->m_dialog_data);
	}

	if (data->m_dialog_data->m_code == SF_DIALOG_CODE_QUIT_NOASK)
	{
		data->m_running = false;
	}
}

static void sf_render(const sf_data_t* data)
{
	if (!data)
	{
		return;
	}

	if (((blz_widget_t*)data->m_dialog)->m_table && ((blz_widget_t*)data->m_dialog)->m_table->m_render)
	{
		((blz_widget_t*)data->m_dialog)->m_table->m_render((blz_widget_t*)data->m_dialog, data->m_dialog_data);
	}

	al_flip_display();
}

static void sf_loop(sf_data_t* data)
{
	if (!data)
	{
		return;
	}

	al_flush_event_queue(data->m_event_queue);
	al_start_timer(data->m_timer);

	if (((blz_widget_t*)data->m_dialog)->m_table && ((blz_widget_t*)data->m_dialog)->m_table->m_start)
	{
		((blz_widget_t*)data->m_dialog)->m_table->m_start((blz_widget_t*)data->m_dialog, data->m_dialog_data);
	}

	while (data->m_running)
	{
		sf_input(data);

		if (data->m_update)
		{
			sf_update(data);
		}

		sf_render(data);

		al_rest(0.01);
	}

	if (((blz_widget_t*)data->m_dialog)->m_table && ((blz_widget_t*)data->m_dialog)->m_table->m_stop)
	{
		((blz_widget_t*)data->m_dialog)->m_table->m_stop((blz_widget_t*)data->m_dialog, data->m_dialog_data);
	}

	al_stop_timer(data->m_timer);
}

int32_t main(int32_t argc, char* argv[])
{
	blz_input_t input =
	{
		.m_keyboard = { { 0 }, 0 },
		.m_mouse = { { 0 }, 0, { 0.0f, 0.0f }, { 0.0f, 0.0f } }
	};

	sf_dialog_data_t dialog_data =
	{
		.m_editor_data = { { 0 }, NULL },
		.m_console_data = { { 0 }, NULL },
		.m_shader_data = { NULL },
		.m_uniform_data = { NULL },
		.m_time_start = 0.0f,
		.m_time_current = 0.0f,
		.m_focus_widget = NULL,
		.m_fonts = { { NULL }, { NULL }, { NULL } },
		.m_code = SF_DIALOG_CODE_NONE,
		.m_text_flags = SF_TEXT_FLAG_CLEAN,
		.m_fullscreen = false
	};

	sf_data_t data =
	{
		.m_input = &input,
		.m_display = NULL,
		.m_event_queue = NULL,
		.m_timer = NULL,
		.m_dialog_data = &dialog_data,
		.m_running = true,
		.m_update = true
	};

	if (sf_init(&data) == 0)
	{
		sf_loop(&data);
	}

	sf_deinit(&data);

	return 0;
}
