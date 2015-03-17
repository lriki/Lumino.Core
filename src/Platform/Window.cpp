
#pragma once

#include "../Internal.h"
#include "Internal.h"
#include "../../include/Lumino/Platform/Window.h"
#include "../../include/Lumino/Platform/NativeWindow.h"
#include "../../include/Lumino/Platform/Application.h"
#include "WindowManagerBase.h"

namespace Lumino
{
namespace Platform
{

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
Window::Window(NativeWindow* nativeWindow)
	: m_nativeWindow(nativeWindow)
{
	LN_SAFE_ADDREF(m_nativeWindow);
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
Window::Window(const String& title, const Size& clientSize, bool resizable)
{
	WindowCreationSettings data;
	data.Title = title;
	data.ClientSize = clientSize;
	data.Fullscreen = false;
	data.Resizable = resizable;
	m_nativeWindow = Internal::ApplicationInstance->m_windowManager->CreateSubWindow(data);
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
Window::~Window()
{
	LN_SAFE_RELEASE(m_nativeWindow);
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
const Size& Window::GetSize() const
{
	return m_nativeWindow->GetSize();
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void Window::SetFullScreenEnabled(bool enabled)
{
	m_nativeWindow->SetFullScreenEnabled(enabled);
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
bool Window::IsFullScreenEnabled() const
{
	return m_nativeWindow->IsFullScreenEnabled();
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
bool Window::IsActive() const
{
	return m_nativeWindow->IsActive();
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void Window::CaptureMouse()
{
	m_nativeWindow->CaptureMouse();
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void Window::ReleaseMouseCapture()
{
	m_nativeWindow->ReleaseMouseCapture();
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void Window::AttachEventListener(IEventListener* listener, int priority)
{
	m_nativeWindow->AttachEventListener(listener, priority);
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void Window::DetachEventListener(IEventListener* listener)
{
	m_nativeWindow->DetachEventListener(listener);
}

} // namespace Platform
} // namespace Lumino



