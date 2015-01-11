
#pragma once

#include <stdio.h>
#include <windows.h>
#include <stdlib.h>
#include <dbghelp.h>

namespace Lumino
{
class BackTrace
{
private:

	//dll�ǂݍ��݃w���p�[
	class LoadLibraryHelper
	{
	private:
		HMODULE DllInstance;

	public:
		LoadLibraryHelper()
		{
			this->DllInstance = NULL;
		}
		virtual ~LoadLibraryHelper()
		{
			if (this->DllInstance != NULL)
			{
				::FreeLibrary(this->DllInstance);
				this->DllInstance = NULL;
			}
		}
		bool Load(const char* inDLLName)
		{
			if (this->DllInstance != NULL)
			{
				return false;
			}
			this->DllInstance = ::LoadLibraryA(inDLLName);
			return this->DllInstance != NULL;
		}
		FARPROC GetProcAddress(const char* inProcName)
		{
			if (!this->DllInstance)
			{
				return NULL;
			}
			return ::GetProcAddress(this->DllInstance, inProcName);
		}
	};

private:

	HANDLE	m_hProcess;
	bool	m_bIsSymbolEngineReady;
	LoadLibraryHelper	mKernel32Librsry;
	LoadLibraryHelper	mDbgHelpLibrsry;

	typedef USHORT(WINAPI *RtlCaptureStackBackTraceDef) (ULONG FramesToSkip, ULONG FramesToCapture, PVOID *BackTrace, /*__out_opt*/ PULONG BackTraceHash);
	RtlCaptureStackBackTraceDef mRtlCaptureStackBackTraceProc;

#if (_WIN64 || __x86_64__)
	//�A�h���X���烂�W���[�������߂�
	typedef BOOL(WINAPI *SymGetModuleInfo64Def) (HANDLE hProcess, DWORD64 dwAddr, PIMAGEHLP_MODULE64 ModuleInfo);
	SymGetModuleInfo64Def mSymGetModuleInfo64Proc;
	//�A�h���X����V���{�������߂�
	typedef BOOL(WINAPI *SymGetSymFromAddr64Def) (HANDLE hProcess, DWORD64 Address, PDWORD64 Displacement, PIMAGEHLP_SYMBOL64 Symbol);
	SymGetSymFromAddr64Def mSymGetSymFromAddr64Proc;
	//�A�h���X����t�@�C���ƍs�ԍ������߂�
	typedef BOOL(WINAPI *SymGetLineFromAddr64Def) (HANDLE hProcess, DWORD64 Address, PDWORD64 Displacement, PIMAGEHLP_LINE64 Line);
	SymGetLineFromAddr64Def mSymGetLineFromAddr64Proc;
#else
	//�A�h���X���烂�W���[�������߂�
	typedef BOOL(WINAPI *SymGetModuleInfoDef) (HANDLE hProcess, DWORD dwAddr, PIMAGEHLP_MODULE ModuleInfo);
	SymGetModuleInfoDef mSymGetModuleInfoProc;
	//�A�h���X����V���{�������߂�
	typedef BOOL(WINAPI *SymGetSymFromAddrDef) (HANDLE hProcess, DWORD Address, PDWORD Displacement, PIMAGEHLP_SYMBOL Symbol);
	SymGetSymFromAddrDef mSymGetSymFromAddrProc;
	//�A�h���X����t�@�C���ƍs�ԍ������߂�
	typedef BOOL(WINAPI *SymGetLineFromAddrDef) (HANDLE hProcess, DWORD dwAddr, PDWORD pdwDisplacement, PIMAGEHLP_LINE Line);
	SymGetLineFromAddrDef mSymGetLineFromAddrProc;
#endif
	//�V���{���G���W���̃I�v�V����
	typedef BOOL(WINAPI *SymSetOptionsDef) (DWORD SymOptions);
	SymSetOptionsDef mSymSetOptionsProc;

	//�V���{���G���W���̏�����
	typedef BOOL(WINAPI *SymInitializeDef) (HANDLE hProcess, PSTR UserSearchPath, BOOL fInvadeProcess);
	SymInitializeDef mSymInitializeProc;

	//�V���{���G���W���̏I��
	typedef BOOL(WINAPI *SymCleanupDef) (HANDLE hProcess);
	SymCleanupDef mSymCleanupProc;

public:

	static const BackTrace* GetInstance()
	{
		static BackTrace s;
		return &s;
	}

	BackTrace()
	{
		// �v���Z�X���L�^
		m_hProcess = ::GetCurrentProcess();

		if (!mKernel32Librsry.Load("kernel32.dll")) {
			return;
		}
		if (!mDbgHelpLibrsry.Load("dbghelp.dll")) {
			return;
		}

		mRtlCaptureStackBackTraceProc = (RtlCaptureStackBackTraceDef)mKernel32Librsry.GetProcAddress("RtlCaptureStackBackTrace");
#if (_WIN64 || __x86_64__)
		mSymGetModuleInfo64Proc = (SymGetModuleInfo64Def)mDbgHelpLibrsry.GetProcAddress("SymGetModuleInfo64");
		mSymGetSymFromAddr64Proc = (SymGetSymFromAddr64Def)mDbgHelpLibrsry.GetProcAddress("SymGetSymFromAddr64");
		mSymGetLineFromAddr64Proc = (SymGetLineFromAddr64Def)mDbgHelpLibrsry.GetProcAddress("SymGetLineFromAddr64");
#else
		mSymGetModuleInfoProc = (SymGetModuleInfoDef)mDbgHelpLibrsry.GetProcAddress("SymGetModuleInfo");
		mSymGetSymFromAddrProc = (SymGetSymFromAddrDef)mDbgHelpLibrsry.GetProcAddress("SymGetSymFromAddr");
		mSymGetLineFromAddrProc = (SymGetLineFromAddrDef)mDbgHelpLibrsry.GetProcAddress("SymGetLineFromAddr");
#endif
		mSymSetOptionsProc = (SymSetOptionsDef)mDbgHelpLibrsry.GetProcAddress("SymSetOptions");
		mSymInitializeProc = (SymInitializeDef)mDbgHelpLibrsry.GetProcAddress("SymInitialize");
		mSymCleanupProc = (SymCleanupDef)mDbgHelpLibrsry.GetProcAddress("SymCleanup");

		// �V���{���G���W���̏�����
		mSymSetOptionsProc(SYMOPT_DEFERRED_LOADS | SYMOPT_LOAD_LINES | SYMOPT_UNDNAME);
		if (mSymInitializeProc(m_hProcess, NULL, TRUE))
		{
			// �V���{���G���W����������
			m_bIsSymbolEngineReady = true;
		}
	}

	~BackTrace()
	{
		if (m_bIsSymbolEngineReady)
		{
			mSymCleanupProc(m_hProcess);
			m_bIsSymbolEngineReady = false;
		}
	}

public:

	/// �X�^�b�N�g���[�X���擾����
	int Backtrace(void** buffer, int n) const
	{
		if (n >= 63) {
			n = 62;
		}
		return (int)mRtlCaptureStackBackTraceProc(0, n, buffer, NULL);
	}

#ifdef LN_X64 //for64bit
#error ���e�X�g
	//�V���{���̉���
	void addressToSymbolString(void* address, char * outBuffer, int len) const
	{
		if (!this->IsSymbolEngineReady)
		{
			//�V���{���G���W���������ł��Ă��Ȃ�.
			_snprintf_s(outBuffer, len, _TRUNCATE, "0x%p @ ??? @ ??? @ ???:???", address);
			return;
		}

		//���W���[����
		IMAGEHLP_MODULE64 imageModule = { sizeof(IMAGEHLP_MODULE64) };
		BOOL r = this->SymGetModuleInfo64Proc(this->Process, (DWORD64)address, &imageModule);
		if (!r)
		{
			_snprintf_s(outBuffer, len, _TRUNCATE, "0x%p @ ??? @ ??? @ ???:???", address);
			return;
		}

		//�V���{�����i�[�o�b�t�@.
		IMAGEHLP_SYMBOL64 * imageSymbol;
		char buffer[MAX_PATH + sizeof(IMAGEHLP_SYMBOL64)] = { 0 };
		imageSymbol = (IMAGEHLP_SYMBOL64*)buffer;
		imageSymbol->SizeOfStruct = sizeof(IMAGEHLP_SYMBOL64);
		imageSymbol->MaxNameLength = MAX_PATH;

		//�֐����̎擾...
		DWORD64 disp = 0;
		r = this->SymGetSymFromAddr64Proc(this->Process, (DWORD64)address, &disp, imageSymbol);
		if (!r)
		{//�֐������킩��܂���
			_snprintf_s(outBuffer, len, _TRUNCATE, "0x%p @ %s @ ??? @ ???:???", address, imageModule.ModuleName);
			return;
		}

		//�s�ԍ��̎擾
		IMAGEHLP_LINE64 line = { sizeof(IMAGEHLP_LINE64) };
		r = this->SymGetLineFromAddr64Proc(this->Process, (DWORD64)address, &disp, &line);
		if (!r)
		{//�s�ԍ���������܂���
			_snprintf_s(outBuffer, len, _TRUNCATE, "0x%p @ %s @ %s @ %s+%d", address,
				imageModule.ModuleName, imageSymbol->Name, imageSymbol->Name, (int)((char*)address - (char*)line.Address));
			return;
		}

		//�s�ԍ����킩��܂���.
		_snprintf_s(outBuffer, len, _TRUNCATE, "0x%p @ %s @ %s @ %s:%d", address, imageModule.ModuleName, imageSymbol->Name, line.FileName, line.LineNumber);
	}
#else // #ifdef LN_X64
	//�V���{���̉���
	void AddressToSymbolString(void* address, char * outBuffer, int len) const
	{
		if (!m_bIsSymbolEngineReady)
		{
			// �V���{���G���W���������ł��Ă��Ȃ�
			_snprintf_s(outBuffer, len, _TRUNCATE, "0x%p @ ??? @ ??? @ ???:???", address);
			return;
		}

		// ���W���[����
		IMAGEHLP_MODULE imageModule = { sizeof(IMAGEHLP_MODULE) };
		BOOL r = mSymGetModuleInfoProc(m_hProcess, (DWORD)address, &imageModule);
		if (!r) {
			_snprintf_s(outBuffer, len, _TRUNCATE, "0x%p @ ??? @ ??? @ ???:???", address);
			return;
		}

		// �V���{�����i�[�o�b�t�@
		IMAGEHLP_SYMBOL * imageSymbol;
		char buffer[MAX_PATH + sizeof(IMAGEHLP_SYMBOL)] = { 0 };
		imageSymbol = (IMAGEHLP_SYMBOL*)buffer;
		imageSymbol->SizeOfStruct = sizeof(IMAGEHLP_SYMBOL);
		imageSymbol->MaxNameLength = MAX_PATH;

		// �֐����̎擾
		DWORD disp = 0;
		r = mSymGetSymFromAddrProc(m_hProcess, (DWORD)address, &disp, imageSymbol);
		if (!r) {
			_snprintf_s(outBuffer, len, _TRUNCATE, "0x%p @ %s @ ??? @ ???:???", address, imageModule.ModuleName);
			return;
		}

		// �s�ԍ��̎擾
		IMAGEHLP_LINE line = { sizeof(IMAGEHLP_LINE) };
		r = mSymGetLineFromAddrProc(m_hProcess, (DWORD)address, &disp, &line);
		if (!r) {
			_snprintf_s(outBuffer, len, _TRUNCATE, "0x%p @ %s @ %s @ %s+%d",
				address, imageModule.ModuleName, imageSymbol->Name, imageSymbol->Name, (int)((char*)address - (char*)line.Address));
			return;
		}

		// ��ʂ葵���� (�t�@�C�����͏o���Ȃ��ł����B�t���p�X�ŏo��̂Ń��[�J���̃t�H���_�\���ۂ킩�肾���c �֐����ƍs�ԍ�������Ώ\��)
		_snprintf_s(outBuffer, len, _TRUNCATE, "0x%p @ %s @ %s @ XXXX:%d", address, imageModule.ModuleName, imageSymbol->Name, line.LineNumber);
		//_snprintf_s(outBuffer, len, _TRUNCATE, "0x%p @ %s @ %s @ %s:%d", address, imageModule.ModuleName, imageSymbol->Name, line.FileName, line.LineNumber);
	}
#endif // #ifdef LN_X64

	// �V���{�����܂Ƃ߂ĉ���
	void AddressToFullSymbolString(void** address, int size, char* outBuffer, int len) const
	{
		int writesize = 0;

		int i = 0;
		for (i = 0; i < size; i++)
		{
			void * p = address[i];
			if (p == NULL) {
				break;
			}

			AddressToSymbolString(p, outBuffer + writesize, len - writesize);
			writesize += (int)strlen(outBuffer + writesize);

			if (len - writesize >= 2) {
				strncat_s(outBuffer + writesize, len - writesize, "\r\n", 2);
				writesize += 2;
			}

			if (len <= writesize) {
				break;
			}
		}
	}

	//�o�b�N�g���[�X�̎擾���ĉ�ʂɕ\��
	//void printBackTrace() const
	//{
	//	void* stackBuffer[50];
	//	char symbolBuffer[1024];

	//	int stacksize = this->backtrace(stackBuffer, 50);
	//	this->addressToFullSymbolString(stackBuffer, stacksize, symbolBuffer, 1024);

	//	puts(symbolBuffer);
	//}

};
	
} // namespace Lumino
