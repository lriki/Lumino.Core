
#include <sys/types.h>
#include <sys/stat.h>
#include "../../Internal.h"
#include "../../../include/Lumino/Base/RefBuffer.h"
#include "../../../include/Lumino/Text/Encoding.h"
#include "../../../include/Lumino/IO/FileStream.h"
#include "../../../include/Lumino/IO/FileUtils.h"

namespace Lumino
{

#define MBCS_FILEPATH(mbcsPath, srcWPath) ?
	char mbcsPath[LN_MAX_PATH + 1]; ?
	if (wcstombs(mbcsPath, srcWPath, LN_MAX_PATH) < 0) { ?
		LN_THROW(0, IOException); ?
		}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
static bool is_stat_writable(struct stat *st, const char *path)
{
	// �����Ȃ��ɏ������݉ł��邩
	if (st->st_mode & S_IWOTH)
		return 1;
	// ���݂̃��[�U�[ID�ɋ�����Ă��邩
	if ((st->st_uid == geteuid()) && (st->st_mode & S_IWUSR))
		return 1;
	// ���݂̃O���[�vID�ɋ�����Ă��邩
	if ((st->st_gid == getegid()) && (st->st_mode & S_IWGRP))
		return 1;
	// ���������ɗ��邱�Ƃ͂قƂ�ǂȂ��͂������O�̂���
	return access(path, W_OK) == 0;
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
bool FileUtils::Exists(const char* filePath)
{
	// ��fopen �ɂ��`�F�b�N��NG�B�t�@�C�����r�����b�N�ŊJ����Ă������Ɏ��s����B
	
	// http://www.ie.u-ryukyu.ac.jp/?kono/lecture/1999/os/info1/file-2.html
	//struct stat st;
	//int ret = ::stat(file, &st);
	//st.st_mode
	int ret = access(filePath, F_OK);
	if (ret == -1) {
		if (errno == ENOENT) {
			return false;
		}
		else {
			// �p�X�������A������������Ȃ������R�͗l�X�B
			// http://linuxjm.sourceforge.jp/html/LDP_man-pages/man2/faccessat.2.html
			LN_THROW(0, IOException, strerror(errno));
		}
	}
	return true;
}
bool FileUtils::Exists(const wchar_t* filePath)
{
	MBCS_FILEPATH(mbcsFilePath, filePath);
	return Exists(mbcsFilePath);
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
uint32_t FileUtils::GetAttribute(const char* filePath)
{
	// Unix �n�̏ꍇ�A�t�@�C���̐擪�� . �ł���ΉB���t�@�C���ł���B
	// mono-master/mono/io-layer/io.c �́A_wapi_stat_to_file_attributes ���Q�l�ɂȂ�B
	struct stat st;
	int ret = ::stat(filePath, &st);
	if (ret == -1) {
		LN_THROW(0, IOException);
	}

	uint32_t attrs = 0;
	if (S_ISDIR(st.st_mode))
	{
		attrs |= FileAttribute_Directory;
		if (!is_stat_writable(&st, filePath)) {
			attrs |= FileAttribute_ReadOnly;
		}
		if (filePath[0] == '.') {
			attrs |= FileAttribute_Hidden;
		}
	}
	else
	{
		if (!is_stat_writable(&st, filePath)) {
			attrs |= FileAttribute_ReadOnly;
		}
		if (filePath[0] == '.') {
			attrs |= FileAttribute_Hidden;
		}
	}
	return attrs;
}

uint32_t FileUtils::GetAttribute(const wchar_t* filePath)
{
	MBCS_FILEPATH(mbcsFilePath, filePath);
	return GetAttribute(mbcsFilePath);
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void FileUtils::Copy(const char* sourceFileName, const char* destFileName, bool overwrite)
{
	// http://ppp-lab.sakura.ne.jp/ProgrammingPlacePlus/c/044.html
	FILE* fpSrc = fopen(sourceFileName, "rb");
	if (fpSrc == NULL){
		LN_THROW(0, IOException);
	}

	FILE* fpDest = fopen(destFileName, "wb");
	if (fpDest == NULL){
		LN_THROW(0, IOException);
	}

	// �o�C�i���f�[�^�Ƃ��� 1byte ���R�s�[
	// (windows �ł̓o�b�t�@�����O���������ǁA����ȊO�͂킩��Ȃ��B
	//  Linux �Ƃ��ŋɒ[�ɒx���Ȃ�悤�Ȃ炱���Ńo�b�t�@�����O���l����)
	while (1)
	{
		byte_t b;
		fread(&b, 1, 1, fpSrc);
		if (feof(fpSrc)){
			break;
		}
		if (ferror(fpSrc)){
			LN_THROW(0, IOException);
		}

		fwrite(&b, sizeof(b), 1, fpDest);
	}

	fclose(fpDest);
	fclose(fpSrc);
}

void FileUtils::Copy(const wchar_t* sourceFileName, const wchar_t* destFileName, bool overwrite)
{
	MBCS_FILEPATH(mbcsSrc, sourceFileName);
	MBCS_FILEPATH(mbcsDest, destFileName);
	Copy(mbcsSrc, mbcsDest, overwrite);
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void FileUtils::Delete(const char* filePath)
{
	int ret = remove(filePath);
	if (ret == -1) LN_THROW(0, IOException, strerror(errno));
}
void FileUtils::Delete(const wchar_t* filePath)
{
	MBCS_FILEPATH(mbcsFilePath, filePath);
	Delete(mbcsFilePath);
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
size_t FileUtils::GetFileSize(const TCHAR* filePath)
{
	LN_THROW( filePath != NULL, ArgumentException );

	FILE* fp;
	errno_t r = _tfopen_s( &fp, filePath, _T("r") );
	LN_THROW( r == 0, FileNotFoundException );

	size_t size = 0;
	try
	{
		size = GetFileSize(fp);
	}
	catch (...)
	{
		fclose( fp );
		throw;
	}
	fclose( fp );
	return size;
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
size_t FileUtils::GetFileSize( FILE* stream )
{
	struct stat stbuf;
	int handle = fileno( stream );
	if ( handle == 0 ) {
		return 0;
	}
	if (fstat(handle, &stbuf) == -1) {
		return 0;
	}
	return stbuf.st_size;
}

} // namespace Lumino