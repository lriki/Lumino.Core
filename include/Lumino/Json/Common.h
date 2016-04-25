#pragma once

LN_NAMESPACE_BEGIN

enum class JsonToken
{
	None = 0,

	/** オブジェクト定義の開始 ('{') */
	StartObject,

	/** オブジェクト定義の終了 ('}') */
	EndObject,

	/** 配列定義の開始 ('[') */
	StartArray,

	/** 配列定義の終了 (']') */
	EndArray,

	/** オブジェクトのプロパティの名前 */
	PropertyName,

	/** 数値型 (double) */
	Double,

	/** "null" 値 */
	Null,

	/** 真偽値 ("true" または "false"。GetValue() で値を確認すること) */
	Boolean,

	/** 文字列値 */
	String,
};

LN_NAMESPACE_END
