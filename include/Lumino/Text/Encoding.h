
#pragma once
#include "../Base/NonCopyable.h"
#include "../Base/RefObject.h"

namespace Lumino
{
class RefBuffer;
class Decoder;
class Encoder;

typedef unsigned short UTF16;

enum EncodingType
{
	EncodingType_Unknown = 0,	///< 不明な文字コード (判別失敗。またはバイナリファイル)
	EncodingType_UTF8,
	EncodingType_UTF8N,
	EncodingType_UTF16L,
	EncodingType_UTF16B,
	//EncodingType_UTF16LN,
	//EncodingType_UTF16BN,
	EncodingType_UTF32L,
	EncodingType_UTF32B,
	//EncodingType_UTF32LN,
	//EncodingType_UTF32BN,

	EncodingType_SJIS,

	EncodingType_Max,		// terminator
};

/// 文字コード変換の結果を受け取るための構造体
struct EncodingConversionResult
{
	size_t	BytesUsed;			///< 変換後の有効バイト数
	size_t	CharsUsed;			///< 変換後の有効文字数(マルチバイト文字を考慮した文字数)
	bool	UsedDefaultChar;	///< 変換不可文字をデフォルト文字('?') に変換したかどうか
};

/**
	@brief		文字エンコーディング
*/
class Encoding : public RefObject
{
public:
	Encoding() : mFallbackReplacementChar((uint32_t)'?') {};
	virtual ~Encoding() {};

public:

	/**
		@brief		環境依存のマルチバイト文字コードのエンコーディングを取得する。
		@details	返されるポインタはグローバルなインスタンスです。このポインタは解放しないでください。
	*/
	static Encoding* GetSystemMultiByteEncoding();

	/**
		@brief		ワイド文字のエンコーディングを取得する。
		@details	返されるポインタはグローバルなインスタンスです。このポインタは解放しないでください。
	*/
	static Encoding* GetWideCharEncoding();

	/**
		@brief		TCHAR 型文字のエンコーディングを取得する。
		@details	返されるポインタはグローバルなインスタンスです。このポインタは解放しないでください。
	*/
	static Encoding* GetTCharEncoding();

	/**
		@brief		UTF-8 エンコーディングを取得する。
		@details	返されるポインタはグローバルなインスタンスです。このポインタは解放しないでください。
	*/
	static Encoding* GetUTF8Encoding();

	/**
		@brief		UTF-16 (Little) エンコーディングを取得する。
		@details	返されるポインタはグローバルなインスタンスです。このポインタは解放しないでください。
	*/
	static Encoding* GetUTF16Encoding();

	/**
		@brief		文字コードを変換する (不正シーケンスがあったら例外)
		@param[in]	src				: 変換元
		@param[in]	srcByteCount	: 変換元のバイト数
		@param[in]	srcEncoding		: 変換元のエンコーディング
		@param[in]	targetEncoding	: 変換先のエンコーディング
		@param[out]	result			: 変換の結果情報
		@return		\0終端文字は付加されません。GetSize() により使用バイト数を確認できます。
					使用後、Release() で開放する必要があります。
	*/
	static RefBuffer* Convert(
		const void* src, size_t srcByteCount, const Encoding* srcEncoding,
		const Encoding* targetEncoding,
		EncodingConversionResult* result);

	/**
		@brief		Encoder と Decoder を指定して文字コードを変換する (不正シーケンスがあったら例外)
		@param[in]	src				: 変換元
		@param[in]	srcByteCount	: 変換元のバイト数
		@param[in]	decoder			: src を中間フォーマットに変換するデコーダ
		@param[in]	encoder			: 中間フォーマットをターゲットに変換するエンコーダ
		@param[out]	result			: 変換の結果情報
		@details	指定された Encoder と Decoder は、変換ステータスを保持できる場合は保持します。
		@return		\0終端文字は付加されません。GetSize() により使用バイト数を確認できます。
					使用後、Release() で開放する必要があります。
	*/
	static RefBuffer* Convert(
		const void* src, size_t srcByteCount, Decoder* decoder,
		Encoder* encoder,
		EncodingConversionResult* result);

public:

	/**
		@brief		1 文字の最小バイト数を取得する
	*/
	virtual int GetMinByteCount() const = 0;

	/**
		@brief		1 文字の最大バイト数を取得する
	*/
	virtual int GetMaxByteCount() const = 0;
	
	/**
		@brief		デコードまたはエンコード先にマッピングできない文字が見つかった時に置換する文字の設定
	*/
	virtual void SetFallbackReplacementChar(uint32_t ch) { mFallbackReplacementChar = ch; }

	/**
		@brief		デコードまたはエンコード先にマッピングできない文字が見つかった時に置換する文字の取得
	*/
	virtual uint32_t GetFallbackReplacementChar() { return mFallbackReplacementChar; }

	/**
		@brief		バイトシーケンスを内部文字列へ変換するデコーダを作成する
	*/
	virtual Decoder* CreateDecoder() const = 0;

	/**
		@brief		内部文字列をバイトシーケンスへ変換するエンコーダを作成する
	*/
	virtual Encoder* CreateEncoder() const = 0;

	/**
		@brief		このエンコードを識別するための前文文字列 (BOM) を取得する
		@return		NULL で終わるバイト列。BOM を持たないエンコードの場合は NULL
	*/
	virtual byte_t* GetPreamble() const = 0;

protected:
	uint32_t	mFallbackReplacementChar;
};




/*===============================================================================
	以下は内部用クラス。新しい文字コードに対応する場合は拡張する必要があるが、
	基本的には意識しなくてOK。
===============================================================================*/

/**
	@brief		エンコード済みバイトシーケンスから内部文字コード (UTF16) への変換を行うクラス
	@details	Decoder は1つの文字列ストリームの変換中、任意の回数 ConvertToUTF16() を呼び出すことができます。
				その間、変換ステータスを保存します。
				つまり、バッファリングIO等で長大な文章を複数のメモリバッファに分割して読み取る場合に、
				バッファの境界でマルチバイト文字が分かれてしまっても、継続して変換を行うことができます。

				ただし、このステータス保存は CanRemain() が true を返す場合のみ使用可能です。
				false を返す場合はあらかじめ全てのソース文字列を1つの連続したメモリバッファに読み込み、
				ConvertToUTF16() に渡す必要があります。

				参考：.NET Decoder.Convert()
				http://msdn.microsoft.com/ja-jp/library/twte1yfs(v=vs.110).aspx
*/
class Decoder : public RefObject
{
public:
	Decoder() : mFallbackReplacementChar(0) {}
	virtual ~Decoder() {}

public:

	/**
		@brief		1 文字の最小バイト数を取得する
	*/
	virtual int GetMinByteCount() = 0;

	/**
		@brief		1 文字の最大バイト数を取得する
	*/
	virtual int GetMaxByteCount() = 0;

	/**
		@brief		Decoder が変換状態を保持できるかを確認する
	*/
	virtual bool CanRemain() = 0;

	/**
		@brief		デコード先にマッピングできない文字が見つかった時に置換する文字の設定
	*/
	virtual void SetFallbackReplacementChar(uint32_t ch) { mFallbackReplacementChar = ch; }

	/**
		@brief		デコード先にマッピングできない文字が見つかった時に置換する文字の取得
	*/
	virtual uint32_t GetFallbackReplacementChar() { return mFallbackReplacementChar; }

	/**
		@brief		このエンコーディングで表現される文字列バッファを、UTF16 に変換する
		@param[in]	inBuffer			: このエンコーディングで表現される文字列バッファ
		@param[in]	inBufferByteCount	: inBuffer のバイト数 (NULL 文字は含まないこと)
		@param[out]	outBuffer			: 変換結果の格納先バッファ
		@param[in]	outBufferCharCount	: outBuffer のサイズ (文字数単位)
		@param[out]	outBytesUsed		: 変換結果のバイト数を格納する変数
		@param[out]	outCharsUsed		: 変換結果の文字数を格納する変数
	*/
	virtual void ConvertToUTF16(const byte_t* inBuffer, size_t inBufferByteCount, UTF16* outBuffer, size_t outBufferCharCount, size_t* outBytesUsed, size_t* outCharsUsed) = 0;

	/**
		@brief	一連の ConvertToUTF16() の呼び出しの中で、変換できない文字を規定文字に変換した文字数を取得する
	*/
	virtual int UsedDefaultCharCount() = 0;

	/**
		@brief	最後の ConvertToUTF16() で、バッファ末尾でマルチバイト文字が途切れていなければ true を返す
	*/
	virtual bool Completed() = 0;

	/**
		@brief	変換ステータスをクリアし、初期状態に戻す
	*/
	virtual void Reset() = 0;

protected:
	uint32_t	mFallbackReplacementChar;
};

/**
	@brief		内部文字コード (UTF16) をエンコードするクラス
	@details	Decoder 同様、CanRemain() が true の場合は変換ステータスを保存します。
*/
class Encoder : public RefObject
{
public:
	Encoder() : mFallbackReplacementChar(0) {}
	virtual ~Encoder() {}

public:

	/**
		@brief		1 文字の最小バイト数を取得する
	*/
	virtual int GetMinByteCount() = 0;

	/**
		@brief		1 文字の最大バイト数を取得する
	*/
	virtual int GetMaxByteCount() = 0;

	/**
		@brief		エンコード先にマッピングできない文字が見つかった時に置換する文字の設定
	*/
	virtual void SetFallbackReplacementChar(uint32_t ch) { mFallbackReplacementChar = ch; }

	/**
		@brief		エンコード先にマッピングできない文字が見つかった時に置換する文字の取得
	*/
	virtual uint32_t GetFallbackReplacementChar() { return mFallbackReplacementChar; }

	/**
		@brief	Decoder が変換状態を保持できるかを確認する
	*/
	virtual bool CanRemain() = 0;

	/**
		@brief		UTF16 文字列を、このエンコーディングで表現される文字列に変換する
		@param[in]	inBuffer			: このエンコーディングで表現される文字列バッファ
		@param[in]	inBufferCharCount	: inBuffer のサイズ (文字数単位。NULL 文字は含まない)
		@param[out]	outBuffer			: 変換結果の格納先バッファ
		@param[in]	outBufferByteCount	: outBuffer のバイト数
		@param[out]	outBytesUsed		: 変換結果のバイト数を格納する変数
		@param[out]	outCharsUsed		: 変換結果の文字数を格納する変数
	*/
	virtual void ConvertFromUTF16(const UTF16* inBuffer, size_t inBufferCharCount, byte_t* outBuffer, size_t outBufferByteCount, size_t* outBytesUsed, size_t* outCharsUsed) = 0;

	/**
		@brief	一連の ConvertToUTF16() の呼び出しの中で、変換できない文字を規定文字に変換した文字数を取得する
	*/
	virtual int UsedDefaultCharCount() = 0;

	/**
		@brief	最後の ConvertToUTF16() で、バッファ末尾でマルチバイト文字が途切れていなければ true を返す
	*/
	virtual bool Completed() = 0;

	/**
		@brief	変換ステータスをクリアし、初期状態に戻す
	*/
	virtual void Reset() = 0;

protected:
	uint32_t	mFallbackReplacementChar;
};


/**
	@brief		プログラムのロケールに合わせたマルチバイトコードエンコーディング
	@details	setlocale() にて指定されたロケールの規定文字コードを表します。
				OS 規定のロケールを使用するには、あらかじめ setlocale( LC_ALL, "" ); のようにするにする必要があります。
				例えば、日本語 Windws であれば ShiftJIS、Linux であれば UTF-8 となります。
	@attention	上記解説の通り、既定の文字コードは環境依存です。
				基本的に ASCII 文字のみで構成されるエンコーディングと考えるのが無難です。
*/
class SystemMultiByteEncoding : public Encoding
{
public:
	SystemMultiByteEncoding() {};
	virtual ~SystemMultiByteEncoding() {};

public:
	// override Encoding
	virtual int GetMinByteCount() const;
	virtual int GetMaxByteCount() const;
	virtual Decoder* CreateDecoder() const { return LN_NEW SystemMultiByteDecoder(); }
	virtual Encoder* CreateEncoder() const { return LN_NEW SystemMultiByteEncoder(); }
	virtual byte_t* GetPreamble() const { return 0; }

private:
	// Decoder
	class SystemMultiByteDecoder : public Decoder
	{
	public:
		SystemMultiByteDecoder() { Reset(); }
		virtual int GetMinByteCount() { return 1; }
		virtual int GetMaxByteCount() { return 6; }
		virtual bool CanRemain() { return false; }
		virtual void ConvertToUTF16(const byte_t* inBuffer, size_t inBufferByteCount, UTF16* outBuffer, size_t outBufferCharCount, size_t* outBytesUsed, size_t* outCharsUsed);
		virtual int UsedDefaultCharCount() { return mUsedDefaultCharCount; }
		virtual bool Completed() { return mCompleted; }
		virtual void Reset() { mUsedDefaultCharCount = 0; mCompleted = false; }

	private:
		int			mUsedDefaultCharCount;
		bool		mCompleted;
	};

	// Encoder
	class SystemMultiByteEncoder : public Encoder
	{
	public:
		SystemMultiByteEncoder() { Reset(); }
		virtual int GetMinByteCount() { return 1; }
		virtual int GetMaxByteCount() { return 6; }
		virtual bool CanRemain() { return false; }
		virtual void ConvertFromUTF16(const UTF16* inBuffer, size_t inBufferCharCount, byte_t* outBuffer, size_t outBufferByteCount, size_t* outBytesUsed, size_t* outCharsUsed);
		virtual int UsedDefaultCharCount() { return mUsedDefaultCharCount; }
		virtual bool Completed() { return mCompleted; }
		virtual void Reset() { mUsedDefaultCharCount = 0; mCompleted = false; }

	private:
		int			mUsedDefaultCharCount;
		bool		mCompleted;
	};
};

/**
	@brief		UTF8 エンコーディング
*/
class UTF8Encoding : public Encoding
{
public:
	UTF8Encoding() {};
	virtual ~UTF8Encoding() {};

public:
	// override Encoding
	virtual int GetMinByteCount() const { return 1; }
	virtual int GetMaxByteCount() const { return 6; }
	virtual Decoder* CreateDecoder() const { return LN_NEW UTF8Decoder(); }
	virtual Encoder* CreateEncoder() const { return LN_NEW UTF8Encoder(); }
	virtual byte_t* GetPreamble() const;

private:
	// Decoder
	class UTF8Decoder : public Decoder
	{
	public:
		UTF8Decoder() { Reset(); }
		virtual int GetMinByteCount() { return 1; }
		virtual int GetMaxByteCount() { return 6; }
		virtual bool CanRemain() { return false; }
		virtual void ConvertToUTF16(const byte_t* inBuffer, size_t inBufferByteCount, UTF16* outBuffer, size_t outBufferCharCount, size_t* outBytesUsed, size_t* outCharsUsed);
		virtual int UsedDefaultCharCount() { return mUsedDefaultCharCount; }
		virtual bool Completed() { return mCompleted; }
		virtual void Reset() { mUsedDefaultCharCount = 0; mCompleted = false; }

	private:
		int			mUsedDefaultCharCount;	///< 一連の ConvertToUTF16() の呼び出しの中で、変換できない文字を規定文字に変換した文字数
		bool		mCompleted;				///< 最後の ConvertToUTF16() で、バッファ末尾でマルチバイト文字が途切れていなければ true
	};

	// Encoder
	class UTF8Encoder : public Encoder
	{
	public:
		UTF8Encoder() { Reset(); }
		virtual int GetMinByteCount() { return 1; }
		virtual int GetMaxByteCount() { return 6; }
		virtual bool CanRemain() { return false; }
		virtual void ConvertFromUTF16(const UTF16* inBuffer, size_t inBufferCharCount, byte_t* outBuffer, size_t outBufferByteCount, size_t* outBytesUsed, size_t* outCharsUsed);
		virtual int UsedDefaultCharCount() { return mUsedDefaultCharCount; }
		virtual bool Completed() { return mCompleted; }
		virtual void Reset() { mUsedDefaultCharCount = 0; mCompleted = false; }

	private:
		int			mUsedDefaultCharCount;	///< 一連の ConvertFromUTF16() の呼び出しの中で、変換できない文字を規定文字に変換した文字数
		bool		mCompleted;				///< 最後の ConvertFromUTF16() で、バッファ末尾でマルチバイト文字が途切れていなければ true
	};
};

/**
	@brief		UTF16 エンコーディング
	@details	変換時のバイトオーダーは規定しません。(Big/Little どちらでもOK)
				GetPreamble() が返す BOM は Little です。
*/
class UTF16Encoding : public Encoding
{
public:
	UTF16Encoding() {};
	virtual ~UTF16Encoding() {};

public:
	// override Encoding
	virtual int GetMinByteCount() const { return 2; }
	virtual int GetMaxByteCount() const { return 4; }
	virtual Decoder* CreateDecoder() const { return LN_NEW UTF16Decoder(); }
	virtual Encoder* CreateEncoder() const { return LN_NEW UTF16Encoder(); }
	virtual byte_t* GetPreamble() const;

private:
	// Decoder
	class UTF16Decoder : public Decoder
	{
	public:
		UTF16Decoder() { Reset(); }
		virtual int GetMinByteCount() { return 2; }
		virtual int GetMaxByteCount() { return 4; }
		virtual bool CanRemain() { return false; }
		virtual void ConvertToUTF16(const byte_t* inBuffer, size_t inBufferByteCount, UTF16* outBuffer, size_t outBufferCharCount, size_t* outBytesUsed, size_t* outCharsUsed);
		virtual int UsedDefaultCharCount() { return mUsedDefaultCharCount; }
		virtual bool Completed() { return mCompleted; }
		virtual void Reset() { mUsedDefaultCharCount = 0; mCompleted = false; }

	private:
		int			mUsedDefaultCharCount;
		bool		mCompleted;
	};

	// Encoder
	class UTF16Encoder : public Encoder
	{
	public:
		UTF16Encoder() { Reset(); }
		virtual int GetMinByteCount() { return 2; }
		virtual int GetMaxByteCount() { return 4; }
		virtual bool CanRemain() { return false; }
		virtual void ConvertFromUTF16(const UTF16* inBuffer, size_t inBufferCharCount, byte_t* outBuffer, size_t outBufferByteCount, size_t* outBytesUsed, size_t* outCharsUsed);
		virtual int UsedDefaultCharCount() { return mUsedDefaultCharCount; }
		virtual bool Completed() { return mCompleted; }
		virtual void Reset() { mUsedDefaultCharCount = 0; mCompleted = false; }

	private:
		int			mUsedDefaultCharCount;
		bool		mCompleted;
	};
};


} // namespace Lumino
