#include "blaze.h"
#include "sf_widget_editor.h"
#include "sf_dialog_data.h"

static bool sf_widget_editor_process_key_backspace(blz_widget_t* widget, ALLEGRO_EVENT event, sf_dialog_data_t* data)
{
    int32_t edit_type = BLAZE_TEXT_EDIT_OPTION_NONE;

    if (!data->m_editor_data.m_text)
    {
        return true;
	}

    if (data->m_editor_data.m_info.m_selection.m_type != BLAZE_TEXT_SELECTION_TYPE_NONE)
    {
		edit_type = BLAZE_TEXT_EDIT_OPTION_EXCISE_SELECTION;
    }
    else if (data->m_editor_data.m_info.m_cursor.m_offset == 0)
    {        
        if (data->m_editor_data.m_info.m_cursor.m_line == 0)
        {
            return true;
        }

		edit_type = BLAZE_TEXT_EDIT_OPTION_MERGE_LINE;
    }
    else
    {
        --data->m_editor_data.m_info.m_cursor.m_offset;
		edit_type = BLAZE_TEXT_EDIT_OPTION_EXCISE_CHAR;
    }

    int32_t edit_result = blz_text_edit(data->m_editor_data.m_text, &data->m_editor_data.m_info, edit_type, 0);

    (void)edit_result; // Silence unused variable warning

    data->m_editor_data.m_info.m_selection.m_type = BLAZE_TEXT_SELECTION_TYPE_NONE;
    data->m_text_flags |= SF_TEXT_FLAG_NEEDS_REBUILD;

    return true;
}

static bool sf_widget_editor_process_key_delete(blz_widget_t* widget, ALLEGRO_EVENT event, sf_dialog_data_t* data)
{
	int32_t edit_type = BLAZE_TEXT_EDIT_OPTION_NONE;

    if (!data->m_editor_data.m_text)
    {
        return true;
    }

    if (data->m_editor_data.m_info.m_selection.m_type != BLAZE_TEXT_SELECTION_TYPE_NONE)
    {
		edit_type = BLAZE_TEXT_EDIT_OPTION_EXCISE_SELECTION;
    }
    else if (data->m_editor_data.m_info.m_cursor.m_offset == (int32_t)al_ustr_size(blz_text_get_line(data->m_editor_data.m_text, data->m_editor_data.m_info.m_cursor.m_line)->m_text))
    {
        const blz_text_node_t* current_line = blz_text_get_line(data->m_editor_data.m_text, data->m_editor_data.m_info.m_cursor.m_line);

        if (!current_line->m_next)
        {
            return true;
		}

        ++data->m_editor_data.m_info.m_cursor.m_line;
        data->m_editor_data.m_info.m_cursor.m_offset = 0;

		edit_type = BLAZE_TEXT_EDIT_OPTION_MERGE_LINE;
    }
    else
    {
		edit_type = BLAZE_TEXT_EDIT_OPTION_EXCISE_CHAR;
    }

    int32_t edit_result = blz_text_edit(data->m_editor_data.m_text, &data->m_editor_data.m_info, edit_type, 0);

    (void)edit_result; // Silence unused variable warning

    data->m_editor_data.m_info.m_selection.m_type = BLAZE_TEXT_SELECTION_TYPE_NONE;
    data->m_text_flags |= SF_TEXT_FLAG_NEEDS_REBUILD;

    return true;
}

static bool sf_widget_editor_process_key_up(blz_widget_t* widget, ALLEGRO_EVENT event, sf_dialog_data_t* data)
{
    if (data->m_editor_data.m_info.m_cursor.m_line == 0)
    {
        return true;
    }
    --data->m_editor_data.m_info.m_cursor.m_line;

    const ALLEGRO_USTR* line = blz_text_get_line(data->m_editor_data.m_text, data->m_editor_data.m_info.m_cursor.m_line)->m_text;

    int32_t line_length = (int32_t)al_ustr_size(line);

    if (data->m_editor_data.m_info.m_cursor.m_offset >= line_length)
    {
        data->m_editor_data.m_info.m_cursor.m_offset = line_length;
    }
    return true;
}

static bool sf_widget_editor_process_key_down(blz_widget_t* widget, ALLEGRO_EVENT event, sf_dialog_data_t* data)
{
	int32_t line_count = (int32_t)blz_text_get_line_count(data->m_editor_data.m_text);

    if (data->m_editor_data.m_info.m_cursor.m_line >= line_count - 1)
    {
        return true;
    }

    ++data->m_editor_data.m_info.m_cursor.m_line;

	const ALLEGRO_USTR* line = blz_text_get_line(data->m_editor_data.m_text, data->m_editor_data.m_info.m_cursor.m_line)->m_text;

    int32_t line_length = (int32_t)al_ustr_size(line);

    if (data->m_editor_data.m_info.m_cursor.m_offset >= line_length)
    {
        data->m_editor_data.m_info.m_cursor.m_offset = line_length;
    }

    return true;
}

static bool sf_widget_editor_process_key_left(blz_widget_t* widget, ALLEGRO_EVENT event, sf_dialog_data_t* data)
{
    if (data->m_editor_data.m_info.m_cursor.m_offset == 0)
    {
        if (data->m_editor_data.m_info.m_cursor.m_line == 0)
        {
			return true;
        }
        --data->m_editor_data.m_info.m_cursor.m_line;

        const ALLEGRO_USTR* line = blz_text_get_line(data->m_editor_data.m_text, data->m_editor_data.m_info.m_cursor.m_line)->m_text;

        int32_t line_length = (int32_t)al_ustr_size(line);

        data->m_editor_data.m_info.m_cursor.m_offset = line_length;
    }
    else
    {
        --data->m_editor_data.m_info.m_cursor.m_offset;
    }

    return true;
}

static bool sf_widget_editor_process_key_right(blz_widget_t* widget, ALLEGRO_EVENT event, sf_dialog_data_t* data)
{
    if (data->m_editor_data.m_info.m_cursor.m_offset == (int32_t)al_ustr_size(blz_text_get_line(data->m_editor_data.m_text, data->m_editor_data.m_info.m_cursor.m_line)->m_text))
    {
        const blz_text_node_t* next_line = blz_text_get_line(data->m_editor_data.m_text, (size_t)(data->m_editor_data.m_info.m_cursor.m_line + 1));
        if (!next_line)
        {
            return true;
        }
        ++data->m_editor_data.m_info.m_cursor.m_line;
        data->m_editor_data.m_info.m_cursor.m_offset = 0;
    }
    else
    {
        ++data->m_editor_data.m_info.m_cursor.m_offset;
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
	int32_t split_result = blz_text_edit(data->m_editor_data.m_text, &data->m_editor_data.m_info, BLAZE_TEXT_EDIT_OPTION_SPLIT_LINE, 0);

	(void)split_result; // Silence unused variable warning

    data->m_editor_data.m_info.m_selection.m_type = BLAZE_TEXT_SELECTION_TYPE_NONE;
    data->m_text_flags |= SF_TEXT_FLAG_NEEDS_REBUILD;

    return true;
}

static bool sf_widget_editor_process_key_tab(blz_widget_t* widget, ALLEGRO_EVENT event, sf_dialog_data_t* data)
{
    int32_t c = BLAZE_TAB_CHAR;
    int32_t result = blz_text_edit(data->m_editor_data.m_text, &data->m_editor_data.m_info, BLAZE_TEXT_EDIT_OPTION_INSERT_CHAR, &c);

    if (result == 0)
    {
        data->m_text_flags |= SF_TEXT_FLAG_NEEDS_REBUILD;
    }

    return true;
}

static bool sf_widget_editor_process_key_home(blz_widget_t* widget, ALLEGRO_EVENT event, sf_dialog_data_t* data)
{
    data->m_editor_data.m_info.m_cursor.m_offset = 0;
    
    return true;
}

static bool sf_widget_editor_process_key_end(blz_widget_t* widget, ALLEGRO_EVENT event, sf_dialog_data_t* data)
{
    data->m_editor_data.m_info.m_cursor.m_offset = (int32_t)al_ustr_size(blz_text_get_line(data->m_editor_data.m_text, data->m_editor_data.m_info.m_cursor.m_line)->m_text);

    return true;
}

static bool sf_widget_editor_process_key_pgup(blz_widget_t* widget, ALLEGRO_EVENT event, sf_dialog_data_t* data)
{
    int32_t visible_line_count = (int32_t)(widget->m_size.m_height / ((float)al_get_font_line_height(data->m_fonts[BLAZE_FONT_ID_EDITOR].m_font) + data->m_editor_data.m_info.m_line_spacing));
    
    data->m_editor_data.m_info.m_cursor.m_line -= visible_line_count;
    
    if (data->m_editor_data.m_info.m_cursor.m_line < 0)
    {
        data->m_editor_data.m_info.m_cursor.m_line = 0;
    }

    return true;
}

static bool sf_widget_editor_process_key_pgdn(blz_widget_t* widget, ALLEGRO_EVENT event, sf_dialog_data_t* data)
{
    int32_t visible_line_count = (int32_t)(widget->m_size.m_height / ((float)al_get_font_line_height(data->m_fonts[BLAZE_FONT_ID_EDITOR].m_font) + data->m_editor_data.m_info.m_line_spacing));
    data->m_editor_data.m_info.m_cursor.m_line += visible_line_count;
    int32_t line_count = (int32_t)blz_text_get_line_count(data->m_editor_data.m_text);

    if (data->m_editor_data.m_info.m_cursor.m_line >= line_count)
    {
        data->m_editor_data.m_info.m_cursor.m_line = line_count - 1;
    }

    return true;
}

static bool sf_widget_editor_process_key_default(blz_widget_t* widget, ALLEGRO_EVENT event, sf_dialog_data_t* data)
{
    int32_t c = event.keyboard.unichar;
	int32_t result = blz_text_edit(data->m_editor_data.m_text, &data->m_editor_data.m_info, BLAZE_TEXT_EDIT_OPTION_INSERT_CHAR, &c);

    if (result == 0)
    {
        data->m_text_flags |= SF_TEXT_FLAG_NEEDS_REBUILD;
	}

    return true;
}

static bool sf_widget_editor_process_control_key_cut(blz_widget_t* widget, ALLEGRO_EVENT event, sf_dialog_data_t* data)
{
    int32_t cut = blz_text_cut_to_clipboard(data->m_editor_data.m_text, &data->m_editor_data.m_info);

    (void)cut; // Silence unused variable warning

    data->m_text_flags |= SF_TEXT_FLAG_NEEDS_REBUILD;

    return false;
}

static bool sf_widget_editor_process_control_key_copy(blz_widget_t* widget, ALLEGRO_EVENT event, sf_dialog_data_t* data)
{
	int32_t copied = blz_text_copy_to_clipboard(data->m_editor_data.m_text, &data->m_editor_data.m_info);

	(void)copied; // Silence unused variable warning

    return false;
}

static bool sf_widget_editor_process_control_key_paste(blz_widget_t* widget, ALLEGRO_EVENT event, sf_dialog_data_t* data)
{
	int32_t pasted = blz_text_edit(data->m_editor_data.m_text, &data->m_editor_data.m_info, BLAZE_TEXT_EDIT_OPTION_PASTE_CLIPBOARD, 0);

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

static const sf_widget_editor_process_key_func_t sf_widget_editor_process_key_funcs[ALLEGRO_KEY_MAX] =
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

static const sf_widget_editor_process_key_func_t sf_widget_editor_process_control_key_funcs[ALLEGRO_KEY_MAX] =
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

    return false;
}
