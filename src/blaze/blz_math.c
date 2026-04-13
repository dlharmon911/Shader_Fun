#include "blaze/blz_math.h"

#define BLAZE_EPSILON 1e-6

bool blz_math_is_zero(double d)
{
    return fabs(d) < BLAZE_EPSILON;
}

bool blz_math_is_zero_f(float f)
{
    return fabsf(f) < (float)BLAZE_EPSILON;
}

bool blz_math_is_equal(double a, double b)
{
    return blz_math_is_zero(a - b);
}

bool blz_math_is_equal_f(float a, float b)
{
    return blz_math_is_zero_f(a - b);
}
