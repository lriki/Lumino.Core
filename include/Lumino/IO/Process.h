/**
	@file	Process.h
*/
#pragma once
#include "../Base/Delegate.h"
#include "../Threading/Thread.h"
#include "Common.h"
#include "StreamReader.h"
#include "StreamWriter.h"

namespace Lumino
{

/**
	@brief	外部プロセスを起動し、通信を行うクラスです。
*/
class Process
{
public:
	Process();
	~Process();
	
public:

	/**
		@brief		開始するプロセスの作業ディレクトリを指定します。 (規定値:現在のカレントディレクトリ)
		@details	Start() の前に設定する必要があります。
	*/
	void SetWorkingDirectory(const PathName& directoryPath);
	
	/**
		@brief		標準入力をリダイレクトするかを設定します。 (規定値:false)
		@details	Start() の前に設定する必要があります。
	*/
	void SetRedirectStandardInput(bool enabled);

	/**
		@brief		標準出力をリダイレクトするかを設定します。 (規定値:false)
		@details	Start() の前に設定する必要があります。
	*/
	void SetRedirectStandardOutput(bool enabled);

	/**
		@brief		標準エラー出力をリダイレクトするかを設定します。 (規定値:false)
		@details	Start() の前に設定する必要があります。
	*/
	void SetRedirectStandardError(bool enabled);

	/**
		@brief		標準出力に行が書き込まれたときに呼び出されるコールバック関数を設定します。
		@details	非同期読み取りは Start() の後、BeginOutputReadLine() を呼び出すことで開始します。
					コールバックは Start() を実行したスレッドとは別のスレッドから非同期的に呼び出されます。
	*/
	void SetOutputDataReceivedCallback(const Delegate01<String>& callback);

	/**
		@brief		プログラムのファイルパスとコマンドライン引数を指定してプロセスを起動します。
		@param[in]	program	: プログラム名または実行ファイルパス
		@param[in]	args	: コマンドライン引数
		@exception	FileNotFoundException	program が見つからなかった。
	*/
	void Start(const PathName& program, const String& args = String());

	/**
		@brief		プロセスの終了を待機します。
		@param[in]	timeoutMSec	: タイムアウト時間 (ミリ秒)
		@return		true:正常終了  false:タイムアウトした
	*/
	bool WaitForExit(int timeoutMSec = -1);

	/**
		@brief		プロセスの状態を取得します。
		@attention	リダイレクトを非同期で行っている場合、ProcessStatus::Finished を返してもまだリダイレクトスレッドは終了していないことがあります。
					完了させるには WaitForExit() を呼び出してください。
	*/
	ProcessStatus GetState();

	/**
		@brief		プロセスの終了コードを取得します。
		@details	GetState() が ProcessStatus::Finished を返さない限り、戻り値は有効ではありません。
	*/
	int GetExitCode();

	/**
		@brief		標準出力の非同期読み取り操作を開始します。
	*/
	void BeginOutputReadLine();

	/**
		@brief		標準入力の書き込みに使用されるストリームを取得します。
	*/
	StreamWriter* GetStandardInput() const;

	/**
		@brief		標準出力の読み取りに使用されるストリームを取得します。
		@details	この関数は同期的な読み取りを行う際に使用します。
					取得した StreamReader の ReadLine() や ReadToEnd() は、子プロセスが標準出力に書き込みを行うか、プロセスが終了するまで待機し、
					プロセスが終了することで EOF となります。
	*/
	StreamReader* GetStandardOutput() const;

	/**
		@brief		標準エラー出力の読み取りに使用されるストリームを取得します。
		@see		GetStandardOutput()
	*/
	StreamReader* GetStandardError() const;

public:

	/**
		@brief		プロセスを起動し、終了するまで待機します。
		@param[in]	program		: プログラム名または実行ファイルパス
		@param[in]	args		: コマンドライン引数
		@param[out]	stdOutput	: 標準入力をリダイレクトし文字列として受け取る場合、その文字列を格納する変数のポインタを指定する
		@return		プロセスの終了コード
		@exception	FileNotFoundException	: program が見つからなかった。
	*/
	static int Execute(const PathName& program, const String& args = String(), String* stdOutput = NULL);

private:
	void TryGetExitCode();
	void Dispose();
	void Thread_ReadStdOutput();

private:
	PathName					m_workingDirectory;
	bool						m_redirectStandardInput;
	bool						m_redirectStandardOutput;
	bool						m_redirectStandardError;
	RefPtr<StreamWriter>		m_standardInputWriter;
	RefPtr<StreamReader>		m_standardOutputReader;
	RefPtr<StreamReader>		m_standardErrorReader;
	Delegate01<String>			m_outputDataReceivedCallback;
	Threading::DelegateThread	m_readStdOutputThread;
	int							m_exitCode;
	bool						m_crashed;
	bool						m_disposed;
	bool						m_runningReadThread;

#ifdef _WIN32
	HANDLE					m_hInputRead;			// 標準出力の読み取り側 (子プロセス側)
	HANDLE					m_hInputWrite;			// 標準出力の書き込み側 (このプロセス側)
	HANDLE					m_hOutputRead;			// 標準出力の読み取り側 (このプロセス側)
	HANDLE					m_hOutputWrite;			// 標準出力の書き込み側 (子プロセス側)
	HANDLE					m_hErrorRead;			// 標準エラー出力の読み取り側 (このプロセス側)
	HANDLE					m_hErrorWrite;			// 標準エラー出力の書き込み側 (子プロセス側)
	PROCESS_INFORMATION		m_processInfo;
#endif
};

} // namespace Lumino
