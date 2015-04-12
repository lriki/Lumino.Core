#pragma once

namespace Lumino
{

#pragma pack(1)
struct ArchiveHeader
{
	char	ID[4];			///< 'l' 'n' 'a' ' '
	uint8_t	Version;		///< �t�@�C���o�[�W����
	byte_t	Padding[11];	///< �\���̃T�C�Y�� 16 �̔{���ɂȂ�悤�ɂ��邽�߂̃p�f�B���O
};
#pragma pack()

} // namespace Lumino
