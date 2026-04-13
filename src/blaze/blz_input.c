#include "blaze/blz_input.h"

void blz_input_keyboard_flush_buffer(blz_input_t* input)
{
	if (!input)
	{
		return;
	}

	al_ustr_truncate(input->m_keyboard.m_buffer, 0);
}

void blz_input_keyboard_reset_state(blz_input_t* input)
{
	if (!input)
	{
		return;
	}

	for (size_t i = 0; i < BLAZE_INPUT_KEY_COUNT; ++i)
	{
		input->m_keyboard.m_button[i] &= ~BLAZE_INPUT_BUTTON_CHANGED;
	}

	input->m_keyboard.m_state &= ~BLAZE_INPUT_BUTTON_CHANGED;
}

void blz_input_keyboard_reset_button_state(blz_input_t* input, int32_t keycode)
{
	if (!input || keycode < 0 || keycode >= BLAZE_INPUT_KEY_COUNT)
	{
		return;
	}
	
	input->m_keyboard.m_button[keycode] &= ~BLAZE_INPUT_BUTTON_CHANGED;
	
	if (input->m_keyboard.m_state & BLAZE_INPUT_BUTTON_CHANGED)
	{
		input->m_keyboard.m_state &= ~BLAZE_INPUT_BUTTON_CHANGED;
	}
}

void blz_input_mouse_reset_state(blz_input_t* input)
{
	if (!input)
	{
		return;
	}
	
	for (size_t i = 0; i < BLAZE_INPUT_MOUSE_BUTTON_COUNT; ++i)
	{
		input->m_mouse.m_button[i] &= ~BLAZE_INPUT_BUTTON_CHANGED;
	}

	input->m_mouse.m_state &= ~BLAZE_INPUT_BUTTON_CHANGED;
}

void blz_input_mouse_reset_button_state(blz_input_t* input, int32_t button)
{
	if (!input || button < 0 || button >= BLAZE_INPUT_MOUSE_BUTTON_COUNT)
	{
		return;
	}
	
	input->m_mouse.m_button[button] &= ~BLAZE_INPUT_BUTTON_CHANGED;
	
	if (input->m_mouse.m_state & BLAZE_INPUT_BUTTON_CHANGED)
	{
		input->m_mouse.m_state &= ~BLAZE_INPUT_BUTTON_CHANGED;
	}
}

void blz_input_reset_state(blz_input_t* input)
{
	if (!input)
	{
		return;
	}

	blz_input_keyboard_reset_state(input);
	blz_input_mouse_reset_state(input);
}

bool blz_input_is_key_pressed(const blz_input_t* input, int32_t keycode)
{
	if (!input || keycode < 0 || keycode >= BLAZE_INPUT_KEY_COUNT)
	{
		return false;
	}

	return (input->m_keyboard.m_button[keycode] & BLAZE_INPUT_BUTTON_PRESSED) != 0;
}

bool blz_input_was_key_pressed(const blz_input_t* input, int32_t keycode)
{
	if (!input || keycode < 0 || keycode >= BLAZE_INPUT_KEY_COUNT)
	{
		return false;
	}

	return (input->m_keyboard.m_button[keycode] & BLAZE_INPUT_BUTTON_CHANGED) != 0 &&
		(input->m_keyboard.m_button[keycode] & BLAZE_INPUT_BUTTON_PRESSED) != 0;
}

bool blz_input_is_key_released(const blz_input_t* input, int32_t keycode)
{
	if (!input || keycode < 0 || keycode >= BLAZE_INPUT_KEY_COUNT)
	{
		return false;
	}

	return (input->m_keyboard.m_button[keycode] & BLAZE_INPUT_BUTTON_PRESSED) == 0;
}

bool blz_input_was_key_released(const blz_input_t* input, int32_t keycode)
{
	if (!input || keycode < 0 || keycode >= BLAZE_INPUT_KEY_COUNT)
	{
		return false;
	}
	return (input->m_keyboard.m_button[keycode] & BLAZE_INPUT_BUTTON_CHANGED) != 0 &&
		(input->m_keyboard.m_button[keycode] & BLAZE_INPUT_BUTTON_PRESSED) == 0;
}

bool blz_input_is_mouse_button_pressed(const blz_input_t* input, int32_t button)
{
	if (!input || button < 0 || button >= BLAZE_INPUT_MOUSE_BUTTON_COUNT)
	{
		return false;
	}

	return (input->m_mouse.m_button[button] & BLAZE_INPUT_BUTTON_PRESSED) != 0;
}

bool blz_input_was_mouse_button_pressed(const blz_input_t* input, int32_t button)
{
	if (!input || button < 0 || button >= BLAZE_INPUT_MOUSE_BUTTON_COUNT)
	{
		return false;
	}

	return (input->m_mouse.m_button[button] & BLAZE_INPUT_BUTTON_CHANGED) != 0 &&
		(input->m_mouse.m_button[button] & BLAZE_INPUT_BUTTON_PRESSED) != 0;
}

bool blz_input_is_mouse_button_released(const blz_input_t* input, int32_t button)
{
	if (!input || button < 0 || button >= BLAZE_INPUT_MOUSE_BUTTON_COUNT)
	{
		return false;
	}
	return (input->m_mouse.m_button[button] & BLAZE_INPUT_BUTTON_PRESSED) == 0;
}

bool blz_input_was_mouse_button_released(const blz_input_t* input, int32_t button)
{
	if (!input || button < 0 || button >= BLAZE_INPUT_MOUSE_BUTTON_COUNT)
	{
		return false;
	}
	return (input->m_mouse.m_button[button] & BLAZE_INPUT_BUTTON_CHANGED) != 0 &&
		(input->m_mouse.m_button[button] & BLAZE_INPUT_BUTTON_PRESSED) == 0;
}
