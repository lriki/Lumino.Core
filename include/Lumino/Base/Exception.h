
#pragma once

#include "String.h"

// exp �̏�������������Ȃ������ꍇ�Atype �Ɏw�肵����O�� throw ����
#define LN_THROW( exp, type, ... )	{ if (!(exp)) { type e = type(__VA_ARGS__); e.SetSourceLocationInfo(__FILE__, __LINE__); throw e; } }

/// ��O�N���X�̊�{�I�ȃR���X�g���N�^��錾����
#define LN_EXCEPTION_BASIC_CONSTRUCTOR_DECL(className) \
	className(const char* message, ...) \
	{ \
		va_list args; \
		va_start(args, message); \
		SetMessage(message, args); \
		va_end(args); \
	} \
	className(const wchar_t* message, ...) \
	{ \
		va_list args; \
		va_start(args, message); \
		SetMessage(message, args); \
		va_end(args); \
	}

namespace Lumino
{

/**
	@brief	��O�x�[�X�N���X
*/
class LUMINO_EXPORT Exception : public std::exception
{
public:
	Exception();
	virtual ~Exception() throw();
	Exception& SetSourceLocationInfo( const char* filePath, int fileLine );

public:
	
	/**
		@brief	��O�̏ڍ׃��b�Z�[�W���擾���܂��B
	*/
	const TCHAR* GetMessage() const { return mMessage; }

	/**
		@brief		��O�������ɏڍ׏����_���v����t�@�C��������������
		@param[in]	filePath	: �_���v��t�@�C�� (�������� LN_MAX_PATH �ȓ�)
		@return		false �̏ꍇ�A�t�@�C���A�N�Z�X�Ɏ��s����
		@details	�w�肳�ꂽ�p�X�ŁA�t�@�C����V�K�쐬���܂��B
					�ȍ~�A���̗�O�N���X�̃R���X�g���N�^���Ă΂�邽�тɁA�ڍ׏�񂪒ǋL����܂��B
	*/
	static bool InitDumpFile(const char* filePath);

	/**
		@brief		��O�̃R�s�[���쐬����
		@note		�ʃX���b�h�Ŕ���������O��ێ����邽�߂Ɏg�p����B
	*/
	virtual Exception* Copy() const { return LN_NEW Exception( *this ); }

public:
	// override std::exception
	virtual const char* what() const  throw() { return mSymbolBuffer; }

protected:
	void SetMessage(const char* format, va_list args);
	void SetMessage(const wchar_t* format, va_list args);

private:
	static const int MaxMessageBufferSize = 1024;

	TCHAR		mSourceFilePath[LN_MAX_PATH];
	int			mSourceFileLine;
	void*		mStackBuffer[32];
	int			mStackBufferSize;
	char		mSymbolBuffer[2048];
	TCHAR		mMessage[MaxMessageBufferSize];
};

/**
	@brief	�������s����O

*/
class OutOfMemoryException 
	: public Exception
{
public:
	LN_EXCEPTION_BASIC_CONSTRUCTOR_DECL(OutOfMemoryException);
	OutOfMemoryException() {}
	virtual ~OutOfMemoryException() throw() {}

public:
	// override Exception
	virtual Exception* Copy() const { return LN_NEW OutOfMemoryException( *this ); }
};

/**
	@brief		���̑���IO��O
	@details	�ǂݎ�葮���̃t�@�C�����������݃��[�h�ŃI�[�v�����悤�Ƃ��������B
*/
class IOException 
	: public Exception
{
public:
	LN_EXCEPTION_BASIC_CONSTRUCTOR_DECL(IOException);
	IOException() {}
	virtual ~IOException() throw() {}

public:
	// override Exception
	virtual Exception* Copy() const { return LN_NEW IOException( *this ); }
};

/**
	@brief	���\�b�h�ɓn���ꂽ�����̂����ꂩ�������ȏꍇ�ɃX���[������O�B
*/
class ArgumentException
	: public Exception
{
public:
	LN_EXCEPTION_BASIC_CONSTRUCTOR_DECL(ArgumentException);
	ArgumentException() {}
	virtual ~ArgumentException() throw() {}

public:
	// override Exception
	virtual Exception* Copy() const { return LN_NEW ArgumentException( *this ); }
};

/**
	@brief	�I�u�W�F�N�g�̌��݂̏�Ԃɑ΂��Ė����ȌĂяo�����s��ꂽ
*/
class InvalidOperationException
	: public Exception
{
public:
	LN_EXCEPTION_BASIC_CONSTRUCTOR_DECL(InvalidOperationException);
	InvalidOperationException() {}
	virtual ~InvalidOperationException() throw() {}

public:
	// override Exception
	virtual Exception* Copy() const { return LN_NEW InvalidOperationException(*this); }
};

/**
	@brief		�T�|�[�g����Ȃ��@�\���Ăяo�����Ƃ���
	@details	�ǂݎ���p�X�g���[���ɑ΂��ď������݂��s�����ꍇ���B
*/
class NotSupportedException 
	: public Exception
{
public:
	LN_EXCEPTION_BASIC_CONSTRUCTOR_DECL(NotSupportedException);
	NotSupportedException() {}
	virtual ~NotSupportedException() throw() {}

public:
	// override Exception
	virtual Exception* Copy() const { return LN_NEW NotSupportedException( *this ); }
};

/**
	@brief	�f�B�X�N��ɑ��݂��Ȃ��t�@�C���ɃA�N�Z�X���悤�Ƃ��Ď��s�����Ƃ��ɃX���[������O
*/
class FileNotFoundException
	: public Exception
{
public:
	LN_EXCEPTION_BASIC_CONSTRUCTOR_DECL(FileNotFoundException);
	FileNotFoundException() {}
	virtual ~FileNotFoundException() throw() {}

public:
	// override Exception
	virtual Exception* Copy() const { return LN_NEW FileNotFoundException( *this ); }
};

/**
	@brief	�����ȃf�B���N�g���ɃA�N�Z�X���悤�Ƃ����Ƃ��ɃX���[������O
*/
class DirectoryNotFoundException
	: public Exception
{
public:
	LN_EXCEPTION_BASIC_CONSTRUCTOR_DECL(DirectoryNotFoundException);
	DirectoryNotFoundException() {}
	virtual ~DirectoryNotFoundException() throw() {}

public:
	// override Exception
	virtual Exception* Copy() const { return LN_NEW DirectoryNotFoundException( *this ); }
};

/**
	@brief	�t�@�C���╶���񓙂̌`�����s��
*/
class InvalidFormatException
	: public Exception
{
public:
	LN_EXCEPTION_BASIC_CONSTRUCTOR_DECL(InvalidFormatException);
	InvalidFormatException() {}
	virtual ~InvalidFormatException() throw() {}

public:
	// override Exception
	virtual Exception* Copy() const { return LN_NEW InvalidFormatException(*this); }
};

/**
	@brief	�������̋@�\���Ăяo����
*/
class NotImplementedException
	: public Exception
{
public:
	LN_EXCEPTION_BASIC_CONSTRUCTOR_DECL(NotImplementedException);
	NotImplementedException() {}
	virtual ~NotImplementedException() throw() {}

public:
	// override Exception
	virtual Exception* Copy() const { return LN_NEW NotImplementedException( *this ); }
};

/**
	@brief	C/C++�����^�C��API���ŃG���[����������
*/
class RuntimeException
	: public Exception
{
public:
	LN_EXCEPTION_BASIC_CONSTRUCTOR_DECL(RuntimeException);
	RuntimeException() {}
	virtual ~RuntimeException() throw() {}

public:
	// override Exception
	virtual Exception* Copy() const { return LN_NEW RuntimeException(*this); }
};

/**
	@brief	�����R�[�h�̕ϊ����A�}�b�s���O�ł��Ȃ������܂��͕s���V�[�P���X����������
*/
class EncodingFallbackException
	: public Exception
{
public:
	LN_EXCEPTION_BASIC_CONSTRUCTOR_DECL(EncodingFallbackException);
	EncodingFallbackException() {}
	virtual ~EncodingFallbackException() throw() {}

public:
	// override Exception
	virtual Exception* Copy() const { return LN_NEW EncodingFallbackException(*this); }
};


#ifdef LN_WIN32
/**
	@brief	WindowsAPI �̃G���[��������O�N���X (GetLastError())
*/
class Win32Exception 
	: public Exception
{
public:
	Win32Exception(DWORD dwLastError);
	virtual ~Win32Exception() throw() {}

public:
	DWORD			GetLastErrorCode() const { return m_dwLastErrorCode; }
	const TCHAR*	GetFormatMessage() const { return m_pFormatMessage; }

public:
	// override Exception
	virtual Exception* Copy() const { return LN_NEW Win32Exception( *this ); }

private:
	DWORD		m_dwLastErrorCode;
	TCHAR		m_pFormatMessage[512];	///< FormatMessage() �Ŏ擾�������b�Z�[�W
};
#endif

} // namespace Lumino
