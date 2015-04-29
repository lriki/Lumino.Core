
#pragma once

namespace Lumino
{

/**
	@brief		2次元上のオブジェクトサイズを表すクラスです。
*/
class Size
{
public:
	int		Width;		///< X 方向の大きさ
	int		Height;		///< Y 方向の大きさ

public:

	/**
		@brief	すべての要素を 0 で初期化します。
	*/
	Size() { Set(0, 0); }
	
	/**
		@brief	幅と高さを指定して初期化します。
	*/
	Size(int w, int h) { Set(w, h); }

public:
	
	/**
		@brief	各要素を設定します。
	*/
	void Set(int w, int h) { Width = w; Height = h; }

	/**
		@brief	要素がすべて 0 かを判定します。
	*/
	bool IsZero() const { return (Width == 0 && Height == 0); }

	/**
		@brief	いずれかの要素が 0 かを判定します。
	*/
	bool IsAnyZero() const { return (Width == 0 || Height == 0); }

public:
	bool operator == (const Size& obj) const { return (Width == obj.Width && Height == obj.Height); }
	bool operator != (const Size& obj) const { return !operator==(obj); }
};

/**
	@brief		2次元上のオブジェクトサイズを表すクラスです。(浮動小数点形式)
*/
class SizeF
{
public:
	float	Width;		///< X 方向の大きさ
	float	Height;		///< Y 方向の大きさ

public:

	/**
		@brief	すべての要素を 0 で初期化します。
	*/
	SizeF() { Set(0, 0); }
	
	/**
		@brief	幅と高さを指定して初期化します。
	*/
	SizeF(float w, float h) { Set(w, h); }

public:
	
	/**
		@brief	各要素を設定します。
	*/
	void Set(float w, float h) { Width = w; Height = h; }

	/**
		@brief	要素がすべて 0 かを判定します。
	*/
	bool IsZero() const { return (Width == 0.0f && Height == 0.0f); }

	/**
		@brief	いずれかの要素が 0 かを判定します。
	*/
	bool IsAnyZero() const { return (Width == 0.0f || Height == 0.0f); }

public:
	bool operator == (const SizeF& obj) const { return (Width == obj.Width && Height == obj.Height); }
	bool operator != (const SizeF& obj) const { return !operator==(obj); }
};


} // namespace Lumino
