
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

	/// ���̃E�B���h�E�Ɋ��蓖�Ă��Ă���S�ẴC�x���g���X�i�[�ɃC�x���g�𑗐M����
	/// (�E�B���h�E�V�X�e���ɑ��M����̂ł͂Ȃ��_�ɒ���)
	bool SendEventToAllListener(const EventArgs& e);

public:
	// override Window
	virtual void AttachEventListener(IEventListener* listener, int priority) { mListenerEntryArray.Add(priority, listener); }
	virtual void DetachEventListener(IEventListener* listener) { return mListenerEntryArray.Remove(listener); }

protected:
	Application*						mApplication;	///< (�z�Q�Ɩh�~�̂��߁A�Q�ƃJ�E���g�͑��삵�Ȃ�)
	SortedArray<int, IEventListener*>	mListenerEntryArray;
};

} // namespace Platform
} // namespace Lumino
