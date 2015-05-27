
/*

ドキュメントメモ：
	・エンティティ参照は認識しますが解決はしません。XmlNodeType_EntityReference



XmlNodeType_EntityReference のサンプル
http://www.atmarkit.co.jp/fxml/tecs/007entity/07.html


&lt; とかのエンティティは解決してからハンドラに通知しなければ意味が無いので
StringRef 使うことは無いかも。

*/

#include "../Internal.h"
#include <Lumino/Base/StringUtils.h>
#include <Lumino/IO/StringReader.h>
#include <Lumino/Xml/XmlReader.h>

namespace Lumino
{

//=============================================================================
// XmlReader
//=============================================================================

struct Entity
{
	const TCHAR* Pattern;
	int Length;
	TCHAR Value;
};

static const int ReservedEntitiesCount = 5;
static const Entity ReservedEntities[ReservedEntitiesCount] =
{
	{ _T("quot"),	4, '\"' },
	{ _T("amp"),	3, '&' },
	{ _T("apos"),	4, '\'' },
	{ _T("lt"),		2, '<' },
	{ _T("gt"),		2, '>' },
};

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
XmlReader::XmlReader(const String& str)
	: m_reader(LN_NEW StringReader(str))
	, m_currentNodesPos(0)
	, m_line(1)
	, m_col(1)
	, m_stockElementCount(0)
{
	m_nodes.Reserve(32);
	m_textCache.Reserve(1024);
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
XmlReader::XmlReader(TextReader* textReader)
	: m_reader(textReader, true)
	, m_currentNodesPos(0)
	, m_line(1)
	, m_col(1)
	, m_stockElementCount(0)
{
	m_nodes.Reserve(32);
	m_textCache.Reserve(1024);
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
XmlReader::~XmlReader()
{
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
bool XmlReader::Read()
{
	m_textCache.Clear();
	m_nodes.Clear();
	m_currentNodesPos = 0;

	int ch = m_reader->Peek();
	if (ch < 0) { return false; }	// もう読み取れる文字が無い
	if (ch == '<') {
		if (!ParseElementInner()) { return false; }
	}
	else  {
		if (!ParseElementOuter()) { return false; }
	}

	m_currentNodesPos = 0;
	m_stockElementCount--;

	return true;
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
XmlNodeType XmlReader::GetNodeType()  const
{
	if (m_nodes.IsEmpty()) {
		return XmlNodeType_None;
	}
	return m_nodes[m_currentNodesPos].Type;
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
const String& XmlReader::GetName()
{
	if (m_nodes.IsEmpty()) {
		m_tmpName.SetEmpty();
	}
	else
	{
		const TCHAR* name = &m_textCache[m_nodes[m_currentNodesPos].NameStartPos];
		m_tmpName.AssignCStr(name, m_nodes[m_currentNodesPos].NameLen);
	}
	return m_tmpName;
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
const String& XmlReader::GetValue()
{
	if (m_nodes.IsEmpty()) {
		m_tmpName.SetEmpty();
	}
	else
	{
		NodeData& data = m_nodes[m_currentNodesPos];
		if (data.ValueStartPos == -1 || data.ValueLen == 0) {
			m_tmpName.SetEmpty();	// Node はあるけど値が無かった
		}
		else
		{
			const TCHAR* name = &m_textCache[data.ValueStartPos];
			m_tmpName.AssignCStr(name, data.ValueLen);
		}
	}
	return m_tmpName;
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
bool XmlReader::IsEmptyElement() const
{
	if (m_nodes.IsEmpty()) {
		return false;
	}
	return m_nodes[m_currentNodesPos].IsEmptyElement;
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
bool XmlReader::ParseElementInner()
{
	m_reader->Read();	// skip '<'

	// プレフィックス的なもののチェック
	bool isProcInst = false;
	bool isDocContent = false;
	bool isElementEnd = false;
	bool isComment = false;
	bool isDataStart = false;
	if (m_reader->Peek() == '?')
	{
		isProcInst = true;
		m_reader->Read();	// skip
	}
	else if (m_reader->Peek() == '!')
	{
		isDocContent = true;
		m_reader->Read();	// skip
	}
	else if (m_reader->Peek() == '/')
	{
		isElementEnd = true;
		m_reader->Read();	// skip
	}
	if (isDocContent)
	{
		if (m_reader->Peek() == '-')
		{
			isComment = true;
			m_reader->Read();	// skip
			m_reader->Read();	// skip
		}
		else if (m_reader->Peek() == '[')
		{
			isDataStart = true;
			m_reader->Read();	// skip
		}
	}

	// コメントだった。専用の解析にまわす
	if (isComment) {
		return ParseComment();
	}

	// 要素名 (各要素の種類を区別するため、まず読み取る必要がある)
	int namePos = 0;
	int nameLen = 0;
	if (!ParseName(&namePos, &nameLen)) { return false; }
	if (nameLen == 0) {
		// Error: 要素名が見つからなかった (< との間に空白を入れることはできない)
		m_errorInfo.AddError(ParseError_ElementNameNotFount, m_line, m_col);
		return false;
	}


	// doctypedecl	::=   	'<!DOCTYPE' S Name ( S ExternalID )? S? ( '[' intSubset ']' S? )? '>'
	// GEDecl		::=   	'<!ENTITY' S Name S EntityDef S? '>'
	// PEDecl		::=   	'<!ENTITY' S '%' S Name S PEDef S? '>'
	// NotationDecl	::=   	'<!NOTATION' S Name S ( ExternalID | PublicID ) S? '>'
	// XMLDecl		::=   	'<?xml' VersionInfo EncodingDecl? SDDecl? S? '?>'
	// PI			::=   	'<?' PITarget ( S ( Char* - ( Char* '?>' Char* ) ) )? '?>'		※ PITarget は "xml" 以外
	// includeSect	::=		'<![' S ? 'INCLUDE' S ? '[' extSubsetDecl ']]>'
	// ignoreSect	::=   	'<![' S? 'IGNORE' S? '[' ignoreSectContents* ']]>'

	//const TCHAR* name = &m_textCache[namePos];

	// 要素の解析
	if (!ParseElement(namePos, nameLen, isElementEnd)) { return false; }




	
	// </Grid > のように、後ろに空白はあってもよい。前はダメ。

	return true;
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
bool XmlReader::ParseElementOuter()
{
	enum EntityRefSeq
	{
		Seq_FindAmp = 0,
		Seq_FindSemiColon,
	};
	EntityRefSeq entityRefSeq = Seq_FindAmp;
	int entityRefStart = 0;

	bool tokenIsSpaceOnly = true;
	int tokenStart = 0;

	for (;;)
	{
		int ch = m_reader->Peek();	// 現在位置の文字を読む
		if (ch < 0) { break; }		// EOF
		if (ch == '<') { break; }	// 次のタグの開始かもしれない。終了
		m_reader->Read();			// 1文字進める
		m_textCache.Add((TCHAR)ch);

		// Entity 参照の開始を探している状態 (普通の状態)
		if (entityRefSeq == Seq_FindAmp)
		{
			// Entity 参照の開始が見つかった
			if (ch == '&')
			{
				// ';' を探しに行く
				entityRefSeq = Seq_FindSemiColon;
				entityRefStart = m_textCache.GetCount() - 1;
			}
			else
			{
				// 空白以外の文字が見つかったかチェック
				if (!IsWhiteSpace(ch)) {
					tokenIsSpaceOnly = false;
				}
			}
		}
		// Enitty 参照の終端を探している
		else if (entityRefSeq == Seq_FindSemiColon)
		{
			// Entity 参照の終端が見つかった
			if (ch == ';')
			{
				if (IsReservedEntity(&m_textCache[entityRefStart], m_textCache.GetCount() - entityRefStart)) {
					// &amp; 等、予約済み Entity は XmlNodeType_EntityReference にしない。Text として結合するため、ここではなにもしない。
				}
				else
				{
					// & の前までを1つの NodeData としてストックしておく
					NodeData data1;
					data1.Type = (tokenIsSpaceOnly) ? XmlNodeType_Whitespace : XmlNodeType_Text;
					data1.ValueStartPos = tokenStart;
					data1.ValueLen = entityRefStart - tokenStart;
					m_nodes.Add(data1);

					// &～; までを1つの NodeData としてストックしておく
					NodeData data2;
					data2.Type = XmlNodeType_EntityReference;
					data2.ValueStartPos = entityRefStart;
					data2.ValueLen = m_textCache.GetCount() - entityRefStart;
					m_nodes.Add(data2);
				}

				// & を探している状態にする
				entityRefSeq = Seq_FindAmp;
				tokenStart = m_textCache.GetCount();	// ; の次を指す
				tokenIsSpaceOnly = false;
			}
		}
	}

	if (entityRefSeq != Seq_FindAmp) {
		// TODO: Error: テキスト領域が終わったのに Entity 参照を探している
	}

	if (tokenStart != m_textCache.GetCount())
	{
		// テキストを NodeData としてストックしておく
		NodeData data1;
		data1.Type = (tokenIsSpaceOnly) ? XmlNodeType_Whitespace : XmlNodeType_Text;
		data1.ValueStartPos = tokenStart;
		data1.ValueLen = m_textCache.GetCount() - tokenStart;
		m_nodes.Add(data1);
		++m_stockElementCount;
	}
	else {
		// ここにくるのは "...&book;" のように Entity 参照が終端で終わっているとき。
		// この場合は Text ノードを作る必要は無い。
	}

	return true;
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
bool XmlReader::ParseComment()
{
	// m_reader は "<--" の次の文字を指している
	int hyphenCount = 0;	// 連続する '-' の数
	int start = m_textCache.GetCount();

	for (;;)
	{
		int ch = m_reader->Read();
		if (ch < 0) { break; }	// EOF

		// - が連続している
		if (hyphenCount >= 2)
		{
			if (ch == '>') {
				break;	// コメントが正常に閉じた
			}
			else {
				// Error: コメント内部に連続する -- が見つかった
				m_errorInfo.AddError(ParseError_CommentDoubleHyphen, m_line, m_col);
				return false;
			}
		}

		// 連続する '-' の数をカウント
		if (ch == '-') {
			++hyphenCount;
		}
		else
		{
			if (hyphenCount > 0)
			{
				// コメント終端ではなかった。カウントしていた - をテキスト扱いに戻す
				hyphenCount = 0;
				m_textCache.Add(_T('-'));	// 正常であれば 1 つしかないはず (コメント内で連続する -- は禁止されている)
			}

			m_textCache.Add(ch);
		}
	}

	// コメントを NodeData としてストックしておく
	NodeData data1;
	data1.Type = XmlNodeType_Comment;
	data1.ValueStartPos = start;
	data1.ValueLen = m_textCache.GetCount() - start;
	m_nodes.Add(data1);
	++m_stockElementCount;

	return true;
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
bool XmlReader::ParseName(int* startPos, int* length)
{
	LN_ASSERT(startPos != NULL);
	LN_ASSERT(length != NULL);

	*startPos = m_textCache.GetCount();
	*length = 0;
	int count = 0;
	for (;;)
	{
		int ch = m_reader->Peek();
		if (ch < 0) { break; }	// EOF

		if (IsAlphaNum(ch) ||
			ch == '_' ||
			ch == ':' ||
			(ch == '-' && count > 0) ||
			(ch == '.'&& count > 0))
		{
			++count;
			m_textCache.Add(ch);
			m_reader->Read();
		}
		else {
			break;
		}
	}

	*length = count;
	return true;
}

//-----------------------------------------------------------------------------
// STag			::=   	'<' Name ( S Attribute )* S? '>'
//-----------------------------------------------------------------------------
bool XmlReader::ParseElement(int nameStart, int nameLength, bool isEnd)
{
	// m_reader は要素名の次の空白を指している

	// NodeData 化してストック
	NodeData data;
	data.Type = (isEnd) ? XmlNodeType_EndElement : XmlNodeType_Element;
	data.NameStartPos = nameStart;
	data.NameLen = nameLength;
	m_nodes.Add(data);
	int dataIdx = m_nodes.GetCount() - 1;
	++m_stockElementCount;

	for (;;)
	{
		// 空白*
		SkipWhitespace();

		// タグ終端確認
		if (m_reader->Peek() == '/')
		{
			m_reader->Read();
			if (m_reader->Read() == '>') {
				m_nodes[dataIdx].IsEmptyElement = true;
				break;
			}
			else {
				// Error: 正常なタグ終端ではない
				m_errorInfo.AddError(ParseError_ElementInvalidEmptyTagEnd, m_line, m_col);
				return false;
			}
		}
		else if (m_reader->Peek() == '>') {
			m_reader->Read();
			break;
		}

		// 属性
		if (!ParseAttribute()) { return false; }
		m_nodes[dataIdx].AttrCount++;
	}

	return true;
}

//-----------------------------------------------------------------------------
// Attribute	::=   	Name Eq AttValue
//-----------------------------------------------------------------------------
bool XmlReader::ParseAttribute()
{
	// m_reader は属性名の先頭を指している

	// 属性名
	int nameStart, nameLength;
	if (!ParseName(&nameStart, &nameLength)) { return false; }

	// 空白*
	SkipWhitespace();

	// =
	if (m_reader->Peek() != '=') {
		// Error: = が見つからなかった
		m_errorInfo.AddError(ParseError_CommentDoubleHyphen, m_line, m_col);
		return false;
	}
	m_reader->Read();

	// 空白*
	SkipWhitespace();

	// "
	if (m_reader->Peek() != '"') {
		// Error: " が見つからなかった
		m_errorInfo.AddError(ParseError_AttributeQuoteNotFount, m_line, m_col);
		return false;
	}
	m_reader->Read();

	// 文字列
	int valueStart = m_textCache.GetCount();
	int ch = 0;
	for (;;)
	{
		int ch = m_reader->Read();
		if (ch < 0) { break; }		// EOF
		if (ch == '"') { break; }	// 文字列終端
		m_textCache.Add(ch);
	}

	// "
	if (ch != '"') {
		// Error: " が見つからなかった
		m_errorInfo.AddError(ParseError_AttributeQuoteNotFount, m_line, m_col);
		return false;
	}

	// NodeData 化してストック
	NodeData data;
	data.Type = XmlNodeType_Attribute;
	data.NameStartPos = nameStart;
	data.NameLen = nameLength;
	data.ValueStartPos = valueStart;
	data.ValueLen = m_textCache.GetCount() - valueStart;
	m_nodes.Add(data);
	++m_stockElementCount;
	return true;
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
//bool XmlReader::ParseSkipElement()
//{
//	// m_reader は "DOCTYPE" 等の識別文字列+空白の次の文字を指している
//
//	int leftAngle = 1;
//	int rightAngle = 0;
//}

#if 0
//-----------------------------------------------------------------------------
// <!DOCTYPE...>
//-----------------------------------------------------------------------------
bool XmlReader::ParseDocumentType()
{
	// m_reader は "DOCTYPE" の次の文字を指している

	enum DocTypeSec
	{
		DocTypeSec_FindLeftBracket = 0,	// [ を探している
		DocTypeSec_FindRightBracket,	// ] を探している
		DocTypeSec_Finished,			// 終端待ち
	};
	DocTypeSec sec = DocTypeSec_FindLeftBracket;

	int start = -1;
	for (;;)
	{
		int ch = m_reader->Read();
		if (ch < 0) { break; }	// EOF

		// [ を探している
		if (sec == DocTypeSec_FindLeftBracket)
		{
			if (ch == '[')
			{
				start = m_textCache.GetCount() + 1;	// ここから Value を開始する
				sec = DocTypeSec_FindRightBracket;
			}
			else if (ch == '>') {
				break;	// 要素終端
			}
		}
		// ] を探している
		else if (sec == DocTypeSec_FindRightBracket)
		{
			if (ch == ']') {
				sec = DocTypeSec_Finished;
			}
			else {
				m_textCache.Add(ch);	// 値として文字を取得しておく
			}
		}
		else {
			if (ch == '>') {
				break;	// 要素終端
			}
		}
	}

	// NodeData 化してストックしておく
	NodeData data1;
	data1.Type = XmlNodeType_Comment;
	data1.ValueStartPos = start;
	data1.ValueLen = m_textCache.GetCount() - start;
	m_nodes.Add(data1);
	++m_stockElementCount;
	return true;
}
#endif

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
bool XmlReader::IsTextChar(int ch)
{
	return true;//!IsWhiteSpace(ch);	// TODO: ちゃんと制御文字とかくべつする
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
bool XmlReader::IsReservedEntity(const TCHAR* text, int len)
{
	if (len < 2 ||			// & と ; と識別文字の最低 3 文字以上はあるはず
		text[0] != '&' ||
		text[len - 1] != ';') {
		return false;
	}
	text++;
	len -= 2;

	for (int i = 0; i < ReservedEntitiesCount; ++i)
	{
		if (ReservedEntities[i].Length == len &&
			StringUtils::StrNCmp(ReservedEntities[i].Pattern, text, len) == 0)
		{
			return true;
		}
	}
	return false;
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
bool XmlReader::IsWhiteSpace(int ch)
{
	// S	   :: = (#x20 | #x9 | #xD | #xA) +
	return (ch == 0x20 || ch == 0x09 || ch == 0x0D || ch == 0x0A);
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
bool XmlReader::SkipWhitespace()
{
	while (IsWhiteSpace(m_reader->Peek())) {
		m_reader->Read();
	}
	return !m_reader->IsEOF();
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
bool XmlReader::ParseWhiteSpace()
{
	return false;
}


//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
bool XmlReader::IsAlphaNum(int ch)
{
	return (ch < 128) ? (isalnum(ch)!=0) : true;
}

} // namespace Lumino
