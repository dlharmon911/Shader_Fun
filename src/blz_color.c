#include "blaze/blz_common.h"
#include "blaze/blz_color.h"
#include "blaze/blz_color_u8_to_float.h"
#include "blaze/blz_color_list.h"

#define _blz_float_to_u8(value) ((uint8_t)((value) * 255.0f))
#define _blz_red_component(color) _blz_float_to_u8((color).r)
#define _blz_green_component(color) _blz_float_to_u8((color).g)
#define _blz_blue_component(color) _blz_float_to_u8((color).b)
#define _blz_alpha_component(color) _blz_float_to_u8((color).a)
#define _blz_to_red_component(color) _blz_u8_to_float[(color >> 24) & 0xff]
#define _blz_to_green_component(color) _blz_u8_to_float[(color >> 16) & 0xff]
#define _blz_to_blue_component(color) _blz_u8_to_float[(color >> 8) & 0xff]
#define _blz_to_alpha_component(color) _blz_u8_to_float[(color) & 0xff]
#define _blz_color_map_from_rgba(r, g, b, a) (((uint32_t)(r) << 24) | ((uint32_t)(g) << 16) | ((uint32_t)(b) << 8) | (uint32_t)(a))


ALLEGRO_COLOR blz_color_map_rgba_to_acolor(uint32_t rgba)
{
	ALLEGRO_COLOR color =
	{
		.r = _blz_to_red_component(rgba),
		.g = _blz_to_green_component(rgba),
		.b = _blz_to_blue_component(rgba),
		.a = _blz_to_alpha_component(rgba)
	};

	return color;
}

ALLEGRO_COLOR blz_color_map_rgb_to_acolor(uint32_t rgb)
{
	uint32_t rgba = (rgb << 8) | 0xFF; // Set alpha to 255
	return blz_color_map_rgba_to_acolor(rgba);
}

void blz_color_map_to_rgba_u8(uint32_t rgba, uint8_t* r, uint8_t* g, uint8_t* b, uint8_t* a)
{
	if (!r || !g || !b || !a)
	{
		return;
	}

	*r = (rgba >> 24) & 0xFF;
	*g = (rgba >> 16) & 0xFF;
	*b = (rgba >> 8) & 0xFF;
	*a = rgba & 0xFF;
}

void blz_color_map_to_rgb_u8(uint32_t rgb, uint8_t* r, uint8_t* g, uint8_t* b)
{
	if (!r || !g || !b)
	{
		return;
	}

	*r = (rgb >> 16) & 0xFF;
	*g = (rgb >> 8) & 0xFF;
	*b = rgb & 0xFF;
}

void blz_color_map_to_rgba_f(uint32_t rgba, float* r, float* g, float* b, float* a)
{
	if (!r || !g || !b || !a)
	{
		return;
	}

	*r = _blz_to_red_component(rgba);
	*g = _blz_to_green_component(rgba);
	*b = _blz_to_blue_component(rgba);
	*a = _blz_to_alpha_component(rgba);
}

void blz_color_map_to_rgb_f(uint32_t rgb, float* r, float* g, float* b)
{
	if (!r || !g || !b)
	{
		return;
	}

	*r = _blz_to_red_component(rgb << 8); // Shift left to align with RGBA format
	*g = _blz_to_green_component(rgb << 8);
	*b = _blz_to_blue_component(rgb << 8);
}

uint32_t blz_color_map_from_acolor(ALLEGRO_COLOR color)
{
	return _blz_color_map_from_rgba(_blz_red_component(color), _blz_green_component(color), _blz_blue_component(color), _blz_alpha_component(color));

}

uint32_t blz_color_map_from_rgba_u8(uint8_t r, uint8_t g, uint8_t b, uint8_t a)
{
	return _blz_color_map_from_rgba(r, g, b, a);
}

uint32_t blz_color_map_from_rgb_u8(uint8_t r, uint8_t g, uint8_t b)
{
	return _blz_color_map_from_rgba(r, g, b, 255);
}

uint32_t blz_color_map_from_rgba_f(float r, float g, float b, float a)
{
	return _blz_color_map_from_rgba(_blz_float_to_u8(r), _blz_float_to_u8(g), _blz_float_to_u8(b), _blz_float_to_u8(a));
}

uint32_t blz_color_map_from_rgb_f(float r, float g, float b)
{
	return _blz_color_map_from_rgba(_blz_float_to_u8(r), _blz_float_to_u8(g), _blz_float_to_u8(b), 255);
}

blz_stringview_t blz_color_get_name(size_t index)
{
	assert(index < BLAZE_COLOR_COUNT);
	return blz_stringview_from_cstr(_blz_color_list[index].m_name);
}

uint32_t blz_color_get_rgba(size_t index)
{
	assert(index < BLAZE_COLOR_COUNT);
	return _blz_color_list[index].m_rgba;
}

void blz_color_get_gradient(uint32_t base_color, uint32_t layers[4])
{
	// Extract RGB components (assuming 0xRRGGBB format)
	uint8_t r = (base_color >> 24) & 0xFF;
	uint8_t g = (base_color >> 16) & 0xFF;
	uint8_t b = (base_color >> 8) & 0xFF;

	// 1. 25% Lighter (Tint)
	// Formula: current + (255 - current) * 0.25
	uint8_t r1 = r + (uint8_t)((255 - r) * 0.25f);
	uint8_t g1 = g + (uint8_t)((255 - g) * 0.25f);
	uint8_t b1 = b + (uint8_t)((255 - b) * 0.25f);
	layers[0] = (r1 << 24) | (g1 << 16) | (b1 << 8) | 0xff;

	// Darker shades (Shades)
	// Formula: current * (1 - percentage)
	float factors[] = { 0.75f, 0.70f, 0.50f }; // 25%, 30%, 50% darker

	for (int i = 0; i < 3; i++) 
	{
		uint8_t rn = (uint8_t)(r * factors[i]);
		uint8_t gn = (uint8_t)(g * factors[i]);
		uint8_t bn = (uint8_t)(b * factors[i]);
		layers[i + 1] = (rn << 24) | (gn << 16) | (bn << 8) | 0xff;
	}
}
