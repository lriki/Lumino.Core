#include "Common.h"

class Test_Vector4 : public ::testing::Test
{
protected:
	virtual void SetUp() {}
	virtual void TearDown() {}
};

//------------------------------------------------------------------------------
TEST_F(Test_Vector4, Basic)
{
	Vector4 sample1(1, 2, 3, 4);

	// �R���X�g���N�^
	{
		Vector4 v1;
		Vector4 v2(1, 2, 3, 4);
		Vector4 v3(Vector2(1, 2), 3, 4);
		Vector4 v4(Vector3(1, 2, 3), 4);
		ASSERT_VEC4_NEAR(0, 0, 0, 0, v1);
		ASSERT_VEC4_NEAR(1, 2, 3, 4, v2);
		ASSERT_VEC4_NEAR(1, 2, 3, 4, v3);
		ASSERT_VEC4_NEAR(1, 2, 3, 4, v4);
	}
	// this->Set
	{
		Vector4 v1;
		v1.Set(1, 2, 3, 4);
		ASSERT_VEC4_NEAR(1, 2, 3, 4, v1);
	}
	// this->GetXY
	{
		Vector2 v = sample1.GetXY();
		ASSERT_VEC2_NEAR(1, 2, v);
	}
	// this->GetXYZ
	{
		Vector3 v = sample1.GetXYZ();
		ASSERT_VEC3_NEAR(1, 2, 3, v);
	}
	// this->GetLength / GetLengthSquared
	{
		ASSERT_NEAR(5.477226, sample1.GetLength(), LN_FLOAT_THRESHOLD);
		ASSERT_NEAR(30.000000, sample1.GetLengthSquared(), LN_FLOAT_THRESHOLD);
	}
	// this->Clamp
	{
		Vector4 v1(1, 2, 3, 4);
		v1.Clamp(Vector4(0, 4, 2, 3), Vector4(0.5, 5, 3, 8));
		ASSERT_VEC4_NEAR(0.5, 4, 3, 4, v1);

		Vector4 v2(1, 2, 3, 4);
		v2.Clamp(2, 3);
		ASSERT_VEC4_NEAR(2, 2, 3, 3, v2);
	}
	// this->IsNaNOrInf
	{
		Vector4 v(1, 2, 3, 4);
		ASSERT_FALSE(v.IsNaNOrInf());
		volatile  float d = 0.0f;
		v.x /= d;
		ASSERT_TRUE(v.IsNaNOrInf());
	}

	// Vector4::Normalize
	{
		Vector4 v1 = Vector4::Normalize(sample1);
		ASSERT_VEC4_NEAR(0.182574, 0.365148, 0.547723, 0.730297, v1);
	}
	// Vector4::Dot
	{
		Vector4 v1(5, 6, 7, 8);
		float d = Vector4::Dot(sample1, v1);
		ASSERT_FLOAT_EQ(70.000000, d);
	}
	// Vector4::Min / Max
	{
		Vector4 v1 = Vector4::Min(Vector4(1, 3, 5, 7), Vector4(4, 2, 6, 8));
		ASSERT_VEC4_NEAR(1, 2, 5, 7, v1);

		Vector4 v2 = Vector4::Max(Vector4(1, 3, 5, 7), Vector4(4, 2, 6, 8));
		ASSERT_VEC4_NEAR(4, 3, 6, 8, v2);
	}
	// Vector4::Transform
	{
		Matrix m = Matrix::MakeRotationYawPitchRoll(1, 2, 3);
		Vector4 v1 = Vector4::Transform(sample1, m);
		ASSERT_VEC4_NEAR(-3.144919, -1.962654, -0.507415, 4.000000, v1);
	}
	// Vector4::Lerp()
	{
		Vector4 v1 = Vector4::Lerp(
			Vector4(1, 2, 3, 4),
			Vector4(3, 4, 7, 8),
			0.75);
		ASSERT_VEC4_NEAR(2.500000, 3.500000, 6.000000, 7.000000, v1);
	}
	// Vector4::Hermite()
	{
		Vector4 v1 = Vector4::Hermite(
			Vector4(1, 2, 3, 4),
			Vector4(3, 4, 7, 8),
			Vector4(0.3f, 0.4f, -0.5f, -0.7f),
			Vector4(0.03f, 0.04f, -1.5f, 2.5f),
			0.75);
		ASSERT_VEC4_NEAR(0.545781, 0.831875, 0.585938, 0.057813, v1);
	}
	// Vector4::CatmullRom()
	{
		Vector4 v1 = Vector4::CatmullRom(
			Vector4(1, 2, 3, 4),
			Vector4(3, 4, 7, 8),
			Vector4(0.3f, 0.4f, -0.5f, -0.7f),
			Vector4(0.03f, 0.04f, -1.5f, 2.5f),
			0.75);
		ASSERT_VEC4_NEAR(0.914297, 1.203437, 1.187500, 0.935937, v1);
	}

	// assign operator
	{
		Vector4 v1;

		v1.Set(1, 2, 3, 4);
		v1 += Vector4(1, 2, 3, 4);
		ASSERT_VEC4_NEAR(2, 4, 6, 8, v1);
		v1.Set(1, 2, 3, 4);
		v1 += 5;
		ASSERT_VEC4_NEAR(6, 7, 8, 9, v1);

		v1.Set(1, 2, 3, 4);
		v1 -= Vector4(1, 2, 3, 4);
		ASSERT_VEC4_NEAR(0, 0, 0, 0, v1);
		v1.Set(1, 2, 3, 4);
		v1 -= 5;
		ASSERT_VEC4_NEAR(-4, -3, -2, -1, v1);

		v1.Set(1, 2, 3, 4);
		v1 *= Vector4(5, 6, 7, 8);
		ASSERT_VEC4_NEAR(5, 12, 21, 32, v1);
		v1.Set(1, 2, 3, 4);
		v1 *= 5;
		ASSERT_VEC4_NEAR(5, 10, 15, 20, v1);

		v1.Set(1, 2, 3, 4);
		v1 /= Vector4(1, 2, 3, 4);
		ASSERT_VEC4_NEAR(1, 1, 1, 1, v1);
		v1.Set(10, 20, 30, 40);
		v1 /= 5;
		ASSERT_VEC4_NEAR(2, 4, 6, 8, v1);
	}
	// binary operator
	{
		Vector4 v1;

		v1 = Vector4(1, 2, 3, 4) + Vector4(1, 2, 3, 4);
		ASSERT_VEC4_NEAR(2, 4, 6, 8, v1);
		v1 = Vector4(1, 2, 3, 4) + 5;
		ASSERT_VEC4_NEAR(6, 7, 8, 9, v1);
		v1 = 6 + Vector4(1, 2, 3, 4);
		ASSERT_VEC4_NEAR(7, 8, 9, 10, v1);

		v1 = Vector4(1, 2, 3, 4) - Vector4(1, 2, 3, 4);
		ASSERT_VEC4_NEAR(0, 0, 0, 0, v1);
		v1 = Vector4(1, 2, 3, 4) - 5;
		ASSERT_VEC4_NEAR(-4, -3, -2, -1, v1);
		v1 = 6 - Vector4(1, 2, 3, 4);
		ASSERT_VEC4_NEAR(5, 4, 3, 2, v1);

		v1 = Vector4(1, 2, 3, 4) * Vector4(5, 6, 7, 8);
		ASSERT_VEC4_NEAR(5, 12, 21, 32, v1);
		v1 = Vector4(1, 2, 3, 4) * 5;
		ASSERT_VEC4_NEAR(5, 10, 15, 20, v1);
		v1 = 6 * Vector4(1, 2, 3, 4);
		ASSERT_VEC4_NEAR(6, 12, 18, 24, v1);

		v1 = Vector4(10, 20, 30, 40) / Vector4(10, 20, 30, 40);
		ASSERT_VEC4_NEAR(1, 1, 1, 1, v1);
		v1 = Vector4(10, 20, 30, 40) / 5;
		ASSERT_VEC4_NEAR(2, 4, 6, 8, v1);
		v1 = 20 / Vector4(1, 2, 4, 5);
		ASSERT_VEC4_NEAR(20, 10, 5, 4, v1);

		v1.Set(2, 4, 6, 8);
		ASSERT_TRUE(v1 == Vector4(2, 4, 6, 8));
		ASSERT_FALSE(v1 != Vector4(2, 4, 6, 8));
	}
	// unary operator
	{
		Vector4 v1(1, 2, 3, 4);
		v1 = -v1;
		ASSERT_VEC4_NEAR(-1, -2, -3, -4, v1);
	}

#ifdef D3DX9_TEST
	D3DXVECTOR4 dxsample1(1, 2, 3, 4);

	// D3DXVec4Length
	{
		//dumpFLOAT("D3DXVec4Length", D3DXVec4Length(&dxsample1));
		//dumpFLOAT("D3DXVec4LengthSq", D3DXVec4LengthSq(&dxsample1));
	}
	// D3DXVec4Normalize
	{
		D3DXVECTOR4 v1;
		D3DXVec4Normalize(&v1, &dxsample1);
		//dumpD3DXVECTOR4("D3DXVec4Normalize", v1);
	}
	// D3DXVec4Dot
	{
		D3DXVECTOR4 v1(5, 6, 7, 8);
		//dumpFLOAT("D3DXVec4Dot", D3DXVec4Dot(&dxsample1, &v1));
	}
	// D3DXVec4Transform
	{
		D3DXMATRIX m1;
		D3DXMatrixRotationYawPitchRoll(&m1, 1, 2, 3);
		D3DXVECTOR4 v1;
		D3DXVec4Transform(&v1, &dxsample1, &m1);
	}
	// D3DXVec4Lerp
	{
		D3DXVECTOR4 dxv;
		D3DXVECTOR4 dxv1(1, 2, 3, 4);
		D3DXVECTOR4 dxv2(3, 4, 7, 8);
		D3DXVec4Lerp(&dxv, &dxv1, &dxv2, 0.75);
		//dumpD3DXVECTOR4("D3DXVec4Lerp", dxv);
	}
	// D3DXVec4Hermite
	{
		D3DXVECTOR4 dxv;
		D3DXVECTOR4 dxv1(1, 2, 3, 4);
		D3DXVECTOR4 dxv2(3, 4, 7, 8);
		D3DXVECTOR4 dxv3(0.3f, 0.4f, -0.5f, -0.7f);
		D3DXVECTOR4 dxv4(0.03f, 0.04f, -1.5f, 2.5f);
		D3DXVec4Hermite(&dxv, &dxv1, &dxv2, &dxv3, &dxv4, 0.75);
		//dumpD3DXVECTOR4("D3DXVec4Hermite", dxv);
	}
	// D3DXVec4CatmullRom
	{
		D3DXVECTOR4 dxv;
		D3DXVECTOR4 dxv1(1, 2, 3, 4);
		D3DXVECTOR4 dxv2(3, 4, 7, 8);
		D3DXVECTOR4 dxv3(0.3f, 0.4f, -0.5f, -0.7f);
		D3DXVECTOR4 dxv4(0.03f, 0.04f, -1.5f, 2.5f);
		D3DXVec4CatmullRom(&dxv, &dxv1, &dxv2, &dxv3, &dxv4, 0.75);
		//dumpD3DXVECTOR4("D3DXVec4CatmullRom", dxv);
	}
	
#endif
}
