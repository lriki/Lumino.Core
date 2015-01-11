
#pragma once

#include "../Base/RefObject.h"
#include "../Base/Size.h"
#include "Application.h"
#include "EventListener.h"

namespace Lumino
{
namespace Platform
{

/**
	@brief	プラットフォーム固有のウィンドウシステムのウィンドウを抽象化します。
*/
class Window
	: public RefObject
{
public:

	/**
		@brief		クライアント領域のサイズを取得します。
	*/
	virtual const Size& GetSize() const = 0;
	
	/**
		@brief		フルスクリーンの有効フラグを設定します。
		@details	enabled に true を設定するとフルスクリーン、false を設定するとウィンドウモードになります。
					この関数はウィンドウスタイルを変更して最大化するだけです。
					ユーザー定義ウィンドウの場合はなにもしません。
	*/
	virtual void SetFullScreenEnabled(bool enabled) = 0;

	/**
		@brief		フルスクリーン状態かを判定します。
	*/
	virtual bool IsFullScreenEnabled() const = 0;

	/**
		@brief		ウィンドウがアクティブかどうかを示す値を取得します。
	*/
	virtual bool IsActive() const = 0;

	/**
		@brief		このウィンドウにマウスキャプチャを設定します。
	*/
	virtual void CaptureMouse() = 0;

	/**
		@brief		このウィンドウからマウスキャプチャを削除します。
	*/
	virtual void ReleaseMouseCapture() = 0;

	/**
		@brief		このウィンドウにイベントリスナーをアタッチします。(priority が大きいものが先に処理される。必ずデタッチすること)
	*/
	void AttachEventListener(IEventListener* listener, int priority) { mListenerEntryArray.Add(priority, listener); }

	/**
		@brief		このウィンドウからイベントリスナーをデタッチします。
	*/
	void DetachEventListener(IEventListener* listener) { return mListenerEntryArray.Remove(listener); }


	/// このウィンドウに割り当てられている全てのイベントリスナーにイベントを送信する
	/// (ウィンドウシステムに送信するのではない点に注意)
	bool SendEventToAllListener(const EventArgs& e);

protected:
	Window() {}
	virtual ~Window() {}

protected:
	SortedMultiArray<int, IEventListener*>	mListenerEntryArray;
};

} // namespace Platform
} // namespace Lumino



