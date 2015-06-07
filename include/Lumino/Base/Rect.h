
#pragma once

#include "Point.h"
#include "Size.h"

namespace Lumino
{

/**
	@brief		2次元の矩形を表すクラスです。
*/
class Rect
{
public:

	int		X;			///< 左辺の X 座標
	int		Y;			///< 上辺の Y 座標
	int		Width;		///< 幅
	int		Height;		///< 高さ

public:

	/**
		@brief	すべての要素を 0 で初期化します。
	*/
	Rect() { Set(0, 0, 0, 0); }

	/**
		@brief	位置とサイズを指定して初期化します。
	*/
	Rect(int x, int y, int width, int height) { Set(x, y, width, height); }
	
	/**
		@brief	位置とサイズを指定して初期化します。
	*/
	Rect(const Point& point, const Size& size) { Set(point.X, point.Y, size.Width, size.Height); }

	/**
		@brief	位置とサイズを指定して初期化します。
	*/
	Rect(int x, int y, const Size& size) { Set(x, y, size.Width, size.Height); }

	/**
		@brief	位置とサイズを指定して初期化します。
	*/
	Rect(const Point& point, int width, int height) { Set(point.X, point.Y, width, height); }

	/**
		@brief	指定した矩形をコピーして初期化します。
	*/
	Rect(const Rect& rect) { Set(rect.X, rect.Y, rect.Width, rect.Height); }

public:

	/**
		@brief	各要素を設定します。
	*/
	void Set(int x, int y, int width, int height) { X = x; Y = y; Width = width; Height = height; }

	/**
		@brief	左辺の x 軸の値を取得します。
	*/
	int GetLeft() const { return X; }

	/**
		@brief	上辺の y 軸の位置を取得します。
	*/
	int GetTop() const { return Y; }

	/**
		@brief	右辺の x 軸の値を取得します。
	*/
	int GetRight() const { return X + Width; }

	/**
		@brief	底辺の y 軸の値を取得します。
	*/
	int GetBottom() const { return Y + Height; }

	/**
		@brief	左上隅の位置を取得します。
	*/
	Point GetTopLeft() const { return Point(GetLeft(), GetTop()); }
	
	/**
		@brief	右上隅の位置を取得します。
	*/
	Point GetTopRight() const { return Point(GetRight(), GetTop()); }
	
	/**
		@brief	左下隅の位置を取得します。
	*/
	Point GetBottomLeft() const { return Point(GetLeft(), GetBottom()); }

	/**
		@brief	右下隅の位置を取得します。
	*/
	Point GetBottomRight() const { return Point(GetRight(), GetBottom()); }

	/**
		@brief	左上隅の位置を設定します。
	*/
	void SetLocation(const Point& pt) { X = pt.X; Y = pt.Y; }

	/**
		@brief	左上隅の位置を取得します。
	*/
	Point GetLocation() const { return Point(X, Y); }
	
	/**
		@brief	幅と高さを設定します。
	*/
	void SetSize(const Size& size) { Width = size.Width; Height = size.Height; }
	
	/**
		@brief	幅と高さを取得します。
	*/
	Size GetSize() const { return Size(Width, Height); }

	/**
		@brief		正方向の幅または高さを持たないかを判定します。
		@details	幅と高さのどちらか一方が 0 または負の値であれば true を返します。
					0 であるかを判定することはできませんので注意してください。
	*/
	bool IsEmpty() const { return (Width <= 0 || Height <= 0); }

	/**
		@brief	要素がすべて 0 かを判定します。
	*/
	bool IsZero() const { return (X == 0 && Y == 0 && Width == 0 && Height == 0); }

	/**
		@brief	矩形内に指定した点が含まれているかどうかを判定します。
	*/
	bool Contains(Point point) const { return Contains(point.X, point.Y); }
	
	/**
		@brief	矩形内に指定した点が含まれているかどうかを判定します。
	*/
	bool Contains(int x, int y) const
	{
		if (IsEmpty()) {
			return false;
		}
		return ((x >= X) && (x - Width <= X) && (y >= Y) && (y - Height <= Y));
	}
	
	/**
		@brief	指定した矩形全体が、この矩形内部に含まれているかを判定します。
	*/
	bool Contains(const Rect& rect) const
	{
		if (IsEmpty() || rect.IsEmpty()) {
			return false;
		}
		return (
			X <= rect.X &&
			Y <= rect.Y &&
			X + Width >= rect.X + rect.Width &&
			Y + Height >= rect.Y + rect.Height);
	}

	/**
		@brief	指定した矩形に収まるように、この矩形をクリッピングします。
	*/
	void Clip(const Rect& rect)
	{
		int l = (X < rect.X) ? rect.X : X;
		int t = (Y < rect.Y) ? rect.Y : Y;
		int r = (GetRight() > rect.GetRight()) ? rect.GetRight() : GetRight();
		int b = (GetBottom() > rect.GetBottom()) ? rect.GetBottom() : GetBottom();
		X = l;
		Y = t;
		Width = r - l;
		Height = b - t;
	}

	/**
		@brief	四角形を拡大または縮小します。
	*/
	void Inflate(int width, int height)
	{
		X -= width;
		Y -= height;
		Width += width * 2;
		Height += height * 2;
		if (Width < 0) Width = 0;
		if (Height < 0) Height = 0;
	}

public:
	bool operator == (const Rect& obj) const { return (X == obj.X && Y == obj.Y && Width == obj.Width && Height == obj.Height); }
	bool operator != (const Rect& obj) const { return !operator==(obj); }
};

/**
	@brief		2次元の矩形を表すクラスです。
*/
class RectF
{
public:
	float		X;			///< 左辺の X 座標
	float		Y;			///< 上辺の Y 座標
	float		Width;		///< 幅
	float		Height;		///< 高さ

public:

	/**
		@brief	すべての要素を 0 で初期化します。
	*/
	RectF() { Set(0, 0, 0, 0); }

	/**
		@brief	位置とサイズを指定して初期化します。
	*/
	RectF(float x, float y, float width, float height) { Set(x, y, width, height); }
	
	/**
		@brief	位置とサイズを指定して初期化します。
	*/
	RectF(const PointF& point, const SizeF& size) { Set(point.X, point.Y, size.Width, size.Height); }

	/**
		@brief	指定した矩形をコピーして初期化します。
	*/
	RectF(const RectF& rect) { Set(rect.X, rect.Y, rect.Width, rect.Height); }

	/**
		@brief	指定した矩形をコピーして初期化します。
	*/
	RectF(const Rect& rect) { Set((float)rect.X, (float)rect.Y, (float)rect.Width, (float)rect.Height); }

public:

	/**
		@brief	各要素を設定します。
	*/
	void Set(float x, float y, float width, float height) { X = x; Y = y; Width = width; Height = height; }

	/**
		@brief	左辺の x 軸の値を取得します。
	*/
	float GetLeft() const { return X; }

	/**
		@brief	上辺の y 軸の位置を取得します。
	*/
	float GetTop() const { return Y; }

	/**
		@brief	右辺の x 軸の値を取得します。
	*/
	float GetRight() const { return X + Width; }

	/**
		@brief	底辺の y 軸の値を取得します。
	*/
	float GetBottom() const { return Y + Height; }

	/**
		@brief	左上隅の位置を取得します。
	*/
	PointF GetTopLeft() const { return PointF(GetLeft(), GetTop()); }
	
	/**
		@brief	右上隅の位置を取得します。
	*/
	PointF GetTopRight() const { return PointF(GetRight(), GetTop()); }
	
	/**
		@brief	左下隅の位置を取得します。
	*/
	PointF GetBottomLeft() const { return PointF(GetLeft(), GetBottom()); }

	/**
		@brief	右下隅の位置を取得します。
	*/
	PointF GetBottomRight() const { return PointF(GetRight(), GetBottom()); }

	/**
		@brief	左上隅の位置を設定します。
	*/
	void SetLocation(const PointF& pt) { X = pt.X; Y = pt.Y; }

	/**
		@brief	左上隅の位置を取得します。
	*/
	PointF GetLocation() const { return PointF(X, Y); }
	
	/**
		@brief	幅と高さを設定します。
	*/
	void SetSize(const SizeF& size) { Width = size.Width; Height = size.Height; }
	
	/**
		@brief	幅と高さを取得します。
	*/
	SizeF GetSize() const { return SizeF(Width, Height); }

	/**
		@brief		正方向の幅または高さを持たないかを判定します。
		@details	幅と高さのどちらか一方が 0 または負の値であれば true を返します。
					0 であるかを判定することはできませんので注意してください。
	*/
	bool IsEmpty() const { return (Width <= 0 || Height <= 0); }

	/**
		@brief	要素がすべて 0 かを判定します。
	*/
	bool IsZero() const { return (X == 0 && Y == 0 && Width == 0 && Height == 0); }

	/**
		@brief	矩形内に指定した点が含まれているかどうかを判定します。
	*/
	bool Contains(PointF point) const { return Contains(point.X, point.Y); }
	
	/**
		@brief	矩形内に指定した点が含まれているかどうかを判定します。
	*/
	bool Contains(float x, float y) const
	{
		if (IsEmpty()) {
			return false;
		}
		return ((x >= X) && (x - Width <= X) && (y >= Y) && (y - Height <= Y));
	}
	
	/**
		@brief	指定した矩形全体が、この矩形内部に含まれているかを判定します。
	*/
	bool Contains(const RectF& rect) const
	{
		if (IsEmpty() || rect.IsEmpty()) {
			return false;
		}
		return (
			X <= rect.X &&
			Y <= rect.Y &&
			X + Width >= rect.X + rect.Width &&
			Y + Height >= rect.Y + rect.Height);
	}

	/**
		@brief	四角形を拡大または縮小します。
	*/
	void Inflate(float width, float height)
	{
		X -= width;
		Y -= height;
		Width += width * 2;
		Height += height * 2;
		if (Width < 0) Width = 0;
		if (Height < 0) Height = 0;
	}

public:
	bool operator == (const RectF& obj) const { return (X == obj.X && Y == obj.Y && Width == obj.Width && Height == obj.Height); }
	bool operator != (const RectF& obj) const { return !operator==(obj); }
};


} // namespace Lumino
