#pragma once
#include "../Base/StringRef.h"
#include "PathName.h"

LN_NAMESPACE_BEGIN

/**
	@brief
*/
template<typename TChar>
class GenericDirectoryIterator
{
public:
	typedef GenericStringRef<TChar>	StringRefT;
	typedef GenericPathName<TChar>		PathNameT;

	GenericDirectoryIterator();
	GenericDirectoryIterator(const StringRefT& path);
	GenericDirectoryIterator(const PathNameT& path);
	GenericDirectoryIterator(const GenericDirectoryIterator& iterator);

	~GenericDirectoryIterator();
		
	const PathNameT& GetPath() const;

	GenericDirectoryIterator& operator = (const StringRefT& path);
	GenericDirectoryIterator& operator = (const PathNameT& path);
	GenericDirectoryIterator& operator = (const GenericDirectoryIterator& it);

	GenericDirectoryIterator& operator ++ ();		// prefix
	GenericDirectoryIterator operator ++ (int);		// postfix
	
	bool operator == (const GenericDirectoryIterator& right) const;
	bool operator != (const GenericDirectoryIterator& right) const;

private:
	PathNameT m_path;
};


//------------------------------------------------------------------------------
template<typename TChar>
inline const GenericPathName<TChar>& GenericDirectoryIterator<TChar>::GetPath() const
{
	return m_path;;
}

//------------------------------------------------------------------------------
template<typename TChar>
inline bool GenericDirectoryIterator<TChar>::operator == (const GenericDirectoryIterator<TChar>& right) const
{
	return m_path.Equals(right.m_path);
}

//------------------------------------------------------------------------------
template<typename TChar>
inline bool GenericDirectoryIterator<TChar>::operator != (const GenericDirectoryIterator<TChar>& right) const
{
	return !m_path.Equals(right.m_path);
}

LN_NAMESPACE_END
