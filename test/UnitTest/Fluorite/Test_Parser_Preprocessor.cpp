#include "TestConfig.h"
#include "../src/Parser/Context.h"
#include "../src/Parser/EntityDatabase.h"
#include "../src/Parser/Frontend/Cpp/Preprocessor.h"

class Test_Parser_Preprocessor : public ::testing::Test
{
protected:
	Context m_context;
	ByteBuffer m_buffer;
	CppLexer m_lex;
	Preprocessor m_prepro;
	RefPtr<CompileUnitFile> m_fileCache;
	CompileOptions m_compileOptions;
	DiagnosticsItemSet m_diag;
	TokenListPtr m_tokens;

	Array<PathName> m_additionalIncludePaths;
	MacroMapContainer m_definedMacros;

	virtual void SetUp() 
	{
		m_additionalIncludePaths.Add(LN_LOCALFILE("TestData"));
	}
	virtual void TearDown() {}


	void Preprocess(const char* code)
	{
		bool r = TryPreprocess(code);
		if (!r) {
			LN_THROW(0, InvalidOperationException);
		}
	}

	bool TryPreprocess(const char* code)
	{
		m_context.Clear();
		m_diag.ClearItems();
		m_fileCache = RefPtr<CompileUnitFile>::MakeRef();
		m_fileCache->Initialize(LN_LOCALFILE("test.c"));
		m_buffer = ByteBuffer(code);
		m_tokens = m_lex.Tokenize(m_buffer, &m_diag);

		EntityDatabase db;
		ReferenceTracker rt;
		rt.Initialize(&db);
		bool r = m_prepro.BuildPreprocessedTokenList(&m_context, m_fileCache, m_tokens, m_fileCache, &m_additionalIncludePaths, m_definedMacros, &rt, &m_diag) == ResultState::Success;
		return r;
	}
};
//#error aaa
//#error "aaa"
/*
	defined()

	#defnie
	#error
	#endif
	#else
	#elif
	#include
	#ifndef
	#ifdef
	#line
	#pragma
	#undef
	#warning
	#
*/


//-----------------------------------------------------------------------------
TEST_F(Test_Parser_Preprocessor, Basic)
{
	{
		const char* code = "#define AAA";
		Preprocess(code);
	}
	{
		const char* code = "#define AAA 1";
		Preprocess(code);
	}
	{
		const char* code = "#define AAA 1 ";
		Preprocess(code);
	}
}

//-----------------------------------------------------------------------------
TEST_F(Test_Parser_Preprocessor, Basic_define)
{
#if 0
	// <Test> �֐��`���}�N���̒�` (��������)
	{
		const char* code = "#define AAA() 1";
		Preprocess(code);
		m_fileCache->outputMacroMap;
	}
	// <Test> �֐��`���}�N���̒�` (����1��)
	{
		const char* code = "#define AAA(x) x+1";
		Preprocess(code);
	}
	// <Test> �֐��`���}�N���̒�` (����2�ȏ�)
	{
		const char* code = "#define AAA(x, y) x+y";
		Preprocess(code);
	}
	// <Test> �֐��`���}�N���̒�` (�ϒ�����)
	{
		const char* code = "#define AAA(...) __VA_ARGS__";
		Preprocess(code);
	}
	// <Test> �֐��`���}�N���̒�` (����1�� + �ϒ�����)
	{
		const char* code = "#define AAA(x, ...) x+__VA_ARGS__";
		Preprocess(code);
	}
#endif
}

//-----------------------------------------------------------------------------
TEST_F(Test_Parser_Preprocessor, Basic_if)
{
	// <Test> #if 1
	{
		const char* code =
			"#if 1\n"
			"1\n"
			"#endif\n";
		Preprocess(code);
		ASSERT_EQ(true, m_tokens->GetAt(5).IsValid());	// '1'
		ASSERT_EQ(true, m_tokens->GetAt(6).IsValid());	// '\n'
	}
	// <Test> #if 0
	{
		const char* code =
			"#if 0\n"
			"1\n"
			"#endif\n";
		Preprocess(code);
		ASSERT_EQ(false, m_tokens->GetAt(5).IsValid());	// '1'
		ASSERT_EQ(false, m_tokens->GetAt(6).IsValid());	// '\n'
	}
	// <Test> #if true
	{
		const char* code =
			"#if true\n"
			"1\n"
			"#endif\n";
		Preprocess(code);
		ASSERT_EQ(true, m_tokens->GetAt(5).IsValid());	// '1'
	}
	// <Test> #if false
	{
		const char* code =
			"#if false\n"
			"1\n"
			"#endif\n";
		Preprocess(code);
		ASSERT_EQ(false, m_tokens->GetAt(5).IsValid());	// '1'
	}
	// <Test> #if �� (���� 0 �ȊO)
	{
		const char* code =
			"#if 1*2-1\n"
			"1\n"
			"#endif\n";
		Preprocess(code);
		ASSERT_EQ(true, m_tokens->GetAt(9).IsValid());	// '1'
	}
	// <Test> #if �� (���� 0)
	{
		const char* code =
			"#if 1-1\n"
			"1\n"
			"#endif\n";
		Preprocess(code);
		ASSERT_EQ(false, m_tokens->GetAt(7).IsValid());	// '1'
	}


	// <Test> #if defined
	{
		const char* code =
			"#define AAA\n"
			"#if defined AAA\n"
			"1\n"
			"#endif\n";
		Preprocess(code);
		ASSERT_EQ(false, m_tokens->GetAt(7).IsValid());	// '1'
	}
	// <Test> #if defined
	{
		const char* code =
			"#if defined AAA\n"
			"1\n"
			"#endif\n";
		Preprocess(code);
		ASSERT_EQ(false, m_tokens->GetAt(7).IsValid());	// '1'
	}
	// <Test> #if defined()
	{
		const char* code =
			"#define AAA\n"
			"#if defined(AAA)\n"
			"1\n"
			"#endif\n";
		Preprocess(code);
		ASSERT_EQ(true, m_tokens->GetAt(13).IsValid());
	}
	// <Test> #if defined()
	{
		const char* code =
			"#if !defined(AAA)\n"
			"1\n"
			"#endif\n";
		Preprocess(code);
		ASSERT_EQ(true, m_tokens->GetAt(9).IsValid());
	}


	// <Test> #if �}�N��
	{
		const char* code =
			"#define AAA 1\n"
			"#if AAA\n"
			"1\n"
			"#endif\n";
		Preprocess(code);
		ASSERT_EQ(true, m_tokens->GetAt(12).IsValid());
	}
	// <Test> #if ����`�}�N���� 0 �ɂȂ�
	{
		const char* code =
			"#if AAA\n"
			"1\n"
			"#endif\n";
		Preprocess(code);
		ASSERT_EQ(false, m_tokens->GetAt(5).IsValid());
	}
	// <Test> #if �}�N�����܂ގ�
	{
		const char* code =
			"#define AAA 1\n"
			"#if AAA-1\n"
			"1\n"
			"#endif\n";
		Preprocess(code);
		ASSERT_EQ(false, m_tokens->GetAt(14).IsValid());
	}
}

//-----------------------------------------------------------------------------
TEST_F(Test_Parser_Preprocessor, Basic_elif)
{
	// <Test> #if ���L���Ȃ� #elif �͏�ɖ���
	{
		const char* code =
			"#if 1\n"
			"1\n"
			"#elif 1\n"
			"2\n"
			"#endif";
		Preprocess(code);
		ASSERT_EQ(false, m_tokens->GetAt(12).IsValid());	// '2'
	}
	// <Test> #if �������Ȃ� #elif �͏������m�F���� (�L��)
	{
		const char* code =
			"#if 0\n"
			"1\n"
			"#elif 1\n"
			"2\n"
			"#endif";
		Preprocess(code);
		ASSERT_EQ(true, m_tokens->GetAt(12).IsValid());	// '2'
	}
	// <Test> #if �������Ȃ� #elif �͏������m�F���� (����)
	{
		const char* code =
			"#if 0\n"
			"1\n"
			"#elif 0\n"
			"2\n"
			"#endif\n";
		Preprocess(code);
		ASSERT_EQ(false, m_tokens->GetAt(12).IsValid());	// '2'
	}

	// <Test> ������ #elif (#if ���L���ƂȂ�p�^�[��)
	{
		const char* code =
			"#if 1\n"
			"1\n"
			"#elif 0\n"
			"2\n"
			"#elif 0\n"
			"3\n"
			"#else\n"
			"4\n"
			"#endif\n";
		Preprocess(code);
		ASSERT_EQ(true, m_tokens->GetAt(5).IsValid());		// '1'
		ASSERT_EQ(false, m_tokens->GetAt(12).IsValid());	// '2'
		ASSERT_EQ(false, m_tokens->GetAt(19).IsValid());	// '3'
		ASSERT_EQ(false, m_tokens->GetAt(24).IsValid());	// '4'
	}

	// <Test> ������ #elif (1�߂� #elif ���L���ƂȂ�p�^�[��)
	{
		const char* code =
			"#if 0\n"
			"1\n"
			"#elif 1\n"
			"2\n"
			"#elif 0\n"
			"3\n"
			"#else\n"
			"4\n"
			"#endif\n";
		Preprocess(code);
		ASSERT_EQ(false, m_tokens->GetAt(5).IsValid());		// '1'
		ASSERT_EQ(true, m_tokens->GetAt(12).IsValid());		// '2'
		ASSERT_EQ(false, m_tokens->GetAt(19).IsValid());	// '3'
		ASSERT_EQ(false, m_tokens->GetAt(24).IsValid());	// '4'
	}

	// <Test> ������ #elif (2�߂� #elif ���L���ƂȂ�p�^�[��)
	{
		const char* code =
			"#if 0\n"
			"1\n"
			"#elif 0\n"
			"2\n"
			"#elif 1\n"
			"3\n"
			"#else\n"
			"4\n"
			"#endif\n";
		Preprocess(code);
		ASSERT_EQ(false, m_tokens->GetAt(5).IsValid());		// '1'
		ASSERT_EQ(false, m_tokens->GetAt(12).IsValid());	// '2'
		ASSERT_EQ(true, m_tokens->GetAt(19).IsValid());		// '3'
		ASSERT_EQ(false, m_tokens->GetAt(24).IsValid());	// '4'
	}

	// <Test> ������ #elif (#else ���L���ƂȂ�p�^�[��)
	{
		const char* code =
			"#if 0\n"
			"1\n"
			"#elif 0\n"
			"2\n"
			"#elif 0\n"
			"3\n"
			"#else\n"
			"4\n"
			"#endif\n";
		Preprocess(code);
		ASSERT_EQ(false, m_tokens->GetAt(5).IsValid());		// '1'
		ASSERT_EQ(false, m_tokens->GetAt(12).IsValid());	// '2'
		ASSERT_EQ(false, m_tokens->GetAt(19).IsValid());	// '3'
		ASSERT_EQ(true, m_tokens->GetAt(24).IsValid());		// '4'
	}
}

//-----------------------------------------------------------------------------
TEST_F(Test_Parser_Preprocessor, Basic_ifdef)
{
	// <Test> �����̈�
	{
		const char* code =
			"#define AAA\n"
			"#ifdef AAAB\n"
			"1\n"
			"#endif\n";
		Preprocess(code);

		// ���l 1 �͖����̈�
		ASSERT_EQ(CommonTokenType::ArithmeticLiteral, m_tokens->GetAt(10).GetCommonType());
		ASSERT_EQ(false, m_tokens->GetAt(10).IsValid());
	}
	// <Test> �L���̈�
	{
		const char* code =
			"#define AAA\n"
			"#ifdef AAA\n"
			"1\n"
			"#endif\n";
		Preprocess(code);

		// ���l 1 �͗L���̈�
		ASSERT_EQ(CommonTokenType::ArithmeticLiteral, m_tokens->GetAt(10).GetCommonType());
		ASSERT_EQ(true, m_tokens->GetAt(10).IsValid());
	}
}

//-----------------------------------------------------------------------------
TEST_F(Test_Parser_Preprocessor, Basic_ifndef)
{
	// <Test> �����̈�
	{
		const char* code =
			"#define AAA\n"
			"#ifndef AAAB\n"
			"1\n"
			"#endif\n";
		Preprocess(code);

		// ���l 1 �͗L���̈�
		ASSERT_EQ(CommonTokenType::ArithmeticLiteral, m_tokens->GetAt(10).GetCommonType());
		ASSERT_EQ(true, m_tokens->GetAt(10).IsValid());
	}
	// <Test> �L���̈�
	{
		const char* code =
			"#define AAA\n"
			"#ifndef AAA\n"
			"1\n"
			"#endif\n";
		Preprocess(code);

		// ���l 1 �͖����̈�
		ASSERT_EQ(CommonTokenType::ArithmeticLiteral, m_tokens->GetAt(10).GetCommonType());
		ASSERT_EQ(false, m_tokens->GetAt(10).IsValid());
	}
}

//-----------------------------------------------------------------------------
TEST_F(Test_Parser_Preprocessor, Basic_empty)
{
	// <Test> ��̃f�B���N�e�B�u (#����)
	{
		const char* code =
			"#\n"	// ������NewLine
			"#";	// ������EOF
		Preprocess(code);
		// ���������̂ŁA�G���[��x�����������OK
	}
}

#define AAAAAA
#undef AAAAAA

#if AAAAAA

printf;

#endif


#define DDD(x, y) x + y
#if DDD(1, -1)
printf;
#endif

#define FFF(x, y) x+y
#define EEE FFF(1, 1)
#undef FFF
#define FFF(x, y) x-y	// �������D��
#if EEE
printf;
#endif

#define HHH 1
#define GGG defined(HHH)
#if GGG
printf;
#endif

//#define CCC 1+1
//#define CCC 1+1 
//#define CCC 1/**/+1
//#define CCC 1 + 1


#if 0
#pm_include
#if 0
#else
#endif
#endif


//-----------------------------------------------------------------------------
TEST_F(Test_Parser_Preprocessor, Basic_else)
{
	// <Test> �L�� �� ����
	{
		const char* code =
			"#define AAA\n"
			"#ifdef AAA\n"
			"1\n"
			"#else\n"
			"2\n"
			"#endif\n";
		Preprocess(code);

		// 1 �͗L���̈�
		ASSERT_EQ(CommonTokenType::ArithmeticLiteral, m_tokens->GetAt(10).GetCommonType());
		ASSERT_EQ(true, m_tokens->GetAt(10).IsValid());
		// 2 �͖����̈�
		ASSERT_EQ(CommonTokenType::ArithmeticLiteral, m_tokens->GetAt(15).GetCommonType());
		ASSERT_EQ(false, m_tokens->GetAt(15).IsValid());
	}
	// <Test> ���� �� �L��
	{
		const char* code =
			"#ifdef AAA\n"
			"1\n"
			"#else\n"
			"2\n"
			"#endif";
		Preprocess(code);

		// 1 �͖����̈�
		ASSERT_EQ(CommonTokenType::ArithmeticLiteral, m_tokens->GetAt(5).GetCommonType());
		ASSERT_EQ(false, m_tokens->GetAt(5).IsValid());
		// 2 �͗L���̈�
		ASSERT_EQ(CommonTokenType::ArithmeticLiteral, m_tokens->GetAt(10).GetCommonType());
		ASSERT_EQ(true, m_tokens->GetAt(10).IsValid());

	}
}

#define AAA 1-

#if AAA 1
#error AA
#endif


//-----------------------------------------------------------------------------
TEST_F(Test_Parser_Preprocessor, Unit_include)
{
	// <Test> include �t�@�C�����̃}�N����`�����o����B
	//{
	//	const char* code =
	//		"#include \"test.h\"\n"
	//		"#ifdef TEST\n"
	//		"1\n"
	//		"#endif";
	//	Preprocess(code);
	//	ASSERT_EQ(true, m_tokens->GetAt(10).IsValid());
	//}
	// <Test> ���� include �t�@�C���������� include ����A���ꂼ���`�}�N�����قȂ�ꍇ
	{
		const char* code =
			"#include \"IncludeTest1.h\"\n"
			"#ifdef CCC\n"
			"1\n"
			"#endif\n"
			"#ifdef BBB\n"
			"1\n"
			"#endif\n"
			"#define AAA\n"
			"#include \"IncludeTest1.h\"\n"
			"#ifdef BBB\n"
			"1\n"
			"#endif";
		Preprocess(code);
		ASSERT_EQ(true, m_tokens->GetAt(10).IsValid());
		ASSERT_EQ(false, m_tokens->GetAt(20).IsValid());
		ASSERT_EQ(true, m_tokens->GetAt(40).IsValid());		// AAA ����`���ꂽ�̂� .h ���� BBB ����`�����
	}
}


//-----------------------------------------------------------------------------
TEST_F(Test_Parser_Preprocessor, Unit_pragma_once)
{
	// <Test> #pragma once
	{
		const char* code =
			"#include \"pragma_once.h\"\n"
			"#ifdef BBB\n"
			"1\n"
			"#endif\n"
			"#define AAA"
			"#include \"pragma_once.h\"\n"
			"#ifdef BBB\n"
			"1\n"
			"#endif";
		Preprocess(code);
		ASSERT_EQ(true, m_tokens->GetAt(10).IsValid());
		// 1 �͖����̈�
	}
}

//-----------------------------------------------------------------------------
TEST_F(Test_Parser_Preprocessor, Illigal)
{
	// �ǂ����בւ��Ă���Ԃ������̂�
#define BBB2	AAA2
#define AAA2	100
	printf("%d\n", BBB2);
#undef AAA2
#define AAA2	200
	printf("%d\n", BBB2);

	// <Illigal> #if �̒萔��������
	{
		const char* code = "#if";
		ASSERT_EQ(false, TryPreprocess(code));
		ASSERT_EQ(DiagnosticsCode::Preprocessor_InvalidConstantExpression, m_diag.GetItems()->GetAt(0).GetCode());
	}
	// <Illigal> #if �̒萔���������ł͂Ȃ�
	{
		const char* code = "#if 1.5 + 2";
		ASSERT_EQ(false, TryPreprocess(code));
		ASSERT_EQ(DiagnosticsCode::Preprocessor_InvalidConstantExpression, m_diag.GetItems()->GetAt(0).GetCode());
	}
	// <Illigal> #if �̂܂��� #else ������
	{
		const char* code = "#else\n";
		ASSERT_EQ(false, TryPreprocess(code));
		ASSERT_EQ(DiagnosticsCode::Preprocessor_UnexpectedElse, m_diag.GetItems()->GetAt(0).GetCode());
	}
	// <Illigal> defined �̌�낪���ʎq�� ( �ȊO
	{
		const char* code = "#if defined + AAA\n";
		ASSERT_EQ(false, TryPreprocess(code));
		ASSERT_EQ(DiagnosticsCode::Preprocessor_ExpectedDefinedId, m_diag.GetItems()->GetAt(0).GetCode());
	}
	// <Illigal> #else ���A���ŗ���
	{
		const char* code =
			"#ifdef AAA\n"
			"#else\n"
			"#else\n";
		ASSERT_EQ(false, TryPreprocess(code));
		ASSERT_EQ(DiagnosticsCode::Preprocessor_UnexpectedElse, m_diag.GetItems()->GetAt(0).GetCode());
	}
	// <Illigal> #if �̑O�� #endif ������
	{
		const char* code =
			"#endif";
		ASSERT_EQ(false, TryPreprocess(code));
		ASSERT_EQ(DiagnosticsCode::Preprocessor_UnexpectedEndif, m_diag.GetItems()->GetAt(0).GetCode());
	}
	// <Illigal> #if �̑O�� #elif ������
	{
		const char* code = "#elif\n";
		ASSERT_EQ(false, TryPreprocess(code));
		ASSERT_EQ(DiagnosticsCode::Preprocessor_UnexpectedElif, m_diag.GetItems()->GetAt(0).GetCode());
	}
	// <Illigal> #if �n�ɑΉ����� #endif ��������Ȃ������B
	{
		const char* code = "#if 1\n";
		ASSERT_EQ(false, TryPreprocess(code));
		ASSERT_EQ(DiagnosticsCode::Preprocessor_NoExistsEndif, m_diag.GetItems()->GetAt(0).GetCode());
	}
}



//-----------------------------------------------------------------------------
TEST_F(Test_Parser_Preprocessor, Example)
{

}
