#ifndef _GUARD_BLAZE_SHAPES_H
#define _GUARD_BLAZE_SHAPES_H

#include "blaze/blz_common.h"

typedef struct blz_sizei_tag_t
{
	size_t m_width;
	size_t m_height;
} blz_sizei_t;

typedef struct blz_vec2i_tag_t
{
	int32_t m_x;
	int32_t m_y;
} blz_vec2i_t;

typedef struct blz_sizef_tag_t
{
	float m_width;
	float m_height;
} blz_sizef_t;

typedef struct blz_vec2f_tag_t
{
	float m_x;
	float m_y;
} blz_vec2f_t;

typedef struct blz_vec3f_tag_t
{
	union
	{
		blz_vec2f_t m_vec2;
		struct
		{
			float m_x;
			float m_y;
		};
	};
	float m_z;
} blz_vec3f_t;

typedef struct blz_vec4f_tag_t
{
	union
	{
		blz_vec3f_t m_vec3;
		struct
		{
			union
			{
				blz_vec2f_t m_vec2;
				struct
				{
					float m_x;
					float m_y;
				};
			};
			float m_z;
		};
	};
	float m_w;
} blz_vec4f_t;

typedef struct blz_circle_tag_t
{
	blz_vec2f_t m_center;
	float m_radius;
} blz_circle_t;

typedef struct blz_rectangle_tag_t
{
	blz_vec2f_t m_position;
	blz_sizef_t m_size;
} blz_rectangle_t;

typedef struct blz_box_tag_t
{
	blz_vec2f_t m_top_left;
	blz_vec2f_t m_bottom_right;
} blz_box_t;

#endif // !_GUARD_BLAZE_SHAPES_H
