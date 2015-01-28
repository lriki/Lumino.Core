#pragma once

namespace Lumino
{

enum EncodingType
{
	EncodingType_Unknown = 0,	///< �s���ȕ����R�[�h (���ʎ��s)
	EncodingType_UTF8,
	EncodingType_UTF16,
	EncodingType_UTF16B,
	EncodingType_UTF32,
	EncodingType_UTF32B,
};

class IEncodingChecker
{
public:

	virtual ~IEncodingChecker() {}


};

class EncodingDetector
{
public:
	typedef unsigned char Byte;

public:
	EncodingType Detect(const void* bytes, size_t bytesSize);

private:
	EncodingType CheckUTFBom();

private:
	const Byte*		m_buffer;
	size_t			m_bufferSize;
};



}

