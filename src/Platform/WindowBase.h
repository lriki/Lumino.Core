
#pragma once

#include "../../include/Lumino/Platform/Window.h"

namespace Lumino
{
namespace Platform
{

/**
	@brief	
*/
class WindowBase
	: public Window
{
public:
	WindowBase(Application* app);
	virtual ~WindowBase();

public:
	void SetApplication(Application* app) { mApplication = app; }

	/// このウィンドウに割り当てられている全てのイベントリスナーにイベントを送信する
	/// (ウィンドウシステムに送信するのではない点に注意)
	bool SendEventToAllListener(const EventArgs& e);

public:
	// override Window
	virtual void AttachEventListener(IEventListener* listener, int priority) { mListenerEntryArray.Add(priority, listener); }
	virtual void DetachEventListener(IEventListener* listener) { return mListenerEntryArray.Remove(listener); }

protected:
	Application*						mApplication;	///< (循環参照防止のため、参照カウントは操作しない)
	SortedArray<int, IEventListener*>	mListenerEntryArray;
};

} // namespace Platform
} // namespace Lumino
