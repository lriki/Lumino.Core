
#pragma once

namespace Lumino
{

#define LN_CONSTEXPR

#define LN_DECLARE_FLAGS(Flags, Enum) typedef EnumFlags<Enum> Flags;

#define LN_DECLARE_OPERATORS_FOR_FLAGS(FlagsEnumClass) \
	inline EnumFlags<FlagsEnumClass::enum_type> operator|(FlagsEnameType::enum_type f1, FlagsEnumClass::enum_type f2) throw() \
	{ \
		return EnumFlags<FlagsEnumClass::enum_type>(f1) | f2; \
	}
	//inline EnumFlags<FlagsEnumClass::enum_type> operator|(FlagsEnumClass::enum_type f1, EnumFlags<FlagsEnumClass::enum_type> f2) throw() \
	//{ \
	//	return f2 | f1; \
	//}




///< �t�@�C�����J�����@
class FileOpenMode
{
public:

	enum _FileOpenMode	// ���� enum �̌^���͕K���K�v�B
	{
		None		= 0x0000,
		Read		= 0x0001,									///< �ǂݎ��A�N�Z�X
		Write		= 0x0002,									///< �������݃A�N�Z�X
		ReadWrite = Read | Write,	///< �ǂݎ��y�я������݃A�N�Z�X
	
		Append		= 0x0004,		///< �t�@�C�����J���A�t�@�C���̖������V�[�N���� (�ǋL���[�h)
		Truncate	= 0x0008,		///< �t�@�C�������݂���ꍇ�A�T�C�Y�� 0 �ɂ���
	
		Deferring	= 0x0100,		///< �t�@�C����x�����[�h�ŊJ��
	};
	//LN_DECLARE_FLAGS(Flags, _FileOpenMode);


	typedef _FileOpenMode enum_type;
	FileOpenMode() : m_value(0){}
	FileOpenMode(_FileOpenMode v)
	{
		m_value = v;
	}
	//FileOpenMode()
	//FileOpenMode operator&(_FileOpenMode right)
	//{
	//	FileOpenMode value;
	//	value.m_value = m_value & right;
	//	return value;
	//}
	FileOpenMode operator|(_FileOpenMode right)
	{
		FileOpenMode value;
		value.m_value = m_value | right;
		return value;
	}

	friend inline FileOpenMode operator&(const FileOpenMode& f1, FileOpenMode::enum_type f2) throw();

	LN_CONSTEXPR inline bool TestFlag(_FileOpenMode f) const throw() { return (m_value & f) == f && (f != 0 || m_value == f); }
private:
	int	m_value;
};
inline FileOpenMode operator&(const FileOpenMode& f1, FileOpenMode::enum_type f2) throw()
{
	FileOpenMode value;
	value.m_value = f1.m_value & f2;
	return value;
}
inline FileOpenMode operator|(FileOpenMode::enum_type f1, FileOpenMode::enum_type f2) throw()
{
	return FileOpenMode(f1) | f2;
}



} // namespace Lumino
