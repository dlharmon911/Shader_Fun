#ifndef _GUARD_SHADER_FUN_WIDGET_EDITOR_H_
#define _GUARD_SHADER_FUN_WIDGET_EDITOR_H_

#include "blaze.h"

typedef struct sf_widget_editor_data_tag_t
{
	blz_text_info_t m_info;
	blz_text_t* m_text;
	float m_text_x_offset;
} sf_widget_editor_data_t;

const blz_widget_vtable_t* sf_widget_editor_get_vtable(void);

bool sf_widget_editor_control_key_func(blz_widget_t* widget, const ALLEGRO_EVENT* event, void* data);
bool sf_widget_editor_key_func(blz_widget_t* widget, const ALLEGRO_EVENT* event, void* data);
bool sf_widget_editor_mouse_func(blz_widget_t* widget, const ALLEGRO_EVENT* event, void* data);


#endif // !_GUARD_SHADER_FUN_WIDGET_EDITOR_H_

