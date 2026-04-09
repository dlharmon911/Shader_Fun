#ifndef _GUARD_BLAZE_INPUT_H
#define _GUARD_BLAZE_INPUT_H

#include "blaze/blz_common.h"
#include "blaze/blz_shapes.h"

#define BLAZE_INPUT_KEY_COUNT 227
#define BLAZE_INPUT_MOUSE_BUTTON_COUNT 5

enum BLAZE_INPUT_BUTTON_FLAGS
{
	BLAZE_INPUT_BUTTON_DEFAULT = 0,
	BLAZE_INPUT_BUTTON_PRESSED = 1,
	BLAZE_INPUT_BUTTON_CHANGED = 2
};

typedef struct blz_input_keyboard_tag_t
{
	ALLEGRO_USTR* m_buffer;
	int32_t m_button[BLAZE_INPUT_KEY_COUNT];
	int32_t m_state;
} blz_input_keyboard_t;

typedef struct blz_input_mouse_tag_t
{
	int32_t m_button[BLAZE_INPUT_MOUSE_BUTTON_COUNT];
	int32_t m_state;
	blz_vec4f_t m_position;
	blz_vec4f_t m_delta;
} blz_input_mouse_t;

typedef struct blz_input_tag_t
{
	blz_input_keyboard_t m_keyboard;
	blz_input_mouse_t m_mouse;
} blz_input_t;

void blz_input_keyboard_flush_buffer(blz_input_t* input);
void blz_input_keyboard_reset_state(blz_input_t* input);
void blz_input_keyboard_reset_button_state(blz_input_t* input, int32_t keycode);
void blz_input_mouse_reset_state(blz_input_t* input);
void blz_input_mouse_reset_button_state(blz_input_t* input, int32_t button);
void blz_input_reset_state(blz_input_t* input);

bool blz_input_is_key_pressed(const blz_input_t* input, int32_t keycode);
bool blz_input_was_key_pressed(const blz_input_t* input, int32_t keycode);
bool blz_input_is_key_released(const blz_input_t* input, int32_t keycode);
bool blz_input_was_key_released(const blz_input_t* input, int32_t keycode);

bool blz_input_is_mouse_button_pressed(const blz_input_t* input, int32_t button);
bool blz_input_was_mouse_button_pressed(const blz_input_t* input, int32_t button);
bool blz_input_is_mouse_button_released(const blz_input_t* input, int32_t button);
bool blz_input_was_mouse_button_released(const blz_input_t* input, int32_t button);

#endif // !_GUARD_BLAZE_INPUT_H
