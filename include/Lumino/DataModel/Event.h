
#pragma once
#include "../Base/Array.h"
#include "../Base/Delegate.h"

namespace Lumino
{

#define LN_EVENT_CLASS_NAME				Event01
#define LN_EVENT_DELEGATE_CLASS_NAME	Delegate01
#define LN_EVENT_TEMPLATE_ARGS			typename A1
#define LN_EVENT_ARGS_DECL				A1 a1
#define LN_EVENT_CALL_ARGS				a1
#define LN_EVENT_TEMPLATE_TYPES			A1
#include "Event.inl"

#define LN_EVENT_CLASS_NAME				Event02
#define LN_EVENT_DELEGATE_CLASS_NAME	Delegate02
#define LN_EVENT_TEMPLATE_ARGS			typename A1, typename A2
#define LN_EVENT_ARGS_DECL				A1 a1, A2 a2
#define LN_EVENT_CALL_ARGS				a1, a2
#define LN_EVENT_TEMPLATE_TYPES			A1, A2
#include "Event.inl"

#define LN_EVENT_CLASS_NAME				Event03
#define LN_EVENT_DELEGATE_CLASS_NAME	Delegate03
#define LN_EVENT_TEMPLATE_ARGS			typename A1, typename A2, typename A3
#define LN_EVENT_ARGS_DECL				A1 a1, A2 a2, A3 a3
#define LN_EVENT_CALL_ARGS				a1, a2, a3
#define LN_EVENT_TEMPLATE_TYPES			A1, A2, A3
#include "Event.inl"

#define LN_EVENT_CLASS_NAME				Event04
#define LN_EVENT_DELEGATE_CLASS_NAME	Delegate04
#define LN_EVENT_TEMPLATE_ARGS			typename A1, typename A2, typename A3, typename A4
#define LN_EVENT_ARGS_DECL				A1 a1, A2 a2, A3 a3, A4 a4
#define LN_EVENT_CALL_ARGS				a1, a2, a3, a4
#define LN_EVENT_TEMPLATE_TYPES			A1, A2, A3, A4
#include "Event.inl"

} // namespace Lumino