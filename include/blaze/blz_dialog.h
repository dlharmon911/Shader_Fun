#ifndef _GUARD_BLAZE_DIALOG_H_
#define _GUARD_BLAZE_DIALOG_H_

#include "blaze/blz_common.h"
#include "blaze/blz_shapes.h"
#include "blaze/blz_widget.h"

typedef struct blz_dialog_t blz_dialog_t;

typedef struct blz_widget_vtable_t blz_dialog_vtable_t;

typedef struct blz_dialog_child_t
{
	size_t m_id;
	blz_widget_t* m_widget;
} blz_dialog_child_t;

typedef struct blz_dialog_t
{
	blz_widget_t m_self;
	blz_dialog_child_t* m_children;
} blz_dialog_t;

blz_dialog_t* blz_dialog_create(float x, float y, float width, float height, void* data, const blz_dialog_vtable_t* vtable);
void blz_dialog_destroy(blz_dialog_t* dialog, void* data);
void* blz_dialog_add_child(blz_dialog_t* dialog, size_t id, blz_widget_t* child, void* data);
blz_widget_t* blz_dialog_get_child(blz_dialog_t* dialog, size_t id);
const blz_widget_t* blz_dialog_get_child_const(const blz_dialog_t* dialog, size_t id);

#endif // !_GUARD_BLAZE_DIALOG_H_

