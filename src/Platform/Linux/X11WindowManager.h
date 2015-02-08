
#pragma once

#include "../WindowManagerBase.h"
#include "Win32WindowBase.h"

namespace Lumino
{
namespace Platform
{

class X11WindowManager
	: public WindowManagerBase
{
public:
	X11WindowManager();
	virtual ~X11WindowManager();

public:
	// override Application
	virtual void CreateMainWindow(const WindowCreationSettings& settings);
	virtual Window* GetMainWindow() { return m_mainWindow; }
	virtual void DoEvents();
	virtual void Finalize();
	
public:
	Display* GetX11Display() { return m_x11Display; }
	Screen* GetX11DefaultScreen() { return m_x11DefaultScreen; }
	::Window* GetX11RootWindow() { return m_x11RootWindow; }

private:
	RefPtr<X11Window>	m_mainWindow;
    Display*			m_x11Display;
    Screen*				m_x11DefaultScreen;
    ::Window			m_x11RootWindow;
};

} // namespace Platform
} // namespace Lumino
