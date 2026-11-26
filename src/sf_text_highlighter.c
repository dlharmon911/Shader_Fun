#include "blaze.h"
#include "sf_text_highlighter.h"

enum SF_TOKEN_TYPE
{
	SF_TOKEN_TYPE_IDENTIFIER,
	SF_TOKEN_TYPE_NUMBER,
	SF_TOKEN_TYPE_OPERATOR,
	SF_TOKEN_TYPE_COMMENT,
	SF_TOKEN_TYPE_PREPROCESSOR,
	SF_TOKEN_TYPE_OTHER
};

enum SF_TEXT_COLOR
{
	SF_TEXT_COLOR_KEYWORD,
	SF_TEXT_COLOR_FUNCTION,
	SF_TEXT_COLOR_NUMBER,
	SF_TEXT_COLOR_OPERATOR,
	SF_TEXT_COLOR_COMMENT,
	SF_TEXT_COLOR_PREPROCESSOR,
	SF_TEXT_COLOR_UNKNOWN_IDENTIFIER,
	SF_TEXT_COLOR_SELECTED_TEXT,
	SF_TEXT_COLOR_DEFAULT,
	SF_TEXT_COLOR_COUNT
};

enum
{
	BLAZE_TEXT_KEYWORD_COUNT = 167,
	BLAZE_TEXT_FUNCTION_NAME_COUNT = 196,
	BLAZE_TEXT_OPERATOR_COUNT = 51
};

static const ALLEGRO_COLOR SF_TEXT_COLORS_LIGHT_MODE[SF_TEXT_COLOR_COUNT] =
{
	[SF_TEXT_COLOR_KEYWORD] = { 0.0f, 0.0f, 1.0f, 1.0f },
	[SF_TEXT_COLOR_FUNCTION] = { 0.6f, 0.0f, 1.0f, 1.0f },
	[SF_TEXT_COLOR_NUMBER] = { 1.0f, 0.0f, 0.0f, 1.0f },
	[SF_TEXT_COLOR_OPERATOR] = { 0.5f, 0.0f, 0.5f, 1.0f },
	[SF_TEXT_COLOR_COMMENT] = { 0.0f, 0.4f, 0.0f, 1.0f },
	[SF_TEXT_COLOR_PREPROCESSOR] = { 0.5f, 0.5f, 0.5f, 1.0f },
	[SF_TEXT_COLOR_UNKNOWN_IDENTIFIER] = { 0.35f, 0.2f, 0.35f, 1.0f },
	[SF_TEXT_COLOR_SELECTED_TEXT] = { 0.75f, 0.85f, 1.0f, 1.0f },
	[SF_TEXT_COLOR_DEFAULT] = { 0.0f, 0.0f, 0.0f, 1.0f }
};

static const ALLEGRO_COLOR SF_TEXT_COLORS_DARK_MODE[SF_TEXT_COLOR_COUNT] =
{
	[SF_TEXT_COLOR_KEYWORD] = { 0.5f, 0.5f, 1.0f, 1.0f },
	[SF_TEXT_COLOR_FUNCTION] = { 0.8f, 0.6f, 1.0f, 1.0f },
	[SF_TEXT_COLOR_NUMBER] = { 1.0f, 0.5f, 0.5f, 1.0f },
	[SF_TEXT_COLOR_OPERATOR] = { 1.0f, 0.5f, 1.0f, 1.0f },
	[SF_TEXT_COLOR_COMMENT] = { 0.5f, 1.0f, 0.5f, 1.0f },
	[SF_TEXT_COLOR_PREPROCESSOR] = { 0.7f, 0.7f, 0.7f, 1.0f },
	[SF_TEXT_COLOR_UNKNOWN_IDENTIFIER] = { 1.0f, 0.8f, 1.0f, 1.0f },
	[SF_TEXT_COLOR_SELECTED_TEXT] = { 0.75f, 0.85f, 1.0f, 1.0f },
	[SF_TEXT_COLOR_DEFAULT] = { 1.0f, 1.0f, 1.0f, 1.0f }
};

static const ALLEGRO_COLOR* SF_TEXT_COLORS = SF_TEXT_COLORS_LIGHT_MODE;

static const blz_stringview_t SF_GLSL_KEYWORD_NAME[BLAZE_TEXT_KEYWORD_COUNT] =
{
	{ 6, "active" },
	{ 3, "asm" },
	{ 9, "attribute" },
	{ 4, "bool" },
	{ 5, "break" },
	{ 5, "bvec2" },
	{ 5, "bvec3" },
	{ 5, "bvec4" },
	{ 4, "case" },
	{ 4, "cast" },
	{ 8, "centroid" },
	{ 5, "class" },
	{ 6, "common" },
	{ 5, "const" },
	{ 8, "continue" },
	{ 7, "default" },
	{ 7, "discard" },
	{ 2, "do" },
	{ 6, "double" },
	{ 5, "dvec2" },
	{ 5, "dvec3" },
	{ 5, "dvec4" },
	{ 4, "else" },
	{ 4, "enum" },
	{ 6, "extern" },
	{ 8, "external" },
	{ 5, "false" },
	{ 6, "filter" },
	{ 5, "fixed" },
	{ 4, "flat" },
	{ 5, "float" },
	{ 3, "for" },
	{ 5, "fvec2" },
	{ 5, "fvec3" },
	{ 5, "fvec4" },
	{ 4, "goto" },
	{ 4, "half" },
	{ 5, "highp" },
	{ 5, "hvec2" },
	{ 5, "hvec3" },
	{ 5, "hvec4" },
	{ 2, "if" },
	{ 8, "iimage1D" },
	{ 13, "iimage1DArray" },
	{ 8, "iimage2D" },
	{ 13, "iimage2DArray" },
	{ 8, "iimage3D" },
	{ 12, "iimageBuffer" },
	{ 10, "iimageCube" },
	{ 7, "image1D" },
	{ 12, "image1DArray" },
	{ 18, "image1DArrayShadow" },
	{ 13, "image1DShadow" },
	{ 7, "image2D" },
	{ 12, "image2DArray" },
	{ 18, "image2DArrayShadow" },
	{ 13, "image2DShadow" },
	{ 7, "image3D" },
	{ 11, "imageBuffer" },
	{ 9, "imageCube" },
	{ 2, "in" },
	{ 6, "inline" },
	{ 5, "inout" },
	{ 5, "input" },
	{ 3, "int" },
	{ 9, "interface" },
	{ 9, "invariant" },
	{ 10, "isampler1D" },
	{ 15, "isampler1DArray" },
	{ 10, "isampler2D" },
	{ 15, "isampler2DArray" },
	{ 12, "isampler2DMS" },
	{ 17, "isampler2DMSArray" },
	{ 14, "isampler2DRect" },
	{ 10, "isampler3D" },
	{ 14, "isamplerBuffer" },
	{ 12, "isamplerCube" },
	{ 5, "ivec2" },
	{ 5, "ivec3" },
	{ 5, "ivec4" },
	{ 6, "layout" },
	{ 4, "long" },
	{ 4, "lowp" },
	{ 4, "mat2" },
	{ 6, "mat2x2" },
	{ 6, "mat2x3" },
	{ 6, "mat2x4" },
	{ 4, "mat3" },
	{ 6, "mat3x2" },
	{ 6, "mat3x3" },
	{ 6, "mat3x4" },
	{ 4, "mat4" },
	{ 6, "mat4x2" },
	{ 6, "mat4x3" },
	{ 6, "mat4x4" },
	{ 7, "mediump" },
	{ 9, "namespace" },
	{ 8, "noinline" },
	{ 13, "noperspective" },
	{ 3, "out" },
	{ 6, "output" },
	{ 6, "packed" },
	{ 9, "partition" },
	{ 9, "precision" },
	{ 6, "public" },
	{ 6, "return" },
	{ 9, "row_major" },
	{ 9, "sampler1D" },
	{ 14, "sampler1DArray" },
	{ 20, "sampler1DArrayShadow" },
	{ 15, "sampler1DShadow" },
	{ 9, "sampler2D" },
	{ 14, "sampler2DArray" },
	{ 20, "sampler2DArrayShadow" },
	{ 11, "sampler2DMS" },
	{ 16, "sampler2DMSArray" },
	{ 13, "sampler2DRect" },
	{ 19, "sampler2DRectShadow" },
	{ 15, "sampler2DShadow" },
	{ 9, "sampler3D" },
	{ 13, "sampler3DRect" },
	{ 13, "samplerBuffer" },
	{ 11, "samplerCube" },
	{ 17, "samplerCubeShadow" },
	{ 5, "short" },
	{ 6, "sizeof" },
	{ 6, "smooth" },
	{ 6, "static" },
	{ 6, "struct" },
	{ 6, "superp" },
	{ 6, "switch" },
	{ 8, "template" },
	{ 4, "this" },
	{ 4, "true" },
	{ 7, "typedef" },
	{ 8, "uimage1D" },
	{ 13, "uimage1DArray" },
	{ 8, "uimage2D" },
	{ 13, "uimage2DArray" },
	{ 8, "uimage3D" },
	{ 12, "uimageBuffer" },
	{ 10, "uimageCube" },
	{ 4, "uint" },
	{ 7, "uniform" },
	{ 5, "union" },
	{ 8, "unsigned" },
	{ 10, "usampler1D" },
	{ 15, "usampler1DArray" },
	{ 10, "usampler2D" },
	{ 15, "usampler2DArray" },
	{ 12, "usampler2DMS" },
	{ 17, "usampler2DMSArray" },
	{ 14, "usampler2DRect" },
	{ 10, "usampler3D" },
	{ 14, "usamplerBuffer" },
	{ 12, "usamplerCube" },
	{ 5, "using" },
	{ 5, "uvec2" },
	{ 5, "uvec3" },
	{ 5, "uvec4" },
	{ 7, "varying" },
	{ 4, "vec2" },
	{ 4, "vec3" },
	{ 4, "vec4" },
	{ 4, "void" },
	{ 8, "volatile" },
	{ 5, "while" }
};

static const blz_stringview_t SF_GLSL_FUNCTION_NAME[BLAZE_TEXT_FUNCTION_NAME_COUNT] =
{
	{ 16, "EmitStreamVertex" },
	{ 10, "EmitVertex" },
	{ 12, "EndPrimitive" },
	{ 18, "EndStreamPrimitive" },
	{ 3, "abs" },
	{ 4, "acos" },
	{ 5, "acosh" },
	{ 3, "all" },
	{ 14, "allInvocations" },
	{ 19, "allInvocationsEqual" },
	{ 3, "any" },
	{ 13, "anyInvocation" },
	{ 4, "asin" },
	{ 5, "asinh" },
	{ 4, "atan" },
	{ 5, "atanh" },
	{ 9, "atomicAdd" },
	{ 9, "atomicAnd" },
	{ 14, "atomicCompSwap" },
	{ 13, "atomicCounter" },
	{ 16, "atomicCounterAdd" },
	{ 16, "atomicCounterAnd" },
	{ 21, "atomicCounterCompSwap" },
	{ 22, "atomicCounterDecrement" },
	{ 21, "atomicCounterExchange" },
	{ 22, "atomicCounterIncrement" },
	{ 16, "atomicCounterMax" },
	{ 16, "atomicCounterMin" },
	{ 15, "atomicCounterOr" },
	{ 21, "atomicCounterSubtract" },
	{ 16, "atomicCounterXor" },
	{ 14, "atomicExchange" },
	{ 9, "atomicMax" },
	{ 9, "atomicMin" },
	{ 8, "atomicOr" },
	{ 9, "atomicXor" },
	{ 7, "barrier" },
	{ 8, "bitCount" },
	{ 15, "bitfieldExtract" },
	{ 14, "bitfieldInsert" },
	{ 15, "bitfieldReverse" },
	{ 4, "ceil" },
	{ 5, "clamp" },
	{ 3, "cos" },
	{ 4, "cosh" },
	{ 5, "cross" },
	{ 4, "dFdx" },
	{ 10, "dFdxCoarse" },
	{ 8, "dFdxFine" },
	{ 4, "dFdy" },
	{ 10, "dFdyCoarse" },
	{ 8, "dFdyFine" },
	{ 7, "degrees" },
	{ 11, "determinant" },
	{ 8, "distance" },
	{ 3, "dot" },
	{ 5, "equal" },
	{ 3, "exp" },
	{ 4, "exp2" },
	{ 11, "faceforward" },
	{ 7, "findLSB" },
	{ 7, "findMSB" },
	{ 14, "floatBitsToInt" },
	{ 15, "floatBitsToUint" },
	{ 5, "floor" },
	{ 3, "fma" },
	{ 5, "fract" },
	{ 5, "frexp" },
	{ 10, "ftransform" },
	{ 6, "fwidth" },
	{ 12, "fwidthCoarse" },
	{ 10, "fwidthFine" },
	{ 11, "greaterThan" },
	{ 16, "greaterThanEqual" },
	{ 18, "groupMemoryBarrier" },
	{ 14, "imageAtomicAdd" },
	{ 14, "imageAtomicAnd" },
	{ 19, "imageAtomicCompSwap" },
	{ 19, "imageAtomicExchange" },
	{ 14, "imageAtomicMax" },
	{ 14, "imageAtomicMin" },
	{ 13, "imageAtomicOr" },
	{ 14, "imageAtomicXor" },
	{ 9, "imageLoad" },
	{ 12, "imageSamples" },
	{ 9, "imageSize" },
	{ 10, "imageStore" },
	{ 12, "imulExtended" },
	{ 14, "intBitsToFloat" },
	{ 21, "interpolateAtCentroid" },
	{ 19, "interpolateAtOffset" },
	{ 19, "interpolateAtSample" },
	{ 7, "inverse" },
	{ 11, "inversesqrt" },
	{ 5, "isinf" },
	{ 5, "isnan" },
	{ 5, "ldexp" },
	{ 6, "length" },
	{ 8, "lessThan" },
	{ 13, "lessThanEqual" },
	{ 3, "log" },
	{ 4, "log2" },
	{ 14, "matrixCompMult" },
	{ 3, "max" },
	{ 13, "memoryBarrier" },
	{ 26, "memoryBarrierAtomicCounter" },
	{ 19, "memoryBarrierBuffer" },
	{ 18, "memoryBarrierImage" },
	{ 19, "memoryBarrierShared" },
	{ 3, "min" },
	{ 3, "mix" },
	{ 3, "mod" },
	{ 4, "modf" },
	{ 6, "noise1" },
	{ 6, "noise2" },
	{ 6, "noise3" },
	{ 6, "noise4" },
	{ 9, "normalize" },
	{ 3, "not" },
	{ 8, "notEqual" },
	{ 12, "outerProduct" },
	{ 14, "packDouble2x32" },
	{ 12, "packHalf2x16" },
	{ 13, "packSnorm2x16" },
	{ 12, "packSnorm4x8" },
	{ 13, "packUnorm2x16" },
	{ 12, "packUnorm4x8" },
	{ 3, "pow" },
	{ 7, "radians" },
	{ 7, "reflect" },
	{ 7, "refract" },
	{ 5, "round" },
	{ 9, "roundEven" },
	{ 8, "shadow1D" },
	{ 11, "shadow1DLod" },
	{ 12, "shadow1DProj" },
	{ 15, "shadow1DProjLod" },
	{ 8, "shadow2D" },
	{ 11, "shadow2DLod" },
	{ 12, "shadow2DProj" },
	{ 15, "shadow2DProjLod" },
	{ 4, "sign" },
	{ 3, "sin" },
	{ 4, "sinh" },
	{ 10, "smoothstep" },
	{ 4, "sqrt" },
	{ 4, "step" },
	{ 3, "tan" },
	{ 4, "tanh" },
	{ 10, "texelFetch" },
	{ 16, "texelFetchOffset" },
	{ 7, "texture" },
	{ 9, "texture1D" },
	{ 12, "texture1DLod" },
	{ 13, "texture1DProj" },
	{ 16, "texture1DProjLod" },
	{ 9, "texture2D" },
	{ 12, "texture2DLod" },
	{ 13, "texture2DProj" },
	{ 16, "texture2DProjLod" },
	{ 9, "texture3D" },
	{ 12, "texture3DLod" },
	{ 13, "texture3DProj" },
	{ 16, "texture3DProjLod" },
	{ 11, "textureCube" },
	{ 14, "textureCubeLod" },
	{ 13, "textureGather" },
	{ 19, "textureGatherOffset" },
	{ 20, "textureGatherOffsets" },
	{ 11, "textureGrad" },
	{ 17, "textureGradOffset" },
	{ 10, "textureLod" },
	{ 16, "textureLodOffset" },
	{ 13, "textureOffset" },
	{ 11, "textureProj" },
	{ 15, "textureProjGrad" },
	{ 21, "textureProjGradOffset" },
	{ 14, "textureProjLod" },
	{ 20, "textureProjLodOffset" },
	{ 17, "textureProjOffset" },
	{ 18, "textureQueryLevels" },
	{ 15, "textureQueryLod" },
	{ 14, "textureSamples" },
	{ 11, "textureSize" },
	{ 9, "transpose" },
	{ 5, "trunc" },
	{ 9, "uaddCarry" },
	{ 15, "uintBitsToFloat" },
	{ 12, "umulExtended" },
	{ 16, "unpackDouble2x32" },
	{ 14, "unpackHalf2x16" },
	{ 15, "unpackSnorm2x16" },
	{ 14, "unpackSnorm4x8" },
	{ 15, "unpackUnorm2x16" },
	{ 14, "unpackUnorm4x8" },
	{ 10, "usubBorrow" }
}
;

static const blz_stringview_t SF_GLSL_OPERATOR_NAME[BLAZE_TEXT_OPERATOR_COUNT] =
{
	// 3-character operators
	{ 3, "<<=" },{ 3, ">>=" },

	// 2-character operators
	{ 2, "++" }, { 2, "--" }, { 2, "+=" }, { 2, "-=" }, { 2, "*=" },
	{ 2, "/=" }, { 2, "%=" }, { 2, "==" }, { 2, "!=" }, { 2, "<=" },
	{ 2, ">=" }, { 2, "&&" }, { 2, "||" }, { 2, "^^" }, { 2, "&=" },
	{ 2, "|=" }, { 2, "^=" }, { 2, "?:" }, { 2, "<<" }, { 2, ">>" },
	{ 2, "->" }, { 2, "<<" }, { 2, ">>" }, { 2, "()" }, { 2, "[]" },
	{ 2, "{}" },
	
	// 1-character operators
	{ 1, "+" }, { 1, "-" }, { 1, "*" }, { 1, "/" }, { 1, "%" }, 
	{ 1, "=" },	{ 1, "<" }, { 1, ">" }, { 1, "!" }, { 1, "&" }, 
	{ 1, "|" }, { 1, "^" }, { 1, "." }, { 1, "(" }, { 1, ")" }, 
	{ 1, "[" }, { 1, "]" }, { 1, "{" },	{ 1, "}" }, { 1, "," }, 
	{ 1, ";" }, { 1, ":" }, { 1, "~" }
};

typedef struct blz_text_token_tag_t blz_text_token_t;
typedef struct sf_text_highlighter_data_tag_t sf_text_highlighter_data_t;

struct blz_text_token_tag_t
{
	blz_stringview_t m_value;
	int32_t m_type;
};

struct sf_text_highlighter_data_tag_t
{
	bool m_multiline_comment;
};

static bool _blz_text_is_stringview_array(blz_stringview_t text, const blz_stringview_t* data, size_t count)
{
	if (!text.m_length || !data || !count)
	{
		return false;
	}

	for (size_t i = 0; i < count && text.m_data[0] >= data[i].m_data[0]; ++i)
	{
		if (blz_stringview_equals(data[i], text))
		{
			return true;
		}
	}

	return false;
}

static blz_stringview_t _blz_text_parse_identifier(blz_stringview_t line, blz_text_token_t* token)
{
	token->m_type = SF_TOKEN_TYPE_IDENTIFIER;
	token->m_value.m_data = line.m_data;
	token->m_value.m_length = 0;

	while (true)
	{
		char c = line.m_data[0];

		if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9') || c == '_')
		{
			line.m_data++;
			line.m_length--;
			token->m_value.m_length++;
		}
		else
		{
			break;
		}
	}

	return line;
}

static blz_stringview_t _blz_text_parse_number(blz_stringview_t line, blz_text_token_t* token)
{
	token->m_type = SF_TOKEN_TYPE_NUMBER;
	token->m_value.m_data = line.m_data;
	token->m_value.m_length = 0;

	char* endptr = NULL;

	float f = strtof(line.m_data, &endptr);

	(void)f;

	int32_t count = (int32_t)(endptr - line.m_data);

	line.m_data += count;
	line.m_length -= count;
	token->m_value.m_length = (size_t)count;

	return line;
}

static blz_stringview_t _blz_text_parse_operator(blz_stringview_t line, blz_text_token_t* token)
{
	token->m_type = SF_TOKEN_TYPE_OPERATOR;
	token->m_value.m_data = line.m_data;
	token->m_value.m_length = 0;

	for (int32_t i = 0; i < BLAZE_TEXT_OPERATOR_COUNT; ++i)
	{
		blz_stringview_t op = SF_GLSL_OPERATOR_NAME[i];

		if (line.m_length < op.m_length)
		{
			continue;
		}

		blz_stringview_t prefix = { op.m_length, line.m_data };

		if (0 == blz_stringview_compare(op, prefix))
		{
			line.m_data += op.m_length;
			line.m_length -= op.m_length;
			token->m_value.m_length = op.m_length;
			break;
		}
	}

	return line;
}

static blz_stringview_t _blz_text_parse_comment(blz_stringview_t line, blz_text_token_t* token, bool* multiline_comment)
{
	token->m_type = SF_TOKEN_TYPE_COMMENT;
	token->m_value.m_data = line.m_data;
	token->m_value.m_length = 0;

	if (*multiline_comment)
	{
		// we are already in a multiline comment, just find the end of it
		for (int32_t i = 0; i < line.m_length; ++i)
		{
			if (line.m_data[i] == '*' && (size_t)(i + 1) < line.m_length && line.m_data[i + 1] == '/')
			{
				*multiline_comment = false;
				line.m_data += i + 2;
				line.m_length -= (size_t)(i + 2);
				token->m_value.m_length = (size_t)(i + 2);
				return line;
			}
		}
	}

	token->m_value.m_length = line.m_length;
	line.m_data += line.m_length;
	line.m_length = 0;

	return line;
}

static blz_stringview_t _blz_text_parse_preprocessor(blz_stringview_t line, blz_text_token_t* token)
{
	token->m_type = SF_TOKEN_TYPE_PREPROCESSOR;
	token->m_value.m_data = line.m_data;
	token->m_value.m_length = 0;
	while (line.m_length)
	{
		char c = line.m_data[0];

		if (c == '/' && line.m_length > 0 && (line.m_data[0] == '/' || line.m_data[0] == '*'))
		{
			break;
		}

		line.m_data++;
		line.m_length--;

		if (c == BLAZE_NEWLINE_CHAR)
		{
			break;
		}
		token->m_value.m_length++;
	}

	return line;
}

static blz_stringview_t _blz_text_get_token(blz_stringview_t line, blz_text_token_t* token, bool* multiline_comment)
{
	if (!token || !line.m_data || !line.m_length)
	{
		return line;
	}

	char c = line.m_data[0];

	if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_')
	{
		return _blz_text_parse_identifier(line, token);
	}
	else if ((c >= '0' && c <= '9') || 
		(c == '-' && line.m_length > 1 && (line.m_data[1] >= '0' && line.m_data[1] <= '9')) ||
		(c == '.' && line.m_length > 1 && (line.m_data[1] >= '0' && line.m_data[1] <= '9')))
	{
		return _blz_text_parse_number(line, token);
	}
	else if (c == '/' && line.m_length > 1 && (line.m_data[1] == '/' || line.m_data[1] == '*'))
	{
		if (line.m_data[1] == '*')
		{
			*multiline_comment = true;
		}

		return _blz_text_parse_comment(line, token, multiline_comment);
	}
	else if (c == '#' && line.m_length > 1)
	{
		return _blz_text_parse_preprocessor(line, token);
	}
	else if (c == '+' || c == '-' || c == '*' || c == '/' || c == '%' ||
		c == '=' || c == '<' || c == '>' || c == '!' || c == '&' ||
		c == '|' || c == '^')
	{
		return _blz_text_parse_operator(line, token);
	}

	token->m_type = SF_TOKEN_TYPE_OTHER;
	token->m_value.m_data = line.m_data;
	token->m_value.m_length = 1;

	return (blz_stringview_t) { line.m_length - 1, line.m_data + 1 };
}

static void _blz_text_parse_multiline_comment(const blz_font_t* font, blz_stringview_t* line, blz_vec2f_t position, int32_t* offset, const blz_text_info_t* info, bool* multiline_comment)
{
	size_t line_length = line->m_length;

	// this is an already commented line, we just need to find the end of the comment if any and draw it
	for (int32_t i = 0; i < line->m_length; ++i)
	{
		if (line->m_data[i] == '*' && i + 1 < (int32_t)line->m_length && line->m_data[i + 1] == '/')
		{
			*multiline_comment = false;
			line_length = (size_t)(i + 2);
			break;
		}
	}

	blz_text_draw_line_tab_delimited(font, *line, position, offset, SF_TEXT_COLORS[SF_TEXT_COLOR_COMMENT]);

	line->m_data += line_length;
	line->m_length -= line_length;
}

static void sf_text_highlighter_begin(const blz_font_t* font, const blz_text_t* text, blz_vec2f_t position, blz_sizef_t size, const blz_text_info_t* info, void* data)
{
	if (!font || !text || !info || !data)
	{
		return;
	}

	sf_text_highlighter_data_t* hdata = (sf_text_highlighter_data_t*)data;

	hdata->m_multiline_comment = false;
}

static void sf_text_highlighter_finish(const blz_font_t* font, const blz_text_t* text, blz_vec2f_t position, blz_sizef_t size, const blz_text_info_t* info, void* data)
{
	if (!font || !text || !info || !data)
	{
		return;
	}

	sf_text_highlighter_data_t* hdata = (sf_text_highlighter_data_t*)data;

	hdata->m_multiline_comment = false;
}

static void sf_text_highlighter_per_line(const blz_font_t* font, blz_stringview_t line, blz_vec2f_t position, const blz_text_info_t* info, void* data)
{
	blz_text_token_t token = { 0 };
	sf_text_highlighter_data_t* hdata = (sf_text_highlighter_data_t*)data;
	int32_t offset = 0;

	if (hdata->m_multiline_comment)
	{
		_blz_text_parse_multiline_comment(font, &line, position, &offset, info, &hdata->m_multiline_comment);
	}

	while (line.m_length)
	{
		size_t color = SF_TEXT_COLOR_DEFAULT;
		line = _blz_text_get_token(line, &token, &hdata->m_multiline_comment);

		switch (token.m_type)
		{
		case SF_TOKEN_TYPE_IDENTIFIER:
		{
			if (_blz_text_is_stringview_array(token.m_value, SF_GLSL_KEYWORD_NAME, BLAZE_TEXT_KEYWORD_COUNT))
			{
				color = SF_TEXT_COLOR_KEYWORD;
			}
			else if (_blz_text_is_stringview_array(token.m_value, SF_GLSL_FUNCTION_NAME, BLAZE_TEXT_FUNCTION_NAME_COUNT))
			{
				color = SF_TEXT_COLOR_FUNCTION;
			}
			else
			{
				color = SF_TEXT_COLOR_UNKNOWN_IDENTIFIER;
			}
		} break;
		case SF_TOKEN_TYPE_NUMBER:
		{
			color = SF_TEXT_COLOR_NUMBER;
		} break;
		case SF_TOKEN_TYPE_COMMENT:
		{
			color = SF_TEXT_COLOR_COMMENT;
		} break;
		case SF_TOKEN_TYPE_OPERATOR:
		{
			color = SF_TEXT_COLOR_OPERATOR;
		} break;
		case SF_TOKEN_TYPE_PREPROCESSOR:
		{
			color = SF_TEXT_COLOR_PREPROCESSOR;
		} break;
		default: break;
		}

		if (token.m_value.m_length)
		{
			blz_text_draw_line_tab_delimited(font, token.m_value, position, &offset, SF_TEXT_COLORS[color]);
		}
	}
}

const blz_text_highlighter_t* sf_text_highlighter_get(void)
{
	static sf_text_highlighter_data_t _sf_text_highlighter_data = 
	{
		.m_multiline_comment = false
	};

	static blz_text_highlighter_t sf_text_highlighter =
	{
		.m_begin = sf_text_highlighter_begin,
		.m_per_line = sf_text_highlighter_per_line,
		.m_finish = sf_text_highlighter_finish,
		.m_user_data = &_sf_text_highlighter_data
	};

	return &sf_text_highlighter;
}
