
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
	Application*	mApplication;	///< (zŠÂŽQÆ–hŽ~‚Ì‚½‚ßAŽQÆƒJƒEƒ“ƒg‚Í‘€ì‚µ‚È‚¢)
};

} // namespace Platform
} // namespace Lumino
