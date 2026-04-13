#include "blaze/blz_dialog.h"
#include "blaze/blz_frame.h"
#include "blaze/blz_darray.h"

blz_dialog_t* blz_dialog_create(float x, float y, float width, float height, void* data, const blz_dialog_vtable_t* vtable)
{
	blz_dialog_t* dialog = (blz_dialog_t*)al_malloc(sizeof(blz_dialog_t));

	if (dialog == NULL)
	{
		return NULL;
	}

	dialog->m_children = NULL;

	((blz_widget_t*)dialog)->m_table = vtable;
	((blz_widget_t*)dialog)->m_position.m_x = x;
	((blz_widget_t*)dialog)->m_position.m_y = y;
	((blz_widget_t*)dialog)->m_size.m_width = width;
	((blz_widget_t*)dialog)->m_size.m_height = height;

	if (vtable && vtable->m_initialize)
	{
		if (vtable->m_initialize((blz_widget_t*)dialog, data) != 0)
		{
			al_free(dialog);
			return NULL;
		}
	}
	else
	{
		blz_widget_default_initialize((blz_widget_t*)dialog, data);
	}

	return dialog;
}

void blz_dialog_destroy(blz_dialog_t* dialog, void* data)
{
	if (dialog == NULL)
	{
		return;
	}

	if (dialog->m_children != NULL)
	{
		blz_darray_destroy(dialog->m_children);
	}

	if (((blz_widget_t*)dialog)->m_table->m_uninitialize != NULL)
	{
		((blz_widget_t*)dialog)->m_table->m_uninitialize(((blz_widget_t*)dialog), data);
	}

	al_free(dialog);
}

void* blz_dialog_add_child(blz_dialog_t* dialog, size_t id, blz_widget_t* child, void* data)
{
	if (dialog == NULL || child == NULL)
	{
		return NULL;
	}

	blz_dialog_child_t* dialog_child = (blz_dialog_child_t*)al_malloc(sizeof(blz_dialog_child_t));
	if (dialog_child == NULL)
	{
		return NULL;
	}

	dialog_child->m_id = id;
	dialog_child->m_widget = child;

	if (!blz_darray_pushback(&dialog->m_children, dialog_child, sizeof(blz_dialog_child_t)))
	{
		return NULL;	
	}

	return child;
}

blz_widget_t* blz_dialog_get_child(blz_dialog_t* dialog, size_t id)
{
	size_t count = blz_darray_size(dialog->m_children);
	for (size_t i = 0; i < count; ++i)
	{
		blz_dialog_child_t* dialog_child = blz_darray_at(dialog->m_children, i);
		if (dialog_child && dialog_child->m_id == id)
		{
			return dialog_child->m_widget;
		}
	}

	return NULL;
}

const blz_widget_t* blz_dialog_get_child_const(const blz_dialog_t* dialog, size_t id)
{
	size_t count = blz_darray_size(dialog->m_children);
	for (size_t i = 0; i < count; ++i)
	{
		const blz_dialog_child_t* dialog_child = blz_darray_at_const(dialog->m_children, i);
		if (dialog_child && dialog_child->m_id == id)
		{
			return dialog_child->m_widget;
		}
	}

	return NULL;
}

