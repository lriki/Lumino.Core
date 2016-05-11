
#include "../Internal.h"
#include "../../include/Lumino/Base/RefObject.h"

LN_NAMESPACE_BEGIN

//==============================================================================
// RefObject
//==============================================================================

//------------------------------------------------------------------------------
RefObject::RefObject()
	: mReferenceCount(1)
	, m_refPtrReferenced(0)
{}

//------------------------------------------------------------------------------
RefObject::~RefObject()
{
}

//------------------------------------------------------------------------------
int32_t RefObject::GetRefCount() const
{ 
	return mReferenceCount.Get();
}

//------------------------------------------------------------------------------
int32_t RefObject::AddRef()
{
	return mReferenceCount.Increment();
}

//------------------------------------------------------------------------------
int32_t RefObject::Release()
{
    int32_t count = mReferenceCount.Decrement();
	if ( count <= 0 ) {
		 delete this;
	}
    return count;
}

//------------------------------------------------------------------------------
void RefObject::TryGCAddRef()
{
	//if (m_refPtrReferenced.Get() == 0)
	{
		m_refPtrReferenced.Increment();
		if (m_refPtrReferenced.Get() == 1)
		{
			return;
		}
	}
	AddRef();
}

//------------------------------------------------------------------------------
void RefObject::GCRelease()
{
	m_refPtrReferenced.Decrement();
	Release();
}

LN_NAMESPACE_END
