
#pragma once

namespace Lumino
{

/**
	@brief		2������̃I�u�W�F�N�g�T�C�Y��\���N���X
*/
class Size
{
public:

	int		Width;		///< X �����̑傫��
	int		Height;		///< Y �����̑傫��

public:

	/// �f�t�H���g�R���X�g���N�^
	Size() { Set(0, 0); }

	/// �R���X�g���N�^
	Size(int w, int h) { Set(w, h); }

	/// �e�v�f��ݒ肵�܂��B
	void Set(int w, int h) { Width = w; Height = h; }

	/// �v�f�����ׂ� 0 ���𔻒肵�܂��B
	bool IsZero() const { return (Width == 0 && Height == 0); }

	/// �����ꂩ�̗v�f�� 0 ���𔻒肵�܂��B
	bool IsAnyZero() const { return (Width == 0 || Height == 0); }

public:

	bool operator == (const Size& obj) const { return (Width == obj.Width && Height == obj.Height); }
	bool operator != (const Size& obj) const { return (Width != obj.Width || Height != obj.Height); }
};

} // namespace Lumino
