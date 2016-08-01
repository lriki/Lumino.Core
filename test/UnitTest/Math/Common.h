#pragma once

#include <gtest/gtest.h>
#include <limits.h>
#include <math.h>
#include <LuminoMath.h>
using namespace ln;

// float �̉��Z���@���R���p�C���ɂ���ĈقȂ�A���ʂ������ɈႭ�Ȃ�B
// ���̍����z�����邽�߁A��r���錅�����w�肷��B
// (��̓I�ɂǂ��قȂ�̂��͒������BVS2010 �� VS2013 �ł� Vector3::Project() �̌덷����ԑ傫������)
#define LN_FLOAT_THRESHOLD 0.0001/*0.000001*/

#define ASSERT_VEC2_NEAR(x_, y_, v) \
	ASSERT_NEAR(x_, v.x, LN_FLOAT_THRESHOLD); \
	ASSERT_NEAR(y_, v.y, LN_FLOAT_THRESHOLD);

#define ASSERT_VEC3_NEAR(x_, y_, z_, v) \
	ASSERT_NEAR(x_, v.x, LN_FLOAT_THRESHOLD); \
	ASSERT_NEAR(y_, v.y, LN_FLOAT_THRESHOLD); \
	ASSERT_NEAR(z_, v.z, LN_FLOAT_THRESHOLD);

#define ASSERT_VEC4_NEAR(x_, y_, z_, w_, v) \
	ASSERT_NEAR(x_, v.x, LN_FLOAT_THRESHOLD); \
	ASSERT_NEAR(y_, v.y, LN_FLOAT_THRESHOLD); \
	ASSERT_NEAR(z_, v.z, LN_FLOAT_THRESHOLD); \
	ASSERT_NEAR(w_, v.w, LN_FLOAT_THRESHOLD);

#define ASSERT_QUA_NEAR(x_, y_, z_, w_, q) \
	ASSERT_NEAR(x_, q.x, LN_FLOAT_THRESHOLD); \
	ASSERT_NEAR(y_, q.y, LN_FLOAT_THRESHOLD); \
	ASSERT_NEAR(z_, q.z, LN_FLOAT_THRESHOLD); \
	ASSERT_NEAR(w_, q.w, LN_FLOAT_THRESHOLD);

#define ASSERT_MAT_NEAR(M11_,M12_,M13_,M14_, M21_,M22_,M23_,M24_, M31_,M32_,M33_,M34_, M41_,M42_,M43_,M44_, M) \
	ASSERT_NEAR(M11_, M.m11, LN_FLOAT_THRESHOLD); \
	ASSERT_NEAR(M12_, M.m12, LN_FLOAT_THRESHOLD); \
	ASSERT_NEAR(M13_, M.m13, LN_FLOAT_THRESHOLD); \
	ASSERT_NEAR(M14_, M.m14, LN_FLOAT_THRESHOLD); \
	ASSERT_NEAR(M21_, M.m21, LN_FLOAT_THRESHOLD); \
	ASSERT_NEAR(M22_, M.m22, LN_FLOAT_THRESHOLD); \
	ASSERT_NEAR(M23_, M.m23, LN_FLOAT_THRESHOLD); \
	ASSERT_NEAR(M24_, M.m24, LN_FLOAT_THRESHOLD); \
	ASSERT_NEAR(M31_, M.m31, LN_FLOAT_THRESHOLD); \
	ASSERT_NEAR(M32_, M.m32, LN_FLOAT_THRESHOLD); \
	ASSERT_NEAR(M33_, M.m33, LN_FLOAT_THRESHOLD); \
	ASSERT_NEAR(M34_, M.m34, LN_FLOAT_THRESHOLD); \
	ASSERT_NEAR(M41_, M.m41, LN_FLOAT_THRESHOLD); \
	ASSERT_NEAR(M42_, M.m42, LN_FLOAT_THRESHOLD); \
	ASSERT_NEAR(M43_, M.m43, LN_FLOAT_THRESHOLD); \
	ASSERT_NEAR(M44_, M.m44, LN_FLOAT_THRESHOLD);

#define ASSERT_PLANE_NEAR(x_, y_, z_, w_, plane) \
	ASSERT_NEAR(x_, plane.Normal.x, LN_FLOAT_THRESHOLD); \
	ASSERT_NEAR(y_, plane.Normal.y, LN_FLOAT_THRESHOLD); \
	ASSERT_NEAR(z_, plane.Normal.z, LN_FLOAT_THRESHOLD); \
	ASSERT_NEAR(w_, plane.D, LN_FLOAT_THRESHOLD);

//#define D3DX9_TEST

#ifdef D3DX9_TEST
#include <d3dx9math.h>
#pragma comment(lib, "d3dx9.lib")

inline void dumpFLOAT(const char* header, FLOAT v)
{
	printf("%s : FLOAT(%f)\n", header, v);
}
inline void dumpD3DXVECTOR2(const char* header, const D3DXVECTOR2& v)
{
	printf("%s : D3DXVECTOR2(%f, %f)\n", header, v.x, v.y);
}
inline void dumpD3DXVECTOR3(const char* header, const D3DXVECTOR3& v)
{
	printf("%s : D3DXVECTOR3(%f, %f, %f)\n", header, v.x, v.y, v.z);
}
inline void dumpD3DXVECTOR4(const char* header, const D3DXVECTOR4& v)
{
	printf("%s : D3DXVECTOR4(%f, %f, %f, %f)\n", header, v.x, v.y, v.z, v.w);
}
inline void dumpD3DXQUATERNION(const char* header, const D3DXQUATERNION& v)
{
	printf("%s : D3DXQUATERNION(%f, %f, %f, %f)\n", header, v.x, v.y, v.z, v.w);
}
inline void dumpD3DXMATRIX(const char* header, const D3DXMATRIX& v)
{
	printf("%s : D3DXMATRIX(\n", header);
	printf("    %f, %f, %f, %f,\n", v.m[0][0],v.m[0][1],v.m[0][2],v.m[0][3]);
	printf("    %f, %f, %f, %f,\n", v.m[1][0],v.m[1][1],v.m[1][2],v.m[1][3]);
	printf("    %f, %f, %f, %f,\n", v.m[2][0],v.m[2][1],v.m[2][2],v.m[2][3]);
	printf("    %f, %f, %f, %f);\n",v.m[3][0],v.m[3][1],v.m[3][2],v.m[3][3]);
}
inline void dumpD3DXPLANE(const char* header, const D3DXPLANE& v)
{
	printf("%s : D3DXPLANE(%f, %f, %f, %f)\n", header, v.a, v.b, v.c, v.d);
}
#endif

