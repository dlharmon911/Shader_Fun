#ifndef _GUARD_BLAZE_SHAPES_H
#define _GUARD_BLAZE_SHAPES_H

#include "blaze/blz_common.h"

typedef struct blz_sizei_tag_t blz_sizei_t;
typedef struct blz_vec2i_tag_t blz_vec2i_t;
typedef struct blz_sizef_tag_t blz_sizef_t;
typedef struct blz_vec2f_tag_t blz_vec2f_t;
typedef struct blz_vec3f_tag_t blz_vec3f_t;
typedef struct blz_vec4f_tag_t blz_vec4f_t;
typedef struct blz_circle_tag_t blz_circle_t;
typedef struct blz_rectangle_tag_t blz_rectangle_t;
typedef struct blz_box_tag_t blz_box_t;

struct blz_sizei_tag_t
{
	size_t m_width;
	size_t m_height;
};

struct blz_vec2i_tag_t
{
	int32_t m_x;
	int32_t m_y;
};

struct blz_sizef_tag_t
{
	float m_width;
	float m_height;
};

struct blz_vec2f_tag_t
{
	float m_x;
	float m_y;
};

struct blz_vec3f_tag_t
{
	union
	{
		struct blz_vec2f_tag_t m_vec2;
		struct
		{
			float m_x;
			float m_y;
		};
	};
	float m_z;
};

struct blz_vec4f_tag_t
{
	union
	{
		struct blz_vec3f_tag_t m_vec3;
		struct
		{
			union
			{
				struct blz_vec2f_tag_t m_vec2;
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
};

struct blz_circle_tag_t
{
	struct blz_vec2f_tag_t m_center;
	float m_radius;
};

struct blz_rectangle_tag_t
{
	struct blz_vec2f_tag_t m_position;
	struct blz_sizef_tag_t m_size;
};

struct blz_box_tag_t
{
	struct blz_vec2f_tag_t m_top_left;
	struct blz_vec2f_tag_t m_bottom_right;
};

#endif // !_GUARD_BLAZE_SHAPES_H
