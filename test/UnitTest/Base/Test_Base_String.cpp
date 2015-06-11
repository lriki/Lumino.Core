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
		EXPECT_EQ(str1, str2.GetCStr());
	}

	{
		//std::string str1(NULL);
		//EXPECT_TRUE(str1.empty());
	}

	{
		StringA strA((char*)NULL);
		ASSERT_TRUE(strA.IsEmpty());
		StringW strW((wchar_t*)NULL);
		ASSERT_TRUE(strW.IsEmpty());
	}

}

//---------------------------------------------------------------------
TEST_F(Test_Base_String, Operators)
{
	std::string strAStd = "a";
	std::wstring strWStd = L"w";
	StringA strASample = "a";
	StringW strWSample = L"w";
	StringA strAEmpty;
	StringW strWEmpty;
	StringA strA;
	StringW strW;


	// <Test> operator= (文字列ポインタ)
	{
		strA = "a";
		strW = L"w";
		ASSERT_STREQ("a", strA);
		ASSERT_STREQ(L"w", strW);
	}
	// <Test> operator= (GenericString)
	{
		strA = strASample;
		strW = strWSample;
		ASSERT_STREQ("a", strA);
		ASSERT_STREQ(L"w", strW);
	}
	// <Test> operator= (std::string)
	{
		strA = strAStd;
		strW = strWStd;
		ASSERT_STREQ("a", strA);
		ASSERT_STREQ(L"w", strW);
	}
	// <Test> operator= (文字列ポインタ・ASCII/Wide逆の型)
	{
		strA = L"w";
		strW = "a";
		ASSERT_STREQ("w", strA);
		ASSERT_STREQ(L"a", strW);
	}
	// <Test> operator= (GenericString・ASCII/Wide逆の型)
	{
		strA = strWSample;
		strW = strASample;
		ASSERT_STREQ("w", strA);
		ASSERT_STREQ(L"a", strW);
	}
	// <Test> operator= (std::string・ASCII/Wide逆の型)
	{
		strA = strWStd;
		strW = strAStd;
		ASSERT_STREQ("w", strA);
		ASSERT_STREQ(L"a", strW);
	}
	// <Test> operator= (NULL)
	{
		strA = ((char*)NULL);
		strW = ((wchar_t*)NULL);
		ASSERT_TRUE(strA.IsEmpty());
		ASSERT_TRUE(strW.IsEmpty());
	}
	// <Test> operator= (自己代入)
	{
		strA = strASample;
		strW = strWSample;
		strA = strA;
		strW = strW;
		ASSERT_STREQ("a", strA);
		ASSERT_STREQ(L"w", strW);
	}

	strA = "a";
	strW = L"w";
	// <Test> operator== (GenericString)
	{
		ASSERT_TRUE(strA == strASample);
		ASSERT_TRUE(strW == strWSample);
	}
	// <Test> operator== (文字列ポインタ)
	{
		ASSERT_TRUE(strA == "a");
		ASSERT_TRUE(strW == "w");
	}
	// <Test> operator== (NULL)
	{
		ASSERT_FALSE(strA == ((char*)NULL));
		ASSERT_FALSE(strW == ((wchar_t*)NULL));
		ASSERT_TRUE(strAEmpty == ((char*)NULL));
		ASSERT_TRUE(strWEmpty == ((wchar_t*)NULL));
	}
	// <Test> operator== (自己比較)
	{
		ASSERT_TRUE(strA == strA);
		ASSERT_TRUE(strW == strW);
	}
	// <Test> operator!= (GenericString)
	{
		ASSERT_FALSE(strA != strASample);
		ASSERT_FALSE(strW != strWSample);
	}
	// <Test> operator!= (文字列ポインタ)
	{
		ASSERT_FALSE(strA != "a");
		ASSERT_FALSE(strW != "w");
	}
	// <Test> operator!= (NULL)
	{
		ASSERT_TRUE(strA != ((char*)NULL));
		ASSERT_TRUE(strW != ((wchar_t*)NULL));
		ASSERT_FALSE(strAEmpty != ((char*)NULL));
		ASSERT_FALSE(strWEmpty != ((wchar_t*)NULL));
	}

	strA = "a";
	strW = L"w";
	// <Test> operator+= (GenericString)
	{
		strA += strASample;
		strW += strWSample;
		ASSERT_STREQ("aa", strA);
		ASSERT_STREQ(L"ww", strW);
	}
	// <Test> operator+= (文字列ポインタ)
	{
		strA += "a";
		strW += L"w";
		ASSERT_STREQ("aaa", strA);
		ASSERT_STREQ(L"www", strW);
	}
	// <Test> operator+= (文字)
	{
		strA += 'a';
		strW += L'w';
		ASSERT_STREQ("aaaa", strA);
		ASSERT_STREQ(L"wwww", strW);
	}
	// <Test> operator+= (NULL)
	{
		strA += ((char*)NULL);
		strW += ((wchar_t*)NULL);
		ASSERT_STREQ("aaaa", strA);
		ASSERT_STREQ(L"wwww", strW);
	}

	strA = "a";
	strW = L"w";
	StringA strA2 = "A";
	StringW strW2 = "W";
	// <Test> operator < (GenericString)
	{
		ASSERT_TRUE(strA2 < strA);
		ASSERT_TRUE(strW2 < strW);
	}
	// <Test> operator < (文字列ポインタ)
	{
		ASSERT_TRUE(strA < "b");
		ASSERT_TRUE(strW < L"x");
	}
	// <Test> operator < (空文字列)
	{
		ASSERT_FALSE(strA < StringA());
		ASSERT_FALSE(strW < StringW());
		ASSERT_FALSE(strA < "");
		ASSERT_FALSE(strW < L"");
	}
	// <Test> operator < (NULL)
	{
		ASSERT_FALSE(strA < ((char*)NULL));
		ASSERT_FALSE(strW < ((wchar_t*)NULL));
	}

	// <Test> operator > (GenericString)
	{
		ASSERT_FALSE(strA2 > strA);
		ASSERT_FALSE(strW2 > strW);
	}
	// <Test> operator > (文字列ポインタ)
	{
		ASSERT_FALSE(strA > "b");
		ASSERT_FALSE(strW > L"x");
	}
	// <Test> operator > (空文字列)
	{
		ASSERT_TRUE(strA > StringA());
		ASSERT_TRUE(strW > StringW());
		ASSERT_TRUE(strA > "");
		ASSERT_TRUE(strW > L"");
	}
	// <Test> operator > (NULL)
	{
		ASSERT_TRUE(strA > ((char*)NULL));
		ASSERT_TRUE(strW > ((wchar_t*)NULL));
	}

	// <Test> operator <= (GenericString)
	{
		ASSERT_TRUE(strA2 <= strA);
		ASSERT_TRUE(strW2 <= strW);
	}
	// <Test> operator <= (文字列ポインタ)
	{
		ASSERT_TRUE(strA <= "b");
		ASSERT_TRUE(strW <= L"x");
	}
	// <Test> operator >= (GenericString)
	{
		ASSERT_FALSE(strA2 >= strA);
		ASSERT_FALSE(strW2 >= strW);
	}
	// <Test> operator >= (文字列ポインタ)
	{
		ASSERT_FALSE(strA >= "b");
		ASSERT_FALSE(strW >= L"x");
	}
}

//---------------------------------------------------------------------
TEST_F(Test_Base_String, XYChar)
{
	String str1("multi");
	ASSERT_STREQ(_T("multi"), str1.GetCStr());

	String str2(L"wide");
	ASSERT_STREQ(_T("wide"), str2.GetCStr());

	str1 = "multi";
	ASSERT_STREQ(_T("multi"), str1.GetCStr());

	str2 = L"wide";
	ASSERT_STREQ(_T("wide"), str2.GetCStr());
}

//---------------------------------------------------------------------
TEST_F(Test_Base_String, AssignCStr)
{
	// char、wchar_t の相互変換
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
		ASSERT_STREQ(L"f", tstr3);
	}

	// 部分変換
	{
		StringA str1;
		str1.AssignCStr("test", 0, 2);
		ASSERT_STREQ("te", str1);

		StringA str2;
		str2.AssignCStr(L"test", 0, 2);
		ASSERT_STREQ("te", str1);

		StringW wstr1;
		wstr1.AssignCStr("test", 0, 2);
		ASSERT_STREQ(L"te", wstr1);

		StringW wstr2;
		wstr2.AssignCStr(L"test", 0, 2);
		ASSERT_STREQ(L"te", wstr1);
	}

	// 同型の NULL ポインタを指定する => 空文字が設定される
	{
		StringA strA;
		strA.AssignCStr((char*)NULL, 1, 1);
		ASSERT_TRUE(strA.IsEmpty());

		StringW strW;
		strW.AssignCStr((wchar_t*)NULL, 1, 1);
		ASSERT_TRUE(strA.IsEmpty());
	}

	//// 例外
	//{
	//	StringA str1;
	//	ASSERT_THROW(
	//		str1.AssignCStr((const char*)NULL, 1, 1),
	//		ArgumentException);

	//	ASSERT_THROW(
	//		str1.AssignCStr((const wchar_t*)NULL, 1, 1),
	//		ArgumentException);
	//}
}

//---------------------------------------------------------------------
TEST_F(Test_Base_String, Format)
{
	// StringA Max 文字数チェック
	{
		char buf1[2048 + 1] = { 0 };
		for (int i = 0; i < 2048; i++) {
			buf1[i] = 'a';
		}

		StringA str1;
		str1.Format("%s", buf1);
		ASSERT_STREQ(str1, buf1);	// 同じ文字ができていればOK
		

		ASSERT_THROW(
			str1.Format("%sb", buf1),	// 1文字多い。失敗する
			ArgumentException);
	}

#ifdef LN_MSVC
	// 可変長実引数への指定は保障外だが、一応動くか見ておく。ちなみに GCC ではコンパイルエラーになる。
	{
		String str1(_T("str1"));
		String str2;
		str2.Format(_T("[%s]"), str1);
		ASSERT_STREQ(_T("[str1]"), str2);
	}
#endif

#if _WIN32	// unix not impl
	// StringW Max 文字数チェック
	{
		wchar_t buf1[2048 + 1] = { 0 };
		for (int i = 0; i < 2048; i++) {
			buf1[i] = L'a';
		}

		StringW str1;
		str1.Format(L"%s", buf1);
		ASSERT_TRUE(str1 == buf1);	// 同じ文字ができていればOK

		ASSERT_THROW(
			str1.Format(L"%sb", buf1),	// 1文字多い。失敗する
			ArgumentException);
	}
#endif
}

//---------------------------------------------------------------------
TEST_F(Test_Base_String, ConvertTo)
{
	{
		StringA str1("test");
		RefPtr<ByteBuffer> buf(str1.ConvertTo(Text::Encoding::GetWideCharEncoding()));
		wchar_t* wstr = (wchar_t*)buf->GetData();
		ASSERT_EQ(sizeof(wchar_t) * 4, buf->GetSize());
		ASSERT_EQ(L't', wstr[0]);
		ASSERT_EQ(L'e', wstr[1]);
		ASSERT_EQ(L's', wstr[2]);
		ASSERT_EQ(L't', wstr[3]);
	}

	{
		StringW str1(L"test");
		RefPtr<ByteBuffer> buf(str1.ConvertTo(Text::Encoding::GetSystemMultiByteEncoding()));
		char* astr = (char*)buf->GetData();
		ASSERT_EQ(4, buf->GetSize());
		ASSERT_EQ('t', astr[0]);
		ASSERT_EQ('e', astr[1]);
		ASSERT_EQ('s', astr[2]);
		ASSERT_EQ('t', astr[3]);
	}
}

//---------------------------------------------------------------------
TEST_F(Test_Base_String, IndexOf)
{
	String str1(_T("abcdef"));
	ASSERT_EQ(1, str1.IndexOf(_T("bcd")));
	ASSERT_EQ(4, str1.IndexOf(_T('e')));
}

//---------------------------------------------------------------------
TEST_F(Test_Base_String, LastIndexOf)
{
	String str = "abcdef";
	ASSERT_EQ(3, str.LastIndexOf(_T("de")));
	ASSERT_EQ(1, str.LastIndexOf(_T("bc"), 2));
	ASSERT_EQ(-1, str.LastIndexOf(_T("cd"), 2));
	ASSERT_EQ(2, str.LastIndexOf(_T("cd"), 4, 3));
	ASSERT_EQ(-1, str.LastIndexOf(_T("bc"), 4, 3));
}

//---------------------------------------------------------------------
TEST_F(Test_Base_String, Trim)
{
	// 前後
	{
		String str1(_T(" abc def "));
		String t = str1.Trim();
		ASSERT_STREQ(_T("abc def"), t);
	}
	// 前だけ
	{
		String str1(_T(" abc def"));
		String t = str1.Trim();
		ASSERT_STREQ(_T("abc def"), t);
	}
	// 後ろだけ
	{
		String str1(_T("abc def "));
		String t = str1.Trim();
		ASSERT_STREQ(_T("abc def"), t);
	}
	// 空文字
	{
		String str1(_T(""));
		String t = str1.Trim();
		ASSERT_STREQ(_T(""), t);
	}
	// 空白だけ
	{
		String str1(_T(" "));
		String t = str1.Trim();
		ASSERT_STREQ(_T(""), t);
	}
	// 空白だけ * 2
	{
		String str1(_T("  "));
		String t = str1.Trim();
		ASSERT_STREQ(_T(""), t);
	}
	// 空白だけ * 3
	{
		String str1(_T("   "));
		String t = str1.Trim();
		ASSERT_STREQ(_T(""), t);
	}
}

//---------------------------------------------------------------------
TEST_F(Test_Base_String, Remove)
{
	String str1(_T("abcdef"));

	String str2 = str1.Remove(_T('c'));
	ASSERT_STREQ(_T("abdef"), str2);

	// 大文字小文字を区別しない
	String str3 = str2.Remove(_T('D'), CaseSensitivity_CaseInsensitive);
	ASSERT_STREQ(_T("abef"), str3);
}

//---------------------------------------------------------------------
TEST_F(Test_Base_String, Replace)
{
	StringA str1("test");
	StringA str2 = str1.Replace("es", "b");
	ASSERT_EQ('t', str2[0]);
	ASSERT_EQ('b', str2[1]);
	ASSERT_EQ('t', str2[2]);
}

//---------------------------------------------------------------------
TEST_F(Test_Base_String, Equals)
{
	// <Issue> 文字列の先頭が同じだけで一致判定にならないこと。
	{
		String str1("abc");
		String str2("abcd");
		ASSERT_FALSE(str1.Equals(str2));
		ASSERT_FALSE(str2.Equals(str1));
	}
}

//---------------------------------------------------------------------
TEST_F(Test_Base_String, Compare)
{
	{
		String str1("abc");
		EXPECT_EQ(0, str1.Compare(_T("abc")));
		EXPECT_LE(0, str1.Compare(_T("ab")));	// v1 < v2
		EXPECT_GT(0, str1.Compare(_T("abd")));	// v1 > v2
	}

	{
		String str1("abc");
		String str2("abcd");
		ASSERT_TRUE(str1 < str2);
	}
}

//---------------------------------------------------------------------
TEST_F(Test_Base_String, Left)
{
	String str1(_T("abcdef"));
	ASSERT_STREQ(_T("ab"), str1.Left(2));
}

//---------------------------------------------------------------------
TEST_F(Test_Base_String, Right)
{
	String str1(_T("abcdef"));
	ASSERT_STREQ(_T("ef"), str1.Right(2));
}

//---------------------------------------------------------------------
TEST_F(Test_Base_String, Mid)
{
	String str1(_T("abcdef"));
	ASSERT_STREQ(_T("cde"), str1.Mid(2, 3));
}

//---------------------------------------------------------------------
TEST_F(Test_Base_String, Split)
{
	// 普通の分割
	{
		String str(_T("a,b,c"));
		ArrayList<String> lines = str.Split(_T(","), StringSplitOptions_None);
		EXPECT_EQ(3, lines.GetCount());
		ASSERT_STREQ(_T("a"), lines[0]);
		ASSERT_STREQ(_T("b"), lines[1]);
		ASSERT_STREQ(_T("c"), lines[2]);
	}

	// 空文字
	{
		String str = _T("");
		ArrayList<String> lines = str.Split(_T(","), StringSplitOptions_None);
		EXPECT_EQ(1, lines.GetCount());
	}

	// 空文字の RemoveEmptyEntries
	{
		String str = _T("");
		ArrayList<String> lines = str.Split(_T(","), StringSplitOptions_RemoveEmptyEntries);
		EXPECT_EQ(0, lines.GetCount());
	}

	// 区切り無し
	{
		String str = _T("abc");
		ArrayList<String> lines = str.Split(_T(","), StringSplitOptions_None);
		EXPECT_EQ(1, lines.GetCount());
	}

	// 区切り無し RemoveEmptyEntries
	{
		String str = _T("abc");
		ArrayList<String> lines = str.Split(_T(","), StringSplitOptions_RemoveEmptyEntries);
		EXPECT_EQ(1, lines.GetCount());
	}

	// トークン無し
	{
		String str = _T(",");
		ArrayList<String> lines = str.Split(_T(","), StringSplitOptions_None);
		EXPECT_EQ(2, lines.GetCount());
	}

	// トークン無し RemoveEmptyEntries
	{
		String str = _T(",");
		ArrayList<String> lines = str.Split(_T(","), StringSplitOptions_RemoveEmptyEntries);
		EXPECT_EQ(0, lines.GetCount());
	}

	// トークン無し 2
	{
		String str = _T(",,");
		ArrayList<String> lines = str.Split(_T(","), StringSplitOptions_None);
		EXPECT_EQ(3, lines.GetCount());
	}

	// トークン無し 2 RemoveEmptyEntries
	{
		String str = _T(",,");
		ArrayList<String> lines = str.Split(_T(","), StringSplitOptions_RemoveEmptyEntries);
		EXPECT_EQ(0, lines.GetCount());
	}

	// 空要素付き
	{
		String str = _T("a,,c");
		ArrayList<String> lines = str.Split(_T(","), StringSplitOptions_None);
		EXPECT_EQ(3, lines.GetCount());
		ASSERT_STREQ(_T("a"), lines[0]);
		ASSERT_STREQ(_T(""), lines[1]);
		ASSERT_STREQ(_T("c"), lines[2]);
	}

	// 空要素付き RemoveEmptyEntries
	{
		String str = _T("a,,c");
		ArrayList<String> lines = str.Split(_T(","), StringSplitOptions_RemoveEmptyEntries);
		EXPECT_EQ(2, lines.GetCount());
		ASSERT_STREQ(_T("a"), lines[0]);
		ASSERT_STREQ(_T("c"), lines[1]);
	}

	// 先頭空要素付き
	{
		String str = _T(",b,c");
		ArrayList<String> lines = str.Split(_T(","), StringSplitOptions_None);
		EXPECT_EQ(3, lines.GetCount());
		ASSERT_STREQ(_T(""), lines[0]);
		ASSERT_STREQ(_T("b"), lines[1]);
		ASSERT_STREQ(_T("c"), lines[2]);
	}

	// 先頭空要素付き RemoveEmptyEntries
	{
		String str = _T(",b,c");
		ArrayList<String> lines = str.Split(_T(","), StringSplitOptions_RemoveEmptyEntries);
		EXPECT_EQ(2, lines.GetCount());
		ASSERT_STREQ(_T("b"), lines[0]);
		ASSERT_STREQ(_T("c"), lines[1]);
	}

	// 終端空要素付き
	{
		String str = _T("a,b,");
		ArrayList<String> lines = str.Split(_T(","), StringSplitOptions_None);
		EXPECT_EQ(3, lines.GetCount());
		ASSERT_STREQ(_T("a"), lines[0]);
		ASSERT_STREQ(_T("b"), lines[1]);
		ASSERT_STREQ(_T(""), lines[2]);
	}

	// 終端空要素付き RemoveEmptyEntries
	{
		String str = _T("a,b,");
		ArrayList<String> lines = str.Split(_T(","), StringSplitOptions_RemoveEmptyEntries);
		EXPECT_EQ(2, lines.GetCount());
		ASSERT_STREQ(_T("a"), lines[0]);
		ASSERT_STREQ(_T("b"), lines[1]);
	}

	// 両端空要素付き
	{
		String str = _T(",b,");
		ArrayList<String> lines = str.Split(_T(","), StringSplitOptions_None);
		EXPECT_EQ(3, lines.GetCount());
		ASSERT_STREQ(_T(""), lines[0]);
		ASSERT_STREQ(_T("b"), lines[1]);
		ASSERT_STREQ(_T(""), lines[2]);
	}

	// 両端空要素付き RemoveEmptyEntries
	{
		String str = _T(",b,");
		ArrayList<String> lines = str.Split(_T(","), StringSplitOptions_RemoveEmptyEntries);
		EXPECT_EQ(1, lines.GetCount());
		ASSERT_STREQ(_T("b"), lines[0]);
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

//---------------------------------------------------------------------
TEST_F(Test_Base_String, ToInt)
{
	// 実行できるか
	ASSERT_EQ(10, String(_T("10")).ToInt8());
	ASSERT_EQ(10, String(_T("10")).ToInt16());
	ASSERT_EQ(10, String(_T("10")).ToInt32());
	ASSERT_EQ(10, String(_T("10")).ToInt64());
	ASSERT_EQ(10, String(_T("10")).ToUInt8());
	ASSERT_EQ(10, String(_T("10")).ToUInt16());
	ASSERT_EQ(10, String(_T("10")).ToUInt32());
	ASSERT_EQ(10, String(_T("10")).ToUInt64());

	// 異常系
	ASSERT_THROW(String(_T("10")).ToInt8(1), ArgumentException);
	ASSERT_THROW(String(_T("-")).ToInt8(), InvalidFormatException);
	ASSERT_THROW(String(_T("qwer")).ToInt8(), InvalidFormatException);
	ASSERT_THROW(String(_T("0xfffffffffffffffff")).ToInt8(), OverflowException);
}

//---------------------------------------------------------------------
TEST_F(Test_Base_String, TryToInt)
{
	// 実行できるか
	{
		int8_t v;
		ASSERT_TRUE(String(_T("10")).TryToInt8(&v));
		ASSERT_EQ(10, v);
	}
	{
		int16_t v;
		ASSERT_TRUE(String(_T("10")).TryToInt16(&v));
		ASSERT_EQ(10, v);
	}
	{
		int32_t v;
		ASSERT_TRUE(String(_T("10")).TryToInt32(&v));
		ASSERT_EQ(10, v);
	}
	{
		int64_t v;
		ASSERT_TRUE(String(_T("10")).TryToInt64(&v));
		ASSERT_EQ(10, v);
	}
	{
		uint8_t v;
		ASSERT_TRUE(String(_T("10")).TryToUInt8(&v));
		ASSERT_EQ(10, v);
	}
	{
		uint16_t v;
		ASSERT_TRUE(String(_T("10")).TryToUInt16(&v));
		ASSERT_EQ(10, v);
	}
	{
		uint32_t v;
		ASSERT_TRUE(String(_T("10")).TryToUInt32(&v));
		ASSERT_EQ(10, v);
	}
	{
		uint64_t v;
		ASSERT_TRUE(String(_T("10")).TryToUInt64(&v));
		ASSERT_EQ(10, v);
	}

	// 異常系
	int8_t v;
	ASSERT_FALSE(String(_T("10")).TryToInt8(&v, 1));
	ASSERT_FALSE(String(_T("-")).TryToInt8(&v));
	ASSERT_FALSE(String(_T("qwer")).TryToInt8(&v));
	ASSERT_FALSE(String(_T("0xfffffffffffffffff")).TryToInt8(&v));
}
