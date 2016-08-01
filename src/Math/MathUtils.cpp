﻿
#include <math.h>
#include <float.h>

// 標準ヘッダの math.h と被るので Utils を付けている
#include "../../include/Lumino/Math/MathUtils.h"

LN_NAMESPACE_BEGIN

//==============================================================================
// Math
//==============================================================================

const float Math::PI		= 3.1415926535897932384626433832795f;
const float Math::PIOver2	= 1.5708f;

//------------------------------------------------------------------------------
unsigned int Math::NextPow2(unsigned int value)
{
	unsigned int i = 1;
	while (value > (i <<= 1)) {
		if (!i) break;
	}
	return i;
}

//------------------------------------------------------------------------------
bool Math::EqualsFloat(float value1, float value2)
{
	return ((value2 - FLT_EPSILON) < value1) && (value1 < (value2 + FLT_EPSILON));
}

//------------------------------------------------------------------------------
bool Math::IsNaN(float value)
{
#ifdef _MSC_VER
	return (_isnan(value) != 0);
#else
	return (isnan(value) != 0);
#endif
}

//------------------------------------------------------------------------------
bool Math::IsNaN(double value)
{
#ifdef _MSC_VER
	return (_isnan(value) != 0);
#else
	return (isnan(value) != 0);
#endif
}

//------------------------------------------------------------------------------
bool Math::IsInf(float value)
{
#ifdef _MSC_VER
	return (_isnan(value) == 0 && _finite(value) == 0);
#else
	return (isinf(value) != 0);
#endif
}

//------------------------------------------------------------------------------
bool Math::IsInf(double value)
{
#ifdef _MSC_VER
	return (_isnan(value) == 0 && _finite(value) == 0);
#else
	return (isinf(value) != 0);
#endif
}

//------------------------------------------------------------------------------
float Math::Lerp(float v1, float v2, float t)
{
	return v1 + ((v2 - v1) * t);
}

//------------------------------------------------------------------------------
float Math::QuadAccel(float p, float v, float a, float t)
{
	return p + (v * t) + (0.5f * a * t * t);
}

//------------------------------------------------------------------------------
float Math::Hermite(float v1, float a1, float v2, float a2, float t)
{
	float a = 2.f * (v1 - v2) + (a1 + a2);
	float b = 3.f * (v2 - v1) - (2.f * a1) - a2;
	float r = a;
	r *= t;
	r += b;
	r *= t;
	r += a1;
	r *= t;
	return r + v1;
}

//------------------------------------------------------------------------------
float Math::CatmullRom(float v1, float v2, float v3, float v4, float t)
{
	float d1 = (v3 - v1) * 0.5f;
	float d2 = (v4 - v2) * 0.5f;
	return (2.0f*v2 - 2.0f*v3 + d1 + d2)*t*t*t + (-3.0f*v2 + 3.0f*v3 - 2.0f*d1 - d2)*t*t + d1*t + v2;
}

LN_NAMESPACE_END
