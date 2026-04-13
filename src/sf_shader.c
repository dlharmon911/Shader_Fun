#include "sf_shader.h"

static const char* SF_SHADER_VERTEX_STRING = 
"#version 330 core\n"
"\n"
"in vec4 al_pos;\n"
"uniform mat4 al_projview_matrix;\n"
"out vec4 gl_Position;\n"
"out vec3 u_position;\n"
"\n"
"void main()\n"
"{\n"
"\tgl_Position = al_projview_matrix * al_pos;\n"
"}\n";

static const char* SF_SHADER_PIXEL_STRING_PREFIX =
"#version 330 core\n"
"\n"
"out vec4 gl_FragColor;\n";

static const char* sf_uniform_name_strs[SF_UNIFORM_TYPE_COUNT] = 
{
	"bool",
	"int",
	"float",
	"ivec2",
	"ivec3",
	"ivec4",
	"vec2",
	"vec3",
	"vec4",
	"mat4"
};

const char* glsl_suffix_code =
"\nvoid main()\n"
"{\n"
"    vec2 fragCoord = (gl_FragCoord.xy - u_position);\n"
"    fragCoord.y = u_resolution.y - (u_world.y - fragCoord.y);\n"
"    mainImage(gl_FragColor, fragCoord);\n"
"}\n\n";

static int32_t sf_shader_generate_uniform_text(ALLEGRO_USTR* text, const sf_uniform_t* uniform)
{
	if (!text || !uniform)
	{
		return -1;
	}

	if (!al_ustr_append_cstr(text, "uniform ") ||
		!al_ustr_append_cstr(text, sf_uniform_name_strs[uniform->m_type]) ||
		!al_ustr_append_cstr(text, " ") ||
		!al_ustr_append_cstr(text, uniform->m_name) || 
		!al_ustr_append_cstr(text, ";\n"))
	{
		return -1;
	}

	return 0;
}

static blz_stringview_t sf_get_next_line(blz_stringview_t view, blz_stringview_t* line)
{
	if (!line)
	{
		return view;
	}

	line->m_data = view.m_data;
	line->m_length = 0;

	while (line->m_length < view.m_length)
	{
		char c = view.m_data[line->m_length];

		++line->m_length;

		if (c == '\n')
		{
			return blz_stringview_ltrim(view, line->m_length);
		}
	}

	return view;

}

static int32_t sf_shader_process_line(ALLEGRO_USTR* text, blz_stringview_t line)
{
	if (!text || !line.m_data)
	{
		return -1;
	}

	for (size_t i = 0; i < line.m_length; ++i)
	{
		int32_t c = (int32_t)line.m_data[i];
		if (c == '\\' && i + 1 < line.m_length)
		{
			int32_t next_c = (int32_t)line.m_data[i + 1];

			if (next_c == 't')
			{
				c = '\t';
				++i;
			}
			else if (next_c == 'n')
			{
				c = '\n';
				++i;
			}
			else
			{
				return -1;
			}
		}

		if (!al_ustr_append_chr(text, c))
		{
			return -1;
		}
		
	}

	return 0;
}

static int32_t sf_shader_generate_text(ALLEGRO_USTR* text, blz_stringview_t view, const sf_uniform_t* uniform)
{
	if (!text || !view.m_data || !uniform)
	{
		return -1;
	}

	size_t uniform_count = blz_darray_size(uniform);

	for (size_t i = 0; i < uniform_count; ++i)
	{
		const sf_uniform_t* current_uniform = (const sf_uniform_t*)blz_darray_at_const(uniform, i);
		if (sf_shader_generate_uniform_text(text, current_uniform) != 0)
		{
			return -1;
		}
	}

	if (!al_ustr_append_cstr(text, "\n"))
	{
		return -1;
	}

	while (view.m_length)
	{
		blz_stringview_t line = { 0,0 };
		view = sf_get_next_line(view, &line);

		if (line.m_length && sf_shader_process_line(text, line) != 0)
		{
			return -1;
		}
	}

	if (!al_ustr_append_cstr(text, glsl_suffix_code))
	{
		return -1;
	}

	return 0;
}

static int32_t _sf_shader_generate(ALLEGRO_SHADER** shader, ALLEGRO_USTR** text, const char* text_str, const sf_uniform_t* uniform)
{
	if (!shader || !text || !text_str || !uniform)
	{
		return -1;
	}
	
	*text = al_ustr_new(SF_SHADER_PIXEL_STRING_PREFIX);

	if (!*text)
	{
		return -1;
	}

	blz_stringview_t text_view = { text_str, strlen(text_str) };

	if (sf_shader_generate_text(*text, text_view, uniform) != 0)
	{
		return -1;
	}

	(*shader) = al_create_shader(ALLEGRO_SHADER_GLSL);

	if (!(*shader))
	{
		return -1;
	}

	if (!al_attach_shader_source((*shader), ALLEGRO_VERTEX_SHADER, SF_SHADER_VERTEX_STRING))
	{
		return -1;
	}

	const char* text_cstr = al_cstr(*text);

	if (!al_attach_shader_source((*shader), ALLEGRO_PIXEL_SHADER, text_cstr))
	{
		return -1;
	}

	if (al_build_shader(*shader) == 0)
	{
		return -1;
	}

	return 0;
}

ALLEGRO_SHADER* sf_shader_generate(const ALLEGRO_USTR* text_str, const sf_uniform_t* uniform)
{
	ALLEGRO_USTR* text = NULL;
	ALLEGRO_SHADER* shader = NULL;

	if (_sf_shader_generate(&shader, &text, al_cstr(text_str), uniform) != 0)
	{
		if (shader)
		{
			al_destroy_shader(shader);
			shader = NULL;
		}

		if (text)
		{
			al_ustr_free(text);
			text = NULL;
		}
	}

	return shader;
}

void sf_shader_render(ALLEGRO_SHADER* shader, sf_uniform_t* uniform, blz_vec2f_t position, blz_sizef_t resolution)
{
	ALLEGRO_SHADER* current_shader = al_get_current_shader();
	ALLEGRO_BITMAP* target = al_get_target_bitmap();
	blz_sizef_t world = { (float)al_get_bitmap_width(target), (float)al_get_bitmap_height(target) };
	sf_uniform_t* u_position = sf_uniform_get(uniform, "u_position");
	sf_uniform_t* u_world = sf_uniform_get(uniform, "u_world");

	al_use_shader(shader);

	sf_uniform_set_float_vec(u_position, &position.m_x, 2);
	sf_uniform_set_float_vec(u_world, &world.m_width, 2);

	sf_uniform_update_shader(uniform, shader);

	if (blz_math_is_equal_f(world.m_width, resolution.m_width) && blz_math_is_equal_f(world.m_height, resolution.m_height))
	{
		al_set_shader_float_vector("u_resolution", 2, &world.m_width, 1);
	}

	al_draw_filled_rectangle(position.m_x, position.m_y, position.m_x + resolution.m_width, position.m_y + resolution.m_height, (ALLEGRO_COLOR) { 1.0f, 1.0f, 1.0f, 1.0f });

	al_use_shader(current_shader);
}
