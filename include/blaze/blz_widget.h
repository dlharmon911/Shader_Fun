#ifndef _GUARD_BLAZE_WIDGET_H_
#define _GUARD_BLAZE_WIDGET_H_

#include "blaze/blz_common.h"
#include "blaze/blz_shapes.h"

typedef struct blz_widget_t blz_widget_t;

typedef int32_t(*blz_widget_initialize_func_t)(blz_widget_t* widget, void* data);
typedef void(*blz_widget_uninitialize_func_t)(blz_widget_t* widget, void* event_data);
typedef void(*blz_widget_start_func_t)(blz_widget_t* widget, void* event_data);
typedef void(*blz_widget_stop_func_t)(blz_widget_t* widget, void* event_data);
typedef void(*blz_widget_update_func_t)(blz_widget_t* widget, void* data);
typedef void(*blz_widget_render_func_t)(const blz_widget_t* widget, const void* data);
typedef bool(*blz_widget_on_event_func_t)(blz_widget_t* widget, ALLEGRO_EVENT event, void* event_data);

typedef struct blz_widget_vtable_t
{
	blz_widget_initialize_func_t m_initialize;
	blz_widget_uninitialize_func_t m_uninitialize;
	blz_widget_start_func_t m_start;
	blz_widget_stop_func_t m_stop;
	blz_widget_update_func_t m_update;
	blz_widget_render_func_t m_render;
	blz_widget_on_event_func_t m_on_event;
} blz_widget_vtable_t;

typedef struct blz_widget_t
{
	const blz_widget_vtable_t* m_table;
	blz_vec2f_t m_position;
	blz_sizef_t m_size;
} blz_widget_t;

blz_widget_t* blz_widget_create(float x, float y, float width, float height, void* data, const blz_widget_vtable_t* vtable);

int32_t blz_widget_default_initialize(blz_widget_t* widget, void* data);
void blz_widget_default_uninitialize(blz_widget_t* widget);
void blz_widget_default_update(blz_widget_t* widget, void* data);
void blz_widget_default_render(const blz_widget_t* widget, const void* data);
bool blz_widget_default_on_event(blz_widget_t* widget, ALLEGRO_EVENT event, void* event_data);

#endif // !_GUARD_BLAZE_WIDGET_H_

