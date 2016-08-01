
#ifndef LUMINO_MATH_PLANE_H
#define LUMINO_MATH_PLANE_H

#include "Common.h"
#include "Vector3.h"

LN_NAMESPACE_BEGIN

/**
	@brief		���ʂ��`���܂��B
*/
class LUMINO_EXPORT Plane
{
public:

	Vector3	Normal;		///< Plane �̖@�� �x�N�g��
	float	D;			///< ���_����̖@���ɉ����� Plane �̋���

public:

	/**
		@brief		�e�v�f�� 0.0 ��ݒ肵�ăC���X�^���X�����������܂��B
	*/
	Plane();

	/**
		@brief		�e�v�f���w�肵�ăC���X�^���X�����������܂��B
		@param[in]	a	: �@���� X ����
		@param[in]	b	: �@���� Y ����
		@param[in]	c	: �@���� Z ����
		@param[in]	d	: ���_����̖@���ɉ����� Plane �̋���
	*/
	Plane(float a, float b, float c, float d);

	Plane(const Vector3& normal, float d);
	
	/**
		@brief		���ʏ��1�_�Ɩ@�����w�肵�ăC���X�^���X�����������܂��B
		@param[in]	point	: ���ʏ�̓_
		@param[in]	normal	: �@�� (���K���ς݂ł��邱��)
	*/
	Plane(const Vector3& point, const Vector3& normal);

	/**
		@brief		���ʏ��3�_���w�肵�ăC���X�^���X�����������܂��B
		@param[in]	point1	: ���ʏ�̓_1
		@param[in]	point2	: ���ʏ�̓_2
		@param[in]	point3	: ���ʏ�̓_3
	*/
	Plane(const Vector3& point1, const Vector3& point2, const Vector3& point3);

public:

	/**
		@brief		���̕��ʂ𐳋K�����܂��B
		@details	�ʖ@�� Normal �𐳋K�����A����Ɏg�p�����W���� D �ɂ��K�p���܂��B
	*/
	void Normalize();

	/**
		@brief		�_�����̕��ʂ̗����ɂ��邩�𔻒肵�܂��B�_���ʏ�ɂ���ꍇ�͕\���Ɣ��肵�܂��B
		@param[in]	point	: ���肷��_�̍��W
		@return		true �̏ꍇ�A�����ɂ���
		@details	�ʂ͖@�����L�тĂ�������\���ł��B
	*/
	bool CheckInside(const Vector3& point) const;

	/**
		@brief		�_�����̕��ʂ̗����ɂ��邩�𔻒肵�܂��B�_���ʏ�ɂ���ꍇ�������Ɣ��肵�܂��B
		@param[in]	point	: ���肷��_�̍��W
		@return		true �̏ꍇ�A�����ɂ���
	*/
	bool CheckInsideLower(const Vector3& point) const;

	/**
		@brief		�������̕��ʂ̗����ɂ��邩�𔻒肵�܂��B
		@param[in]	point	: ���̒��S���W
		@param[in]	radius	: ���̔��a
		@return		true �̏ꍇ�A�����ɂ��� (�ڐG���Ă���ꍇ�� true)
		@details	�ʂ͖@�����L�тĂ�������\���ł��B
	*/
	bool CheckInside(const Vector3& point, float radius) const;

	/**
		@brief		���̕��ʂƐ������������邩�𔻒肵�܂��B
		@param[in]	start	: �����̎n�_
		@param[in]	end		: �����̏I�_
		@param[in]	point	: ��_�̍��W���i�[����x�N�g���̃|�C���^
		@return		��������ꍇ�� true�A���s�ȏꍇ�� false
	*/
	bool Intersects(const Vector3& start, const Vector3& end, Vector3* point) const;

	/**
		@brief		�w�肳�ꂽ�s����g�p���Ă��̕��ʂ����W�ϊ����܂��B
		@param[in]	mat		: �����̊�ɂȂ�s��
		@return		�ϊ����ꂽ����
	*/
	void Transform(const Matrix& mat);

	/**
		@brief		�f�o�b�O�p�ɕ������W���o�͂��܂��B
		@param[in]	format	: �����w�蕶����
		@param[in]	stream	: �o�͐�X�g���[��
		@details	format �� NULL �̏ꍇ�A������ "%f, %f, %f, %f\n" ���g�p���܂��B
	*/
	void Print(const char* format = NULL, FILE* stream = NULL) const;

public:

	/**
		@brief		���K���������ʂ�Ԃ��܂��B
		@param[in]	plane	: �����̊�ɂȂ镽��
		@return		���K�����ꂽ����
		@details	�ʖ@�� Normal �𐳋K�����A����Ɏg�p�����W���� D �ɂ��K�p���܂��B
	*/
	static Plane Normalize(const Plane& plane);

	/**
		@brief		���ʂ� 4D �x�N�g���̓��ς��v�Z���܂��B
		@param[in]	plane	: �����̊�ɂȂ镽��
		@param[in]	vec		: �����̊�ɂȂ�x�N�g��
	*/
	static float Dot(const Plane& plane, const Vector4& vec);

	/**
		@brief		���ʂ� 3D �x�N�g���̓��ς��v�Z���܂��B(�x�N�g���� W �� 1.0 �Ƃ��Čv�Z����)
		@param[in]	plane	: �����̊�ɂȂ镽��
		@param[in]	vec		: �����̊�ɂȂ�x�N�g��
		@note		�߂�l�����̒l�ł���ꍇ�A�_�͕��ʂ̕\���ɂȂ�܂��B
	*/
	static float DotCoord(const Plane& plane, const Vector3& vec);

	/**
		@brief		���ʂ� 3D �x�N�g���̓��ς��v�Z���܂��B(�x�N�g���� W �� 0.0 �Ƃ��Čv�Z����)
		@param[in]	plane	: �����̊�ɂȂ镽��
		@param[in]	vec		: �����̊�ɂȂ�x�N�g��
	*/
	static float DotNormal(const Plane& plane, const Vector3& vec);

	/**
		@brief		�w�肳�ꂽ�s����g�p���ĕ��ʂ����W�ϊ����܂��B
		@param[in]	plane	: �����̊�ɂȂ镽��
		@param[in]	mat		: �����̊�ɂȂ�s��
		@return		�ϊ����ꂽ����
	*/
	static Plane Transform(const Plane& plane, const Matrix& mat);

};

LN_NAMESPACE_END

#include "Plane.inl"

#endif // LUMINO_MATH_PLANE_H
