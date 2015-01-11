
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
	Application*	mApplication;	///< (循環参照防止のため、参照カウントは操作しない)
};

} // namespace Platform
} // namespace Lumino
