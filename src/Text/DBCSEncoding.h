
#pragma once
#include "../../include/Lumino/Text/Encoding.h"

namespace Lumino
{

/**
	@brief	�ő� 2byte(DoubleByte) �̃}���`�o�C�g�����R�[�h�̃G���R�[�f�B���O
*/
class DBCSEncoding : public Encoding
{
public:
	static const int LeadBytePairsSize = 12;
	static const int TableSize = 65535;

	struct TableInfo
	{
		const unsigned char*	LeadBytePairs;		// size = LeadBytePairsSize (NULL �̏ꍇ�A���̕����R�[�h�� DB �ł͂Ȃ�)
		const unsigned short*	DBCSToUTF16Table;	// size = TableSize
		const unsigned short*	UTF16ToDBCSTable;	// size = TableSize
	};

	static const TableInfo Tables[EncodingType_Max];
	
public:
	DBCSEncoding(EncodingType type);
	virtual ~DBCSEncoding() {};

public:
	// override Encoding
	virtual int GetMinByteCount() const { return 1; }
	virtual int GetMaxByteCount() const { return 2; }
	virtual Decoder* CreateDecoder() const { return LN_NEW DBCSDecoder(&Tables[m_encodingType]); }
	virtual Encoder* CreateEncoder() const { return LN_NEW DBCSEncoder(&Tables[m_encodingType]); }
	virtual byte_t* GetPreamble() const { return NULL; }

private:
	EncodingType	m_encodingType;

private:
	// Decoder
	class DBCSDecoder : public Decoder
	{
	public:
		DBCSDecoder(const TableInfo* tableInfo) { m_tableInfo = tableInfo;  Reset(); }
		virtual int GetMinByteCount() { return 1; }
		virtual int GetMaxByteCount() { return 2; }
		virtual bool CanRemain() { return true; }
		virtual void ConvertToUTF16(const byte_t* inBuffer, size_t inBufferByteCount, UTF16* outBuffer, size_t outBufferCharCount, size_t* outBytesUsed, size_t* outCharsUsed);
		virtual int UsedDefaultCharCount() { return m_usedDefaultCharCount; }
		virtual bool Completed() { return m_lastLeadByte == 0; }
		virtual void Reset() { m_usedDefaultCharCount = 0; m_lastLeadByte = 0; }

	private:
		const TableInfo*	m_tableInfo;
		byte_t				m_lastLeadByte;			///< �O��� Convert �Ŗ����Ɍ���������s�o�C�g��ۑ�����
		int					m_usedDefaultCharCount;	///< ��A�� ConvertToUTF16() �̌Ăяo���̒��ŁA�ϊ��ł��Ȃ��������K�蕶���ɕϊ�����������
	};

	// Encoder
	class DBCSEncoder : public Encoder
	{
	public:
		DBCSEncoder(const TableInfo* tableInfo) { m_tableInfo = tableInfo; Reset(); }
		virtual int GetMinByteCount() { return 1; }
		virtual int GetMaxByteCount() { return 2; }
		virtual bool CanRemain() { return true; }
		virtual void ConvertFromUTF16(const UTF16* inBuffer, size_t inBufferCharCount, byte_t* outBuffer, size_t outBufferByteCount, size_t* outBytesUsed, size_t* outCharsUsed);
		virtual int UsedDefaultCharCount() { return m_usedDefaultCharCount; }
		virtual bool Completed() { return true; }
		virtual void Reset() { m_usedDefaultCharCount = 0; }

	private:
		const TableInfo*	m_tableInfo;
		int					m_usedDefaultCharCount;	///< ��A�� ConvertFromUTF16() �̌Ăяo���̒��ŁA�ϊ��ł��Ȃ��������K�蕶���ɕϊ�����������
	};
};

} // namespace Lumino
