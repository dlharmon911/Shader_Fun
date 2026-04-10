#ifndef _GUARD_SHADER_FUN_WIDGET_CONSOLE_H_
#define _GUARD_SHADER_FUN_WIDGET_CONSOLE_H_

#include "blaze.h"

typedef struct sf_widget_console_data_tag_t
{
	blz_text_info_t m_info;
	blz_text_t* m_text;
} sf_widget_console_data_t;

const blz_widget_vtable_t* sf_widget_console_get_vtable(void);


#endif // !_GUARD_SHADER_FUN_WIDGET_CONSOLE_H_

