#include "../Internal.h"
#include <Lumino/IO/Process.h>

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
{
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
Process::~Process()
{
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


#ifdef _WIN32
//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
//void Process::Start(const PathName& filePath, const String& args)
//{
//	enum { R = 0, W = 1 };
//	BOOL bResult;
//	HANDLE hProcess = ::GetCurrentProcess();
//
//	SECURITY_ATTRIBUTES sa;
//	sa.nLength = sizeof(SECURITY_ATTRIBUTES);
//	sa.lpSecurityDescriptor = NULL;
//	sa.bInheritHandle = TRUE;
//
//	// 標準出力のパイプを作る
//	{
//		HANDLE hPipe[2] = { 0, 0 };
//		bResult = ::CreatePipe(&hPipe[R], &hPipe[W], &sa, 0);
//		LN_THROW(bResult != FALSE, Win32Exception, ::GetLastError());
//
//		// パイプのこのプロセス側を非継承で複製する
//		if (!::DuplicateHandle(hProcess, hPipe[R], hProcess, &m_hOutputRead, 0, FALSE, DUPLICATE_SAME_ACCESS))
//		{
//			DWORD dwErr = ::GetLastError();
//			::CloseHandle(hPipe[R]);
//			::CloseHandle(hPipe[W]);
//			LN_THROW(0, Win32Exception, dwErr);
//		}
//		::CloseHandle(hPipe[R]);
//		m_hOutputWrite = hPipe[W];
//
//		// 標準出力の Reader を作る
//		RefPtr<InternalPipeStream> stream(LN_NEW InternalPipeStream(InternalPipeStream::ReadSide, m_hOutputRead));
//		m_standardOutputReader.Attach(LN_NEW StreamReader(stream, WindowsCodePageEncoding::GetSystemCodePageEncoding()));
//	}
//}

#endif

} // namespace Lumino
