﻿#include <TestConfig.h>

class Test_Base_Point : public ::testing::Test
{
protected:
	virtual void SetUp() {}
	virtual void TearDown() {}
};

#define ASSERT_EQ_POINT(x, y, v) \
	ASSERT_EQ(x, v.X); \
	ASSERT_EQ(y, v.Y);

#define ASSERT_EQ_POINT_FLOAT(x, y, v) \
	ASSERT_FLOAT_EQ(x, v.X); \
	ASSERT_FLOAT_EQ(y, v.Y);

//-----------------------------------------------------------------------------
TEST_F(Test_Base_Point, Basic)
{
	Point pt1;
	ASSERT_EQ_POINT(0, 0, pt1);
	Point pt2(1, 2);
	ASSERT_EQ_POINT(1, 2, pt2);

	pt2.Set(3, 4);
	ASSERT_EQ_POINT(3, 4, pt2);

	ASSERT_TRUE(pt1.IsZero());
	ASSERT_FALSE(pt2.IsZero());

	Point pt3(3, 4);
	ASSERT_TRUE(pt2 == pt3);
	ASSERT_FALSE(pt1 == pt2);
	ASSERT_FALSE(pt2 != pt3);
	ASSERT_TRUE(pt1 != pt2);
}

//-----------------------------------------------------------------------------
TEST_F(Test_Base_Point, Float)
{
	PointF pt1;
	ASSERT_EQ_POINT_FLOAT(0, 0, pt1);
	PointF pt2(0.5f, 0.75f);
	ASSERT_EQ_POINT_FLOAT(0.5f, 0.75f, pt2);

	pt2.Set(3.5f, 4.25f);
	ASSERT_EQ_POINT_FLOAT(3.5f, 4.25f, pt2);

	ASSERT_TRUE(pt1.IsZero());
	ASSERT_FALSE(pt2.IsZero());

	PointF pt3(3.5f, 4.25f);
	ASSERT_TRUE(pt2 == pt3);
	ASSERT_FALSE(pt1 == pt2);
	ASSERT_FALSE(pt2 != pt3);
	ASSERT_TRUE(pt1 != pt2);
}
