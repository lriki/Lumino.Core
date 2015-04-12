/**
	@file	ArchiveMaker.h
*/
#pragma once

#include <map>
#include <string>
#include "../Base/RefString.h"
#include "../Base/Unicode.h"
#include "../Threading/Mutex.h"
#include "Interface.h"

namespace Lumino
{
class ArchiveStream;

/**
	@brief	アーカイブファイルに読み取りアクセスするクラスです。
	@note	アーカイブファイル内のファイル名文字コードは UTF-16。
			ファイルマップに展開するときは環境依存の TCHAR に変換される。
*/
class Archive
	: public RefObject
{
public:
	static const int Version_001 = 001;
	static const byte_t InternalKey[16];

public:
	Archive();
	virtual ~Archive();

public:

	/**
		@brief      アーカイブファイルを開いてアクセスの準備をします。
		@param[in]	filePath	: アーカイブファイルの絶対パス
		@param[in]	key			: パスワード文字列
		@code
				// アーカイブファイル内に「Chara.png」「Map/Ground.png」というファイルがある場合…

				// 例 1 )
				// 実行ファイルと同じフォルダにあるアーカイブファイル "Image.lna" を使う
				archive.initialize( "Image.lna" );

				file.open( "Image/Chara.png" );
				file.open( "Image/Map/Ground.png" );

				// 例 2 )
				// 実行ファイルと同じフォルダにある「Data」フォルダ内のアーカイブファイル "Image.lna" を使う
				archive.initialize( "Data/Image.lna" );

				file.open( "Data/Image/Chara.png" );
				file.open( "Data/Image/Map/Ground.png" );
		@endcode
	*/
	void Open(const PathName& filePath, const String* key);

	/**
		@brief	アーカイブ内に指定したパスのファイルが存在するか確認します。
	*/
	bool ExistsFile(const PathName& fileFullPath);

	/**
		@brief	アーカイブ内のファイルを読み取るためのストリームを作成します。
	*/
	ArchiveStream* CreateStream(const PathName& fileFullPath);

private:

	// 数値を 16 にそろえるために加算する数値「( 16 - ( v_ % 16 ) ) % 16」の最適化 ( 5 は 11、27 は 5 等 )
    int _padding16( int v ) const { return ( v != 0 ) ? ( 16 - ( v & 0x0000000f ) ) & 0x0000000f : 16; }
	uint32_t _padding16(uint32_t v) const { return (v != 0) ? (16 - (v & 0x0000000f)) & 0x0000000f : 16; }

	/// 一時バッファのサイズチェック
	void _checkTempBuffer(uint32_t request_size);

	/// パディングを考慮して整数を読み込む
	uint32_t _readU32Padding16();

	/// パディングを考慮して整数を読み込む (ファイル名長さ、ファイルサイズ用)
	void _readU32Padding16(uint32_t* v0, uint32_t* v1);

	/// パディングを考慮してデータを読み込む
	int _readPadding16( void* buffer, int count );

private:

	/// ファイルひとつ分の情報
	struct Entry
	{
		uint32_t		mOffset;		///< ストリーム先頭からファイルの位置までのオフセット
		uint32_t		mSize;			///< ファイルサイズ
	};

	typedef std::map<std::wstring, Entry>	EntriesMap;
	typedef std::pair<std::wstring, Entry>	EntriesPair;

private:

	// camellia key table type.
	static const int L_CAMELLIA_TABLE_BYTE_LEN = 272;
	static const int L_CAMELLIA_TABLE_WORD_LEN = (L_CAMELLIA_TABLE_BYTE_LEN / 4);
	typedef unsigned int KEY_TABLE_TYPE[L_CAMELLIA_TABLE_WORD_LEN];

    static const int KEY_SIZE  = 128;

    std::wstring		mArchiveDirectory;  ///< アーカイブファイルをディレクトリに見立てた時の、そこまのパス ( [.lnaの親フルパス]/[拡張子を除いたアーカイブファイル名]/ )
	EntriesMap	        mEntriesMap;	    ///< ファイル名に対応するファイル情報を格納する map
	FILE*		        mStream;		    ///< アーカイブファイルのストリーム
	int			        mFileNum;		    ///< アーカイブファイル内のファイル数
    Base::RefTString    mKey;			    ///< 復号キー (char)
	KEY_TABLE_TYPE      mKeyTable/*[256]*/;
    byte_t*             mTempBuffer;
	uint32_t				mTempBufferSize;
    Threading::Mutex	mLock;
};

/**
	@brief	アーカイブからファイルを読むためのストリームクラスです。
*/
class ArchiveStream
    : public Stream
{
private:
	ArchiveStream(Archive* archive, FILE* stream, uint32_t data_offset, uint32_t data_size);
	virtual ~ArchiveStream();

public:

	/// ファイル ( データ ) サイズの取得
    virtual int getSize() { return mDataSize; }

	/// ファイルポインタの位置の取得
	virtual int getPosition() { return mSeekPoint; }

	/// データの読み込み
	virtual int read( void* buffer, int buffer_size, int read_size = -1 );

	/// ファイルポインタの設定
	virtual void seek( int offset, int origin = SEEK_SET );

private:

    Archive*        mArchive;           ///< このクラスを作成したアーカイブクラス
    FILE*           mStream;            ///< ファイルストリーム
	uint32_t		mDataOffset;        ///< ファイルの先頭からデータの先頭位置までのオフセット
	uint32_t		mDataSize;          ///< データサイズ
	uint32_t		mSeekPoint;         ///< シーク位置

    friend class Archive;
};

} // namespace Lumino
