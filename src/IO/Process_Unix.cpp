#include "../Internal.h"
#include <Lumino/IO/Process.h>

extern char **environ;

namespace Lumino
{

#ifdef _WIN32
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
#endif


//=============================================================================
// Process
//=============================================================================
//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
Process::Process()
	: m_workingDirectory()
	, m_redirectStandardInput(false)
	, m_redirectStandardOutput(false)
	, m_redirectStandardError(false)
	, m_standardInputWriter()
	, m_standardOutputReader()
	, m_standardErrorReader()
	, m_exitCode(0)
	, m_crashed(false)
	, m_disposed(false)
	, m_hInputRead(NULL)
	, m_hInputWrite(NULL)
	, m_hOutputRead(NULL)
	, m_hOutputWrite(NULL)
	, m_hErrorRead(NULL)
	, m_hErrorWrite(NULL)
{
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
Process::~Process()
{
	Dispose();
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void Process::SetWorkingDirectory(const PathName& directoryPath)
{
	m_workingDirectory = directoryPath;
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void Process::SetRedirectStandardInput(bool enabled)
{
	m_redirectStandardInput = enabled;
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void Process::SetRedirectStandardOutput(bool enabled)
{
	m_redirectStandardOutput = enabled;
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void Process::SetRedirectStandardError(bool enabled)
{
	m_redirectStandardError = enabled;
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
StreamWriter* Process::GetStandardInput() const
{
	LN_CHECK_STATE(m_standardInputWriter != NULL, "Not redirected.");
	return m_standardInputWriter;
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
StreamReader* Process::GetStandardOutput() const
{
	LN_CHECK_STATE(m_standardOutputReader != NULL, "Not redirected.");
	return m_standardOutputReader;
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
StreamReader* Process::GetStandardError() const
{
	LN_CHECK_STATE(m_standardErrorReader != NULL, "Not redirected.");
	return m_standardErrorReader;
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
int Process::Execute(const PathName& program, const String& args, String* stdOutput)
{
	Process proc;
	proc.SetRedirectStandardOutput(stdOutput != NULL);
	proc.Start(program, args);
	if (stdOutput != NULL) {
		*stdOutput = proc.GetStandardOutput()->ReadToEnd();
	}
	proc.WaitForExit();
	return proc.GetExitCode();
}

//#ifdef _WIN32
//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void Process::Start(const PathName& program, const String& args)
{
    pid_t pid = fork();
    if (pid == 0)
	{
		Array<String> argList = args.Split(" ");
		
		// 子プロセス側
		char** argv = new char *[args.GetSize() + 2];
		for (int i = 0; i < arguments.size(); ++i)
		{
			argv[i + 1] = ::strdup(QFile::encodeName(arguments.at(i)).constData());
		}
		argv[arguments.size() + 1] = 0;
		
		execve(argv[0], argv, environ);
		
		// ここには execve が失敗したときだけ来る。
    }
	else
	{
		// 親プロセス側
	}
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
bool Process::WaitForExit(int timeoutMSec)
{
	ElapsedTimer timer;
	
	do
	{
		int status;
		pid_t pid = waitpid(m_pid, &status, WNOHANG);	// WNOHANG: 状態変化が起こった子プロセスがない場合にすぐに復帰する。
		if (pid == 0) {
			// 状態変化していなかった。実行中。
		}
		
		Threading::Thread::Sleep(1);
		
	} while(timeoutMSec == -1 || timer.GetElapsedTime() < timeoutMSec);
	
	
	pid_t r = waitpid(pid, &status, 0); //子プロセスのプロセスIDを指定して、終了を待つ
	if (r < 0) {
		perror("waitpid");
		exit(-1);
	}
	if (WIFEXITED(status)) {
		// 子プロセスが正常終了の場合
		int exit_code = WEXITSTATUS(status); //子プロセスの終了コード
		printf("child exit-code=%d\n", exit_code);
	} else {
		printf("child status=%04x\n", status);
	}

	
	waitpid
	
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
	// ※ http://linuxjm.osdn.jp/html/LDP_man-pages/man2/wait.2.html には wait を実行しないとゾンビ状態になる、とあるが、
	//    mono のソースを見る限りだと waitpid でも大丈夫なようだ。（TODO: 要動作確認）
	
	int status;
	pid_t pid = waitpid(m_pid, &status, WNOHANG);	// WNOHANG: 状態変化が起こった子プロセスがない場合にすぐに復帰する。
	if (pid == 0) {
		// 状態変化していなかった。実行中。
		return ProcessStatus::Running;
	}
	
	// 正常終了
	if (WIFEXITED(status)) {
		// 終了コード
		m_exitCode = WEXITSTATUS(status);
		return ProcessStatus::Finished;
	}
	// 異常終了
	else {
		return ProcessStatus::Crashed;
	}
	//waitid
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
int Process::GetExitCode()
{
	TryGetExitCode();
	return m_exitCode;
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void Process::TryGetExitCode()
{
	GetState();
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

		// 子プロセス強制停止
		if (m_processInfo.hProcess != NULL)
		{
			::TerminateProcess(m_processInfo.hProcess, 0);
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
		//if (m_bRunReadThread)
		//{
		//	m_bRunReadThread = false;
		//	m_ReadStdOutputThread.Wait();
		//}

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
//#endif

} // namespace Lumino
