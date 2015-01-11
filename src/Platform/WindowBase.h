
#pragma once

#include <Platform/Window.h>

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

protected:
	Application*	mApplication;	///< (�z�Q�Ɩh�~�̂��߁A�Q�ƃJ�E���g�͑��삵�Ȃ�)
};

} // namespace Platform
} // namespace Lumino
