#pragma once

LN_NAMESPACE_BEGIN

enum class JsonToken
{
	None = 0,

	/** �I�u�W�F�N�g��`�̊J�n ('{') */
	StartObject,

	/** �I�u�W�F�N�g��`�̏I�� ('}') */
	EndObject,

	/** �z���`�̊J�n ('[') */
	StartArray,

	/** �z���`�̏I�� (']') */
	EndArray,

	/** �I�u�W�F�N�g�̃v���p�e�B�̖��O */
	PropertyName,

	/** ���l�^ (double) */
	Double,

	/** "null" �l */
	Null,

	/** �^�U�l ("true" �܂��� "false"�BGetValue() �Œl���m�F���邱��) */
	Boolean,

	/** ������l */
	String,
};

LN_NAMESPACE_END
