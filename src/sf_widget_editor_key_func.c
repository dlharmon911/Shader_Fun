#include "blaze.h"
#include "sf_widget_editor.h"
#include "sf_dialog_data.h"

static bool sf_widget_editor_process_key_backspace(blz_widget_t* widget, ALLEGRO_EVENT event, sf_dialog_data_t* data)
{
    if (!data->m_editor_data.m_text)
    {
        return true;
	}

    if (data->m_editor_data.m_info.m_selection.m_type != BLAZE_TEXT_SELECTION_TYPE_NONE)
    {
		int32_t excised = blz_text_excise_selection(data->m_editor_data.m_text, &data->m_editor_data.m_info, &data->m_editor_data.m_info.m_selection);
        data->m_editor_data.m_info.m_selection.m_type = BLAZE_TEXT_SELECTION_TYPE_NONE;
		data->m_text_flags |= SF_TEXT_FLAG_NEEDS_REBUILD;

		(void)excised; // Silence unused variable warning

		return true;
    }

    if (data->m_editor_data.m_info.m_cursor_offset == 0)
    {
        if (data->m_editor_data.m_info.m_cursor_line == 0)
        {
            return true;
        }

        blz_text_node_t* current_line = blz_text_get_line(data->m_editor_data.m_text, data->m_editor_data.m_info.m_cursor_line);
        blz_text_node_t* previous_line = blz_text_get_previous_line(data->m_editor_data.m_text, current_line);
        int32_t prev_length = (int32_t)al_ustr_size(previous_line->m_text);

        blz_text_merge(previous_line, &current_line);

        --data->m_editor_data.m_info.m_cursor_line;
        data->m_editor_data.m_info.m_cursor_offset = prev_length;
        data->m_text_flags |= SF_TEXT_FLAG_NEEDS_REBUILD;
        return true;
    }
    else
    {
        blz_text_node_t* current_line = blz_text_get_line(data->m_editor_data.m_text, data->m_editor_data.m_info.m_cursor_line);

        if (!current_line)
        {
            data->m_text_flags |= SF_TEXT_FLAG_NEEDS_REBUILD;
            return true;
        }
        al_ustr_remove_chr(current_line->m_text, data->m_editor_data.m_info.m_cursor_offset - 1);
        --data->m_editor_data.m_info.m_cursor_offset;
        data->m_text_flags |= SF_TEXT_FLAG_NEEDS_REBUILD;
        return true;
    }

    return false;
}

static bool sf_widget_editor_process_key_delete(blz_widget_t* widget, ALLEGRO_EVENT event, sf_dialog_data_t* data)
{
    if (!data->m_editor_data.m_text)
    {
        return true;
    }

    if (data->m_editor_data.m_info.m_selection.m_type != BLAZE_TEXT_SELECTION_TYPE_NONE)
    {
        int32_t excised = blz_text_excise_selection(data->m_editor_data.m_text, &data->m_editor_data.m_info, &data->m_editor_data.m_info.m_selection);
        data->m_editor_data.m_info.m_selection.m_type = BLAZE_TEXT_SELECTION_TYPE_NONE;
        data->m_text_flags |= SF_TEXT_FLAG_NEEDS_REBUILD;

        (void)excised; // Silence unused variable warning

        return true;
    }

    if (data->m_editor_data.m_info.m_cursor_offset == (int32_t)al_ustr_size(blz_text_get_line(data->m_editor_data.m_text, data->m_editor_data.m_info.m_cursor_line)->m_text))
    {
        blz_text_node_t* current_line = blz_text_get_line(data->m_editor_data.m_text, data->m_editor_data.m_info.m_cursor_line);
        blz_text_node_t* next_line = blz_text_get_line(data->m_editor_data.m_text, (size_t)(data->m_editor_data.m_info.m_cursor_line + 1));

        if (!current_line || !next_line)
        {
            data->m_text_flags |= SF_TEXT_FLAG_NEEDS_REBUILD;
            return true;
        }

        if (al_ustr_size(current_line->m_text) == 0)
        {
            blz_text_node_t* prev = blz_text_get_previous_line(data->m_editor_data.m_text, current_line);

            if (prev)
            {
                prev->m_next = next_line;
            }
            else
            {
                data->m_editor_data.m_text->m_head = next_line;
            }

            data->m_text_flags |= SF_TEXT_FLAG_NEEDS_REBUILD;
            return true;
        }

        blz_text_merge(current_line, &next_line);
        data->m_text_flags |= SF_TEXT_FLAG_NEEDS_REBUILD;
        return true;
    }
    else
    {
        blz_text_node_t* current_line = blz_text_get_line(data->m_editor_data.m_text, data->m_editor_data.m_info.m_cursor_line);
        if (current_line)
        {
            al_ustr_remove_chr(current_line->m_text, data->m_editor_data.m_info.m_cursor_offset);
        }

        data->m_text_flags |= SF_TEXT_FLAG_NEEDS_REBUILD;
        return true;
    }

    return false;
}

static bool sf_widget_editor_process_key_up(blz_widget_t* widget, ALLEGRO_EVENT event, sf_dialog_data_t* data)
{
    if (data->m_editor_data.m_info.m_cursor_line == 0)
    {
        return true;
    }
    --data->m_editor_data.m_info.m_cursor_line;
    int32_t line_length = (int32_t)al_ustr_size(blz_text_get_line(data->m_editor_data.m_text, data->m_editor_data.m_info.m_cursor_line)->m_text);
    if (data->m_editor_data.m_info.m_cursor_offset > line_length)
    {
        data->m_editor_data.m_info.m_cursor_offset = line_length;
    }
    return true;
}

static bool sf_widget_editor_process_key_down(blz_widget_t* widget, ALLEGRO_EVENT event, sf_dialog_data_t* data)
{
    if (data->m_editor_data.m_info.m_cursor_line == (int32_t)blz_text_get_line_count(data->m_editor_data.m_text) - 1)
    {
        return true;
    }
    ++data->m_editor_data.m_info.m_cursor_line;
    int32_t line_length = (int32_t)al_ustr_size(blz_text_get_line(data->m_editor_data.m_text, data->m_editor_data.m_info.m_cursor_line)->m_text);
    if (data->m_editor_data.m_info.m_cursor_offset > line_length)
    {
        data->m_editor_data.m_info.m_cursor_offset = line_length;
    }
    return true;
}

static bool sf_widget_editor_process_key_left(blz_widget_t* widget, ALLEGRO_EVENT event, sf_dialog_data_t* data)
{
    if (data->m_editor_data.m_info.m_cursor_offset == 0)
    {
        if (data->m_editor_data.m_info.m_cursor_line == 0)
        {
			return true;
        }
        --data->m_editor_data.m_info.m_cursor_line;
        data->m_editor_data.m_info.m_cursor_offset = (int32_t)al_ustr_size(blz_text_get_line(data->m_editor_data.m_text, data->m_editor_data.m_info.m_cursor_line)->m_text);
    }
    else
    {
        --data->m_editor_data.m_info.m_cursor_offset;
    }

    return true;
}

static bool sf_widget_editor_process_key_right(blz_widget_t* widget, ALLEGRO_EVENT event, sf_dialog_data_t* data)
{
    if (data->m_editor_data.m_info.m_cursor_offset == (int32_t)al_ustr_size(blz_text_get_line(data->m_editor_data.m_text, data->m_editor_data.m_info.m_cursor_line)->m_text))
    {
        const blz_text_node_t* next_line = blz_text_get_line(data->m_editor_data.m_text, (size_t)(data->m_editor_data.m_info.m_cursor_line + 1));
        if (!next_line)
        {
            return true;
        }
        ++data->m_editor_data.m_info.m_cursor_line;
        data->m_editor_data.m_info.m_cursor_offset = 0;
    }
    else
    {
        ++data->m_editor_data.m_info.m_cursor_offset;
    }
    return true;
}

static bool sf_widget_editor_process_key_escape(blz_widget_t* widget, ALLEGRO_EVENT event, sf_dialog_data_t* data)
{
    data->m_code = SF_DIALOG_CODE_ESCAPE;
    return true;
}

static bool sf_widget_editor_process_key_enter(blz_widget_t* widget, ALLEGRO_EVENT event, sf_dialog_data_t* data)
{
    blz_text_node_t* current_line = blz_text_get_line(data->m_editor_data.m_text, data->m_editor_data.m_info.m_cursor_line);
    int32_t size = (int32_t)al_ustr_size(current_line->m_text);

    if (data->m_editor_data.m_info.m_cursor_offset == size)
    {
        blz_text_node_t* next_line = blz_text_get_next_line(data->m_editor_data.m_text, current_line);
        current_line->m_next = blz_text_create_node();
        current_line->m_next->m_next = next_line;
    }
    else
    {
        blz_text_split(&current_line, data->m_editor_data.m_info.m_cursor_offset);
    }

    ++data->m_editor_data.m_info.m_cursor_line;
    data->m_editor_data.m_info.m_cursor_offset = 0;
    data->m_text_flags |= SF_TEXT_FLAG_NEEDS_REBUILD;
    
    return true;
}

static bool sf_widget_editor_process_key_tab(blz_widget_t* widget, ALLEGRO_EVENT event, sf_dialog_data_t* data)
{
    al_ustr_insert_cstr(blz_text_get_line(data->m_editor_data.m_text, data->m_editor_data.m_info.m_cursor_line)->m_text, data->m_editor_data.m_info.m_cursor_offset, "    ");
    data->m_editor_data.m_info.m_cursor_offset += 4;
    
    return true;
}

static bool sf_widget_editor_process_key_home(blz_widget_t* widget, ALLEGRO_EVENT event, sf_dialog_data_t* data)
{
    data->m_editor_data.m_info.m_cursor_offset = 0;
    
    return true;
}

static bool sf_widget_editor_process_key_end(blz_widget_t* widget, ALLEGRO_EVENT event, sf_dialog_data_t* data)
{
    data->m_editor_data.m_info.m_cursor_offset = (int32_t)al_ustr_size(blz_text_get_line(data->m_editor_data.m_text, data->m_editor_data.m_info.m_cursor_line)->m_text);

    return true;
}

static bool sf_widget_editor_process_key_pgup(blz_widget_t* widget, ALLEGRO_EVENT event, sf_dialog_data_t* data)
{
    int32_t visible_line_count = (int32_t)(widget->m_size.m_height / ((float)al_get_font_line_height(data->m_fonts[BLAZE_FONT_ID_EDITOR_REGULAR].m_font) + data->m_editor_data.m_info.m_line_spacing));
    
    data->m_editor_data.m_info.m_cursor_line -= visible_line_count;
    
    if (data->m_editor_data.m_info.m_cursor_line < 0)
    {
        data->m_editor_data.m_info.m_cursor_line = 0;
    }

    return true;
}

static bool sf_widget_editor_process_key_pgdn(blz_widget_t* widget, ALLEGRO_EVENT event, sf_dialog_data_t* data)
{
    int32_t visible_line_count = (int32_t)(widget->m_size.m_height / ((float)al_get_font_line_height(data->m_fonts[BLAZE_FONT_ID_EDITOR_REGULAR].m_font) + data->m_editor_data.m_info.m_line_spacing));
    data->m_editor_data.m_info.m_cursor_line += visible_line_count;
    int32_t line_count = (int32_t)blz_text_get_line_count(data->m_editor_data.m_text);

    if (data->m_editor_data.m_info.m_cursor_line >= line_count)
    {
        data->m_editor_data.m_info.m_cursor_line = line_count - 1;
    }

    return true;
}

static bool sf_widget_editor_process_key_default(blz_widget_t* widget, ALLEGRO_EVENT event, sf_dialog_data_t* data)
{
    if (event.keyboard.unichar >= 0x20)
    {
        al_ustr_insert_chr(blz_text_get_line(data->m_editor_data.m_text, data->m_editor_data.m_info.m_cursor_line)->m_text, data->m_editor_data.m_info.m_cursor_offset, event.keyboard.unichar);
        ++data->m_editor_data.m_info.m_cursor_offset;
        data->m_text_flags |= SF_TEXT_FLAG_NEEDS_REBUILD;

        return true;
    }

    return false;
}

static bool sf_widget_editor_process_control_key_cut(blz_widget_t* widget, ALLEGRO_EVENT event, sf_dialog_data_t* data)
{
    int32_t cut = blz_text_cut_selection(data->m_editor_data.m_text, &data->m_editor_data.m_info, &data->m_editor_data.m_info.m_selection);

    (void)cut; // Silence unused variable warning

    data->m_text_flags |= SF_TEXT_FLAG_NEEDS_REBUILD;

    return false;
}

static bool sf_widget_editor_process_control_key_copy(blz_widget_t* widget, ALLEGRO_EVENT event, sf_dialog_data_t* data)
{
	int32_t copied = blz_text_copy_selection(data->m_editor_data.m_text, &data->m_editor_data.m_info, &data->m_editor_data.m_info.m_selection);

	(void)copied; // Silence unused variable warning

    return false;
}

static bool sf_widget_editor_process_control_key_paste(blz_widget_t* widget, ALLEGRO_EVENT event, sf_dialog_data_t* data)
{
	int32_t pasted = blz_text_paste_selection(data->m_editor_data.m_text, &data->m_editor_data.m_info, &data->m_editor_data.m_info.m_selection);

	(void)pasted; // Silence unused variable warning

	data->m_text_flags |= SF_TEXT_FLAG_NEEDS_REBUILD;

    return false;
}

static bool sf_widget_editor_process_control_key_select_all(blz_widget_t* widget, ALLEGRO_EVENT event, sf_dialog_data_t* data)
{
    if (!widget || !data)
    {
        return false;
    }

    int32_t result = blz_text_select_all(data->m_editor_data.m_text, &data->m_editor_data.m_info);

	(void)result; // Silence unused variable warning

    return true;
}

static bool sf_widget_editor_process_control_key_save(blz_widget_t* widget, ALLEGRO_EVENT event, sf_dialog_data_t* data)
{
    return false;
}

typedef bool (*sf_widget_editor_process_key_func_t)(blz_widget_t* widget, ALLEGRO_EVENT event, sf_dialog_data_t* data);

static const sf_widget_editor_process_key_func_t sf_widget_editor_process_key_funcs[] =
{
	[ALLEGRO_KEY_ESCAPE] = sf_widget_editor_process_key_escape,
    [ALLEGRO_KEY_BACKSPACE] = sf_widget_editor_process_key_backspace,
    [ALLEGRO_KEY_DELETE] = sf_widget_editor_process_key_delete,
	[ALLEGRO_KEY_UP] = sf_widget_editor_process_key_up,
	[ALLEGRO_KEY_DOWN] = sf_widget_editor_process_key_down,
	[ALLEGRO_KEY_LEFT] = sf_widget_editor_process_key_left,
	[ALLEGRO_KEY_RIGHT] = sf_widget_editor_process_key_right,
	[ALLEGRO_KEY_ENTER] = sf_widget_editor_process_key_enter,
    [ALLEGRO_KEY_TAB] = sf_widget_editor_process_key_tab,
	[ALLEGRO_KEY_HOME] = sf_widget_editor_process_key_home,
	[ALLEGRO_KEY_END] = sf_widget_editor_process_key_end,
	[ALLEGRO_KEY_PGUP] = sf_widget_editor_process_key_pgup,
    [ALLEGRO_KEY_PGDN] = sf_widget_editor_process_key_pgdn,
};

static const sf_widget_editor_process_key_func_t sf_widget_editor_process_control_key_funcs[] =
{
    [ALLEGRO_KEY_C] = sf_widget_editor_process_control_key_copy,
	[ALLEGRO_KEY_S] = sf_widget_editor_process_control_key_save,
    [ALLEGRO_KEY_V] = sf_widget_editor_process_control_key_paste,
	[ALLEGRO_KEY_X] = sf_widget_editor_process_control_key_cut,
	[ALLEGRO_KEY_A] = sf_widget_editor_process_control_key_select_all
};

bool sf_widget_editor_key_func(blz_widget_t* widget, const ALLEGRO_EVENT* event, void* data)
{
	bool result = false;

    if (!widget || !event || !data)
    {
        return false;
    }

    const sf_widget_editor_process_key_func_t func = sf_widget_editor_process_key_funcs[event->keyboard.keycode];

    if (func)
    {
        result = func(widget, *event, (sf_dialog_data_t*)data);
    }
    else
    {
        result = sf_widget_editor_process_key_default(widget, *event, (sf_dialog_data_t*)data);
    }

    ((sf_dialog_data_t*)data)->m_editor_data.m_info.m_selection.m_type = BLAZE_TEXT_SELECTION_TYPE_NONE;

    return result;
}

bool sf_widget_editor_control_key_func(blz_widget_t* widget, const ALLEGRO_EVENT* event, void* data)
{
    if (!widget || !event || !data)
    {
        return false;
	}

	const sf_widget_editor_process_key_func_t func = sf_widget_editor_process_control_key_funcs[event->keyboard.keycode];

    if (func)
    {
        return func(widget, *event, (sf_dialog_data_t*)data);
    }
    else
    {
		return sf_widget_editor_process_key_default(widget, *event, (sf_dialog_data_t*)data);
    }

    return false;
}
