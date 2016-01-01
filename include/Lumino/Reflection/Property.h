
#pragma once
#include "../Base/Common.h"
#include "../Base/RefObject.h"
#include "../Base/String.h"
#include "PropertyMetadata.h"

LN_NAMESPACE_BEGIN
namespace tr
{
class ReflectionObject;
class TypeInfo;
class PropertyMetadata;

// TODO; Internal
class PropertyInstanceData
	: public RefObject
{
public:
	Property*			InheritanceKey;		// プロパティを親から継承するとき、this またはこの値をキーとして検索する。
	ReflectionObject*	InheritanceParent;	// ↑ので見つかった親あるいは祖先オブジェクト
	const Property*		InheritanceTarget;	// ↑のオブジェクトのどのプロパティから受け継ぐか (走査の結果継承元が見つからなかった場合、この PropertyInstanceData が表す Propery を指す。このとき InheritanceParent は NULL)
	uint16_t			RevisionCount;
	uint16_t			PathRevisionCount;
	bool				IsDefault;

	PropertyInstanceData()
		: InheritanceTarget(nullptr)
		, InheritanceParent(nullptr)
		, RevisionCount(0)
		, PathRevisionCount(0)
		, IsDefault(true)
	{}
};

/**
	@brief		ReflectionObject のサブクラスが実装できるプロパティを表します。
	@details	プロパティは、メンバ変数に対する簡易的なリフレクションを提供します。		
*/
class Property
{
public:
	Property(TypeInfo* ownerClassType, PropertyMetadata* metadata);


	virtual void SetValue(ReflectionObject* target, Variant value) const { LN_THROW(0, InvalidOperationException); }
	virtual Variant GetValue(const ReflectionObject* target) const { LN_THROW(0, InvalidOperationException); }


};




#define LN_TR_PROPERTY(valueType, propVar) \
	public:  static const ln::tr::Property*	propVar##; \
	private: static void  set_##propVar(CoreObject* obj, valueType value); \
	private: static valueType get_##propVar(const CoreObject* obj); \
	private: static void  changed_##propVar(CoreObject* obj, PropertyChangedEventArgs* e); \
	private: static TypedPropertyInitializer<valueType> init_##propVar; \
	private: static PropertyMetadata metadata_##propVar; \
	private: static std::shared_ptr<PropertyInstanceData>* getInstanceData_##propVar(CoreObject* obj); \
	private: std::shared_ptr<PropertyInstanceData> instanceData_##propVar = NULL;
	// ↑TODO: unique_ptr の方がメモリ使わないかも？

/*
	LN_TR_PROPERTY(String, TextProperty);

		以下のように展開される。

	public:
		static const ln::tr::Property*	TextProperty;

	private:
		// 指定したオブジェクトに値を設定する
		static void	 set_TextProperty(CoreObject* obj, String value);

		// 指定したオブジェクトの値を取得する
		static String get_TextProperty(const CoreObject* obj);

		// 指定したオブジェクトに対して、PropertyMetadata に登録されている PropertyChangedCallback を呼び出す
		static void changed_TextProperty(CoreObject* obj, PropertyChangedEventArgs* e);

		// Property オブジェクトの初期化を行う
		static PropertyInitializer<valueType> init_TextProperty;

		// プロパティのメタ情報定義
		static PropertyMetadata metadata_TextProperty;

		// オブジェクトの持つ PropertyInstanceData のポインタへの参照を取得する (PropertyInstanceData は本当に必要になった時に new される)
		static std::shared_ptr<PropertyInstanceData>* getInstanceData_##propVar(CoreObject* obj);

		// PropertyInstanceData のポインタ
		private: std::shared_ptr<PropertyInstanceData> instanceData_TextProperty = NULL;
*/

} // namespace tr
LN_NAMESPACE_END


