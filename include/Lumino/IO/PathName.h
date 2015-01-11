#pragma once

#include "../Base/String.h"
#include "PathUtils.h"

namespace Lumino
{

/**
	@brief		�p�X�������\���N���X
	@details	�P���Ƀp�X�Z�p���[�^�ŋ�؂�ꂽ������������܂��B
				�p�X���t�H���_���w�����t�@�C�����w�����͋�ʂ��܂���B
				�܂��A�t�@�C���p�X�Ƃ��Ďg�p�ł��Ȃ��������܂܂�Ă��邩�͊m�F���܂���B
				�p�X�������̏�����m�F���܂���B<br>

				�Z�p���[�^�͊��ˑ��ł��BWindows �ł� '\\'�AUNIX �n�ł� '/' �ł��B
				��UNIX �n�ł� '\\' ���t�@�C�����Ƃ��Ďg�p�\�ł��B

				���̃N���X�Ɋ܂܂�� operator < ���̔�r�n�̋@�\�́A�S�đ啶������������ʂ��܂���B
				�܂��A�p�X��P�������������Ŕ�r���s���܂��B
				���啶����������ʂ� Unix�n�Ή��̍ۂɕς��\��������܂����A������ɂ��Ă� Windows ���ł͋�ʂ��܂���B

	@note		wxWidgets �ł� wxFileName �����̃N���X�ɑ�������B
				�啶������������ʂ��邩�́A�r���h���ɂ�� (�v���v������)�B��r���ɃI�v�V����������n�����Ƃŋ�ʂ̗L����ς��邱�Ƃ͉\�B
				���Ȃ݂ɁA�����ł͒P��̕�����ł͂Ȃ��Avolume�A������dir�Afilenaem�Aext �ɕ������ꂽ�����̕�����������o�ϐ��Ƃ��Ď����Ă���B
				http://docs.wxwidgets.org/trunk/classwx_file_name.html#abe8d3ef3a404ed7050ad705d77d47eae

				Qt �ł� QFileInfo / CDir �����̃N���X�ɑ�������B
				QFileInfo �́u�h���C�u���͑啶���v���A�����œƎ��̎g�p�����݂���B
				�啶���������̋�ʂ͊��ˑ��B(QDir::operator== �̎������Q�l�ɂȂ�)
				http://www.qtcentre.org/threads/38941-QFile-exists%28%29-and-QFileInfo-exists%28%29-and-case-sensitive-file-names
*/
template<typename TChar>
class BasicPathName
{
public:
	static const TChar Separator		= (TChar)PathUtils::DirectorySeparatorChar;
	static const TChar AltSeparator		= (TChar)PathUtils::AltDirectorySeparatorChar;
	static const TChar VolumeSeparator	= (TChar)PathUtils::VolumeSeparatorChar;

public:
	typedef BasicPathName<TChar> PathNameT;
	typedef BasicString<TChar> BasicStringT;

public:
	BasicPathName() {}

	/// @copydoc Assign
	explicit BasicPathName(const char* path) { Assign(path); }
	/// @copydoc Assign
	explicit BasicPathName(const wchar_t* path) { Assign(path); }
	/// @copydoc Assign
	explicit BasicPathName(const BasicStringT& path) { Assign(path); }

	/// @copydoc AssignUnderBasePath
	BasicPathName(const PathNameT& basePath, const char* relativePath) { AssignUnderBasePath(basePath, relativePath); }
	/// @copydoc AssignUnderBasePath
	BasicPathName(const PathNameT& basePath, const wchar_t* relativePath) { AssignUnderBasePath(basePath, relativePath); }
	/// @copydoc AssignUnderBasePath
	BasicPathName(const PathNameT& basePath, const BasicStringT& relativePath) { AssignUnderBasePath(basePath, relativePath); }
	/// @copydoc AssignUnderBasePath
	BasicPathName(const PathNameT& basePath, const PathNameT& relativePath) { AssignUnderBasePath(basePath, relativePath); }

	// operators
	BasicPathName& operator = (const BasicStringT& str)	{ Assign(str.c_str()); return (*this); }
	BasicPathName& operator = (const TChar* str) { Assign(str); return (*this); }
	bool operator < (const BasicPathName& right) const { return PathUtils::Compare(m_path.GetCStr(), right.m_path.GetCStr()) < 0; }
	bool operator < (const TChar* right) const { return PathUtils::Compare(m_path.GetCStr(), right) < 0; }
	operator const TChar*() const { return m_path.c_str(); }


public:

	/**
		@brief		�p�X����������蓖�Ă�
		@param[in]	path		: �p�X������
	*/
	void Assign(const char* path);
	/// @copydoc Assign
	void Assign(const wchar_t* path);
	/// @copydoc Assign
	void Assign(const BasicStringT& path) { Assign(path.c_str()); }

	/**
		@brief		�x�[�X�p�X�Ƒ��΃p�X��A�����āA�p�X���쐬����
		@param[in]	basePath		: �p�X������
		@param[in]	relativePath	: �p�X������
		@details	relativePath ���t���p�X�̏ꍇ�� basePath �𖳎����܂��B
	*/
	void AssignUnderBasePath(const PathNameT& basePath, const char* relativePath);
	/// @copydoc AssignUnderBasePath
	void AssignUnderBasePath(const PathNameT& basePath, const wchar_t* relativePath);
	/// @copydoc AssignUnderBasePath
	void AssignUnderBasePath(const PathNameT& basePath, const BasicStringT& relativePath) { AssignUnderBasePath(basePath, relativePath.c_str()); }
	/// @copydoc AssignUnderBasePath
	void AssignUnderBasePath(const PathNameT& basePath, const PathNameT& relativePath) { AssignUnderBasePath(basePath, relativePath.GetCStr()); }

	/**
		@brief		���݂̃p�X�ɕʂ̃p�X�������A������
		@param[in]	path		: �p�X������
		@details	���݂̃p�X�̖����ɃZ�p���[�^�������ꍇ�͕t�����A�������A�����܂��B
					path ����΃p�X�ł���΁A���݂̃p�X��u�������܂��B
	*/
	void Append(const TChar* path);
	/// @copydoc Append
	void Append(const PathNameT& path) { Append(path.m_path.c_str()); }

	/// �󕶎����ݒ肷��
	void SetEmpty() { m_path.SetEmpty(); }

	/// �p�X������̒�����g���q���܂ރt�@�C�����̕�����Ԃ� (��p�X�̏ꍇ�͋󕶎����Ԃ�)
	BasicStringT GetFileName() const { return PathUtils::GetFileName(m_path.c_str()); }

	/// �p�X�� TChar �������Ԃ�
	const TChar* GetCStr() const { return m_path.c_str(); }
	
	/// �p�X�������Ԃ�
	const BasicStringT& GetString() const { return m_path; }

	/// �p�X�������Ԃ� (�����ɕK���Z�p���[�^���ЂƂt������)
	const BasicString<TChar> GetStrEndSeparator() const;

	/// �p�X����ł��邩���m�F����
	bool IsEmpty() const { return m_path.empty(); }

	/// ��΃p�X�ł��邩���m�F����
	bool IsAbsolutePath() const;

	/// ���[�g�f�B���N�g���ł��邩���m�F����
	bool IsRoot() const;

	/// �w�肳�ꂽ�g���q�������Ă��邩���m�F���� (pExt �� . �̗L���͖��Ȃ�)
	bool CheckExt(const TChar* ext) const;

	/**
		@brief		�e�f�B���N�g���� PathName ��Ԃ�
		@details	���݂̃p�X�����[�g�p�X ('C:\' �� '/') �̏ꍇ�A���݂̃p�X��Ԃ��܂��B
					����ȊO�ɐe�f�B���N�g�������ł��Ȃ��ꍇ�͋󕶎���Ԃ��܂��B
					�ȉ��̗���Q�l�ɂ��Ă��������B
		@code
					"C:/dir1/file1.txt"	�� "C:/dir1"
					"C:/dir1"			�� "C:/"
					"C:/"				�� "C:/"
					"C:"				�� "C:"
					"/"					�� "/"		(Unix)
					"file1.txt"			�� ""
					".."				�� ""
					"."					�� ""
					""					�� ""
		@endcode
	*/
	PathNameT GetParent() const;

	/**
		@brief		�p�X��P�������A�t���p�X�ɂ������̂�Ԃ�
		@details	�p�X�����΃p�X�̏ꍇ�̓J�����g�f�B���N�g������ɁA�t���p�X�����߂܂��B
					�܂��AWindows ���ł� / �� \ �ɒu���������܂��B
	*/
	PathNameT CanonicalizePath() const;

	/// ���[�J���� char ������\���Ƃ��ĕԂ�
	std::string ToLocalChar() const;

	/**
		@brief		���̃p�X�ƁA�ʂ̃p�X�����񂪓����������`�F�b�N����
		@param[in]	path	: ��r�Ώۂ̃p�X������
		@details	Separator �� AltSeparator �͓����Ƃ݂Ȃ��܂��B
					�܂��A�啶������������ʂ��܂���B
		@note		���ɂ��啶���������̋�ʂɂ��āc<br>
					��ʂ̗L���́AOS�ł͂Ȃ��t�@�C���V�X�e���ˑ��B
					http://ja.wikipedia.org/wiki/%E3%83%95%E3%82%A1%E3%82%A4%E3%83%AB%E3%82%B7%E3%82%B9%E3%83%86%E3%83%A0#.E6.A9.9F.E8.83.BD
					������v���O�������璲�ׂ�L���Ȏ�i��������Ȃ������c�B
					���Ȃ݂ɁAAdobe���N���X�v���b�g�t�H�[���œ������i���o���Ă�Ƃ�����A���̕ӂ�͏\���ɑΉ�������Ă��Ȃ��l�q�B
					http://helpx.adobe.com/jp/x-productkb/global/cpsid_83180.html
					http://www.clip-studio.com/clip_site/support/faq/detail/svc/52/tid/37429
	*/
	bool Equals(const TChar* path) const { return PathUtils::Equals(m_path.c_str(), path); }
	/// @copydoc Equals
	bool Equals(const PathNameT& path) const { return PathUtils::Equals(m_path.c_str(), path.GetCStr()); }
	/// @copydoc Equals
	bool Equals(const BasicStringT& path) const { return PathUtils::Equals(m_path.c_str(), path.c_str()); }

public:

	/**	
		@brief		����t�H���_���Ń��j�[�N�ȃt�@�C���p�X(��΃p�X)�𐶐����ĕԂ�
		@param[in]	directory	: �t�H���_�p�X
		@param[in]	filePrefix	: �t�@�C�����̐ړ��� �܂��� NULL
		@param[in]	extName		: �t�@�C���̊g���q (�v���t�B�b�N�X�B.���܂߂邱��) �܂��� NULL
		@details	���������t�@�C���p�X�� "<directory>/<filePrefix><ID><extName>" �ƂȂ�܂��B
					ID �͎��ԏ����L�[�Ƃ��Đ����A<br>
					filePrefix�AextName �� NULL �̏ꍇ�͋󕶎������Ō�������܂��B
					�Ⴆ�Η��� NULL �ɂ���ƁA�ԍ������̃t�@�C�����ɂȂ�܂��B
	*/
	static PathNameT GetUniqueFilePathInDirectory(const PathNameT& directory, const TChar* filePrefix, const TChar* extName);

	/// (������̓t�@�C����������Ԃ�)
	static BasicStringT GetUniqueFileNameInDirectory(const PathNameT& directory, const TChar* filePrefix, const TChar* extName);

private:
	BasicStringT	m_path;

private:
	//void ConvertSeparatorToInternal(BasicStringT* path);
};

typedef BasicPathName<TCHAR>	PathName;
typedef BasicPathName<char>		PathNameA;
typedef BasicPathName<wchar_t>	PathNameW;

} // namespace Lumino
