#include "../Internal.h"
#include <Lumino/Base/Environment.h>
#include <Lumino/IO/Console.h>

LN_NAMESPACE_BEGIN

//==============================================================================
// Console
//==============================================================================

#ifdef LN_OS_WIN32

class ConsoleInternal
{
public:
	ConsoleInternal()
		: m_in(NULL)
		, m_out(NULL)
		, m_err(NULL)
		, m_consoleAlloced(false)
	{}

	~ConsoleInternal()
	{
		Free();
	}

	void Alloc()
	{
		// コンソールアプリとして実行している場合はこの if に入ることはない
		if (!m_consoleAlloced && ::AllocConsole())
		{
			freopen_s(&m_in, "CON", "r", stdin);
			freopen_s(&m_out, "CON", "w", stdout);
			freopen_s(&m_err, "CON", "w", stderr);
			m_consoleAlloced = true;
		}
	}

	void Free()
	{
		if (m_in != NULL) { 
			fclose(m_in);
			m_in = NULL;
		}
		if (m_out != NULL) {
			fclose(m_out);
			m_out = NULL;
		}
		if (m_err != NULL) {
			fclose(m_err);
			m_err = NULL;
		}
		if (m_consoleAlloced) {
			::FreeConsole();
			m_consoleAlloced = false;
		}
	}

private:
	FILE*	m_in;
	FILE*	m_out;
	FILE*	m_err;
	bool	m_consoleAlloced;
};
static ConsoleInternal g_console;

//------------------------------------------------------------------------------
void Console::Alloc()
{
	g_console.Alloc();
}

//------------------------------------------------------------------------------
void Console::Free()
{
	g_console.Free();
}

#endif

//------------------------------------------------------------------------------
void Console::WriteLine()
{
	WriteInternal(Environment::GetNewLine<char>());
}

//------------------------------------------------------------------------------
void Console::WriteLine(const StringRefA& str)
{
	StringA s(str);
	WriteInternal(s.c_str());
	WriteLine();
}

//------------------------------------------------------------------------------
void Console::WriteLine(const StringRefW& str)
{
	StringW s(str);
	WriteInternal(s.c_str());
	WriteLine();
}

//------------------------------------------------------------------------------
void Console::WriteLineError()
{
	WriteInternalError(Environment::GetNewLine<char>());
}

//------------------------------------------------------------------------------
void Console::WriteInternal(const char* str)
{
	fprintf(stdout, str);
}

//------------------------------------------------------------------------------
void Console::WriteInternal(const wchar_t* str)
{
	fwprintf(stdout, str);
}

//------------------------------------------------------------------------------
void Console::WriteInternalError(const char* str)
{
	fprintf(stderr, str);
}

//------------------------------------------------------------------------------
void Console::WriteInternalError(const wchar_t* str)
{
	fwprintf(stderr, str);
}

LN_NAMESPACE_END
