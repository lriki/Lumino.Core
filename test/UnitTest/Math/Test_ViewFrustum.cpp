#include "Common.h"

class Test_ViewFrustum : public ::testing::Test
{
protected:
	virtual void SetUp() {}
	virtual void TearDown() {}
};

//------------------------------------------------------------------------------
TEST_F(Test_ViewFrustum, Basic)
{
	// �R���X�g���N�^
	{
		ViewFrustum v1(Matrix::Identity);

		Matrix m2 = Matrix::MakeLookAtLH(Vector3(0, 0, 0), Vector3(0, 0, 1), Vector3(0, 1, 0)) * Matrix::MakePerspectiveFovLH(Math::PI / 4, 640.0f / 480.0f, 1, 1000);
		ViewFrustum v2(m2);
		ASSERT_PLANE_NEAR(-0.000000, -0.000000, -1.000000, 1.000000, v2.GetPlane(FrustumPlane::Near));
		ASSERT_PLANE_NEAR(0.000000, 0.000000, 1.000000, -1000.000610, v2.GetPlane(FrustumPlane::Far));
		ASSERT_PLANE_NEAR(-0.875370, -0.000000, -0.483454, -0.000000, v2.GetPlane(FrustumPlane::Left));
		ASSERT_PLANE_NEAR(0.875370, 0.000000, -0.483454, 0.000000, v2.GetPlane(FrustumPlane::Right));
		ASSERT_PLANE_NEAR(0.000000, 0.923880, -0.382683, 0.000000, v2.GetPlane(FrustumPlane::Top));
		ASSERT_PLANE_NEAR(-0.000000, -0.923880, -0.382683, -0.000000, v2.GetPlane(FrustumPlane::Bottom));
		
		// Ortho
		ViewFrustum v3(Matrix::MakeLookAtRH(Vector3(0, 0, 0), Vector3(0, 0, 1), Vector3(0, 1, 0)) * Matrix::MakeOrthoRH(640, 480, 0, 1000));
		ASSERT_PLANE_NEAR(-0.000000, -0.000000, -1.000000, -0.000000, v3.GetPlane(FrustumPlane::Near));
		ASSERT_PLANE_NEAR(0.000000, 0.000000, 1.000000, -999.999939, v3.GetPlane(FrustumPlane::Far));
		ASSERT_PLANE_NEAR(1.000000, -0.000000, -0.000000, -320.000000, v3.GetPlane(FrustumPlane::Left));
		ASSERT_PLANE_NEAR(-1.000000, 0.000000, 0.000000, -320.000000, v3.GetPlane(FrustumPlane::Right));
		ASSERT_PLANE_NEAR(0.000000, 1.000000, 0.000000, -239.999985, v3.GetPlane(FrustumPlane::Top));
		ASSERT_PLANE_NEAR(-0.000000, -1.000000, -0.000000, -239.999985, v3.GetPlane(FrustumPlane::Bottom));
		//for (int i = 0; i < 6; i++)
		//	v3.GetPlane((FrustumPlane)i).Print();

		Vector3 points[8];
		v3.GetCornerPoints(points);
		ASSERT_VEC3_NEAR(320.000000, 239.999985, 0.000000, points[0]);
		ASSERT_VEC3_NEAR(-320.000000, 239.999985, 0.000000, points[1]);
		ASSERT_VEC3_NEAR(-320.000000, -239.999985, 0.000000, points[2]);
		ASSERT_VEC3_NEAR(320.000000, -239.999985, 0.000000, points[3]);
		ASSERT_VEC3_NEAR(320.000000, 239.999985, 1000.000000, points[4]);
		ASSERT_VEC3_NEAR(-320.000000, 239.999985, 1000.000000, points[5]);
		ASSERT_VEC3_NEAR(-320.000000, -239.999985, 1000.000000, points[6]);
		ASSERT_VEC3_NEAR(320.000000, -239.999985, 1000.000000, points[7]);
		//for (int i = 0; i < 8; i++)
		//	points[i].Print();
	}
}

TEST_F(Test_ViewFrustum, Intersects)
{
	Matrix proj1 = Matrix::MakePerspectiveFovLH(Math::PI / 4, 640.0f / 480.0f, 1, 1000);

	// Z+ ����
	{
		Matrix view = Matrix::MakeLookAtLH(Vector3(0, 0, 0), Vector3(0, 0, 1), Vector3(0, 1, 0));
		ViewFrustum vf1(view * proj1);

		Vector3 pos1(0, 0, 10);
		Vector3 pos2(10, 0, 0);
		ASSERT_TRUE(vf1.Intersects(pos1));
		ASSERT_FALSE(vf1.Intersects(pos2));

		Vector3 sppos1(0, 0, 1002);
		float spr1 = 1;
		ASSERT_FALSE(vf1.Intersects(sppos1, spr1));

		Vector3 sppos2(0, 0, 1000);
		float spr2 = 5;
		ASSERT_TRUE(vf1.Intersects(sppos2, spr2));	
	}

	// Ortho (2D �z��)
	{
		Matrix vp;
		vp = Matrix::MakeLookAtLH(Vector3(320, 240, 0), Vector3(320, 240, 1), Vector3(0, 1, 0));
		vp *= Matrix::MakeOrthoLH(640, 480, 0, 1000);
		ViewFrustum vf(vp);

		Vector3 points[8];
		vf.GetCornerPoints(points);

		ASSERT_TRUE(vf.Intersects(Vector3(0, 0, 0)));
		ASSERT_TRUE(vf.Intersects(Vector3(1, 0, 0)));
		ASSERT_FALSE(vf.Intersects(Vector3(-1, 0, 0)));
		ASSERT_TRUE(vf.Intersects(Vector3(639, 0, 0)));	// ���̂�����͂ǂ����Ă��덷���o��B
		ASSERT_TRUE(vf.Intersects(Vector3(640, 0, 0)));	// �Ƃ肠�����A0�`639�܂�OK�Ȃ炻��ŗǂ��Ƃ���B
		ASSERT_FALSE(vf.Intersects(Vector3(641, 0, 0)));// �ȉ��AY,Z���ɂ��Ă����l�B

		ASSERT_TRUE(vf.Intersects(Vector3(0, 0, 0)));
		ASSERT_FALSE(vf.Intersects(Vector3(0, -1, 0)));
		ASSERT_TRUE(vf.Intersects(Vector3(0, 479, 0)));
		ASSERT_FALSE(vf.Intersects(Vector3(0, 481, 0)));

		ASSERT_TRUE(vf.Intersects(Vector3(0, 0, 0)));
		ASSERT_FALSE(vf.Intersects(Vector3(0, 0, -1)));
		ASSERT_TRUE(vf.Intersects(Vector3(0, 0, 999)));
		ASSERT_FALSE(vf.Intersects(Vector3(0, 0, 1001)));
	}
}
