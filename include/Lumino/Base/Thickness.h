
#pragma once

namespace Lumino
{

/**
	@brief		�l�p�`�̘g�̑�����\���N���X�ł��B(���������_�`��)
*/
class ThicknessF
{
public:
	static const ThicknessF	Zero;	///< ThicknessF(0, 0, 0, 0)

public:
	float		Left;		/**< �l�p�`�̍��ӂ̕� */
	float		Top;		/**< �l�p�`�̏�ӂ̕� */
	float		Right;		/**< �l�p�`�̉E�ӂ̕� */
	float		Bottom;		/**< �l�p�`�̒�ӂ̕� */

public:

	/**
		@brief	���ׂĂ̗v�f�� 0 �ŏ��������܂��B
	*/
	ThicknessF() { Set(0, 0, 0, 0); }
	
	/**
		@brief	���E���ʂ̕��y�я㉺���ʂ̕����w�肵�ď��������܂��B
	*/
	ThicknessF(float leftAndRight, float topAndbottom) { Set(leftAndRight, topAndbottom, leftAndRight, topAndbottom); }
	
	/**
		@brief	�e�ӂ̕����w�肵�ď��������܂��B
	*/
	ThicknessF(float left, float top, float right, float bottom) { Set(left, top, right, bottom); }

public:
	
	/**
		@brief	�e�v�f��ݒ肵�܂��B
	*/
	void Set(float left, float top, float right, float bottom) { Left = left; Top = top; Right = right; Bottom = bottom; }

public:
	bool operator == (const ThicknessF& obj) const { return (Left == obj.Left && Top == obj.Top && Right == obj.Right && Bottom == obj.Bottom); }
	bool operator != (const ThicknessF& obj) const { return !operator==(obj); }
};


} // namespace Lumino
