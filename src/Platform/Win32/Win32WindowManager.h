
#pragma once

#include "../WindowManagerBase.h"
#include "Win32WindowBase.h"

namespace Lumino
{
namespace Platform
{

class Win32WindowManager
	: public WindowManagerBase
{
public:
	Win32WindowManager();
	virtual ~Win32WindowManager();

public:
	// override Application
	virtual void CreateMainWindow(const WindowCreationSettings& settings);
	virtual Window* GetMainWindow() { return mMainWindow; }
	virtual void DoEvents();
	virtual void Finalize();

private:
	RefPtr<Win32WindowBase>	mMainWindow;
};

} // namespace Platform
} // namespace Lumino
