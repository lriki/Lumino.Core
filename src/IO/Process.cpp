#include "../Internal.h"
#include <Lumino/IO/Process.h>

namespace Lumino
{

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
	, m_runningReadThread(false)
#ifdef LN_OS_WIN32
	, m_hInputRead(NULL)
	, m_hInputWrite(NULL)
	, m_hOutputRead(NULL)
	, m_hOutputWrite(NULL)
	, m_hErrorRead(NULL)
	, m_hErrorWrite(NULL)
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
	LN_CHECK_STATE(m_standardOutputReader != NULL);

	// 読み取りスレッドを立てる
	m_readStdOutputThread.Start(LN_CreateDelegate(this, &Process::Thread_ReadStdOutput));
	m_runningReadThread = true;
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void Process::BeginErrorReadLine()
{
	LN_CHECK_STATE(m_standardErrorReader != NULL);

	// 読み取りスレッドを立てる
	m_readStdErrorThread.Start(LN_CreateDelegate(this, &Process::Thread_ReadStdError));
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

} // namespace Lumino
