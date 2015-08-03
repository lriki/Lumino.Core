
/*


エラー処理のポリシー
・異常系処理は、必ず通知する手段を提供する。
	エラーをどう扱うか(あるいは無視するか)は、ライブラリを使う側に任せる。
	そのため、ライブラリ側でエラーを握りつぶしてはならない。
	例えば、
	
	/// 文字列を数値に変換する。
	int ToInt(const char* text)
	{
		if (text == NULL) {
			return 0;		// FailSafe.
		}
		return atoi(text);
	}
	
	といったような FailSafe 処理は、ライブラリユーザーにエラーを通知することなく
	エラーを握りつぶしていることになる。
	
	修正方法①
		関数の仕様に、「NULL を指定した場合は 0 を返す」と明記する。
	
	修正方法②
		return 0; の前に assert や throw、あるいはエラー発生時のコールバックを実装する。
	


エラー処理の変遷・・・

■ver1.0 (2009年頃)
	最初に用意したエラー処理は、エラーコードを戻り値としてエラーを通知していた。
	しかし、以下のような理由により間もなく破綻。
	
	・新しいエラーコードを追加するときは全ての .cpp で共有される .h を変更しなければならない。
	  ベースモジュールとその他を複数のリポジトリに分けている場合、非常に面倒なことになる。
	  
	・呼び出し側で戻り値のチェックを忘れることが頻発。ルールでカバーするのは不可能。
	
	・ほぼ全ての関数の戻り値がエラーコードになったため、呼び出し側で完全にチェックを行うとコードが非常に冗長になった。
	
	・エラーコードで概要はわかるが、詳細がわからない。
	  あるエラーが原因で他のエラーが発生した場合、根本を突き止めるのに時間がかかった。
	  FailSafe 系は特に、本当に稀にしか起こらない問題は根本を突き止めるための情報が揃わず、結局修正できなかった問題もあった。
	  
	・詳細がわからずに調査に時間を使うことは、個人開発ではとんでもないモチベーションの低下につながる・・・。


■ver2.0 (2010年頃)
	前回の問題を受けて、エラー発生時にグローバル変数に詳細情報を残すようにした。(WinAPI の GetLastError() のようなイメージ)
	あらかじめ登録された関数ポインタを呼び出すことで、エラーの根本を特定しやすいようにした。
	しばらくは上手く動いていたが、ライブラリが複雑になるにつれて以下のような問題が出てきた。
	
	・エラー情報がグローバルに格納されるので、スレッドセーフにする必要が出てきた。
	
	・別スレッドのエラーをメインスレッドからユーザーに通知する必要があったが、
	  これは単純な排他処理では対応できない。メインスレッドと別スレッド2つのエラー情報を覚えておく方法が必要になった。
	  スタック的に管理する必要があるし、エラーをクリアする処理も必要。
	  
	・エラーをクリアするなんてことやろうとすると、ほぼ全ての関数の頭に ClearLastError() のように可かなければならず、これも非常に冗長。
	  当然 Clear も排他処理が必要になり、パフォーマンスの低下を招く。


■ver2.1 (2010年頃)
	部分的に例外を使い始めた。
	これまでは例外のコスト (パフォーマンスやメモリリークなどの危険性、コンパイラの互換性) を懸念して使用していなかった。
	スレッドごとにエラー詳細や情報のネストは解決できたが、これまでエラーコードを中心とした例外耐性のないプログラムに対して
	組み込んだため、リソースの解放漏れが頻発した。


■ver3.0 (2012年頃)
	エラー処理を全て例外に移行した。例外の種類や投げ方は .NET、Java、Ruby 等他の言語を参考にした。
	プログラムも、例外耐性を完全にするため、delete 等のリソースの手動解放を可能な限り廃止し、
	スマートポインタを積極的に使用するように全て修正した。
	
	主に .NET を参考に、引数エラーやファイルが存在しないエラーなど、
	ほぼ全てのエラーを積極的に例外として throw するようにした。
	プログラムサイズは若干増えたが、実行速度的にネックとなることは無かった。
	
	後にスタックトレースやファイルへのダンプ等を実装し、
	詳細なエラー情報からスムーズに問題の原因を特定することができるようになったため、
	今後しばらくは例外をメインにエラー処理を行うことになる。
	
	ただし、ライブラリを DLL として作成した際に DLL の外側に例外を出すことになるため、
	作成側と使用側のコンパイラを一致させる必要がでてきた。
	また、std::exception を継承した例外クラスを DLL に export する際、VC++ の /MD を使うと警告された。
	クラスライブラリを DLL として公開するのは危険だと判断し、一旦 DLL の作成は行わないことにした。
	
	一方でこの頃、C++ 以外のプログラム言語でライブラリの機能を使用できるように、
	関数ライブラリの DLL の作成を始めた。
	この DLL はエクスポートした関数(extern "C" した関数)で try-catch を張り、
	例外を絶対に外に出さないようにした上で公開している。
	catch した例外はエラーコードに変換されて戻り値として通知される。


■ver3.1 (2015年頃)
	この頃からできるだけテストを行い、コードカバレッジ等を取るようになってきた。
	その中で、前回積極的に例外を投げるようにした箇所のほとんどがデッドコードになっていることに気が付いた。
	
	また、Qt や UE4 等の様々な C++ 向けフレームワークを参考に、エラー処理を見直していたが、
	そのほとんどは積極的というほど例外を使っていなかった。
	例えば C# では string を引数にとるメソッドに null を渡すと多くの場合 ArgumentException になるが、
	Qt では空文字列とみなして処理を続行する。
	これらはコンパイラの互換性維持とパフォーマンス低下を防ぐためと理由付けされていた。(ソースは失念したが・・・)
	
	・・・というように、要因はいろいろあるが、用途に応じてエラーの通知方法を変えたくなってきた。
	
	・デッドコードな FailSafe 処理に例外をいれてパフォーマンスを下げるのはよろしくないが、
	  今後の機能拡張でデッドコードではなくなるかもしれない。
	  assert で十分だと思われるが、万が一に備えるのであれば Release モードで消えてしまうのは考えモノ。
	  
	・カバレッジを取るときは異常系処理をマークして、集計からは除外したい。
	
	・他にも、XML のパース処理など速度的にクリティカルになりやすい部分は内部的にはエラーコードを使用し、
	  処理が完了した時点でエラーが発生していれば例外として通知するような仕組みにする。
	  クリティカルな部分でどうしても FailSafe 処理を入れたいときは throw ではなく assert を使う。
	
	
	これらを受けて、以下のようなマクロを用意した。
	それぞれ、第1引数の式の結果に応じてエラー処理を実行する。
	- LN_VERIFY
		前提条件の確認。引数のNULLチェックなどに使う。Relese でも式は実行される。(UE の verify マクロに似ている)
	- LN_THROW
		処理の失敗を通知する。主に事後処理として使用する。
	
	あまり増やしすぎても後々混乱するだけなのでこれ以上は拡張しないと思う。
	
	void ToInt(const char* text, int** outValue)
	{
		LN_VERIFY_RETURN(text);
		LN_VERIFY_RETURN(outValue, "outValue is NULL.");
		// cool な処理
		LN_THROW(XXXX, FormatException);
		*outValue = YYYY;
	}

	- 異常系処理をマークする。
	- 用途に応じてエラー処理をカスタマイズできる。
	- エラー処理を書く人が混乱しないよう、できるだけシンプルにする。
	
■ver3.2b
	ライブラリ内部のエラー処理においては、LN_VERIFY だけでは不足になってきた。
	最低限、引数エラーなのか状態エラー (メンバ変数がおかしい) なのかを区別したい。

	UE4 の verify() は、意味的には
	「変な値が来たけど、プログラムは動くようになってます。この値を入れた人がおかしいことしてませんか？」
	であり、表現式は Release でも実行される。
	使う場所も assert と大体同じ。基本的に評価フェーズでエラーを検出するために使う。
	Release では表現式を残すことにより「とにかくアプリを落とさない」FailSafe 的な役目になる。(その際、エラーがあったことは通知しないが・・・)
	とにかく、明確なエラーを表すために使うべきではない。しいて言うならワーニング。


	
*/
#include <time.h>
#include <algorithm>
#include "../Internal.h"
#include "../../include/Lumino/Base/CRT.h"
#include "../../include/Lumino/Base/Exception.h"
#include "../../include/Lumino/Base/StringTraits.h"

#ifdef LN_EXCEPTION_BACKTRACE
	#ifdef LN_WIN32	// Cygwin もこっち
		#include "Win32/BackTrace.h"
	#else
		#include "Unix/BackTrace.h"
	#endif
#endif

namespace Lumino
{

//=============================================================================
// Exception
//=============================================================================

static char gDumpFilePath[LN_MAX_PATH] = { 0 }; 

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
Exception::Exception()
	: mSourceFileLine(0)
	, mStackBufferSize(0)
{
	memset(mStackBuffer, 0, sizeof(mStackBuffer));
	memset(mSymbolBuffer, 0, sizeof(mSymbolBuffer));

#ifdef LN_EXCEPTION_BACKTRACE
	// バックトレース記録
	mStackBufferSize = BackTrace::GetInstance()->Backtrace(mStackBuffer, LN_ARRAY_SIZE_OF(mStackBuffer));

	// バックトレース文字列取得
	BackTrace::GetInstance()->AddressToFullSymbolString(
		mStackBuffer, 
		std::min(mStackBufferSize, 32),
		mSymbolBuffer, 
		LN_ARRAY_SIZE_OF(mSymbolBuffer));
#endif

	// ファイルに保存
	if (strlen(gDumpFilePath) > 0) 
	{
		FILE* fp;
		if (fopen_s(&fp, gDumpFilePath, "w") == 0) 
		{
			time_t timer;
			timer = ::time(NULL);

			char str[256];
			ctime_s(str, 256, &timer);
			
			fprintf(fp, "%s\n%s\n\n", str, mSymbolBuffer);
			fclose(fp);
		}
	}
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
Exception::~Exception() throw()
{
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
Exception& Exception::SetSourceLocationInfo(const char* filePath, int fileLine)
{
	// もしバックトレースが取れていなかったらそれ用の文字列バッファに入れてしまう
	if (mSymbolBuffer[0] == 0x00)
	{
		sprintf_s(mSymbolBuffer, LN_ARRAY_SIZE_OF(mSymbolBuffer), "File:%s Line:%d", filePath, fileLine);
	}

#ifdef LN_UNICODE
	// ワイド文字列へ変換 (文字コードは考慮しない)
	memset(mSourceFilePath, 0, sizeof(mSourceFilePath));
	size_t size;
	errno_t err = mbstowcs_s(&size, mSourceFilePath, LN_MAX_PATH, filePath, LN_MAX_PATH - 1);
	if (err != 0) {
		return *this;
	}
#else
	strcpy(mSourceFilePath, filePath);
#endif

	mSourceFileLine = fileLine;
	return *this;
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
bool Exception::InitDumpFile(const char* filePath)
{
	// ファイルパス保持
	memset(gDumpFilePath, 0, sizeof(gDumpFilePath));
	strcpy_s(gDumpFilePath, LN_MAX_PATH, filePath);

	// ファイル初期化
	FILE* fp;
	if (fopen_s(&fp, filePath, "w") == 0) {
		fclose(fp);
		return true;
	}
	return false;
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void Exception::SetMessage(const char* format, va_list args)
{
	static const int BUFFER_SIZE = MaxMessageBufferSize;
	char buf[BUFFER_SIZE];

	int len = StringTraits::VSPrintf(buf, BUFFER_SIZE, format, args);
	if (len >= BUFFER_SIZE)
	{
		// バッファに収まりきらない場合は終端を ... にして切る
		buf[BUFFER_SIZE - 4] = '.';
		buf[BUFFER_SIZE - 3] = '.';
		buf[BUFFER_SIZE - 2] = '.';
		buf[BUFFER_SIZE - 1] = '\0';
	}

	// TCHAR に合わせてメンバに格納
#ifdef LN_UNICODE
	size_t wlen;
	mbstowcs_s(&wlen, mMessage, BUFFER_SIZE, buf, _TRUNCATE);
#else
	strcpy_s(mMessage, BUFFER_SIZE, buf);
#endif
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void Exception::SetMessage(const wchar_t* format, va_list args)
{
	static const int BUFFER_SIZE = MaxMessageBufferSize;
	wchar_t buf[BUFFER_SIZE];

	int len = StringTraits::VSPrintf(buf, BUFFER_SIZE, format, args);
	if (len >= BUFFER_SIZE)
	{
		// バッファに収まりきらない場合は終端を ... にして切る
		buf[BUFFER_SIZE - 4] = L'.';
		buf[BUFFER_SIZE - 3] = L'.';
		buf[BUFFER_SIZE - 2] = L'.';
		buf[BUFFER_SIZE - 1] = L'\0';
	}

	// TCHAR に合わせてメンバに格納
#ifdef LN_UNICODE
	wcscpy_s(mMessage, BUFFER_SIZE, buf);
#else
	size_t mbcslen;
	wcstombs_s(&mbcslen, mMessage, BUFFER_SIZE, buf, _TRUNCATE);
#endif
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void Exception::SetMessage(const char* format, ...)
{
	va_list args;
	va_start(args, format);
	SetMessage(format, args);
	va_end(args);
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void Exception::SetMessage(const wchar_t* format, ...)
{
	va_list args;
	va_start(args, format);
	SetMessage(format, args);
	va_end(args);
}


#ifdef LN_WIN32
//=============================================================================
// Win32Exception
//=============================================================================

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
Win32Exception::Win32Exception(uint32_t/*DWORD*/ dwLastError)
	: m_dwLastErrorCode( dwLastError )
{
	// エラーメッセージ取得
	memset(m_pFormatMessage, 0, sizeof(m_pFormatMessage));
    :: FormatMessage(
		FORMAT_MESSAGE_FROM_SYSTEM, NULL, m_dwLastErrorCode,
        0, m_pFormatMessage, sizeof(m_pFormatMessage)/sizeof(m_pFormatMessage[0]), NULL);
}

//=============================================================================
// COMException
//=============================================================================

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
COMException::COMException(uint32_t hresult)
	: m_HRESULT(hresult)
{
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
COMException::~COMException() throw()
{
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
Exception* COMException::Copy() const
{
	return LN_NEW COMException(*this);
}
#endif

} // namespace Lumino
