#include "blaze.h"

#define BLAZE_SHADER_VERSION_STR "#version 330 core"

static const char SF_SHADER_VERTEX_STRING[] =
BLAZE_SHADER_VERSION_STR BLAZE_NEWLINE_STR
BLAZE_NEWLINE_STR
"in vec4 al_pos;" BLAZE_NEWLINE_STR
"uniform mat4 al_projview_matrix;" BLAZE_NEWLINE_STR
"out vec4 gl_Position;" BLAZE_NEWLINE_STR
"out vec3 u_position;" BLAZE_NEWLINE_STR
BLAZE_NEWLINE_STR
"void main()" BLAZE_NEWLINE_STR
"{" BLAZE_NEWLINE_STR
BLAZE_TAB_STR "gl_Position = al_projview_matrix * al_pos;" BLAZE_NEWLINE_STR
"}" BLAZE_NEWLINE_STR;

static const char SF_SHADER_PIXEL_STRING_PREFIX[] =
BLAZE_SHADER_VERSION_STR BLAZE_NEWLINE_STR
BLAZE_NEWLINE_STR
"out vec4 gl_FragColor;" BLAZE_NEWLINE_STR;

const char glsl_suffix_code[] =
BLAZE_NEWLINE_STR "void main()" BLAZE_NEWLINE_STR
"{" BLAZE_NEWLINE_STR
BLAZE_TAB_STR "vec2 fragCoord = (gl_FragCoord.xy - u_position);" BLAZE_NEWLINE_STR
BLAZE_TAB_STR "fragCoord.y = u_resolution.y - (u_world.y - fragCoord.y);" BLAZE_NEWLINE_STR
BLAZE_TAB_STR "mainImage(gl_FragColor, fragCoord);" BLAZE_NEWLINE_STR
"}" BLAZE_NEWLINE_STR;
