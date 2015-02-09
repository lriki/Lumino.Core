﻿/*
	VC++ CRT の関数を Linux 用に実装したもの。
*/

#pragma once

#if defined(LN_GNUC) || defined(__CYGWIN__)

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <errno.h>
#include <time.h>

namespace Lumino
{

#define EINVAL          22
#define ERANGE          34
//#define EILSEQ          42
#define STRUNCATE       80
#define _TRUNCATE ((size_t)-1)

typedef int errno_t;
typedef size_t rsize_t;

//-----------------------------------------------------------------------------
// http://msdn.microsoft.com/ja-jp/library/wes2t00f.aspx
//-----------------------------------------------------------------------------
inline errno_t memcpy_s(void * _Dst, rsize_t _DstSize, const void * _Src, rsize_t _MaxCount)
{
	if (_MaxCount == 0) {
        return 0;
    }
	if (_Dst == NULL) {
        return EINVAL;
    }
    if (_Src == NULL || _DstSize < _MaxCount)
    {
        memset(_Dst, 0, _DstSize);
        return EINVAL;
    }

    memcpy(_Dst, _Src, _MaxCount);
    return 0;
}

//-----------------------------------------------------------------------------
// http://msdn.microsoft.com/ja-jp/library/ms235365.aspx
//-----------------------------------------------------------------------------
inline int wcsicmp(const wchar_t* s1, const wchar_t* s2) 
{ 
	wchar_t c1 = *s1, c2 = *s2; 
	while (c1 != 0 && c2 != 0) 
	{ 
		if (c1 >= 'a' && c1 <= 'z') {
			c1 -= 'a' + 'A'; 
		}
		if (c2 >= 'a' && c2 <= 'z') {
			c2 -= 'a' + 'A'; 
		}
		if (c2 < c1) { 
			return -1; 
		}
		else if (c2 > c1) {
			return 1; 
		}
		c1 = *(++s1); c2 = *(++s2); 
	} 
	return 0; 
}

//-----------------------------------------------------------------------------
// http://msdn.microsoft.com/ja-jp/library/eyktyxsx.aspx
//-----------------------------------------------------------------------------
inline errno_t mbstowcs_s( size_t *pConvertedChars, wchar_t *wcstr, size_t sizeInWords, const char *mbstr, size_t count)
{
	if (wcstr == NULL && count > 0)
		return EINVAL;
	if (wcstr != NULL && sizeInWords == 0)
		return EINVAL;
	if (mbstr == NULL)
		return EINVAL;
		
	int len = strlen(mbstr);
	if (len > count)
		return ERANGE;
	
	*pConvertedChars = mbstowcs(wcstr, mbstr, count);
	return 0;
}

//-----------------------------------------------------------------------------
// http://msdn.microsoft.com/ja-jp/library/f30dzcf6.aspx
//-----------------------------------------------------------------------------
inline int _snprintf_s(
	char *buffer,
	size_t sizeOfBuffer,
	size_t count,
	const char *format,
	...
)
{
	if (count == _TRUNCATE) {
		count = sizeOfBuffer;
	}
	
	va_list arglist;
	va_start(arglist, format);
	return vsnprintf(buffer, count, format, arglist);
}

//-----------------------------------------------------------------------------
// http://msdn.microsoft.com/ja-jp/library/w6w3kbaf.aspx
//-----------------------------------------------------------------------------
inline errno_t strncat_s(
	char *strDest,
	size_t numberOfElements,
	const char *strSource,
	size_t count
)
{
	// TODO
	strcat(strDest, strSource);
	return 0;
}

//-----------------------------------------------------------------------------
// http://msdn.microsoft.com/ja-jp/library/z5hh6ee9.aspx
//-----------------------------------------------------------------------------
inline errno_t fopen_s( 
   FILE** pFile,
   const char *filename,
   const char *mode 
)
{
	if (!pFile) {
		return EINVAL;
	}
	*pFile = fopen(filename, mode);
	if(*pFile != NULL) {
        return 0;
	}
    return errno;
}

//-----------------------------------------------------------------------------
// http://msdn.microsoft.com/ja-jp/library/z5hh6ee9.aspx
//-----------------------------------------------------------------------------
inline errno_t ctime_s( 
	char* buffer,
	size_t numberOfElements,
	const time_t *time
)
{
	if (!buffer || numberOfElements >= 26 || !time) {
		return EINVAL;
	}
	const char* str = ctime(time);
	strcpy(buffer, str);
	return 0;
}

//-----------------------------------------------------------------------------
// http://msdn.microsoft.com/ja-jp/library/td1esda9.aspx
//-----------------------------------------------------------------------------
inline errno_t strcpy_s(
   char *strDestination,
   size_t numberOfElements,
   const char *strSource 
)
{
	if (!strDestination || !strSource) {
		return EINVAL;
	}
	int len = strlen(strSource);
	if (numberOfElements < len + 1) {
		return ERANGE;
	}
	strcpy(strDestination, strSource);
	return 0;
}

//-----------------------------------------------------------------------------
// https://msdn.microsoft.com/ja-jp/library/td1esda9.aspx
//-----------------------------------------------------------------------------
inline errno_t wcscpy_s(
	wchar_t *strDestination,
	size_t numberOfElements,
	const wchar_t *strSource)
{
	if (!strDestination || !strSource) {
		return EINVAL;
	}
	int len = wcslen(strSource);
	if (numberOfElements < len + 1) {
		return ERANGE;
	}
	wcscpy(strDestination, strSource);
	return 0;
}

//-----------------------------------------------------------------------------
// https://msdn.microsoft.com/ja-jp/library/z5hh6ee9.aspx
//-----------------------------------------------------------------------------
errno_t _fopen_s(
	FILE** pFile,
	const char *filename,
	const char *mode )
{
	if (!pFile || !filename || !mode) {
		return EINVAL;
	}

	*pFile = fopen(filename, mode);
	return 0;
}

//-----------------------------------------------------------------------------
// https://msdn.microsoft.com/ja-jp/library/z5hh6ee9.aspx
//-----------------------------------------------------------------------------
errno_t _wfopen_s(
	FILE** pFile,
	const wchar_t *filename,
	const wchar_t *mode )
{
	if (!pFile || !filename || !mode) {
		return EINVAL;
	}

	char mbcsFilename[LN_MAX_PATH + 1];	// linux なら UTF8
	char mbcsMode[LN_MAX_PATH + 1];	// linux なら UTF8
	if (wcstombs(mbcsFilename, filename, LN_MAX_PATH) < 0) {
		return EINVAL;
	}
	if (wcstombs(mbcsMode, mode, LN_MAX_PATH) < 0) {
		return EINVAL;
	}

	*pFile = fopen(mbcsFilename, mbcsMode);
	return 0;
	// http://www.linuxquestions.org/questions/programming-9/_wfopen-does-not-work-on-linux-692380/
}

} // namespace Lumino

#endif
