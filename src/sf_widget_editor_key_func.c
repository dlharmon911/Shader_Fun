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
		edit_type = BLAZE_TEXT_EDIT_OPTION_EXCISE;
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
		edit_type = BLAZE_TEXT_EDIT_OPTION_EXCISE;
        data->m_editor_data.m_info.m_selection.m_type = BLAZE_TEXT_SELECTION_TYPE_SELECTED;
        data->m_editor_data.m_info.m_selection.m_start = (blz_cursor_t){ data->m_editor_data.m_info.m_cursor.m_line, data->m_editor_data.m_info.m_cursor.m_offset };
        data->m_editor_data.m_info.m_selection.m_end = (blz_cursor_t){ data->m_editor_data.m_info.m_cursor.m_line, data->m_editor_data.m_info.m_cursor.m_offset + 1 };
    }

    int32_t edit_result = blz_text_edit(edit_type, data->m_editor_data.m_text, &data->m_editor_data.m_info, NULL);

    (void)edit_result; // Silence unused variable warning

    data->m_editor_data.m_info.m_selection.m_type = BLAZE_TEXT_SELECTION_TYPE_NONE;
    data->m_text_flags |= SF_TEXT_FLAG_NEEDS_REBUILD;
	data->m_flags |= (SF_TEXT_FLAG_MODIFIED | SF_TEXT_FLAG_TITLE_NEEDS_CHANGED);

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
		edit_type = BLAZE_TEXT_EDIT_OPTION_EXCISE;
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
		edit_type = BLAZE_TEXT_EDIT_OPTION_EXCISE;
		data->m_editor_data.m_info.m_selection.m_type = BLAZE_TEXT_SELECTION_TYPE_SELECTED;
		data->m_editor_data.m_info.m_selection.m_start = (blz_cursor_t){ data->m_editor_data.m_info.m_cursor.m_line, data->m_editor_data.m_info.m_cursor.m_offset };
		data->m_editor_data.m_info.m_selection.m_end = (blz_cursor_t){ data->m_editor_data.m_info.m_cursor.m_line, data->m_editor_data.m_info.m_cursor.m_offset + 1 };
    }

    int32_t edit_result = blz_text_edit(edit_type, data->m_editor_data.m_text, &data->m_editor_data.m_info, NULL);

    (void)edit_result; // Silence unused variable warning

    data->m_editor_data.m_info.m_selection.m_type = BLAZE_TEXT_SELECTION_TYPE_NONE;
    data->m_text_flags |= SF_TEXT_FLAG_NEEDS_REBUILD;
    data->m_flags |= (SF_TEXT_FLAG_MODIFIED | SF_TEXT_FLAG_TITLE_NEEDS_CHANGED);

    return true;
}

static bool sf_widget_editor_process_key_up(blz_widget_t* widget, ALLEGRO_EVENT event, sf_dialog_data_t* data)
{
    if (data->m_editor_data.m_info.m_cursor.m_line == 0)
    {
        return true;
    }
    const ALLEGRO_USTR* line = blz_text_get_line(data->m_editor_data.m_text, data->m_editor_data.m_info.m_cursor.m_line)->m_text;
	blz_stringview_t line_view = (blz_stringview_t)
    {
        .m_data = al_cstr(line),
        .m_length = al_ustr_size(line)
    };
	int32_t offset = blz_text_calculate_tabbed_offset(line_view, data->m_editor_data.m_info.m_cursor.m_offset);

    --data->m_editor_data.m_info.m_cursor.m_line;

    line = blz_text_get_line(data->m_editor_data.m_text, data->m_editor_data.m_info.m_cursor.m_line)->m_text;
	line_view.m_data = al_cstr(line);
	line_view.m_length = al_ustr_size(line);

	data->m_editor_data.m_info.m_cursor.m_offset = blz_text_calculate_untabbed_offset(line_view, offset);

    if (data->m_editor_data.m_info.m_cursor.m_offset >= (int32_t)line_view.m_length)
    {
        data->m_editor_data.m_info.m_cursor.m_offset = (int32_t)line_view.m_length;
    }
    return true;
}

static bool sf_widget_editor_process_key_down(blz_widget_t* widget, ALLEGRO_EVENT event, sf_dialog_data_t* data)
{
	int32_t line_count = (int32_t)blz_text_get_line_count(data->m_editor_data.m_text);
    const ALLEGRO_USTR* line = blz_text_get_line(data->m_editor_data.m_text, data->m_editor_data.m_info.m_cursor.m_line)->m_text;
    blz_stringview_t line_view = (blz_stringview_t)
    {
        .m_data = al_cstr(line),
        .m_length = al_ustr_size(line)
    };
    int32_t offset = blz_text_calculate_tabbed_offset(line_view, data->m_editor_data.m_info.m_cursor.m_offset);

    if (data->m_editor_data.m_info.m_cursor.m_line >= line_count - 1)
    {
        return true;
    }

    ++data->m_editor_data.m_info.m_cursor.m_line;

    line = blz_text_get_line(data->m_editor_data.m_text, data->m_editor_data.m_info.m_cursor.m_line)->m_text;
    line_view.m_data = al_cstr(line);
    line_view.m_length = al_ustr_size(line);

    data->m_editor_data.m_info.m_cursor.m_offset = blz_text_calculate_untabbed_offset(line_view, offset);

    if (data->m_editor_data.m_info.m_cursor.m_offset >= (int32_t)line_view.m_length)
    {
        data->m_editor_data.m_info.m_cursor.m_offset = (int32_t)line_view.m_length;
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
	int32_t split_result = blz_text_edit(BLAZE_TEXT_EDIT_OPTION_SPLIT_LINE, data->m_editor_data.m_text, &data->m_editor_data.m_info, NULL);

	(void)split_result; // Silence unused variable warning

    data->m_editor_data.m_info.m_selection.m_type = BLAZE_TEXT_SELECTION_TYPE_NONE;
    data->m_text_flags |= SF_TEXT_FLAG_NEEDS_REBUILD;
    data->m_flags |= (SF_TEXT_FLAG_MODIFIED | SF_TEXT_FLAG_TITLE_NEEDS_CHANGED);

    return true;
}

static bool sf_widget_editor_process_key_tab(blz_widget_t* widget, ALLEGRO_EVENT event, sf_dialog_data_t* data)
{
    const char text[2] = { BLAZE_TAB_CHAR, 0};

    int32_t result = blz_text_edit(BLAZE_TEXT_EDIT_OPTION_PASTE, data->m_editor_data.m_text, &data->m_editor_data.m_info, (const void*)text);

    if (result == 0)
    {
        data->m_text_flags |= SF_TEXT_FLAG_NEEDS_REBUILD;
        data->m_flags |= (SF_TEXT_FLAG_MODIFIED | SF_TEXT_FLAG_TITLE_NEEDS_CHANGED);
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

    if (c < 0x20 || c >= 0x7f)
    {
        return false;
    }

	const char text[2] = {(char)c, 0};

	int32_t result = blz_text_edit(BLAZE_TEXT_EDIT_OPTION_PASTE, data->m_editor_data.m_text, &data->m_editor_data.m_info, (const void*)text);

    if (result == 0)
    {
        data->m_text_flags |= SF_TEXT_FLAG_NEEDS_REBUILD;
        data->m_flags |= (SF_TEXT_FLAG_MODIFIED | SF_TEXT_FLAG_TITLE_NEEDS_CHANGED);
	}

    return true;
}

static bool sf_widget_editor_process_control_key_cut(blz_widget_t* widget, ALLEGRO_EVENT event, sf_dialog_data_t* data)
{
    int32_t cut = blz_text_cut_to_clipboard(data->m_editor_data.m_text, &data->m_editor_data.m_info);

    (void)cut; // Silence unused variable warning

	if (cut == 0)
	{
		data->m_text_flags |= SF_TEXT_FLAG_NEEDS_REBUILD;
		data->m_flags |= (SF_TEXT_FLAG_MODIFIED | SF_TEXT_FLAG_TITLE_NEEDS_CHANGED);
	}

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
	char* clipboard_text = al_get_clipboard_text(al_get_current_display());
	if (!clipboard_text)
	{
		return false;
	}

	int32_t pasted = blz_text_edit(BLAZE_TEXT_EDIT_OPTION_PASTE, data->m_editor_data.m_text, &data->m_editor_data.m_info, clipboard_text);

	(void)pasted; // Silence unused variable warning

    if (pasted == 0)
    {
        data->m_text_flags |= SF_TEXT_FLAG_NEEDS_REBUILD;
        data->m_flags |= (SF_TEXT_FLAG_MODIFIED | SF_TEXT_FLAG_TITLE_NEEDS_CHANGED);
    }

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
    sf_dialog_data_t* dialog_data = (sf_dialog_data_t*)data;

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

    if (dialog_data->m_flags & SF_TEXT_FLAG_TITLE_NEEDS_CHANGED)
    {
        sf_dialog_data_set_window_title(dialog_data);
        dialog_data->m_flags &= ~SF_TEXT_FLAG_TITLE_NEEDS_CHANGED;
    }

    return result;
}

bool sf_widget_editor_control_key_func(blz_widget_t* widget, const ALLEGRO_EVENT* event, void* data)
{
	bool result = false;
    sf_dialog_data_t* dialog_data = (sf_dialog_data_t*)data;

    if (!widget || !event || !data)
    {
        return false;
	}

	const sf_widget_editor_process_key_func_t func = sf_widget_editor_process_control_key_funcs[event->keyboard.keycode];

    if (func)
    {
        result = func(widget, *event, dialog_data);
    }

    if (dialog_data->m_flags & SF_TEXT_FLAG_TITLE_NEEDS_CHANGED)
    {
        sf_dialog_data_set_window_title(dialog_data);
        dialog_data->m_flags &= ~SF_TEXT_FLAG_TITLE_NEEDS_CHANGED;
    }

    return result;
}
