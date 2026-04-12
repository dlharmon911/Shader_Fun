#ifndef _GUARD_SHADER_FUN_DIALOG_DATA_H_
#define _GUARD_SHADER_FUN_DIALOG_DATA_H_

#include "blaze.h"
#include "sf_uniform.h"
#include "sf_font.h"
#include "sf_widget_console.h"
#include "sf_widget_editor.h"
#include "sf_widget_shader.h"
#include "sf_widget_uniforms.h"

static const float SF_WIDGET_CONSOLE_HEIGHT = 256.0f;
static const float SF_WIDGET_SHADER_WIDTH = 400.0f;
static const float SF_WIDGET_SHADER_HEIGHT = 300.0f;

// code to send to parent dialog
enum SF_DIALOG_CHILD_CODE
{
	SF_DIALOG_CODE_NONE,
	SF_DIALOG_CODE_QUIT,
	SF_DIALOG_CODE_QUIT_NOASK,
	SF_DIALOG_CODE_ESCAPE,
	SF_DIALOG_CODE_SAVE
};

enum SF_TEXT_FLAGS
{
	SF_TEXT_FLAG_CLEAN = 0,
	SF_TEXT_FLAG_ERROR = 1 << 0,
	SF_TEXT_FLAG_NEEDS_REBUILD = 1 << 1
};

enum SF_DIALOG_MAIN_CHILD_ID
{
	SF_DIALOG_MAIN_CHILD_ID_EDITOR = 0,
	SF_DIALOG_MAIN_CHILD_ID_SHADER,
	SF_DIALOG_MAIN_CHILD_ID_UNIFORMS,
	SF_DIALOG_MAIN_CHILD_ID_CONSOLE
};

typedef struct sf_dialog_data_tag_t
{
	sf_widget_editor_data_t m_editor_data;
	sf_widget_console_data_t m_console_data;
	sf_widget_shader_data_t m_shader_data;
	sf_widget_uniform_data_t m_uniform_data;
	blz_widget_t* m_focus_widget;
	sf_font_cache_t m_fonts;
	float m_time_start;
	float m_time_current;
	int32_t m_code;
	int32_t m_text_flags;
	bool m_fullscreen;
} sf_dialog_data_t;

#endif // !_GUARD_SHADER_FUN_DIALOG_DATA_H_

