#include "../Internal.h"
#include <sys/types.h> 
#include <sys/wait.h>
#include <Lumino/Base/ElapsedTimer.h>
#include <Lumino/IO/Process.h>

#if defined(LN_OS_MAC)
#include <crt_externs.h>
#define environ (*_NSGetEnviron())
#else
extern char **environ;
#endif

LN_NAMESPACE_BEGIN

//
//#ifdef _WIN32
//class InternalPipeStream
//	: public Stream
//{
//public:
//	enum Side
//	{
//		ReadSide = 0,
//		WriteSide,
//	};
//
//public:
//	InternalPipeStream(Side side, HANDLE hPipe)
//		: m_side(side)
//		, m_hPipe(hPipe)	// クローズは Process クラスに任せる
//	{}
//
//public:
//	virtual bool CanRead() const { return (m_side == ReadSide); }
//	virtual bool CanWrite() const { return (m_side == WriteSide); }
//	virtual int64_t GetLength() const { LN_THROW(0, InvalidOperationException); return 0; }
//	virtual int64_t GetPosition() const { LN_THROW(0, InvalidOperationException); return 0; }
//	virtual size_t Read(void* buffer, size_t byteCount)
//	{
//		LN_CHECK_STATE(m_side == ReadSide);
//
//		DWORD bytesRead = 0;
//
//		// この行でブロックされる。
//		// ログが溜まったり、子プロセスが終了すると動き出す。
//		if (!::ReadFile(m_hPipe, buffer, (DWORD)byteCount, &bytesRead, NULL) || !bytesRead)
//		{
//			if (::GetLastError() == ERROR_BROKEN_PIPE) {
//				return 0;	// 子プロセスが終了したか、パイプが壊れている。継続不可
//			}
//		}
//		return bytesRead;
//	}
//	virtual void Write(const void* data, size_t byteCount)
//	{
//		LN_CHECK_STATE(m_side == WriteSide);
//
//		DWORD bytesWrite = 0;
//		BOOL bRes = ::WriteFile(m_hPipe, data, (DWORD)byteCount, &bytesWrite, NULL);
//		LN_THROW(bRes != FALSE, Win32Exception, ::GetLastError());
//	}
//	virtual void Seek(int64_t offset, SeekOrigin origin) { LN_THROW(0, InvalidOperationException); }
//	virtual void Flush() {}
//
//private:
//	Side	m_side;
//	HANDLE	m_hPipe;
//};
//#endif


//=============================================================================
// Process
//=============================================================================

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void Process::Start(const PathName& program, const String& args)
{
    pid_t pid = fork();
    if (pid == 0)
	{
		// 子プロセス側はこの if に入る
		StringA utf8Path(program);

        StringA utf8Args(args);
        Array<StringA> argList = utf8Args.Split(" ");
		
		char** argv = new char *[argList.GetCount() + 2];
        argv[0] = ::strdup(utf8Path.c_str());     // 書き込み可能なポインタを渡さなければならないので strdup
		for (int i = 0; i < argList.GetCount(); ++i) {
			argv[i + 1] = ::strdup(argList[i].c_str());
		}
		argv[argList.GetCount() + 1] = NULL;
		
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
	bool exit = false;
	do
	{
		int status;
		pid_t pid = waitpid(m_pid, &status, WNOHANG);	// WNOHANG: 状態変化が起こった子プロセスがない場合にすぐに復帰する。
		LN_THROW(pid >= 0, IOException);
		if (pid == 0) {
			// 状態変化していなかった。実行中。
		}
		else
		{
			if (WIFEXITED(status))
			{
				// 子プロセスが正常終了の場合
				m_exitCode = WEXITSTATUS(status); //子プロセスの終了コード
				exit = true;
			}
			else
			{
				// 子プロセスが異常終了の場合
			}
		}
		
		Threading::Thread::Sleep(1);
		
	} while(timeoutMSec == -1 || timer.GetElapsedTime() < timeoutMSec);
	
	if (exit)
	{
		// いろいろ閉じる
		Dispose();
	}
	else {
		// タイムアウトした
	}
	return true;
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
ProcessStatus Process::GetState()
{
	// ※ http://linuxjm.osdn.jp/html/LDP_man-pages/man2/wait.2.html には wait を実行しないとゾンビ状態になる、とあるが、
	//    mono のソースを見る限りだと waitpid でも大丈夫なようだ。（TODO: 要動作確認）
	//	  wait は終了処理で実行しておく。
	
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
void Process::TryGetExitCode()
{
	//GetState();
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void Process::Dispose()
{
	if (!m_disposed)
	{
		m_disposed = true;
	}
}
//#endif

LN_NAMESPACE_END

