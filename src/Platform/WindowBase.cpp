
#include "../Internal.h"
#include "WindowBase.h"

namespace Lumino
{
namespace Platform
{

//=============================================================================
// WindowBase
//=============================================================================

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
WindowBase::WindowBase(Application* app)
	: mApplication(app)
{
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
WindowBase::~WindowBase()
{
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
bool WindowBase::SendEventToAllListener(const EventArgs& e)
{
	LN_FOREACH(IEventListener* listener, mListenerEntryArray)
	{
		if (listener->OnEvent(e)) {
			return true;
		}
	}
	return false;
}

} // namespace Platform
} // namespace Lumino