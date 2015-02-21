#include <TestConfig.h>

class Test_Base_String : public ::testing::Test
{
protected:
	virtual void SetUp() {}
	virtual void TearDown() {}
};

//---------------------------------------------------------------------
TEST_F(Test_Base_String, Constructor)
{
	{
		std::string str1("test", 0, 4);
		StringA str2("test", 0, 4);
		EXPECT_EQ(str1, str2.c_str());
	}

	
}

//---------------------------------------------------------------------
TEST_F(Test_Base_String, AssignCStr)
{
	// char�Awchar_t �̑��ݕϊ�
	{ 
		StringA str1;
		str1.AssignCStr("test");
		StringA str2;
		str2.AssignCStr(L"test");
		StringW wstr1;
		wstr1.AssignCStr("test");
		StringW wstr2;
		wstr2.AssignCStr(L"test");

		ASSERT_STREQ("test", str1.GetCStr());
		ASSERT_STREQ("test", str2.GetCStr());
		ASSERT_STREQ(L"test", wstr1.GetCStr());
		ASSERT_STREQ(L"test", wstr2.GetCStr());

		StringW tstr3;
		tstr3.AssignCStr("f");
		EXPECT_EQ(L"f", tstr3);
	}

	// �����ϊ�
	{
		StringA str1;
		str1.AssignCStr("test", 0, 2);
		EXPECT_EQ("te", str1);

		StringA str2;
		str2.AssignCStr(L"test", 0, 2);
		EXPECT_EQ("te", str1);

		StringW wstr1;
		wstr1.AssignCStr("test", 0, 2);
		EXPECT_EQ(L"te", wstr1);

		StringW wstr2;
		wstr2.AssignCStr(L"test", 0, 2);
		EXPECT_EQ(L"te", wstr1);
	}

	// ��O
	{
		StringA str1;
		ASSERT_THROW(
			str1.AssignCStr((const char*)NULL, 1, 1),
			ArgumentException);

		ASSERT_THROW(
			str1.AssignCStr((const wchar_t*)NULL, 1, 1),
			ArgumentException);
	}
}

//---------------------------------------------------------------------
TEST_F(Test_Base_String, Format)
{
	// StringA Max �������`�F�b�N
	{
		char buf1[2048 + 1] = { 0 };
		for (int i = 0; i < 2048; i++) {
			buf1[i] = 'a';
		}

		StringA str1;
		str1.Format("%s", buf1);
		EXPECT_EQ(str1, buf1);	// �����������ł��Ă����OK
		

		ASSERT_THROW(
			str1.Format("%sb", buf1),	// 1���������B���s����
			ArgumentException);
	}

#if _WIN32	// unix not impl
	// StringW Max �������`�F�b�N
	{
		wchar_t buf1[2048 + 1] = { 0 };
		for (int i = 0; i < 2048; i++) {
			buf1[i] = L'a';
		}

		StringW str1;
		str1.Format(L"%s", buf1);
		ASSERT_TRUE(str1 == buf1);	// �����������ł��Ă����OK

		ASSERT_THROW(
			str1.Format(L"%sb", buf1),	// 1���������B���s����
			ArgumentException);
	}
#endif
}

//---------------------------------------------------------------------
TEST_F(Test_Base_String, ConvertTo)
{
	{
		StringA str1("test");
		RefPtr<RefBuffer> buf(str1.ConvertTo(Text::Encoding::GetWideCharEncoding()));
		wchar_t* wstr = (wchar_t*)buf->GetPointer();
		ASSERT_EQ(sizeof(wchar_t) * 4, buf->GetSize());
		ASSERT_EQ(L't', wstr[0]);
		ASSERT_EQ(L'e', wstr[1]);
		ASSERT_EQ(L's', wstr[2]);
		ASSERT_EQ(L't', wstr[3]);
	}

	{
		StringW str1(L"test");
		RefPtr<RefBuffer> buf(str1.ConvertTo(Text::Encoding::GetSystemMultiByteEncoding()));
		char* astr = (char*)buf->GetPointer();
		ASSERT_EQ(4, buf->GetSize());
		ASSERT_EQ('t', astr[0]);
		ASSERT_EQ('e', astr[1]);
		ASSERT_EQ('s', astr[2]);
		ASSERT_EQ('t', astr[3]);
	}
}

//---------------------------------------------------------------------
TEST_F(Test_Base_String, Replace)
{
	StringA str1("test");
	StringA str2 = str1.Replace("es", "b");
	ASSERT_EQ('t', str2.at(0));
	ASSERT_EQ('b', str2.at(1));
	ASSERT_EQ('t', str2.at(2));
}

//---------------------------------------------------------------------
TEST_F(Test_Base_String, Trim)
{
	// �O��
	{
		String str1(_T(" abc def "));
		String t = str1.Trim();
		ASSERT_EQ(_T("abc def"), t);
	}
	// �O����
	{
		String str1(_T(" abc def"));
		String t = str1.Trim();
		ASSERT_EQ(_T("abc def"), t);
	}
	// ��낾��
	{
		String str1(_T("abc def "));
		String t = str1.Trim();
		ASSERT_EQ(_T("abc def"), t);
	}
	// �󕶎�
	{
		String str1(_T(""));
		String t = str1.Trim();
		ASSERT_EQ(_T(""), t);
	}
	// �󔒂���
	{
		String str1(_T(" "));
		String t = str1.Trim();
		ASSERT_EQ(_T(""), t);
	}
	// �󔒂��� * 2
	{
		String str1(_T("  "));
		String t = str1.Trim();
		ASSERT_EQ(_T(""), t);
	}
	// �󔒂��� * 3
	{
		String str1(_T("   "));
		String t = str1.Trim();
		ASSERT_EQ(_T(""), t);
	}
}


//---------------------------------------------------------------------
TEST_F(Test_Base_String, Split)
{
	// ���ʂ̕���
	{
		String str(_T("a,b,c"));
		Array<String> lines = str.Split(_T(","), StringSplitOptions_None);
		EXPECT_EQ(3, lines.size());
		EXPECT_EQ(_T("a"), lines[0]);
		EXPECT_EQ(_T("b"), lines[1]);
		EXPECT_EQ(_T("c"), lines[2]);
	}

	// �󕶎�
	{
		String str = _T("");
		Array<String> lines = str.Split(_T(","), StringSplitOptions_None);
		EXPECT_EQ(1, lines.size());
	}

	// �󕶎��� RemoveEmptyEntries
	{
		String str = _T("");
		Array<String> lines = str.Split(_T(","), StringSplitOptions_RemoveEmptyEntries);
		EXPECT_EQ(0, lines.size());
	}

	// ��؂薳��
	{
		String str = _T("abc");
		Array<String> lines = str.Split(_T(","), StringSplitOptions_None);
		EXPECT_EQ(1, lines.size());
	}

	// ��؂薳�� RemoveEmptyEntries
	{
		String str = _T("abc");
		Array<String> lines = str.Split(_T(","), StringSplitOptions_RemoveEmptyEntries);
		EXPECT_EQ(1, lines.size());
	}

	// �g�[�N������
	{
		String str = _T(",");
		Array<String> lines = str.Split(_T(","), StringSplitOptions_None);
		EXPECT_EQ(2, lines.size());
	}

	// �g�[�N������ RemoveEmptyEntries
	{
		String str = _T(",");
		Array<String> lines = str.Split(_T(","), StringSplitOptions_RemoveEmptyEntries);
		EXPECT_EQ(0, lines.size());
	}

	// �g�[�N������ 2
	{
		String str = _T(",,");
		Array<String> lines = str.Split(_T(","), StringSplitOptions_None);
		EXPECT_EQ(3, lines.size());
	}

	// �g�[�N������ 2 RemoveEmptyEntries
	{
		String str = _T(",,");
		Array<String> lines = str.Split(_T(","), StringSplitOptions_RemoveEmptyEntries);
		EXPECT_EQ(0, lines.size());
	}

	// ��v�f�t��
	{
		String str = _T("a,,c");
		Array<String> lines = str.Split(_T(","), StringSplitOptions_None);
		EXPECT_EQ(3, lines.size());
		EXPECT_EQ(_T("a"), lines[0]);
		EXPECT_EQ(_T(""), lines[1]);
		EXPECT_EQ(_T("c"), lines[2]);
	}

	// ��v�f�t�� RemoveEmptyEntries
	{
		String str = _T("a,,c");
		Array<String> lines = str.Split(_T(","), StringSplitOptions_RemoveEmptyEntries);
		EXPECT_EQ(2, lines.size());
		EXPECT_EQ(_T("a"), lines[0]);
		EXPECT_EQ(_T("c"), lines[1]);
	}

	// �擪��v�f�t��
	{
		String str = _T(",b,c");
		Array<String> lines = str.Split(_T(","), StringSplitOptions_None);
		EXPECT_EQ(3, lines.size());
		EXPECT_EQ(_T(""), lines[0]);
		EXPECT_EQ(_T("b"), lines[1]);
		EXPECT_EQ(_T("c"), lines[2]);
	}

	// �擪��v�f�t�� RemoveEmptyEntries
	{
		String str = _T(",b,c");
		Array<String> lines = str.Split(_T(","), StringSplitOptions_RemoveEmptyEntries);
		EXPECT_EQ(2, lines.size());
		EXPECT_EQ(_T("b"), lines[0]);
		EXPECT_EQ(_T("c"), lines[1]);
	}

	// �I�[��v�f�t��
	{
		String str = _T("a,b,");
		Array<String> lines = str.Split(_T(","), StringSplitOptions_None);
		EXPECT_EQ(3, lines.size());
		EXPECT_EQ(_T("a"), lines[0]);
		EXPECT_EQ(_T("b"), lines[1]);
		EXPECT_EQ(_T(""), lines[2]);
	}

	// �I�[��v�f�t�� RemoveEmptyEntries
	{
		String str = _T("a,b,");
		Array<String> lines = str.Split(_T(","), StringSplitOptions_RemoveEmptyEntries);
		EXPECT_EQ(2, lines.size());
		EXPECT_EQ(_T("a"), lines[0]);
		EXPECT_EQ(_T("b"), lines[1]);
	}

	// ���[��v�f�t��
	{
		String str = _T(",b,");
		Array<String> lines = str.Split(_T(","), StringSplitOptions_None);
		EXPECT_EQ(3, lines.size());
		EXPECT_EQ(_T(""), lines[0]);
		EXPECT_EQ(_T("b"), lines[1]);
		EXPECT_EQ(_T(""), lines[2]);
	}

	// ���[��v�f�t�� RemoveEmptyEntries
	{
		String str = _T(",b,");
		Array<String> lines = str.Split(_T(","), StringSplitOptions_RemoveEmptyEntries);
		EXPECT_EQ(1, lines.size());
		EXPECT_EQ(_T("b"), lines[0]);
	}
}

//---------------------------------------------------------------------
TEST_F(Test_Base_String, NewLine)
{
#ifdef _WIN32
	const char* nl = StringA::GetNewLine().GetCStr();
	ASSERT_EQ('\r', nl[0]);
	ASSERT_EQ('\n', nl[1]);
	ASSERT_EQ('\0', nl[2]);

	const wchar_t* wnl = StringW::GetNewLine().GetCStr();
	ASSERT_EQ(L'\r', wnl[0]);
	ASSERT_EQ(L'\n', wnl[1]);
	ASSERT_EQ(L'\0', wnl[2]);

	const TCHAR* tnl = String::GetNewLine().GetCStr();
	ASSERT_EQ(_T('\r'), tnl[0]);
	ASSERT_EQ(_T('\n'), tnl[1]);
	ASSERT_EQ(_T('\0'), tnl[2]);
#else
	const char* nl = StringA::GetNewLine().GetCStr();
	ASSERT_EQ('\n', nl[0]);
	ASSERT_EQ('\0', nl[1]);

	const wchar_t* wnl = StringW::GetNewLine().GetCStr();
	ASSERT_EQ(L'\n', wnl[0]);
	ASSERT_EQ(L'\0', wnl[1]);

	const TCHAR* tnl = String::GetNewLine().GetCStr();
	ASSERT_EQ(_T('\n'), tnl[0]);
	ASSERT_EQ(_T('\0'), tnl[1]);
#endif
}
