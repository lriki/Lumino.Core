
#pragma once

//------------------------------------------------------------------------------
// flags

// Debug �r���h�t���O
#if defined(DEBUG) || defined(_DEBUG)
	#define LN_DEBUG
#endif

#if (defined(_WIN32) || defined(__WIN32__) || defined(WIN32) || defined(__CYGWIN__) || defined(Q_OS_WIN))
	#define LN_WIN32
#endif

// �v���b�g�t�H�[��
#if defined(__WIN32__) || defined(_WIN32) || defined(__CYGWIN__)
	#define LN_WIN32
#else	// �Ƃ肠����	
	#define LN_LINUX
#endif

// �R���p�C��
#if defined(__GNUC__)
	#define LN_GNUC
#elif defined(_MSC_VER)
	#define LN_MSVC
#else
	#error "invalid compiler"
#endif

// �A�[�L�e�N�`��
#if defined(_WIN64) || defined(__x86_64__)
	#define LN_X64
#endif

// �X���b�h���C�u����
#if defined(__CYGWIN__) || !defined(LN_WIN32)
	#define LN_THREAD_POSIX
#else
	#define LN_THREAD_WIN32
#endif

// �����R�[�h�Z�b�g
#if defined (UNICODE) || defined(_UNICODE)
	#ifndef LN_UNICODE
		#define LN_UNICODE
	#endif
#else
	#ifndef LN_MBS
		#define LN_MBS
	#endif
#endif

// DLL exports
#if defined(LUMINO_DLL) && defined(LUMINO_BUILD_DLL)
	/* LUMINO_DLL �� LUMINO_BUILD_DLL �̓�����`�͋֎~�B
	* LUMINO_DLL �̓��C�u������DLL�Ƃ��Ďg���Ƃ��A
	* LUMINO_BUILD_DLL �̓��C�u�����{�̂��r���h����Ƃ��ɂ̂ݎg�p����B
	*/
	#error "You must not have both LUMINO_DLL and LUMINO_BUILD_DLL defined"
#endif

#if defined(_WIN32) && defined(LUMINO_BUILD_DLL)
	#define LUMINO_EXPORT	__declspec(dllexport)

#elif defined(_WIN32) && defined(LUMINO_DLL)
	#define LUMINO_EXPORT	__declspec(dllimport)

#elif defined(__GNUC__) && defined(LUMINO_BUILD_DLL)
	#define LUMINO_EXPORT	__attribute__((visibility("default")))

#else
	#define LUMINO_EXPORT

#endif

//------------------------------------------------------------------------------
// defines

#if defined(LN_MSVC)
	#define LN_STDCALL	__stdcall
	#define LN_CDECL	__cdecl
#else
	#define LN_STDCALL	
	#define LN_CDECL	
#endif

//----------------------------------------------------------------------
// include
#include <assert.h>

#if defined(LN_WIN32)
	#define NOMINMAX
	#include <windows.h>
	#include <process.h>
#else
	#include <limits.h>		// for _PC_PATH_MAX (on Ubuntu)
#endif

//----------------------------------------------------------------------
// const

// �t�@�C���p�X�̍ő啶����
#if defined(LN_WIN32)
	#define LN_MAX_PATH		MAX_PATH	// 260
#elif defined(PATH_MAX)
	#define LN_MAX_PATH		PATH_MAX
#elif defined(_PC_PATH_MAX)
	#define LN_MAX_PATH		_PC_PATH_MAX
#else
	#error invalid LN_MAX_PATH
#endif

#ifdef LN_DEBUG
	#define LN_ASSERT(x)	assert(x);
#else
	#define LN_ASSERT(x)
#endif

#ifndef LN_SAFE_FREE
	#define LN_SAFE_FREE( p ) { if ( p ) { free( ( p ) ); ( p ) = NULL; } }
#endif

#ifndef LN_SAFE_DELETE
	#define LN_SAFE_DELETE( p ) { if ( p ) { delete ( p ); ( p ) = NULL; } }
#endif

#ifndef LN_SAFE_DELETE_ARRAY
	#define LN_SAFE_DELETE_ARRAY( p ) { if ( p ) { delete[] ( p ); ( p ) = NULL; } }
#endif

/// �z��̗v�f����Ԃ��}�N��
#ifndef LN_ARRAY_SIZE_OF
	#define LN_ARRAY_SIZE_OF(ary)     ( sizeof(ary) / sizeof( ary[0] ) )
#endif

//----------------------------------------------------------------------
// Base include

#include <string.h>

#include "Typedef.h"
#include "Memory.h"
#include "CRT.h"

namespace Lumino
{
	class ImplAccessor;
} // namespace Lumino
