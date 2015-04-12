//==============================================================================
// Archive 
//------------------------------------------------------------------------------
/*	[13/10/13] ファイル名マップのキーは UTF-16 統一？環境に合わせる？
		速度的な懸念。環境に合わせた方が高速な気がするが…。

		wchar_t のみ使用してくださいっていうなら、確かに高速。
		でも、Ruby とか Lua とか使うときはそんなことできない。
		SJIS or UTF-8 → UTF-16 or UTF-32 の変換が必ず必要。
		でも、この変換は「(UTF-32への変換でも)一度UTF-16を経由する」とかいう必要はなく、
		速度的にはそんなに変わらない。
		
		UTF-16か32かは UnicodeString 内に完全に隠されているので、
		あとは各文字コードから32への変換関数を実装すれば特に問題はない。
		(それが面倒なのだが…)
		
		あと、メモリ使用量が単に倍になるって言う点も心配。
		(棒人間では、約100ファイル、平均20文字(ASCII) → UTF-32 だと 6000kバイトくらい無駄)
		ビットマップに比べたら全然マシだけど…。

		→とりあえず完全にUnicodeString 任せで。
*/
//==============================================================================

#include "stdafx.h"
#include <string>
#include <algorithm>
#include <camellia/camellia.h>
//#include <Dependencies/ConvertUTF/ConvertUTF.h>
#include "../Base/Misc.h"
#include "../Base/StringUtils.h"
#include "../Base/Unicode.h"
#include "../System/Environment.h"
#include "Common.h"
#include "FileUtil.h"
#include "StreamObject.h"
#include "Archive.h"

namespace LNote
{
namespace Core
{
namespace FileIO
{

//==============================================================================
// ■ Archive
//==============================================================================
	
	const lnByte Archive::InternalKey[16] = {
		0x6e, 0x36, 0x38, 0x64, 0x35, 0x6f, 0x68, 0x6d,
		0x33, 0x42, 0x69, 0x61, 0x34, 0x78, 0x37, 0x6c
	};

	//----------------------------------------------------------------------
	//
	//----------------------------------------------------------------------
	Archive::Archive()
		: mStream	        ( NULL )
		, mFileNum	        ( 0 )
        , mTempBuffer       ( NULL )
        , mTempBufferSize   ( 0 )
	{
	}

	//----------------------------------------------------------------------
	//
	//----------------------------------------------------------------------
	Archive::~Archive()
	{
		if ( mStream )
		{
			fclose( mStream );
			mStream = NULL;

            mEntriesMap.clear();
		}
        SAFE_DELETE_ARRAY( mTempBuffer );
	}

	//----------------------------------------------------------------------
	//
	//----------------------------------------------------------------------
	void Archive::initialize( const lnChar* directory, const lnChar* filename, const char* key )
	{
		mKey = ( key ) ? key : "";
		mFileNum = 0;

		// 拡張子を除いた部分をコピーする
#ifdef LNOTE_UNICODE
		mArchiveDirectory  = FileIO::Path::getExtName( directory_ );
        mArchiveDirectory += L"\\";
		mArchiveDirectory += FileIO::Path::getExtName( filename_ );
        mArchiveDirectory += L"\\";
        FileIO::Path::normalizePath( &mArchiveDirectory );
#else
		Base::UnicodeString uniDirPath;
		Base::UnicodeString uniFilePath;
		uniDirPath.setDecodeBytes( directory );
		uniFilePath.setDecodeBytes( filename );
		mArchiveDirectory  = uniDirPath.getString();
        mArchiveDirectory += L"\\";
		mArchiveDirectory += FileIO::Path::getExtName( uniFilePath.getString() );
        mArchiveDirectory += L"\\";
        FileIO::Path::normalizePath( &mArchiveDirectory );
#endif
		// ディレクトリパスが相対パスであれば、カレントディレクトリを使って絶対パス化する
		if (!Path::isFullPath(mArchiveDirectory.c_str()))
		{
			lnChar curDir[LN_MAX_PATH];
			System::Environment::getCurrentDirectory( curDir );

			lnRefStringW wpath = curDir;
			wpath += L"\\";
			wpath += mArchiveDirectory.c_str();

			wchar_t newDir[LN_MAX_PATH];
			Path::canonicalizePath( wpath.c_str(), newDir );

			mArchiveDirectory = newDir;
			mArchiveDirectory += L"\\";
			FileIO::Path::normalizePath( &mArchiveDirectory );
		}

        // 実際のファイル名を作る
		lnString real_name = directory;
        real_name += _T("/");
        real_name += filename;

        // 拡張キーの作成
        if ( key )
        {
            lnByte key_buf[ KEY_SIZE ] = { 0 };
            memcpy( key_buf, key, strlen( key ) );
            memset( mKeyTable, 0, sizeof( mKeyTable ) );
            Camellia_Ekeygen( KEY_SIZE, key_buf, mKeyTable );
        }

        // とりあえず一時バッファを確保しておく
        _checkTempBuffer( 5000 );

		// アーカイブファイルを開く
        mStream = _tfopen( real_name.c_str(), _T("rb") );
		LN_THROW_FileNotFound( mStream, real_name.c_str() );

		// ファイルサイズ取得
		size_t archive_size = FileIO::FileUtils::getFileSize(mStream);

        // 終端から16バイト戻ってからそれを読むとファイル数
		fseek( mStream, -16, SEEK_END );
		mFileNum = _readU32Padding16();

		// ヘッダ読み込み
		fseek( mStream, 0, SEEK_SET );
        ArchiveHeader header;
        fread( &header, 1, sizeof( header ), mStream );

        // マジックナンバーをチェック
        if ( memcmp( header.ID, "lna ", 4 ) != 0 )
		{
			fclose( mStream );
			mStream = NULL;
			LN_THROW_InvalidFormat(0);
		}

		// 内部キーのチェック (ユーザーキーは本当に正しいか？)
		lnByte internalKey[16];
		_readPadding16( internalKey, 16 );
		if ( memcmp( internalKey, Archive::InternalKey, 16 ) != 0 ) {
			LN_THROW_InvalidOperation(0, "invalid archive key: %s", mKey.c_str());
		}

        // ファイル情報を取得していく
		lnU32 name_len;
		wchar_t* name_buf;
		Entry entry;
		for ( int i = 0; i < mFileNum; ++i )
		{
			// ファイル名の長さとファイルサイズを読む
            _readU32Padding16( &name_len, &entry.mSize );
			
#if 0
			name_buf = (wchar_t*)LN_NEW lnU32[name_len + 1];
			UTF16* tmpBuf = LN_NEW UTF16[name_len + 1];
			_readPadding16( tmpBuf, name_len * sizeof(UTF16) );

			wprintf((wchar_t*)tmpBuf);

			ConversionResult r = ConvertUTF16toUTF32(
				(const UTF16**)&tmpBuf,
				(const UTF16*)&tmpBuf[name_len + 1], 
				(UTF32**)&name_buf, 
				(UTF32*)&name_buf[name_len + 1], 
				strictConversion,
				NULL ); 
			if (r != conversionOK) {
				LN_THROW_FileFormat( 0 );
			}
			delete[] tmpBuf;
#endif

#if defined(LNOTE_WCHAR_16)
			// ファイル名を読み込むバッファを確保して読みこみ
			name_buf = LN_NEW wchar_t[name_len + 1];
			_readPadding16( name_buf, name_len * sizeof(wchar_t) );
			name_buf[ name_len ] = L'\0';
#elif defined(LNOTE_WCHAR_32)
			name_buf = LN_NEW wchar_t[name_len + 1];
			/*UTF16**/ tmpBuf = LN_NEW UTF16[name_len + 1];
			_readPadding16( tmpBuf, name_len * sizeof(UTF16) );

			/*ConversionResult*/ r = ConvertUTF16toUTF32(
				(const UTF16**)&tmpBuf,
				(const UTF16*)&tmpBuf[name_len + 1], 
				(UTF32**)&name_buf, 
				(UTF32*)&name_buf[name_len + 1], 
				strictConversion,
				NULL ); 
			if (r != conversionOK) {
				LN_THROW_FileFormat( 0 );
			}
			delete[] tmpBuf;
#endif
            // ここでのファイルポインタがデータ本体の位置
			entry.mOffset = ftell( mStream );

			// 今のシステム用に正規化してからファイルマップに追加
			std::wstring keyPath = name_buf;
			Path::normalizePath( &keyPath );
			mEntriesMap.insert( EntriesPair( keyPath, entry ) );

			// ファイルポインタをデータサイズ分進める
			//fseek( mStream, entry.mSize + ( 16 - ( entry.mSize % 16 ) ) % 16, SEEK_CUR );
            lnU32 ofs = _padding16( entry.mSize );
            fseek( mStream, entry.mSize + ofs, SEEK_CUR );
            
			SAFE_DELETE_ARRAY( name_buf );
		}

		//int cur_fpos = ftell(mStream);
		//_p(cur_fpos);
	}


	//----------------------------------------------------------------------
	//
	//----------------------------------------------------------------------
	void Archive::open(const wchar_t* filename_, FILE** out_stream_, int* data_offset_, int* data_size_, FileType type)
	{
		*out_stream_ = NULL;
		*data_offset_ = 0;
		*data_size_ = 0;

		if ( mFileNum == 0 )
		{
			return;
		}
		else
		{
			std::wstring path = filename_;
			FileIO::Path::normalizePath( &path );

			EntriesMap::iterator it;
			it = mEntriesMap.find( path );
			if ( it != mEntriesMap.end() )
			{
				*out_stream_ = mStream;
				Entry entry = it->second;
				*data_offset_ = entry.mOffset;
				*data_size_ = entry.mSize;
			}
		}
	}

	//----------------------------------------------------------------------
	//
	//----------------------------------------------------------------------
	bool Archive::existsFile( const wchar_t* fileFullPath )
	{
		// アーカイブのディレクトリと同じ
		if ( wcsncmp( fileFullPath, mArchiveDirectory.c_str(), mArchiveDirectory.size() ) == 0 )
		{
			int copyLen = wcslen( fileFullPath ) - mArchiveDirectory.size();

			// ファイル名が空文字ではない
			if ( copyLen > 0 )
			{
				// アーカイブのあるディレクトリ名部分を取り除く
				//std::wstring internalPath = std::wstring( 
				//	fileFullPath, 
				//	mArchiveDirectory.size(), 
				//	copyLen );
				const wchar_t* internalPath = (fileFullPath + mArchiveDirectory.size());

				// 検索
				EntriesMap::iterator it = mEntriesMap.find( internalPath );
				if ( it != mEntriesMap.end() )
				{
					return true;
				}
			}
		}
		return false;
	}

	//----------------------------------------------------------------------
	//
	//----------------------------------------------------------------------
	Stream* Archive::createStream(const wchar_t* fileFullPath, FileType type)
    {
		// アーカイブのディレクトリと同じ？
		LN_THROW_FileNotFound( 
			wcsncmp( fileFullPath, mArchiveDirectory.c_str(), mArchiveDirectory.size() ) == 0,
			fileFullPath );

		int copyLen = wcslen( fileFullPath ) - mArchiveDirectory.size();

		// ファイル名が空文字
		LN_THROW_FileNotFound( copyLen > 0, fileFullPath );	

		// アーカイブのあるディレクトリ名部分を取り除く
		//std::wstring internalPath = std::wstring( 
		//	fileFullPath, 
		//	mArchiveDirectory.size(), 
		//	copyLen );
		const wchar_t* internalPath = (fileFullPath + mArchiveDirectory.size());

        FILE* out_stream;
        int data_offset;
        int data_size;
		open( internalPath, &out_stream, &data_offset, &data_size, type );
        if ( !out_stream ) return NULL;

        return LN_NEW ArchiveInStream( this, out_stream, data_offset, data_size, type );
    }

	//----------------------------------------------------------------------
	//
	//----------------------------------------------------------------------
	lnU32 Archive::read(void* buffer, lnU32 count, FILE* stream, FileType type)
	{
		Threading::ScopedLock lock( mLock );

		lnU32 read_size = 0;
        int cur_fpos = ftell( mStream );            // 例)位置が 20 の場合
		//_p(cur_fpos);
        int over_fpos = ( cur_fpos % 16 );          // 4
        int read_start_pos = cur_fpos - over_fpos;  // 16
            
        // 16 バイト単位になるように戻す
        fseek( mStream, read_start_pos, SEEK_SET ); // seek 16

        int over_count = ( count % 16 );           // count_ が 30 の場合、14
        int r_size = count + over_fpos;            // count_ が 30 の場合、34
        r_size += _padding16( r_size );//( 16 - ( r_size % 16 ) ) % 16;    // 48 バイト読む

        // 必要な領域を確保
        _checkTempBuffer( r_size );

        read_size = fread( mTempBuffer, 1, r_size, mStream );

		//_p(cur_fpos);
		//printf((char*)mTempBuffer);

        int tmpbuf_pos = 0;     // mTempBuffer から読むバイト位置
        lnU32 total = 0;          // mTempBuffer から読んだバイト数の累計 (buffer_ に転送しようとしたバイト数)
        int re_count = count;  // あと残り buffer_ に書き込めるバイト数
        lnByte tmp_buf[ 16 ];
        lnByte* write_pos = static_cast< lnByte* >( buffer );

        // 前方に余分に読んだものがある場合
        if ( over_fpos > 0 )
        {
			Camellia_EncryptBlock(KEY_SIZE, &mTempBuffer[total], mKeyTable, tmp_buf);
            tmpbuf_pos += 16;

            int data_size = 16 - over_fpos; // 余分を除いたデータサイズ
            memcpy( write_pos, &tmp_buf[ over_fpos ], std::min( re_count, data_size ) );   // min は count_ 以上書きこまないため
            write_pos = write_pos + data_size;
                
            total += data_size;
            re_count -= std::min( re_count, data_size );
        }

        while ( true )
        {
            // 今回の読み込みで count_ に到達する場合は終了
            // count_ が 16 丁度の場合はもう一周
            if ( total + 16 >= count )
            {
                break;
            }

			Camellia_DecryptBlock(KEY_SIZE, &mTempBuffer[tmpbuf_pos], mKeyTable, write_pos);
            tmpbuf_pos += 16;
            write_pos += 16;
            total += 16;
            re_count -= 16;
        }

        // 後方に余分に読んだものがある場合
        if ( over_count > 0 )
        {
            //_p(tmpbuf_pos);
            //_p(total);
            //_p(count_);
            //_p(over_count);
			Camellia_DecryptBlock(KEY_SIZE, &mTempBuffer[tmpbuf_pos], mKeyTable, tmp_buf);
            memcpy( write_pos, tmp_buf, std::min( re_count, over_count ) );

            //total += 16;
        }
        else
        {
			Camellia_DecryptBlock(KEY_SIZE, &mTempBuffer[tmpbuf_pos], mKeyTable, tmp_buf);
            memcpy( write_pos, tmp_buf, re_count );
        }



            

        // 呼び出し側での不具合を避けるため、正常に読めている場合は count_ を読んだバイト数とする
        if ( read_size > count )
        {
            read_size = count;

            // ファイルポインタを正しい位置に移動
            fseek( mStream, cur_fpos + count, SEEK_SET );
        }



        //-------------------------------------------------
        // テキストモードの場合は改行コードを LF に変換
		if (type == FileType_Text)
        {
			lnByte* tbuf = (lnByte*)buffer;
			lnU32 readPoint = 0;
			lnU32 writePoint = 0;
			lnU32 loopCount = read_size;
			for ( ; readPoint < loopCount; )
			{
				if ( tbuf[readPoint] == 0x0d ) {	
					// CR+LF → LF
					if ( readPoint < loopCount - 1 && tbuf[readPoint + 1] == 0x0a ) {	
						tbuf[writePoint] = 0x0a;
						++readPoint;
						--read_size;
					}
					// CR → LF
					else {
						tbuf[writePoint] = 0x0a;
					}
				}
				else
				{
					tbuf[writePoint] = tbuf[readPoint];
				}
				readPoint++;
				writePoint++;
			}
        }

		return read_size;
	}

	//----------------------------------------------------------------------
	//
	//----------------------------------------------------------------------
    void Archive::_checkTempBuffer( lnU32 request_size )
    {
        if ( request_size > mTempBufferSize )
        {
            mTempBufferSize = request_size;
            SAFE_DELETE_ARRAY( mTempBuffer );
            mTempBuffer = LN_NEW lnByte[ mTempBufferSize ];
        }
    }

	//----------------------------------------------------------------------
	//
	//----------------------------------------------------------------------
	lnU32 Archive::_readU32Padding16()
    {
        lnByte b[ 16 ] = { 0 };

        // 復号する場合
        if ( mKey.size() > 0 )
        {
            lnByte buf[ 16 ];
            fread( buf, 1, 16, mStream );
			Camellia_DecryptBlock(KEY_SIZE, buf, mKeyTable, b);
        }
        // そのまま読み込む場合
        else
        {
            fread( b, 1, 16, mStream );
        }

        if ( Base::isLittleEndian() )
        {
            return *((lnU32*)b);
        }
        else
        {
            lnU32 r = *((lnU32*)b);
            r = (((r) >> 24) +
                (((r) & 0x00ff0000) >> 8) +
                (((r) & 0x0000ff00) << 8) +
                ((r) << 24));
            return r;
        }
    }

	//----------------------------------------------------------------------
	// 
	//----------------------------------------------------------------------
	void Archive::_readU32Padding16( lnU32* v0, lnU32* v1 )
    {
        lnByte b[ 16 ] = { 0 };

        // 復号する場合
        if ( mKey.size() > 0 )
        {
            lnByte buf[ 16 ];
            fread( buf, 1, 16, mStream );
			Camellia_DecryptBlock(KEY_SIZE, buf, mKeyTable, b);
        }
        // そのまま読み込む場合
        else
        {
            fread( b, 1, 16, mStream );
        }

        // 復号する場合
        if ( 0 )
        {
        }

        if ( Base::isLittleEndian() )
        {
            *v0 = *((lnU32*)(b + 0));
            *v1 = *((lnU32*)(b + 4));
        }
        else
        {
            lnU32 r = *((lnU32*)b);
            *v0 =  (((r) >> 24) +
                    (((r) & 0x00ff0000) >> 8) +
                    (((r) & 0x0000ff00) << 8) +
                    ((r) << 24));

            r = *((lnU32*)(b + 4));
            *v1 =  (((r) >> 24) +
                    (((r) & 0x00ff0000) >> 8) +
                    (((r) & 0x0000ff00) << 8) +
                    ((r) << 24));
        }
    }

	//----------------------------------------------------------------------
	//
	//----------------------------------------------------------------------
	int Archive::_readPadding16( void* buffer, int count )
    {
        int read_size;

        // 復号する場合
        if ( mKey.size() > 0 )
        {
            int count_p16 = _padding16( count );

            // 全体のサイズを計算して、バッファ確保
            int all_size = count + count_p16;//( 16 - ( count_ % 16 ) ) % 16;
            _checkTempBuffer( all_size );

            // 一時バッファに読む
            read_size = fread( mTempBuffer, 1, all_size, mStream );

            int over_size = count_p16;//( 16 - ( count_ % 16 ) ) % 16;
            int i = 0;
            int total = 0;
            
            lnByte* buf = static_cast< lnByte* >( buffer );
            while ( true )
            {
                if ( total + 16 >= count )
                {
                    lnByte read_data[ 16 ];
					Camellia_DecryptBlock(KEY_SIZE, &mTempBuffer[total], mKeyTable, read_data);
                    memcpy( &buf[ i * 16 ], read_data, 16 - over_size );
                    break;
                }
                else
                {
					Camellia_DecryptBlock(KEY_SIZE, &mTempBuffer[total], mKeyTable, &buf[total]);
                }
                ++i;
                total += 16;
            }

            // 仮
            //memcpy( buffer_, mTempBuffer, count_ );
        }
        // 復号しない場合は buffer_ に直接読んで、余分を進める
        else
        {
            read_size = fread( buffer, 1, count, mStream );
            fseek( mStream, _padding16( count ), SEEK_CUR );
            
            //fseek( mStream, ( 16 - ( count_ % 16 ) ) % 16, SEEK_CUR );
            
        }
        return read_size;
    }

//==============================================================================
// ■ ArchiveInStream
//==============================================================================


	//----------------------------------------------------------------------
	//
	//----------------------------------------------------------------------
	ArchiveInStream::ArchiveInStream(Archive* archive, FILE* stream, lnU32 data_offset, lnU32 data_size, FileType type)
        : mArchive          ( archive )
        , mStream           ( stream )
        , mDataOffset       ( data_offset )
        , mDataSize         ( data_size )
        , mSeekPoint        ( 0 )
        , mOpenType( type )
    {
        LN_SAFE_ADDREF( mArchive );
    }

	//----------------------------------------------------------------------
	//
	//----------------------------------------------------------------------
    ArchiveInStream::~ArchiveInStream()
    {
        LN_SAFE_RELEASE( mArchive );
    }

	//----------------------------------------------------------------------
	//
	//----------------------------------------------------------------------
    int ArchiveInStream::read( void* buffer, int buffer_size, int read_size )
    {
        if ( read_size < 0 )
		{
			read_size = buffer_size;
		}

        // 読み込み開始位置に移動
		long read_point = static_cast< long >( mDataOffset ) + mSeekPoint;
		int offset = fseek(mStream, read_point, SEEK_SET);
		LN_ERR2_ASSERT_S(offset == 0);

        // 復号しながら読み込む
		int validSize = mArchive->read( buffer, read_size, mStream, mOpenType );

        // 読んだ分だけファイルポインタを移動
		// ※テキスト形式の場合は、fread() が返すバイト数とシーク位置が異なるときがある。(CR+LF変換)
		//   そのため、validSize ではなく read_size でポインタを進めておく。
		//   例えばこう
		//   [ FileIO::File::getFileSize(fp) = 162 ]
		//   [ fread( buf, 1, FileIO::File::getFileSize(fp), fp ) = 157 ]
		//   [ ftell(fp) = 162 ]
		mSeekPoint += read_size;
		if ( mSeekPoint > mDataSize )
			mSeekPoint = mDataSize;

        return validSize;
    }

	//----------------------------------------------------------------------
	//
	//----------------------------------------------------------------------
    void ArchiveInStream::seek( int offset, int origin )
    {
        switch ( origin )
		{
			case SEEK_CUR:
				mSeekPoint += offset;
				break;

			case SEEK_END:
				mSeekPoint = mDataSize + offset;
				break;

			default :
				mSeekPoint = offset;
				break;
		}

        LN_ERR2_ASSERT_S( mSeekPoint >= 0 );
        LN_ERR2_ASSERT_S( mSeekPoint <= mDataSize );
    }


//==============================================================================
// ■ DummyArchive
//==============================================================================

	//----------------------------------------------------------------------
	//
	//----------------------------------------------------------------------
	bool DummyArchive::existsFile( const wchar_t* fileFullPath )
	{
		return FileUtils::exists(fileFullPath);
	}

	//----------------------------------------------------------------------
	//
	//----------------------------------------------------------------------
	Stream* DummyArchive::createStream(const wchar_t* fileFullPath, FileType type)
	{
		LRefPtr<InFileStream> file( LN_NEW InFileStream() );
		file->open( fileFullPath, type );
		file->addRef();
		return file.getPtr();
	}



} // namespace FileIO
} // namespace Core
} // namespace LNote

//==============================================================================
//
//==============================================================================
