
/*

�h�L�������g�����F
	�E�G���e�B�e�B�Q�Ƃ͔F�����܂��������͂��܂���BXmlNodeType_EntityReference



XmlNodeType_EntityReference �̃T���v��
http://www.atmarkit.co.jp/fxml/tecs/007entity/07.html


&lt; �Ƃ��̃G���e�B�e�B�͉������Ă���n���h���ɒʒm���Ȃ���ΈӖ��������̂�
StringRef �g�����Ƃ͖��������B

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
	if (ch < 0) { return false; }	// �����ǂݎ��镶��������
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
			m_tmpName.SetEmpty();	// Node �͂��邯�ǒl����������
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

	// �v���t�B�b�N�X�I�Ȃ��̂̃`�F�b�N
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

	// �R�����g�������B��p�̉�͂ɂ܂킷
	if (isComment) {
		return ParseComment();
	}

	// �v�f�� (�e�v�f�̎�ނ���ʂ��邽�߁A�܂��ǂݎ��K�v������)
	int namePos = 0;
	int nameLen = 0;
	if (!ParseName(&namePos, &nameLen)) { return false; }
	if (nameLen == 0) {
		// Error: �v�f����������Ȃ����� (< �Ƃ̊Ԃɋ󔒂����邱�Ƃ͂ł��Ȃ�)
		m_errorInfo.AddError(ParseError_ElementNameNotFount, m_line, m_col);
		return false;
	}


	// doctypedecl	::=   	'<!DOCTYPE' S Name ( S ExternalID )? S? ( '[' intSubset ']' S? )? '>'
	// GEDecl		::=   	'<!ENTITY' S Name S EntityDef S? '>'
	// PEDecl		::=   	'<!ENTITY' S '%' S Name S PEDef S? '>'
	// NotationDecl	::=   	'<!NOTATION' S Name S ( ExternalID | PublicID ) S? '>'
	// XMLDecl		::=   	'<?xml' VersionInfo EncodingDecl? SDDecl? S? '?>'
	// PI			::=   	'<?' PITarget ( S ( Char* - ( Char* '?>' Char* ) ) )? '?>'		�� PITarget �� "xml" �ȊO
	// includeSect	::=		'<![' S ? 'INCLUDE' S ? '[' extSubsetDecl ']]>'
	// ignoreSect	::=   	'<![' S? 'IGNORE' S? '[' ignoreSectContents* ']]>'

	//const TCHAR* name = &m_textCache[namePos];

	// �v�f�̉��
	if (!ParseElement(namePos, nameLen, isElementEnd)) { return false; }




	
	// </Grid > �̂悤�ɁA���ɋ󔒂͂����Ă��悢�B�O�̓_���B

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
		int ch = m_reader->Peek();	// ���݈ʒu�̕�����ǂ�
		if (ch < 0) { break; }		// EOF
		if (ch == '<') { break; }	// ���̃^�O�̊J�n��������Ȃ��B�I��
		m_reader->Read();			// 1�����i�߂�
		m_textCache.Add((TCHAR)ch);

		// Entity �Q�Ƃ̊J�n��T���Ă����� (���ʂ̏��)
		if (entityRefSeq == Seq_FindAmp)
		{
			// Entity �Q�Ƃ̊J�n����������
			if (ch == '&')
			{
				// ';' ��T���ɍs��
				entityRefSeq = Seq_FindSemiColon;
				entityRefStart = m_textCache.GetCount() - 1;
			}
			else
			{
				// �󔒈ȊO�̕����������������`�F�b�N
				if (!IsWhiteSpace(ch)) {
					tokenIsSpaceOnly = false;
				}
			}
		}
		// Enitty �Q�Ƃ̏I�[��T���Ă���
		else if (entityRefSeq == Seq_FindSemiColon)
		{
			// Entity �Q�Ƃ̏I�[����������
			if (ch == ';')
			{
				if (IsReservedEntity(&m_textCache[entityRefStart], m_textCache.GetCount() - entityRefStart)) {
					// &amp; ���A�\��ς� Entity �� XmlNodeType_EntityReference �ɂ��Ȃ��BText �Ƃ��Č������邽�߁A�����ł͂Ȃɂ����Ȃ��B
				}
				else
				{
					// & �̑O�܂ł�1�� NodeData �Ƃ��ăX�g�b�N���Ă���
					NodeData data1;
					data1.Type = (tokenIsSpaceOnly) ? XmlNodeType_Whitespace : XmlNodeType_Text;
					data1.ValueStartPos = tokenStart;
					data1.ValueLen = entityRefStart - tokenStart;
					m_nodes.Add(data1);

					// &�`; �܂ł�1�� NodeData �Ƃ��ăX�g�b�N���Ă���
					NodeData data2;
					data2.Type = XmlNodeType_EntityReference;
					data2.ValueStartPos = entityRefStart;
					data2.ValueLen = m_textCache.GetCount() - entityRefStart;
					m_nodes.Add(data2);
				}

				// & ��T���Ă����Ԃɂ���
				entityRefSeq = Seq_FindAmp;
				tokenStart = m_textCache.GetCount();	// ; �̎����w��
				tokenIsSpaceOnly = false;
			}
		}
	}

	if (entityRefSeq != Seq_FindAmp) {
		// TODO: Error: �e�L�X�g�̈悪�I������̂� Entity �Q�Ƃ�T���Ă���
	}

	if (tokenStart != m_textCache.GetCount())
	{
		// �e�L�X�g�� NodeData �Ƃ��ăX�g�b�N���Ă���
		NodeData data1;
		data1.Type = (tokenIsSpaceOnly) ? XmlNodeType_Whitespace : XmlNodeType_Text;
		data1.ValueStartPos = tokenStart;
		data1.ValueLen = m_textCache.GetCount() - tokenStart;
		m_nodes.Add(data1);
		++m_stockElementCount;
	}
	else {
		// �����ɂ���̂� "...&book;" �̂悤�� Entity �Q�Ƃ��I�[�ŏI����Ă���Ƃ��B
		// ���̏ꍇ�� Text �m�[�h�����K�v�͖����B
	}

	return true;
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
bool XmlReader::ParseComment()
{
	// m_reader �� "<--" �̎��̕������w���Ă���
	int hyphenCount = 0;	// �A������ '-' �̐�
	int start = m_textCache.GetCount();

	for (;;)
	{
		int ch = m_reader->Read();
		if (ch < 0) { break; }	// EOF

		// - ���A�����Ă���
		if (hyphenCount >= 2)
		{
			if (ch == '>') {
				break;	// �R�����g������ɕ���
			}
			else {
				// Error: �R�����g�����ɘA������ -- ����������
				m_errorInfo.AddError(ParseError_CommentDoubleHyphen, m_line, m_col);
				return false;
			}
		}

		// �A������ '-' �̐����J�E���g
		if (ch == '-') {
			++hyphenCount;
		}
		else
		{
			if (hyphenCount > 0)
			{
				// �R�����g�I�[�ł͂Ȃ������B�J�E���g���Ă��� - ���e�L�X�g�����ɖ߂�
				hyphenCount = 0;
				m_textCache.Add(_T('-'));	// ����ł���� 1 �����Ȃ��͂� (�R�����g���ŘA������ -- �͋֎~����Ă���)
			}

			m_textCache.Add(ch);
		}
	}

	// �R�����g�� NodeData �Ƃ��ăX�g�b�N���Ă���
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
	// m_reader �͗v�f���̎��̋󔒂��w���Ă���

	// NodeData �����ăX�g�b�N
	NodeData data;
	data.Type = (isEnd) ? XmlNodeType_EndElement : XmlNodeType_Element;
	data.NameStartPos = nameStart;
	data.NameLen = nameLength;
	m_nodes.Add(data);
	int dataIdx = m_nodes.GetCount() - 1;
	++m_stockElementCount;

	for (;;)
	{
		// ��*
		SkipWhitespace();

		// �^�O�I�[�m�F
		if (m_reader->Peek() == '/')
		{
			m_reader->Read();
			if (m_reader->Read() == '>') {
				m_nodes[dataIdx].IsEmptyElement = true;
				break;
			}
			else {
				// Error: ����ȃ^�O�I�[�ł͂Ȃ�
				m_errorInfo.AddError(ParseError_ElementInvalidEmptyTagEnd, m_line, m_col);
				return false;
			}
		}
		else if (m_reader->Peek() == '>') {
			m_reader->Read();
			break;
		}

		// ����
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
	// m_reader �͑������̐擪���w���Ă���

	// ������
	int nameStart, nameLength;
	if (!ParseName(&nameStart, &nameLength)) { return false; }

	// ��*
	SkipWhitespace();

	// =
	if (m_reader->Peek() != '=') {
		// Error: = ��������Ȃ�����
		m_errorInfo.AddError(ParseError_CommentDoubleHyphen, m_line, m_col);
		return false;
	}
	m_reader->Read();

	// ��*
	SkipWhitespace();

	// "
	if (m_reader->Peek() != '"') {
		// Error: " ��������Ȃ�����
		m_errorInfo.AddError(ParseError_AttributeQuoteNotFount, m_line, m_col);
		return false;
	}
	m_reader->Read();

	// ������
	int valueStart = m_textCache.GetCount();
	int ch = 0;
	for (;;)
	{
		int ch = m_reader->Read();
		if (ch < 0) { break; }		// EOF
		if (ch == '"') { break; }	// ������I�[
		m_textCache.Add(ch);
	}

	// "
	if (ch != '"') {
		// Error: " ��������Ȃ�����
		m_errorInfo.AddError(ParseError_AttributeQuoteNotFount, m_line, m_col);
		return false;
	}

	// NodeData �����ăX�g�b�N
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
//	// m_reader �� "DOCTYPE" ���̎��ʕ�����+�󔒂̎��̕������w���Ă���
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
	// m_reader �� "DOCTYPE" �̎��̕������w���Ă���

	enum DocTypeSec
	{
		DocTypeSec_FindLeftBracket = 0,	// [ ��T���Ă���
		DocTypeSec_FindRightBracket,	// ] ��T���Ă���
		DocTypeSec_Finished,			// �I�[�҂�
	};
	DocTypeSec sec = DocTypeSec_FindLeftBracket;

	int start = -1;
	for (;;)
	{
		int ch = m_reader->Read();
		if (ch < 0) { break; }	// EOF

		// [ ��T���Ă���
		if (sec == DocTypeSec_FindLeftBracket)
		{
			if (ch == '[')
			{
				start = m_textCache.GetCount() + 1;	// �������� Value ���J�n����
				sec = DocTypeSec_FindRightBracket;
			}
			else if (ch == '>') {
				break;	// �v�f�I�[
			}
		}
		// ] ��T���Ă���
		else if (sec == DocTypeSec_FindRightBracket)
		{
			if (ch == ']') {
				sec = DocTypeSec_Finished;
			}
			else {
				m_textCache.Add(ch);	// �l�Ƃ��ĕ������擾���Ă���
			}
		}
		else {
			if (ch == '>') {
				break;	// �v�f�I�[
			}
		}
	}

	// NodeData �����ăX�g�b�N���Ă���
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
	return true;//!IsWhiteSpace(ch);	// TODO: �����Ɛ��䕶���Ƃ����ׂ���
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
bool XmlReader::IsReservedEntity(const TCHAR* text, int len)
{
	if (len < 2 ||			// & �� ; �Ǝ��ʕ����̍Œ� 3 �����ȏ�͂���͂�
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
