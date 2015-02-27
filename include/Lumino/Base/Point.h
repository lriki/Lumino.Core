
#pragma once

namespace Lumino
{

/**
	@brief	2次元上の点ズを表すクラスです。
*/
class Point
{
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

	/// 要素がすべて 0 かを判定します。
	bool IsZero() const { return (X == 0 && Y == 0); }

public:

	bool operator == (const Point& obj) const { return (X == obj.X && Y == obj.Y); }
	bool operator != (const Point& obj) const { return (X != obj.X || Y != obj.Y); }
};

} // namespace Lumino
