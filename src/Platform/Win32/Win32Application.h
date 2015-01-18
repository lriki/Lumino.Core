
#pragma once

#include "../../../include/Lumino/Platform/Application.h"
#include "../ApplicationBase.h"
#include "Win32WindowBase.h"

namespace Lumino
{
namespace Platform
{

class Win32Application
	: public ApplicationBase
{
public:
	Win32Application();
	virtual ~Win32Application();

public:
	// override Application
	virtual void CreateMainWindow(const TCHAR* title, Size clienSize, bool windowed, bool resizable);
	virtual Window* GetMainWindow() { return mMainWindow; }
	virtual bool DoEvents();
	virtual void Exit();

private:
	RefPtr<Win32WindowBase>	mMainWindow;
	bool					mEndRequested;
};

} // namespace Platform
} // namespace Lumino
