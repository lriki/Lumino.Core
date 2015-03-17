
#pragma once

#include "../Base/RefObject.h"
#include "../Base/Size.h"
#include "EventListener.h"

namespace Lumino
{
namespace Platform
{

/**
	@brief	ウィンドウシステムのウィンドウを表すクラスです。
*/
class Window
	: public RefObject
{
public:
	/**
		@brief		ウィンドウを作成します。
		@param[in]	title		: ウィンドウタイトル
		@param[in]	size		: クライアント領域のピクセルサイズ
		@param[in]	resizable	: 可変ウィンドウとして作成するかどうか
	*/
	Window(const String& title, const Size& clientSize, bool resizable);
	virtual ~Window();

public:

	/**
		@brief		クライアント領域のサイズを取得します。
	*/
	const Size& GetSize() const;
	
	/**
		@brief		フルスクリーンの有効フラグを設定します。
		@details	enabled に true を設定するとフルスクリーン、false を設定するとウィンドウモードになります。
					この関数はウィンドウスタイルを変更して最大化するだけです。
					ユーザー定義ウィンドウの場合はなにもしません。
	*/
	void SetFullScreenEnabled(bool enabled);

	/**
		@brief		フルスクリーン状態かを判定します。
	*/
	bool IsFullScreenEnabled() const;

	/**
		@brief		ウィンドウがアクティブかどうかを示す値を取得します。
	*/
	bool IsActive() const;

	/**
		@brief		このウィンドウにマウスキャプチャを設定します。
	*/
	void CaptureMouse();

	/**
		@brief		このウィンドウからマウスキャプチャを削除します。
	*/
	void ReleaseMouseCapture();

	/**
		@brief		このウィンドウにイベントリスナーをアタッチします。(priority が大きいものが先に処理される。必ずデタッチすること)
	*/
	void AttachEventListener(IEventListener* listener, int priority);

	/**
		@brief		このウィンドウからイベントリスナーをデタッチします。
	*/
	void DetachEventListener(IEventListener* listener);

private:
	friend class Application;
	Window(NativeWindow* nativeWindow);
	NativeWindow*	m_nativeWindow;
};

} // namespace Platform
} // namespace Lumino



