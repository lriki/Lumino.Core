
#pragma once

namespace Lumino
{

/**
	@brief		2次元上のオブジェクトサイズを表すクラス
*/
class Size
{
public:

	int		Width;		///< X 方向の大きさ
	int		Height;		///< Y 方向の大きさ

public:

	/// デフォルトコンストラクタ
	Size() { Set(0, 0); }

	/// コンストラクタ
	Size(int w, int h) { Set(w, h); }

	/// 各要素を設定します。
	void Set(int w, int h) { Width = w; Height = h; }

	/// 要素がすべて 0 かを判定します。
	bool IsZero() const { return (Width == 0 && Height == 0); }

	/// いずれかの要素が 0 かを判定します。
	bool IsAnyZero() const { return (Width == 0 || Height == 0); }

public:

	bool operator == (const Size& obj) const { return (Width == obj.Width && Height == obj.Height); }
	bool operator != (const Size& obj) const { return (Width != obj.Width || Height != obj.Height); }
};

} // namespace Lumino
