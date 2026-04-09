#include "blaze/blz_widget.h"
#include "blaze/blz_frame.h"

blz_widget_t* blz_widget_create(float x, float y, float width, float height, void* data, const blz_widget_vtable_t* vtable)
{
	blz_widget_t* widget = (blz_widget_t*)al_malloc(sizeof(blz_widget_t));
	
	if (widget == NULL)
	{
		return NULL;
	}
	
	widget->m_table = vtable;
	widget->m_position.m_x = x;
	widget->m_position.m_y = y;
	widget->m_size.m_width = width;
	widget->m_size.m_height = height;

	if (vtable && vtable->m_initialize)
	{
		vtable->m_initialize(widget, data);
	}

	return widget;
}

int32_t blz_widget_default_initialize(blz_widget_t* widget, void* data)
{
	if (widget == NULL)
	{
		return -1;
	}

	return 0;
}

void blz_widget_default_uninitialize(blz_widget_t* widget)
{
	if (widget == NULL)
	{
		return;
	}

	return;
}

void blz_widget_default_update(blz_widget_t* widget, void* data)
{
	if (widget == NULL)
	{
		return;
	}
}

void blz_widget_default_render(const blz_widget_t* widget, const void* data)
{
	if (widget == NULL)
	{
		return;
	}

	blz_draw_frame_f(widget->m_position.m_x, widget->m_position.m_y, widget->m_size.m_width, widget->m_size.m_height, BLAZE_FRAME_TYPE_NORMAL, BLAZE_FRAME_BORDER_NONE);
}

bool blz_widget_default_on_event(blz_widget_t* widget, ALLEGRO_EVENT event, void* event_data)
{
	if (widget == NULL)
	{
		return false;
	}

	return false;
}