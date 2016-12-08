#pragma once

LN_NAMESPACE_BEGIN
namespace tr {

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

	/** ���l�^ (Int32) */
	Int32,

	/** ���l�^ (Int64) */
	Int64,

	/** ���l�^ (double) */
	Double,

	/** "null" �l */
	Null,

	/** �^�U�l ("true" �܂��� "false"�BGetValue() �Œl���m�F���邱��) */
	Boolean,

	/** ������l */
	String,
};

enum class JsonParseResult
{
	Success = 0,
	Error = 1,
};

} // namespace tr
LN_NAMESPACE_END
