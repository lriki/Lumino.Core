/**
	@file	Point.h
*/
#pragma once

LN_NAMESPACE_BEGIN

/**
	@brief	2次元上の点を表すクラスです。
*/
class Point
{
public:
	static const Point	Zero;	///< Point(0, 0)

public:
	int		X;		///< X 座標
	int		Y;		///< Y 座標

public:

	/**
		@brief	すべての要素を 0 で初期化します。
	*/
	Point() { Set(0, 0); }
	
	/**
		@brief	座標を指定して初期化します。
	*/
	Point(int x, int y) { Set(x, y); }

public:
	
	/**
		@brief	各要素を設定します。
	*/
	void Set(int x, int y) { X = x; Y = y; }

	/**
		@brief	要素がすべて 0 かを判定します。
	*/
	bool IsZero() const { return (X == 0 && Y == 0); }

public:
	bool operator == (const Point& obj) const { return (X == obj.X && Y == obj.Y); }
	bool operator != (const Point& obj) const { return !operator==(obj); }
};

/**
	@brief	2次元上の点を表すクラスです。(浮動小数点形式)
*/
class PointF
{
public:
	static const PointF	Zero;	///< PointF(0, 0)

public:
	float	X;		///< X 座標
	float	Y;		///< Y 座標

public:

	/**
		@brief	すべての要素を 0 で初期化します。
	*/
	PointF() { Set(0, 0); }
	
	/**
		@brief	座標を指定して初期化します。
	*/
	PointF(float x, float y) { Set(x, y); }

public:
	
	/**
		@brief	各要素を設定します。
	*/
	void Set(float x, float y) { X = x; Y = y; }

	/**
		@brief	要素がすべて 0 かを判定します。
	*/
	bool IsZero() const { return (X == 0.0f && Y == 0.0f); }

public:
	bool operator == (const PointF& obj) const { return (X == obj.X && Y == obj.Y); }
	bool operator != (const PointF& obj) const { return !operator==(obj); }
};

LN_NAMESPACE_END
