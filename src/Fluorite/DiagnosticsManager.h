
#pragma once
#include "Common.h"

LN_NAMESPACE_BEGIN
namespace parser
{

enum class DiagnosticsCode
{
	Severity_Note		= 0x00000000,
	Severity_Warning	= 0x10000000,
	Severity_Error		= 0x20000000,
	Severity_Fatal		= 0x30000000,

	CppLexerGroup = 0x00010000,
	//CppLexer_InvalidToken = Severity_Error | CppLexerGroup | 0x0001,
	UnexpectedEOFInBlockComment = Severity_Error | CppLexerGroup | 0x0001,


	RpnEvaluatorGroup = 0x00020000,
	RpnEvaluator_InvalidNumericLiteral		= Severity_Error | RpnEvaluatorGroup | 1,
	RpnEvaluator_UnexpectedToken			= Severity_Error | RpnEvaluatorGroup | 2,	/**< �\�����Ȃ��g�[�N����������܂����B*/
	RpnEvaluator_InsufficientToken			= Severity_Error | RpnEvaluatorGroup | 3,	/**< ����]�����邽�߂̃g�[�N�����s�����Ă��܂��B(�����������Ă��Ȃ�) */
	RpnEvaluator_InvalidFuncCallArgsCount	= Severity_Error | RpnEvaluatorGroup | 4,	/**< �֐��Ăяo���̈���������܂���B*/
	RpnEvaluator_InvalidOperatorSide		= Severity_Error | RpnEvaluatorGroup | 5,	/**< ���Z�q�̕ӂɑ΂���I�y�����h������܂���B*/
	RpnEvaluator_InvalidNumericType			= Severity_Error | RpnEvaluatorGroup | 6,	/**< �����Ȑ��l�^�̃g�[�N�������͂���܂����B*/
	RpnEvaluator_OperatorInvalidType		= Severity_Error | RpnEvaluatorGroup | 7,	/**< ���Z�q�̌^�ɖ����Ȍ^({0})�̃I�y�����h���w�肳��Ă��܂��B*/

	PreprocessorGroup = 0x00030000,
	Preprocessor_UnexpectedDirectiveToken	= Severity_Error | PreprocessorGroup | 1,	/**< �\�����Ȃ��v���v���Z�b�T�f�B���N�e�B�u�g�[�N����������܂����B{0} */
	Preprocessor_SyntaxError				= Severity_Error | PreprocessorGroup | 2,	/**< �v���v���Z�b�T�f�B���N�e�B�u�̍\���G���[�ł��B*/
	Preprocessor_InvalidConstantExpression	= Severity_Error | PreprocessorGroup | 4,	/**< �����萔���������ł��B*//* C1017 invalid integer constant expression */
	Preprocessor_UnexpectedElse				= Severity_Error | PreprocessorGroup | 5,	/**< �\�����Ȃ� #else ��������܂����B*/
	Preprocessor_UnexpectedEndif			= Severity_Error | PreprocessorGroup | 6,	/**< �\�����Ȃ� #endif ��������܂����B*/
	Preprocessor_UnexpectedElif				= Severity_Error | PreprocessorGroup | 6,	/**< �\�����Ȃ� #elif ��������܂����B*/
	Preprocessor_NoExistsEndif				= Severity_Error | PreprocessorGroup | 7,	/**< ���̃f�B���N�e�B�u�� #endif ������܂���B*/
	Preprocessor_ExpectedDefinedId			= Severity_Error | PreprocessorGroup | 8,	/**< defined �̌�Ɏ��ʎq���K�v�ł��B*//* C2003 expected 'defined id' */
	Preprocessor_InvalidHeaderName			= Severity_Error | PreprocessorGroup | 9,	/**< �w�b�_���������ł��B*/
	Preprocessor_IncludeFileNotFount		= Severity_Error | PreprocessorGroup | 10,	/**< include �t�@�C�����J���܂���B{0} */
};

enum class SeverityLevel
{
	Note,
	Warning,
	Error,
	Fatal,
};
	
/**
	@brief
*/
class DiagnosticsManager
{
public:
	//void SetFilePath(const PathName& filePath) { m_filePath = filePath; }
	int SetLineNumber(int line) { m_lineNumber = line; }		// 0�`
	int SetColumnNumber(int column) { m_columnNumber = column; }	// 0�`

	void Report();

private:
	int			m_lineNumber;
	int			m_columnNumber;
};

/**
	@brief
*/
class DiagnosticsItem
{
public:
	DiagnosticsCode GetCode() const { return m_code; }
	const StringArray& GetOptions() const { return m_options; }

private:
	friend class DiagnosticsItemSet;
	//DiagnosticsItem();
	//PathName	m_filePath;		// �ł��邾�� DiagnosticsManager ���璼�ڑ������悤�ɂ��A�����������L�ł���悤�ɂ���
	DiagnosticsCode	m_code;
	int				m_lineNumber;
	int				m_columnNumber;
	StringArray		m_options;
};

/**
	@brief	�t�@�C��1�P�ʂ��O���[�v����������
*/
class DiagnosticsItemSet
{
public:
	/*
		�t�@�C���p�X�ȂǕ����̃G���[���������܂Ƃ߂����́B
		�t�@�C���p�X�p�̃������� Item ���ƂɎ������Ȃ��悤�ɂ���̂��ړI��1�B
		���񏈗�����Ƃ����A�G���[���|�[�g�̂��тɃ��b�N����̂͂�낵���Ȃ��B
	*/

	int SetCurrentLineNumber(int line) { m_currentLineNumber = line; }
	int SetCurrentColumnNumber(int column) { m_currentColumnNumber = column; }

	void Report(DiagnosticsCode code);
	void Report(DiagnosticsCode code, String option1);

	void ClearItems() { m_items.Clear(); }
	const Array<DiagnosticsItem>* GetItems() { return &m_items; }

	bool HasError() const { return !m_items.IsEmpty(); }

private:
	PathName	m_filePath;
	int			m_currentLineNumber;
	int			m_currentColumnNumber;
	Array<DiagnosticsItem>	m_items;
};

#define LN_DIAG_REPORT_ERROR(x, errorCode, ...)	\
	if (!(x)) { \
		m_diag->Report(errorCode); \
		return ResultState::Error; \
	}

} // namespace Parser
LN_NAMESPACE_END

