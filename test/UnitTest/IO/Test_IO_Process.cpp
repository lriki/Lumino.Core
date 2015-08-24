#include <TestConfig.h>

class Test_IO_Process : public ::testing::Test
{
protected:
	virtual void SetUp() {}
	virtual void TearDown() {}
};

//-----------------------------------------------------------------------------
TEST_F(Test_IO_Process, Example)
{
	// <Test> リダイレクト無しで起動。
	{
		Process proc;
		proc.Start(_T("LuminoCore_UnitTest"), _T("--proctest1"));
		proc.WaitForExit();
		ASSERT_EQ(5, proc.GetExitCode());
	}

	// <Test> 標準出力をリダイレクトする。
	{
		Process proc;
		proc.SetRedirectStandardOutput(true);
		proc.Start(_T("LuminoCore_UnitTest"), _T("--proctest1"));
		ASSERT_STREQ(_T("stdout"), proc.GetStandardOutput()->ReadToEnd());
	}

	// <Test> 標準入力をリダイレクトする。
	{
		Process proc;
		proc.SetRedirectStandardInput(true);
		proc.Start(_T("LuminoCore_UnitTest"), _T("--proctest2"));
		proc.GetStandardInput()->Write(_T("test\n"));	// 改行が必要
		proc.WaitForExit();
		ASSERT_EQ(4, proc.GetExitCode());				// 文字数が返ってくる
	}

	// <Test> 標準入力と標準出力をリダイレクトする。
	{
		Process proc;
		proc.SetRedirectStandardInput(true);
		proc.SetRedirectStandardOutput(true);
		proc.Start(_T("LuminoCore_UnitTest"), _T("--proctest3"));
		proc.GetStandardInput()->Write(_T("test\n"));	// 改行が必要
		ASSERT_STREQ(_T("[test]"), proc.GetStandardOutput()->ReadToEnd());
	}

	// <Test> プロセスをシンプルに実行する。
	{
		if (Process::Execute(_T("LuminoCore_UnitTest")) != 5) {
			FAIL();
		}
	}

	// <Test> プロセスをシンプルに実行し、標準出力をリダイレクトする。
	{
		String stdOutput;
		if (Process::Execute(_T("LuminoCore_UnitTest"), _T("--proctest1"), &stdOutput) == 5) {
			ASSERT_STREQ(_T("stdout"), stdOutput);
		}
		else {
			FAIL();
		}
	}
}

//-----------------------------------------------------------------------------
TEST_F(Test_IO_Process, Start)
{
	// <Illegal> プログラム名が見つからなかった。
	{
		Process proc;
		ASSERT_THROW(
			proc.Start(_T("XXXX")),
			FileNotFoundException);
	}
}
