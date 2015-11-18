
#include "../Base/Common.h"
#include "../Base/RefObject.h"
#include "../Base/String.h"

LN_NAMESPACE_BEGIN
namespace tr
{
class ReflectionObject;
class Property;
typedef uint64_t LocalValueHavingFlags;

/**
	@brief		
*/
class TypeInfo
{
public:
	typedef uint64_t* (*HasLocalValueFlagsGetter)(ReflectionObject* _this);
	typedef uint8_t RevisionCount;
	static const int MaxProperties = sizeof(uint64_t) * 8;

public:
	TypeInfo(const TCHAR* className, TypeInfo* baseClass, HasLocalValueFlagsGetter getter);

	void RegisterProperty(Property* prop);
	Property* FindProperty(const String& name) const;

	//// childObjProp が継承できるプロパティをこの TypeInfo から探す。見つからなければ NULL を返す。
	//// childObj : childObjProp を持つオブジェクト
	//Property* FindInheritanceProperty(const Property* childObjProp, CoreObject* childObj) const;

	//void RegisterRoutedEvent(RoutedEvent* ev);
	//RoutedEvent* FindRoutedEvent(const String& name) const;	// TODO: いらないかも
	//void InvokeRoutedEvent(CoreObject* owner, const RoutedEvent* ev, RoutedEventArgs* e);

	///// RoutedEventHandler は、ユーザーが動的に追加できるハンドラよりも前に呼び出される。
	///// WPF では「静的ハンドラ」と呼ばれている。動的イベントに登録するのに比べ、メモリを使用しない。
	//void RegisterRoutedEventHandler(const RoutedEvent* ev, RoutedEventHandler* handler);
	//RoutedEventHandler* FindRoutedEventHandler(const RoutedEvent* ev) const;

	//TypeInfo* GetBaseClass() const { return m_baseClass; }
	//uint32_t* GetHasLocalValueFlags(CoreObject* obj) { return m_hasLocalValueFlagsGetter(obj); }

	///// ベースクラスも含めた全てのプロパティを列挙する
	//static void ForEachAllProperty(const TypeInfo* typeInfo, const std::function<void(Property*)>& callback);


	bool operator == (const TypeInfo& info) const { return m_name == info.m_name; }
	bool operator < (const TypeInfo& info) const { return m_name < info.m_name; }

protected:

private:
	//typedef SortedArray<const RoutedEvent*, RoutedEventHandler*>	RoutedEventHandlerList;

	String						m_name;						// クラス名
	TypeInfo*					m_baseClass;				// 継承元クラスを示す TypeInfo
	Array<Property*>			m_propertyList;				// この型のクラスがもつプロパティのリスト
	HasLocalValueFlagsGetter	m_hasLocalValueFlagsGetter;	// プロパティがローカル値を保持しているかを示すビットフラグを取得するコールバック。ローカル値を持たない場合はプロパティの値を親から継承する。

	//Array<RoutedEvent*>		m_routedEventList;
	//RoutedEventHandlerList	m_routedEventHandlerList;
	//
};

} // namespace tr
LN_NAMESPACE_END
