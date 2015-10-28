#include "../Internal.h"
#include <Lumino/IO/Process.h>

LN_NAMESPACE_BEGIN

class InternalPipeStream
	: public Stream
{
public:
	enum Side
	{
		ReadSide = 0,
		WriteSide,
	};

public:
	InternalPipeStream(Side side, HANDLE hPipe)
		: m_side(side)
		, m_hPipe(hPipe)	// クローズは Process クラスに任せる
	{}

public:
	virtual bool CanRead() const { return (m_side == ReadSide); }
	virtual bool CanWrite() const { return (m_side == WriteSide); }
	virtual int64_t GetLength() const { LN_THROW(0, InvalidOperationException); return 0; }
	virtual int64_t GetPosition() const { LN_THROW(0, InvalidOperationException); return 0; }
	virtual size_t Read(void* buffer, size_t byteCount)
	{
		LN_CHECK_STATE(m_side == ReadSide);

		DWORD bytesRead = 0;

		// この行でブロックされる。
		// ログが溜まったり、子プロセスが終了すると動き出す。
		if (!::ReadFile(m_hPipe, buffer, (DWORD)byteCount, &bytesRead, NULL) || !bytesRead)
		{
			if (::GetLastError() == ERROR_BROKEN_PIPE) {
				return 0;	// 子プロセスが終了したか、パイプが壊れている。継続不可
			}
		}
		return bytesRead;
	}
	virtual void Write(const void* data, size_t byteCount)
	{
		LN_CHECK_STATE(m_side == WriteSide);

		DWORD bytesWrite = 0;
		BOOL bRes = ::WriteFile(m_hPipe, data, (DWORD)byteCount, &bytesWrite, NULL);
		LN_THROW(bRes != FALSE, Win32Exception, ::GetLastError());
	}
	virtual void Seek(int64_t offset, SeekOrigin origin) { LN_THROW(0, InvalidOperationException); }
	virtual void Flush() {}

private:
	Side	m_side;
	HANDLE	m_hPipe;
};

//=============================================================================
// Process
//=============================================================================

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void Process::Start(const PathName& program, const String& args)
{
	enum { R = 0, W = 1 };
	BOOL bResult;
	HANDLE hProcess = ::GetCurrentProcess();

	SECURITY_ATTRIBUTES sa;
	sa.nLength = sizeof(SECURITY_ATTRIBUTES);
	sa.lpSecurityDescriptor = NULL;
	sa.bInheritHandle = TRUE;

	// 標準入力のパイプを作る
	if (m_redirectStandardInput)
	{
		HANDLE hPipe[2] = { 0, 0 };
		bResult = ::CreatePipe(&hPipe[R], &hPipe[W], &sa, 0);
		LN_THROW(bResult != FALSE, Win32Exception, ::GetLastError());

		// パイプのこのプロセス側を非継承で複製する
		if (!::DuplicateHandle(hProcess, hPipe[W], hProcess, &m_hInputWrite, 0, FALSE, DUPLICATE_SAME_ACCESS))
		{
			DWORD dwErr = ::GetLastError();
			::CloseHandle(hPipe[R]);
			::CloseHandle(hPipe[W]);
			LN_THROW(0, Win32Exception, dwErr);
		}
		::CloseHandle(hPipe[W]);
		m_hInputRead = hPipe[R];

		// 標準出力の Writer を作る
		RefPtr<InternalPipeStream> stream(LN_NEW InternalPipeStream(InternalPipeStream::WriteSide, m_hInputWrite), false);
		m_standardInputWriter.Attach(LN_NEW StreamWriter(stream, Encoding::GetWin32DefaultCodePageEncoding()));
	}

	// 標準出力のパイプを作る
	if (m_redirectStandardOutput)
	{
		HANDLE hPipe[2] = { 0, 0 };
		bResult = ::CreatePipe(&hPipe[R], &hPipe[W], &sa, 0);
		LN_THROW(bResult != FALSE, Win32Exception, ::GetLastError());

		// パイプのこのプロセス側を非継承で複製する
		if (!::DuplicateHandle(hProcess, hPipe[R], hProcess, &m_hOutputRead, 0, FALSE, DUPLICATE_SAME_ACCESS))
		{
			DWORD dwErr = ::GetLastError();
			::CloseHandle(hPipe[R]);
			::CloseHandle(hPipe[W]);
			LN_THROW(0, Win32Exception, dwErr);
		}
		::CloseHandle(hPipe[R]);
		m_hOutputWrite = hPipe[W];

		// 標準出力の Reader を作る
		RefPtr<InternalPipeStream> stream(LN_NEW InternalPipeStream(InternalPipeStream::ReadSide, m_hOutputRead), false);
		m_standardOutputReader.Attach(LN_NEW StreamReader(stream, Encoding::GetWin32DefaultCodePageEncoding()));
	}

	// 標準エラー出力のパイプを作る
	if (m_redirectStandardError)
	{
		HANDLE hPipe[2] = { 0, 0 };
		bResult = ::CreatePipe(&hPipe[R], &hPipe[W], &sa, 0);
		LN_THROW(bResult != FALSE, Win32Exception, ::GetLastError());

		// パイプのこのプロセス側を非継承で複製する
		if (!::DuplicateHandle(hProcess, hPipe[R], hProcess, &m_hErrorRead, 0, FALSE, DUPLICATE_SAME_ACCESS))
		{
			DWORD dwErr = ::GetLastError();
			::CloseHandle(hPipe[R]);
			::CloseHandle(hPipe[W]);
			LN_THROW(0, Win32Exception, dwErr);
		}
		::CloseHandle(hPipe[R]);
		m_hErrorWrite = hPipe[W];

		// 標準出力の Reader を作る
		RefPtr<InternalPipeStream> stream(LN_NEW InternalPipeStream(InternalPipeStream::ReadSide, m_hErrorRead), false);
		m_standardErrorReader.Attach(LN_NEW StreamReader(stream, Encoding::GetWin32DefaultCodePageEncoding()));
	}

	// 子プロセスの標準出力の出力先を↑で作ったパイプにする
	STARTUPINFO si;
	ZeroMemory(&si, sizeof(si));
	si.cb = sizeof(si);
	si.dwFlags = STARTF_USESTDHANDLES;
	si.hStdInput = m_hInputRead;			// 子プロセスの標準入力はここから読み取る
	si.hStdOutput = m_hOutputWrite;		// 子プロセスの標準出力はここへ
	si.hStdError = m_hErrorWrite;		// 子プロセスの標準エラーはここへ
	si.wShowWindow = SW_HIDE;

	// exe 名と引数を連結してコマンドライン文字列を作る
	String cmdArgs = program;
	if (!args.IsEmpty()) {
		cmdArgs += _T(" ");
		cmdArgs += args;
	}

	// カレントディレクトリ
	LPCTSTR pCurrentDirectory = NULL;
	if (!m_workingDirectory.IsEmpty()) {
		pCurrentDirectory = m_workingDirectory.c_str();
	}

	// 子プロセス開始
	memset(&m_processInfo, 0, sizeof(m_processInfo));
	bResult = ::CreateProcess(
		NULL, (LPTSTR)(LPCTSTR)cmdArgs.c_str(), NULL, NULL, TRUE,
		CREATE_NO_WINDOW, NULL, pCurrentDirectory, &si, &m_processInfo);
	if (bResult == FALSE)
	{
		DWORD dwErr = ::GetLastError();
		if (dwErr == ERROR_FILE_NOT_FOUND) {
			LN_THROW(0, FileNotFoundException, program);
		}
		LN_THROW(0, Win32Exception, dwErr);
	}

	// 子プロセスのスレッドハンドルは不必要なのでクローズしてしまう
	::CloseHandle(m_processInfo.hThread);

	// 子プロセス側は全てクローズしてしまう。
	// ※実際に行われるのは参照カウントのデクリメント。
	//   子側は終了したときに自動的に参照が外れるが、
	//   親側は自分で Close しておかないとリソースリークになる。
	//   また、例えば stdout の親側は ReadFile() で永遠にブロックしてしまう。
	if (m_hInputRead != NULL) {
		::CloseHandle(m_hInputRead);
		m_hInputRead = NULL;
	}
	if (m_hOutputWrite != NULL) {
		::CloseHandle(m_hOutputWrite);
		m_hOutputWrite = NULL;
	}
	if (m_hErrorWrite != NULL) {
		::CloseHandle(m_hErrorWrite);
		m_hErrorWrite = NULL;
	}
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
bool Process::WaitForExit(int timeoutMSec)
{
	if (m_processInfo.hProcess != NULL)
	{
		// 終了した場合は制御を返し、WAIT_OBJECT_0 が返ってくる
		DWORD r = ::WaitForSingleObject(m_processInfo.hProcess, (timeoutMSec < 0) ? INFINITE : timeoutMSec);
		if (r == WAIT_TIMEOUT) {
			return false;	// タイムアウト
		}
	}

	// いろいろ閉じる
	Dispose();
	return true;
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
ProcessStatus Process::GetState()
{
	if (::WaitForSingleObject(m_processInfo.hProcess, 0) == WAIT_OBJECT_0) {
		return ProcessStatus::Running;
	}
	TryGetExitCode();
	return (m_crashed) ? ProcessStatus::Crashed : ProcessStatus::Finished;
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void Process::TryGetExitCode()
{
	if (m_processInfo.hProcess != NULL)
	{
		DWORD exitCode;
		if (!::GetExitCodeProcess(m_processInfo.hProcess, &exitCode)) {
			return;
		}
		m_exitCode = (int)exitCode;

		// クラッシュを確実に検出するのは難しい。
		// 現実的な方法としては、GetExitCodeProcess() は未処理例外の例外コードを返すのでそれをチェックすること。
		// https://social.msdn.microsoft.com/Forums/en-US/7e0746ab-d285-4061-9032-81400875243a/detecting-if-a-child-process-crashed
		m_crashed = (m_exitCode >= 0x80000000 && m_exitCode < 0xD0000000);
	}
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void Process::Dispose()
{
	if (!m_disposed)
	{
		// 終了コードを覚えておく (閉じた後は取得できない)
		TryGetExitCode();

		// 子プロセスのハンドルを閉じる (子プロセスが終了するわけではないので注意)
		if (m_processInfo.hProcess != NULL)
		{
			::CloseHandle(m_processInfo.hProcess);
			m_processInfo.hProcess = NULL;
		}

		// 書き込み側ハンドルは、WaitForSingleObject() の前でクローズしておく。
		// こうしておかないと、子プロセスの ReadFile() がブロックし続けてしまい、
		// スレッドが終了できなくなる。
		//if (m_hInputWrite != NULL) {
		//	::CloseHandle(m_hInputWrite);
		//	m_hInputWrite = NULL;
		//}

		// 読み取りスレッドの終了を待つ
		if (m_runningReadThread)
		{
			m_runningReadThread = false;
			m_readStdOutputThread.Wait();
		}
		if (m_runningErrorReadThread)
		{
			m_runningErrorReadThread = false;
			m_readStdErrorThread.Wait();
		}

		// パイプを閉じる
		if (m_hInputWrite != NULL) {
			::CloseHandle(m_hInputWrite);
			m_hInputWrite = NULL;
		}
		if (m_hOutputRead != NULL) {
			::CloseHandle(m_hOutputRead);
			m_hOutputRead = NULL;
		}
		if (m_hErrorRead != NULL) {
			::CloseHandle(m_hErrorRead);
			m_hErrorRead = NULL;
		}

		if (m_hInputRead != NULL) {
			::CloseHandle(m_hInputRead);
			m_hInputRead = NULL;
		}
		if (m_hOutputWrite != NULL) {
			::CloseHandle(m_hOutputWrite);
			m_hOutputWrite = NULL;
		}
		if (m_hErrorWrite != NULL) {
			::CloseHandle(m_hErrorWrite);
			m_hErrorWrite = NULL;
		}

		m_disposed = true;
	}
}

LN_NAMESPACE_END
