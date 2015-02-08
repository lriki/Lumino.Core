
#pragma once

#include "../../include/Lumino/Platform/Window.h"

namespace Lumino
{
namespace Platform
{
class WindowManagerBase;

/**
	@brief	
*/
class WindowBase
	: public Window
{
public:
	WindowBase(WindowManagerBase* windowManager);
	virtual ~WindowBase();

public:
	//void SetApplication(Win32WindowManager* app) { m_windowManager = app; }

	/// ���̃E�B���h�E�Ɋ��蓖�Ă��Ă���S�ẴC�x���g���X�i�[�ɃC�x���g�𑗐M����
	/// (�E�B���h�E�V�X�e���ɑ��M����̂ł͂Ȃ��_�ɒ���)
	bool SendEventToAllListener(const EventArgs& e);

public:
	// override Window
	virtual void AttachEventListener(IEventListener* listener, int priority) { m_listenerEntryArray.Add(priority, listener); }
	virtual void DetachEventListener(IEventListener* listener) { return m_listenerEntryArray.Remove(listener); }

protected:
	WindowManagerBase*					m_windowManager;	///< (�z�Q�Ɩh�~�̂��߁A�Q�ƃJ�E���g�͑��삵�Ȃ�)
	SortedArray<int, IEventListener*>	m_listenerEntryArray;
};

} // namespace Platform
} // namespace Lumino
