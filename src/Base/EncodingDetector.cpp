
#include <stdio.h>
#include <string.h>
#include "EncodingDetector.h"

namespace Lumino
{

//=============================================================================
// Encoding
//=============================================================================
//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
EncodingType EncodingDetector::Detect(const void* bytes, size_t bytesSize)
{
	if (bytes == NULL || bytesSize == 0) {
		return EncodingType_Unknown;
	}
	m_buffer = (const Byte*)bytes;
	m_bufferSize = bytesSize;

	// UTF �n�� BOM �`�F�b�N
	EncodingType type = CheckUTFBom();
	if (type != EncodingType_Unknown) {
		return type;	// �� UTF�n�m��
	}

	return EncodingType_Unknown;	// ���ʎ��s
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
EncodingType EncodingDetector::CheckUTFBom()
{
	/* UTF8		: 0xEF, 0xBB, 0xBF
	 * UTF16	: 0xFF, 0xFE
	 * UTF16Big	: 0xFE, 0xFF
	 * UTF32	: 0xFF, 0xFE, 0x00, 0x00
	 * UTF32Big	: 0x00, 0x00, 0xFE, 0xFF
	 */

	struct BomData
	{
		EncodingType	Type;
		Byte			Bom[4];
		size_t			Length;
	};
	BomData data[] = 
	{
		{ EncodingType_UTF8,	{ 0xEF, 0xBB, 0xBF, 0x00 }, 3 },
		{ EncodingType_UTF32,	{ 0xFF, 0xFE, 0x00, 0x00 }, 4 },	// UTF16Little �Ɠ����Ȃ̂Ő�ɔ��ʂ��Ȃ��ƃ_��
		{ EncodingType_UTF32B,	{ 0x00, 0x00, 0xFE, 0xFF }, 4 },
		{ EncodingType_UTF16,	{ 0xFF, 0xFE, 0x00, 0x00 }, 2 },
		{ EncodingType_UTF16B,	{ 0xFE, 0xFF, 0x00, 0x00 }, 2 },
	};
	for (int i = 0; i < 5; i++)
	{
		if (data[i].Length <= m_bufferSize &&
			memcmp(m_buffer, data[i].Bom, data[i].Length) == 0)
		{
			return data[i].Type;
		}
	}

	return EncodingType_Unknown;	// BOM ����������
}

}

