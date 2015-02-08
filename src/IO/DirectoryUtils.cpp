
#include "../Internal.h"
#include "../../include/Lumino/IO/PathName.h"
#include "../../include/Lumino/IO/DirectoryUtils.h"

namespace Lumino
{

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
#ifdef _WIN32
Array<String> DirectoryUtils::GetFiles(const TCHAR* drPath, const TCHAR* pattern)
{
	Array<String> fileList;
	PathName dirPathKey(drPath);
	String dirPatternPath(dirPathKey.GetStrEndSeparator());
	if (pattern) {
		dirPathKey.Append(pattern);
	}
	else {
		dirPathKey.Append(_T("*"));
	}

    // �����J�n
	WIN32_FIND_DATA fd;
	HANDLE h = ::FindFirstFile(dirPathKey.GetCStr(), &fd);
	if (h == INVALID_HANDLE_VALUE)
	{
		DWORD dwError = ::GetLastError();
		if (dwError == ERROR_FILE_NOT_FOUND ||
			dwError == ERROR_NO_MORE_FILES){
			// �����͋��B��̔z���Ԃ��B
			return fileList;
		}
		else {
			LN_THROW(0, Win32Exception, dwError);
		}
	}

    do
	{
        if ( fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY ) {
            // �f�B���N�g���̏ꍇ
        }
        else {
            // �t�@�C���̏ꍇ
			fileList.Add(dirPatternPath + fd.cFileName);
        }

		// ���̃t�@�C������������
	} while (::FindNextFile(h, &fd));

	// �I��
	::FindClose(h);
	return fileList;
}

#else
Array<String> DirectoryUtils::GetFiles(const TCHAR* drPath, const TCHAR* pattern)
{
	LN_THROW(0, NotImplementedException);
	// http://www.syuhitu.org/other/dir.html
	Array<String> fileList;
	return fileList;
}
#endif

} // namespace Lumino
