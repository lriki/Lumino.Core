
#pragma once

#include "../../include/Lumino/Base/Array.h"
#include "../../include/Lumino/Platform/Application.h"

namespace Lumino
{
namespace Platform
{

class ApplicationBase
	: public Application
{
protected:
	ApplicationBase() {}
	virtual ~ApplicationBase() {}

public:
	void AddWindow(Window* window) { mWindowArray.Add(window); }
	void RemoveWindow(Window* window) { mWindowArray.Remove(window); }
	void PostEvent();

private:
	Array<Window*> mWindowArray;
};

} // namespace Platform
} // namespace Lumino
