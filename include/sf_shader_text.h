#include "blaze.h"

#define BLAZE_SHADER_VERSION_STR "#version 330 core"

static const char SF_SHADER_VERTEX_STRING[] =
BLAZE_SHADER_VERSION_STR "\n"
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

static const char SF_SHADER_PIXEL_STRING_PREFIX[] =
BLAZE_SHADER_VERSION_STR "\n"
"\n"
"out vec4 gl_FragColor;\n";

const char glsl_suffix_code[] =
"\n" "void main()\n"
"{\n"
"\tvec2 fragCoord = (gl_FragCoord.xy - u_position);\n"
"\tfragCoord.y = u_resolution.y - (u_world.y - fragCoord.y);\n"
"\tmainImage(gl_FragColor, fragCoord);\n"
"}\n";
