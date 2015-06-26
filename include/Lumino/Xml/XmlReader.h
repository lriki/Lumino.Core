
#pragma once
#include "../IO/TextReader.h"
#include "../IO/MemoryStream.h"
#include "XmlError.h"

namespace Lumino
{

/// ノードの種類
enum XmlNodeType
{
	/// Read メソッドが呼び出されなかった場合に、XmlReader によって返されます。
	XmlNodeType_None = 0,

	/// 要素
	XmlNodeType_Element,

	/// 要素の終了タグ
	XmlNodeType_EndElement,

	/// 属性
	XmlNodeType_Attribute,

	/// コメント (例 : <!-- comment -->)
	XmlNodeType_Comment,

	/// マークアップ間の空白
	XmlNodeType_Whitespace,

	/// ノードのテキストの内容。
	XmlNodeType_Text,

	/// CDATA セクション (例 : <![CDATA[テキスト]]>)。
	XmlNodeType_CDATA,

	/// エンティティへの参照 (例 : &book;)
	XmlNodeType_EntityReference,

	/// ドキュメント型宣言 (例 : <!DOCTYPE...>)
	XmlNodeType_DocumentType,

	/// エンティティ宣言 (例 : <!ENTITY...>)
	XmlNodeType_Entity,

	/// 記法宣言 (例 : <!NOTATION...>)
	XmlNodeType_Notation,

	/// 処理命令 (例 : <?pi test?>)
	XmlNodeType_ProcessingInstruction,

	/// XML 宣言 (例 : <?xml version='1.0'?>)
	XmlNodeType_XmlDeclaration,

};

/**
	@brief		SAX スタイルの XML パーサです。
	@details	「Extensible Markup Language (XML) 1.1 (第二版)」をベースに実装されています。
				http://www.eplusx.net/translation/W3C/REC-xml11-20060816/#NT-elementdecl <br>
				ただし、現在全ての仕様には対応されていません。
				対応している仕様は、上記リンク先の目次「2 文書」、「4.3.1 テキスト宣言」および「4.6 定義済み実体」の内容です。
				それ以外はノードの種別のみ認識します。<br>
				例えば <!DOCTYPE...> はノード種別として認識しますが、内容の読み取りはスキップします。
*/
class XmlReader
{
public:
	XmlReader(const String& str);
	XmlReader(TextReader* textReader);
	~XmlReader();

	/**
		@brief		次のノード読み取ります。
		@return		正常に読み取られた場合は true。それ以上読み取るノードが存在しない場合は false。
	*/
	bool Read();

	/**
		@brief		現在のノードの種類を取得します。
	*/
	XmlNodeType GetNodeType() const;

	/**
		@brief		現在のノードの名前を取得します。
	*/
	const String& GetName();

	/**
		@brief		現在のノードの値 (文字列形式) を取得します。
		@details	値が無い場合は空文字列を返します。
	*/
	const String& GetValue();

	/**
		@brief		現在のノードが空要素(<book/> 等) かどうかを確認します。
	*/
	bool IsEmptyElement() const;

	/**
		@brief		現在のノードの属性数を取得します。
	*/
	int GetAttributeCount() const;

	/**
		@brief		現在のノードの最初の属性に移動します。
		@return		属性が存在する場合は true。それ以外の場合は false。
		@details	属性が存在しない場合、現在位置は変更されません。
	*/
	bool MoveToFirstAttribute();

	/**
		@brief		次の属性に移動します。
		@return		次の属性が存在する場合は true。それ以上属性が存在しない場合は false。
		@details	次の属性が存在しない場合、現在位置は変更されません。
	*/
	bool MoveToNextAttribute();

	/**
		@brief		現在の属性ノードを含む要素に移動します。
		@return		現在位置が属性であった場合は true。それ以外の場合は false。
		@details	現在位置が属性でない場合、現在位置は変更されません。
	*/
	bool MoveToElement();

private:
	bool ParseElementInner();	// '<' から始まる
	bool ParseElementOuter();	// '<' 以外から始まる

	bool ParseComment();

	bool ParseName(int* startPos, int* length);

	bool ParseElement(int nameStart, int nameLength, bool isEnd);
	bool ParseAttribute();

	bool ParseSkipElement();
	//bool ParseDocumentType();



	bool IsTextChar(int ch);


	bool IsWhiteSpace(int ch);
	bool SkipWhitespace();

	bool IsReservedEntity(const TCHAR* text, int len);	// & と ; は含まないこと

	bool ParseWhiteSpace();

	bool IsAlphaNum(int ch);

	void ExpandReservedEntities(TCHAR* text, int len);

private:
	struct NodeData
	{
		XmlNodeType	Type;
		int			NameStartPos;
		int			NameLen;
		int			ValueStartPos;	///< m_textCache 上のバイトインデックス。-1 の場合は値が存在しない
		int			ValueLen;		///< m_textCache 上のバイト数
		int			AttrCount;		///< Element の場合に持つ子 Attribute の数
		bool		IsSubData;		///< Attribute 等、他の要素の子であるか
		bool		IsEmptyElement;	///< <a />　のような空要素であるか 

		NodeData()
			: Type(XmlNodeType_None)
			, NameStartPos(-1)
			, NameLen(0)
			, ValueStartPos(-1)
			, ValueLen(0)
			, AttrCount(0)
			, IsSubData(false)
			, IsEmptyElement(false)
		{}
	};

	RefPtr<TextReader>		m_reader;
	Array<TCHAR>			m_textCache;
	Array<NodeData>			m_nodes;
	NodeData*				m_currentNode;
	int						m_currentElementNodePos;
	int						m_currentAttrIndex;		///< 属性ノードを指している場合、その属性インデックス。指していなければ -1
	int						m_line;
	int						m_col;
	XmlError				m_errorInfo;
	int						m_stockElementCount;
	String					m_tmpName;
	int						m_currentAttrCount;		///< 現在のノードの属性数
};

} // namespace Lumino
