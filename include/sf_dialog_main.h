#ifndef _GUARD_SHADER_FUN_DIALOG_MAIN_H_
#define _GUARD_SHADER_FUN_DIALOG_MAIN_H_

#include "blaze.h"

const blz_dialog_vtable_t* sf_dialog_main_get_vtable(void);
int32_t sf_dialog_child_mouse_inside(const blz_dialog_t* dialog, float x, float y);

#endif // !_GUARD_SHADER_FUN_DIALOG_MAIN_H_

