#include "../Internal.h"
#include <Lumino/IO/Process.h>

LN_NAMESPACE_BEGIN

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
	, m_standardInputEncoding(Encoding::GetSystemMultiByteEncoding())
	, m_standardOutputEncoding(Encoding::GetSystemMultiByteEncoding())
	, m_standardErrorEncoding(Encoding::GetSystemMultiByteEncoding())
	, m_standardOutputExternalStream(nullptr)
	, m_standardInputWriter()
	, m_standardOutputReader()
	, m_standardErrorReader()
	, m_stdinPipeStream(nullptr)
	, m_stdoutPipeStream(nullptr)
	, m_stderrPipeStream(nullptr)
	, m_exitCode(0)
	, m_crashed(false)
	, m_disposed(false)
	, m_runningReadThread(false)
#ifdef LN_OS_WIN32
	, m_hInputRead(nullptr)
	, m_hInputWrite(nullptr)
	, m_hOutputRead(nullptr)
	, m_hOutputWrite(nullptr)
	, m_hErrorRead(nullptr)
	, m_hErrorWrite(nullptr)
#else
    , m_pid(NULL)
#endif
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

#ifdef LN_CPP11
//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void Process::SetOutputDataReceivedCallback(const Delegate<void(String)>& callback)
{
	m_outputDataReceivedCallback = callback;
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void Process::SetErrorDataReceivedCallback(const Delegate<void(String)>& callback)
{
	m_errorDataReceivedCallback = callback;
}
#else
//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void Process::SetOutputDataReceivedCallback(const Delegate01<String>& callback)
{
	m_outputDataReceivedCallback = callback;
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void Process::SetErrorDataReceivedCallback(const Delegate01<String>& callback)
{
	m_errorDataReceivedCallback = callback;
}
#endif

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void Process::Start(const PathName& program, const StringArray& argsList)
{
	String args;
	for (int i = 0; i < argsList.GetCount(); ++i)
	{
		if (!args.IsEmpty()) {
			args += _T(' ');
		}

		String tmp = argsList[i];
		if (tmp.Contains(_T(' ')) || tmp.Contains(_T('\t')))
		{
			if (!tmp.StartsWith(_T('\"')) && !tmp.EndsWith(_T('\"')))
			{
				tmp = _T('\"') + tmp + _T('\"');
			}
		}

		args += tmp;
	}

	Start(program, args);
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
StreamWriter* Process::GetStandardInput() const
{
	LN_CHECK_STATE(m_standardInputWriter != nullptr, "Not redirected.");
	return m_standardInputWriter;
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
StreamReader* Process::GetStandardOutput() const
{
	LN_CHECK_STATE(m_standardOutputReader != nullptr, "Not redirected.");
	return m_standardOutputReader;
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
StreamReader* Process::GetStandardError() const
{
	LN_CHECK_STATE(m_standardErrorReader != nullptr, "Not redirected.");
	return m_standardErrorReader;
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
int Process::Execute(const PathName& program, const String& args, String* outStdOutput, String* outStdError)
{
	Process proc;
	proc.SetRedirectStandardOutput(outStdOutput != nullptr);
	proc.SetRedirectStandardError(outStdError != nullptr);
	proc.Start(program, args);
	if (outStdOutput != nullptr) {
		*outStdOutput = proc.GetStandardOutput()->ReadToEnd();
	}
	if (outStdError != nullptr) {
		*outStdError = proc.GetStandardError()->ReadToEnd();
	}
	proc.WaitForExit();
	return proc.GetExitCode();
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
void Process::BeginOutputReadLine()
{
	LN_CHECK_STATE(m_standardOutputReader != nullptr);

	// 読み取りスレッドを立てる
#ifdef LN_CPP11
	m_readStdOutputThread.Start(Delegate<void()>(this, &Process::Thread_ReadStdOutput));
#else
	m_readStdOutputThread.Start(LN_CreateDelegate(this, &Process::Thread_ReadStdOutput));
#endif
	m_runningReadThread = true;
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void Process::BeginErrorReadLine()
{
	LN_CHECK_STATE(m_standardErrorReader != nullptr);

	// 読み取りスレッドを立てる
#ifdef LN_CPP11
	m_readStdErrorThread.Start(Delegate<void()>(this, &Process::Thread_ReadStdError));
#else
	m_readStdErrorThread.Start(LN_CreateDelegate(this, &Process::Thread_ReadStdError));
#endif
	m_runningErrorReadThread = true;
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void Process::Thread_ReadStdOutput()
{
	String strLine;
	while (m_standardOutputReader->ReadLine(&strLine))
	{
		if (!m_outputDataReceivedCallback.IsEmpty()) {
			m_outputDataReceivedCallback.Call(strLine);
		}
	}
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void Process::Thread_ReadStdError()
{
	String strLine;
	while (m_standardErrorReader->ReadLine(&strLine))
	{
		if (!m_errorDataReceivedCallback.IsEmpty()) {
			m_errorDataReceivedCallback.Call(strLine);
		}
	}
}

LN_NAMESPACE_END
