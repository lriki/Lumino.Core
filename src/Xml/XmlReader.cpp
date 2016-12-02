
/*

�h�L�������g�����F
	�E�G���e�B�e�B�Q�Ƃ͔F�����܂��������͂��܂���BXmlNodeType::EntityReference



XmlNodeType::EntityReference �̃T���v��
http://www.atmarkit.co.jp/fxml/tecs/007entity/07.html


&lt; �Ƃ��̃G���e�B�e�B�͉������Ă���n���h���ɒʒm���Ȃ���ΈӖ��������̂�
StringRef �g�����Ƃ͖��������B



�A���S���Y�� �����F

	�\�Ȍ��� alloc ��}���邽�߁A��������ȃA���S���Y���ɂȂ��Ă���B

	1 �x�ɉ�͂���̂́A�v�f1���܂��͗v�f�̊O��1�̈敪�B
	�܂�A

		<a b="b"> C&AAA; </a>

	�Ƃ��� XML ������� 3 ��ɕ�����ĉ�͂����B

	�e�L�X�g " C&AAA; " �́A[Text][EntitiyReference][WhiteSpace] �� 3 �ɕ�����邱�Ƃɒ��ӁB
	���̏ꍇ�A�ŏ��� Read() �ł� [Text]�A���� Read() �ł� [EntitiyReference] ��Ԃ��A
	�S�ĕԂ��I��������� Read() �ŁA���̃m�[�h�̉�͂Ɉڂ�B


	�L���b�V���ɂ��āE�E�E

		<data a="1" b="2">

	���\������m�[�h�� [Element][Attribute][Attribute] ��3�B
	��͂��ꂽ�m�[�h�� m_nodes �ɒǉ�����Ă����B

	�����ɖ��O�ƒl�̕������ m_textCache �ɒǉ�����Ă����B
	���̗��ǂݎ��� m_textCache �ɂ� "data12" �������Ă���B
	���� m_textCache �̂����ǂ�����ǂ��܂ł�1�̕�����Ȃ̂��́A�e�m�[�h���m���Ă���B


	### 

*/

// "  2<test>"  // �^�O�̑O�ɕςȕ���������ƃG���[

// 4&book1;     // "  4" �� Text�A"&book1;" �� EntityReference

/*
* lt �Ȃǂ̒�`�ς݃G���e�B�e�B�� EntityReference �ɂȂ�Ȃ��B
* �΂��āA������ lt2 �Ƃ���`����� EntityReference �ɂȂ�B
*
* XML�d�l�ł͂����Ȃ��Ă��邪�A
* <!ENTITY lt     "&#38;#60;">
*
* <!ENTITY lt2     "&#38;#60;">
* A&lt2;S      // "A" "EntityReference" "S" �ɕ������
*
*/
// & lt;   �G���e�B�e�B�Q�Ƃ̊ԂɃX�y�[�X��NG

#include "../Internal.h"
#include <Lumino/Base/StringHelper.h>
#include <Lumino/Base/StringBuilder.h>
#include <Lumino/Base/Resource.h>
#include <Lumino/IO/PathName.h>
#include <Lumino/IO/StringReader.h>
#include <Lumino/IO/StreamReader.h>
#include <Lumino/Xml/XmlReader.h>

LN_NAMESPACE_BEGIN

//==============================================================================
// XmlReader
//==============================================================================

struct Entity
{
	const TCHAR* Pattern;
	int Length;
	TCHAR Value;
};

static const int ReservedEntitiesCount = 5;
static const Entity ReservedEntities[ReservedEntitiesCount] =
{
	{ _T("lt"),		2, '<' },
	{ _T("gt"),		2, '>' },
	{ _T("amp"),	3, '&' },
	{ _T("apos"),	4, '\'' },
	{ _T("quot"),	4, '\"' },
};

//------------------------------------------------------------------------------
XmlReader::XmlReader()
	: m_reader()
	, m_parsingState(ParsingState::ReadElement)
	, m_currentElementNodePos(-1)
	, m_currentPartialCount(0)
	, m_line(1)
	, m_col(1)
	, m_stockElementCount(0)
{
}

//------------------------------------------------------------------------------
XmlReader::XmlReader(const String& str)
	: XmlReader()
{
	m_reader.Attach(LN_NEW StringReader(str), false);
	m_nodes.Reserve(32);
	m_textCache.Reserve(1024);
}

//------------------------------------------------------------------------------
XmlReader::XmlReader(TextReader* textReader)
	: XmlReader()
{
	m_reader.Attach(textReader, true);
	m_nodes.Reserve(32);
	m_textCache.Reserve(1024);
}

//------------------------------------------------------------------------------
XmlReader::~XmlReader()
{
}

//------------------------------------------------------------------------------
void XmlReader::InitializeReader(TextReader* reader)
{
	LN_CHECK_STATE(m_reader == nullptr);
	m_reader = reader;
}

//------------------------------------------------------------------------------
// ��Read() ���̂͑����m�[�h�͕Ԃ��Ȃ��B
bool XmlReader::Read()
{
	bool r = ReadInternal();
	LN_THROW(!m_errorInfo.HasError(), XmlException, m_errorInfo.message.c_str());
	return r;
}

//------------------------------------------------------------------------------
XmlNodeType XmlReader::GetNodeType()  const
{
	if (m_currentNode == nullptr || m_nodes.IsEmpty())
	{
		return XmlNodeType::None;
	}
	return m_currentNode->Type;
}

//------------------------------------------------------------------------------
const String& XmlReader::GetName()
{
	if (m_nodes.IsEmpty()) {
		m_tmpName.SetEmpty();
	}
	else
	{
		if (m_currentNode->NameStartPos == -1 || m_currentNode->NameLen == 0) {
			m_tmpName.SetEmpty();	// Node �͂��邯�ǖ��O����������
		}
		else
		{
			const TCHAR* name = &m_textCache[m_currentNode->NameStartPos];
			m_tmpName.AssignCStr(name, m_currentNode->NameLen);
		}
	}
	return m_tmpName;
}

//------------------------------------------------------------------------------
const String& XmlReader::GetValue()
{
	if (m_nodes.IsEmpty()) {
		m_tmpValue.SetEmpty();
	}
	else
	{
		if (m_currentNode->ValueStartPos == -1 || m_currentNode->ValueLen == 0) {
			m_tmpValue.SetEmpty();	// Node �͂��邯�ǒl����������
		}
		else
		{
			const TCHAR* name = &m_textCache[m_currentNode->ValueStartPos];
			m_tmpValue.AssignCStr(name, m_currentNode->ValueLen);

			// ��`�ς� Entity ��W�J����
			ExpandReservedEntities(m_tmpValue.GetData(), m_tmpValue.GetLength());
		}
	}
	return m_tmpValue;
}

//------------------------------------------------------------------------------
bool XmlReader::IsEmptyElement() const
{
	if (m_nodes.IsEmpty()) {
		return false;
	}
	return m_currentNode->IsEmptyElement;
}

//------------------------------------------------------------------------------
bool XmlReader::IsStartElement()
{
	return (MoveToContent() == XmlNodeType::Element);
}

//------------------------------------------------------------------------------
bool XmlReader::IsStartElement(const String& name)
{
	return
		(MoveToContent() == XmlNodeType::Element) &&
		(GetName() == name);
}

//------------------------------------------------------------------------------
int XmlReader::GetAttributeCount() const
{
	return m_currentAttrCount;
}

//------------------------------------------------------------------------------
bool XmlReader::MoveToFirstAttribute()
{
	if (m_currentAttrCount <= 0) {
		return false;
	}

	m_currentAttrIndex = 0;
	m_currentNode = &m_nodes[m_currentElementNodePos + 1];
	return true;
}

//------------------------------------------------------------------------------
bool XmlReader::MoveToNextAttribute()
{
	if (m_currentAttrIndex + 1 >= m_currentAttrCount) {
		return false;
	}

	m_currentAttrIndex++;
	m_currentNode = &m_nodes[m_currentElementNodePos + 1 + m_currentAttrIndex];
	return true;
}

//------------------------------------------------------------------------------
bool XmlReader::MoveToElement()
{
	if (m_currentNode == NULL || m_currentNode->Type != XmlNodeType::Attribute) {
		return false;
	}

	m_currentAttrIndex = -1;
	m_currentNode = &m_nodes[m_currentElementNodePos];
	m_nodes.Resize(m_currentElementNodePos + 1);	// TODO: ���̕ӂ̌��炷�������C���f�b�N�X���炷�����ɂ��������E�E�E
	return true;
}

//------------------------------------------------------------------------------
XmlNodeType XmlReader::MoveToContent()
{
	do {
		switch (GetNodeType())
		{
		case XmlNodeType::Attribute:
			MoveToElement();
			return GetNodeType();
		case XmlNodeType::Element:
		case XmlNodeType::EndElement:
		case XmlNodeType::CDATA:
		case XmlNodeType::Text:
		case XmlNodeType::EntityReference:
			return GetNodeType();
		}
	} while (Read());

	return GetNodeType();
}

//------------------------------------------------------------------------------
String XmlReader::ReadString()
{
	MoveToElement();
	if (GetNodeType() == XmlNodeType::Element)
	{
		if (IsEmptyElement()) {
			return String::GetEmpty();
		}
		else if (!Read()) {
			LN_THROW(0, InvalidOperationException);
		}
		if (GetNodeType() == XmlNodeType::EndElement) {
			return String::GetEmpty();
		}
	}
	String result;
	while (GetNodeType() == XmlNodeType::Text) {
		result += GetValue();
		if (!Read()) {
			break;
		}
	}
	return result;
}


//------------------------------------------------------------------------------
StringRef XmlReader::GetStringFromCache(int pos, int len)
{
	return StringRef(&m_textCache[pos], len);
}

//------------------------------------------------------------------------------
StringRef XmlReader::GetNodeName(const NodeData& node)
{
	if (node.NameLen == 0) return StringRef();
	return StringRef(&m_textCache[node.NameStartPos], node.NameLen);
}

//------------------------------------------------------------------------------
bool XmlReader::ReadInternal()
{
	while (true)
	{
		switch (m_parsingState)
		{
			case ParsingState::ReadElement:
			{
				//if (m_stockElementCount > 0)
				//{
				//	m_currentElementNodePos += m_currentNode->AttrCount;	// �����m�[�h��ǂݔ�΂� (�ƌ����Ă����݂� Text �m�[�h�݂̂��� if �ɓ����Ă��邩������Ӗ��͖�����)
				//	m_currentElementNodePos++;								// ���̃m�[�h���w��
				//	m_currentNode = &m_nodes[m_currentElementNodePos];
				//	m_currentAttrCount = m_currentNode->AttrCount;
				//	m_stockElementCount--;
				//	return true;
				//}

				//m_textCache.Clear();
				//m_nodes.Clear();
				//m_currentElementNodePos = 0;
				//m_currentAttrCount = 0;
				//m_currentNode = NULL;
				//m_currentAttrIndex = -1;

				int ch = m_reader->Peek();
				if (ch < 0) { return false; }	// �����ǂݎ��镶��������
				if (ch == '<') {
					if (!ParseElementInner()) { return false; }
				}
				else {
					if (!ParseElementOuter()) { return false; }
				}

				//m_currentElementNodePos = 0;
				//m_stockElementCount--;

				m_currentNode = &m_nodes[m_currentElementNodePos];
				m_currentAttrCount = m_currentNode->AttrCount;

				return true;

			}

			case ParsingState::PopNode:
			{
				PopNode();
				m_parsingState = ParsingState::ReadElement;	// TODO: ReadDocument �Ƃ����H�Ƃ����������h�L�������g�͖��������� End �Ƃ��̂ق����������B
				continue;
			}
			case ParsingState::IterateAttributes:
			{
				MoveToElement();
				if (m_currentNode->IsEmptyElement)
					m_parsingState = ParsingState::PopNode;	// ��^�O�Ȃ��� pop ���K�v
				else
					m_parsingState = ParsingState::ReadElement;
				continue;
			}
			case ParsingState::IteratePartialElements:
			{
				++m_currentElementNodePos;
				if (m_currentElementNodePos >= m_stockElementCount)
				{
					m_currentElementNodePos = m_stockElementCount - m_currentPartialCount - 1;	//�@��A�� Text ���n�܂�O�̗v�f���w�� (���� EndElement ������͂�)
					m_nodes.Resize(m_currentElementNodePos+1);
					m_stockElementCount = m_nodes.GetCount();	// TODO: m_stockElementCount�͔z��T�C�Y�ő�p�ł��Ȃ����ȁH
					m_currentPartialCount = 0;
					m_parsingState = ParsingState::ReadElement;
					continue;
				}
				else
				{
					m_currentNode = &m_nodes[m_currentElementNodePos];
					return true;
				}
			}
		}
	}
}

//------------------------------------------------------------------------------
int XmlReader::PushNode(const NodeData& node)
{
	bool isLastPartial = (!m_nodes.IsEmpty() && m_nodes.GetLast().IsPartial);

	m_nodes.Add(node);
	int dataIdx = m_nodes.GetCount() - 1;
	++m_stockElementCount;

	if (isLastPartial)
	{
		// �������ɃX�^�b�N�̃g�b�v�� partial �Ȃ�A���� Text �� Entity �������Ă���Ƃ������ƁB
		// ��A�̃e�L�X�g���p�[�X���́A�ŏ��Ɍ��������v�f�� "���݂̃m�[�h" �ɂ������B
	}
	else
	{
		++m_currentElementNodePos;
	}

	if (node.IsPartial)
	{
		++m_currentPartialCount;
	}
	return dataIdx;
}

//------------------------------------------------------------------------------
void XmlReader::PopNode()
{
	m_textCache.Resize(m_textCache.GetCount() - m_nodes.GetLast().NameLen);


	//m_nodes.RemoveLast();
	m_nodes.Resize(m_currentElementNodePos);
	--m_stockElementCount;


	--m_currentElementNodePos;
	if (m_currentElementNodePos < 0)
	{
		m_currentNode = nullptr;
	}
	else
	{
		m_currentNode = &m_nodes[m_currentElementNodePos];
	}
}

//------------------------------------------------------------------------------
// �^�O�̊J�n�ʒu����̉�́Breader �� <aaa> �̂悤�ȃ^�O�� < ���w���Ă���B
bool XmlReader::ParseElementInner()
{
	m_reader->Read();	// skip '<'

	// �v���t�B�b�N�X�I�Ȃ��̂̃`�F�b�N
	bool isProcInst = false;
	bool isDocContent = false;
	bool isElementEnd = false;
	bool isComment = false;
	bool isDataStart = false;
	if (m_reader->Peek() == '?')		// <?xxx>
	{
		isProcInst = true;
		m_reader->Read();	// skip
	}
	else if (m_reader->Peek() == '!')	// <!xxx>
	{
		isDocContent = true;
		m_reader->Read();	// skip
	}
	else if (m_reader->Peek() == '/')	// </xxx>
	{
		isElementEnd = true;
		m_reader->Read();	// skip
	}
	if (isDocContent)
	{
		if (m_reader->Peek() == '-')	// <-- -->
		{
			isComment = true;
			m_reader->Read();	// skip
			m_reader->Read();	// skip
		}
		else if (m_reader->Peek() == '[')	// <[ ]>
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
		m_errorInfo.AddError(detail::ParseError_ElementNameNotFount, m_line, m_col);
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



	// "<?xml..." ��
	if (isProcInst)
	{
		const TCHAR* name = &m_textCache[namePos];
		bool isXmlDecl = (nameLen == 3 && StringTraits::StrNICmp(name, _T("xml"), 3) == 0);
		ParseXmlDeclOrPI(namePos, nameLen, isXmlDecl);
	}
	// ���̑��̗v�f
	else if (isElementEnd)
	{
		if (!ParseEndElement(namePos, nameLen)) { return false; }
	}
	else
	{
		if (!ParseElement(namePos, nameLen)) { return false; }
	}





	
	// </Grid > �̂悤�ɁA���ɋ󔒂͂����Ă��悢�B�O�̓_���B

	return true;
}

//------------------------------------------------------------------------------
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
	int tokenStart = m_textCache.GetCount();

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
					// &amp; ���A�\��ς� Entity �� XmlNodeType::EntityReference �ɂ��Ȃ��BText �Ƃ��Č������邽�߁A�����ł͂Ȃɂ����Ȃ��B
					tokenIsSpaceOnly = false;
				}
				else
				{
					// & �̑O�܂ł�1�� NodeData �Ƃ��ăX�g�b�N���Ă���
					NodeData data1;
					data1.Type = (tokenIsSpaceOnly) ? XmlNodeType::Whitespace : XmlNodeType::Text;
					data1.ValueStartPos = tokenStart;
					data1.ValueLen = entityRefStart - tokenStart;
					data1.IsPartial = true;
					PushNode(data1);

					// &�`; �̓�����1�� NodeData �Ƃ��ăX�g�b�N���Ă���
					NodeData data2;
					data2.Type = XmlNodeType::EntityReference;
					data2.NameStartPos = entityRefStart + 1;
					data2.NameLen = (m_textCache.GetCount() - entityRefStart) - 2;
					data2.IsPartial = true;
					PushNode(data2);

					// ��Ԃ����ɖ߂�
					tokenStart = m_textCache.GetCount();	// ; �̎����w��
					tokenIsSpaceOnly = true;
				}

				// & ��T���Ă����Ԃɂ���
				entityRefSeq = Seq_FindAmp;
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
		data1.Type = (tokenIsSpaceOnly) ? XmlNodeType::Whitespace : XmlNodeType::Text;
		data1.ValueStartPos = tokenStart;
		data1.ValueLen = m_textCache.GetCount() - tokenStart;
		data1.IsPartial = true;
		PushNode(data1);
	}
	else
	{
		// �����ɂ���̂� "...&book;" �̂悤�� Entity �Q�Ƃ��I�[�ŏI����Ă���Ƃ��B
		// ���̏ꍇ�� Text �m�[�h�����K�v�͖����B
	}

	// ������� Read() �ŁA�X�^�b�N�ɋl�ߍ��񂾃m�[�h�����ɏo���Ă���
	m_parsingState = ParsingState::IteratePartialElements;
	return true;
}

//------------------------------------------------------------------------------
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
				m_errorInfo.AddError(detail::ParseError_CommentDoubleHyphen, m_line, m_col);
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
	data1.Type = XmlNodeType::Comment;
	data1.ValueStartPos = start;
	data1.ValueLen = m_textCache.GetCount() - start;
	PushNode(data1);

	// ����� Read() �ŁA�X�^�b�N (m_nodes) �ɐς�ł��� Comment �������Ăق���
	m_parsingState = ParsingState::PopNode;
	return true;
}

//------------------------------------------------------------------------------
/*
	http://www.eplusx.net/translation/W3C/REC-xml11-20060816/#NT-Name
	[4]   	NameStartChar	::=   	":" | [A-Z] | "_" | [a-z] | [#xC0-#xD6] | [#xD8-#xF6] | [#xF8-#x2FF] | [#x370-#x37D] | [#x37F-#x1FFF] | [#x200C-#x200D] | [#x2070-#x218F] | [#x2C00-#x2FEF] | [#x3001-#xD7FF] | [#xF900-#xFDCF] | [#xFDF0-#xFFFD] | [#x10000-#xEFFFF]
	[4a]   	NameChar		::=   	NameStartChar | "-" | "." | [0-9] | #xB7 | [#x0300-#x036F] | [#x203F-#x2040]
	[5]   	Name			::=   	NameStartChar ( NameChar )*
	[6]   	Names			::=   	Name ( #x20 Name )*
	[7]   	Nmtoken			::=   	( NameChar )+
	[8]   	Nmtokens		::=   	Nmtoken ( #x20 Nmtoken )*

	������ascii�̂ݍl����
*/
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

//------------------------------------------------------------------------------
// XML �錾�܂��͏������߂���͂��� (<?xxxx ... ?>)
//	<?xml version=...
//	     ^ �J�n���̓ǂݎ��ʒu�͂����B
//
//	XMLDecl		::=   	'<?xml' VersionInfo EncodingDecl? SDDecl? S? '?>'
//	PI			::=   	'<?' PITarget ( S ( Char* - ( Char* '?>' Char* ) ) )? '?>'
//
//	Value �� "?XXX " �̌��̕����S�āB
//	<?xml version="1.0" encoding="UTF-8"  ?>	�� "version="1.0" encoding="UTF-8"  " �܂ŁB�I�[�͋󔒂��܂ށB
//------------------------------------------------------------------------------
bool XmlReader::ParseXmlDeclOrPI(int nameStart, int nameLength, bool isXmlDecl)
{
	// NodeData �����ăX�g�b�N
	NodeData data;
	data.Type = (isXmlDecl) ? XmlNodeType::XmlDeclaration : XmlNodeType::ProcessingInstruction;
	data.NameStartPos = nameStart;	// m_textCache ��̖��O�̂���ʒu
	data.NameLen = nameLength;		// m_textCache ��̖��O�̒���
	m_nodes.Add(data);
	int dataIdx = m_nodes.GetCount() - 1;
	++m_stockElementCount;

	// ��*
	SkipWhitespace();

	int len = 0;
	while (!m_reader->IsEOF())
	{
		// �^�O�I�[�m�F
		if (m_reader->Peek() == '?')
		{
			m_reader->Read();
			if (m_reader->Read() == '>') {
				break;
			}
			else {
				// Error: ����ȃ^�O�I�[�ł͂Ȃ�
				m_errorInfo.AddError(detail::ParseError_ElementInvalidEmptyTagEnd, m_line, m_col);
				return false;
			}
		}
		else {
			m_reader->Read();
		}
		++len;
	}

	// Value �������o���Ă���
	if (len > 0)
	{
		data.ValueStartPos = m_textCache.GetCount();
		data.ValueLen = len;
	}
	return true;
}

//------------------------------------------------------------------------------
// STag			::=   	'<' Name ( S Attribute )* S? '>'
// 
//		�^�O���̕����͓ǂݎ��ς݂ŁAm_reader �͗v�f���̎��̋󔒂��w���Ă���B
//
bool XmlReader::ParseElement(int nameStart, int nameLength)
{
	// NodeData �����ăX�g�b�N
	NodeData data;
	data.Type = XmlNodeType::Element;
	data.NameStartPos = nameStart;
	data.NameLen = nameLength;
	int dataIdx = PushNode(data);

	for (;;)
	{
		// ��*
		SkipWhitespace();

		// �^�O�I�[�m�F
		if (m_reader->Peek() == '/')
		{
			m_reader->Read();
			if (m_reader->Read() == '>')
			{
				// <aaa /> �̂悤�Ȏq�v�f�������Ȃ��^�O������
				m_nodes[dataIdx].IsEmptyElement = true;

				if (m_parsingState == ParsingState::IterateAttributes)
				{
					// IterateAttributes �Ȃ��Ԃ��ێ�����B
					// ���� Read() �ł� Attributs ��� �� ��^�O��� �� ���̓ǂݎ�� �Ɛi�߂�B
				}
				else
				{
					// ����� Read() �ŁA�X�^�b�N (m_nodes) �ɐς�ł��� EndElement �������Ăق���
					m_parsingState = ParsingState::PopNode;
				}
				break;
			}
			else {
				// Error: ����ȃ^�O�I�[�ł͂Ȃ�
				m_errorInfo.AddError(detail::ParseError_ElementInvalidEmptyTagEnd, m_line, m_col);
				return false;
			}
		}
		else if (m_reader->Peek() == '>') {
			m_reader->Read();
			break;
		}

		// �����܂ł����瑮��������
		if (!ParseAttribute()) { return false; }
		m_nodes[dataIdx].AttrCount++;
		m_parsingState = ParsingState::IterateAttributes;
	}

	return true;
}

//------------------------------------------------------------------------------
// �^�O���̕����͓ǂݎ��ς݂ŁAm_reader �͗v�f���̎��̋󔒂��w���Ă���B
bool XmlReader::ParseEndElement(int nameStart, int nameLength)
{
	NodeData* top = &m_nodes[m_currentElementNodePos];
	if (GetNodeName(*top) != GetStringFromCache(nameStart, nameLength))
	{
		m_errorInfo.AddError(
			detail::ParseError_TagMismatch, m_line, m_col,
			String::Format(StringRef(InternalResource::GetString(InternalResource::Xml_TagMismatch)), GetNodeName(*top), GetStringFromCache(nameStart, nameLength)));
		return false;
	}


	// �X�^�b�N (m_nodes) �̐擪�ɂ� StartElement ������͂��B������̂ĂāAEndElement �ɂ���B
	m_currentNode = &m_nodes[m_currentElementNodePos];
	m_currentNode->Init();
	m_currentNode->Type = XmlNodeType::EndElement;
	m_currentNode->NameStartPos = nameStart;
	m_currentNode->NameLen = nameLength;

	// �󔒂��΂���
	SkipWhitespace();

	// > ��ǂ񂾂犮��
	if (m_reader->Peek() != '>') return false;
	m_reader->Read();

	// ����� Read() �ŁA�X�^�b�N (m_nodes) �ɐς�ł��� EndElement �������Ăق���
	m_parsingState = ParsingState::PopNode;
	return true;
}

//------------------------------------------------------------------------------
// Attribute	::=   	Name Eq AttValue
//------------------------------------------------------------------------------
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
		m_errorInfo.AddError(detail::ParseError_CommentDoubleHyphen, m_line, m_col);
		return false;
	}
	m_reader->Read();

	// ��*
	SkipWhitespace();

	// "
	if (m_reader->Peek() != '"') {
		// Error: " ��������Ȃ�����
		m_errorInfo.AddError(detail::ParseError_AttributeQuoteNotFount, m_line, m_col);
		return false;
	}
	m_reader->Read();

	// ������
	int valueStart = m_textCache.GetCount();
	int ch = 0;
	for (;;)
	{
		ch = m_reader->Read();
		if (ch < 0) { break; }		// EOF
		if (ch == '"') { break; }	// ������I�[
		m_textCache.Add(ch);
	}

	// "
	if (ch != '"') {
		// Error: " ��������Ȃ�����
		m_errorInfo.AddError(detail::ParseError_AttributeQuoteNotFount, m_line, m_col);
		return false;
	}

	// NodeData �����ăX�g�b�N
	NodeData data;
	data.Type = XmlNodeType::Attribute;
	data.NameStartPos = nameStart;
	data.NameLen = nameLength;
	data.ValueStartPos = valueStart;
	data.ValueLen = m_textCache.GetCount() - valueStart;
	m_nodes.Add(data);
	//++m_stockElementCount;	// �����̏ꍇ�̓X�g�b�N�����グ�Ȃ�
	return true;
}

//------------------------------------------------------------------------------
//bool XmlReader::ParseSkipElement()
//{
//	// m_reader �� "DOCTYPE" ���̎��ʕ�����+�󔒂̎��̕������w���Ă���
//
//	int leftAngle = 1;
//	int rightAngle = 0;
//}

#if 0
//------------------------------------------------------------------------------
// <!DOCTYPE...>
//------------------------------------------------------------------------------
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
	data1.Type = XmlNodeType::Comment;
	data1.ValueStartPos = start;
	data1.ValueLen = m_textCache.GetCount() - start;
	m_nodes.Add(data1);
	++m_stockElementCount;
	return true;
}
#endif

//------------------------------------------------------------------------------
bool XmlReader::IsTextChar(int ch)
{
	return true;//!IsWhiteSpace(ch);	// TODO: �����Ɛ��䕶���Ƃ����ׂ���
}

//------------------------------------------------------------------------------
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
			StringTraits::StrNCmp(ReservedEntities[i].Pattern, text, len) == 0)
		{
			return true;
		}
	}
	return false;
}

//------------------------------------------------------------------------------
bool XmlReader::IsWhiteSpace(int ch)
{
	// S	   :: = (#x20 | #x9 | #xD | #xA) +
	return (ch == 0x20 || ch == 0x09 || ch == 0x0D || ch == 0x0A);
}

//------------------------------------------------------------------------------
bool XmlReader::SkipWhitespace()
{
	while (IsWhiteSpace(m_reader->Peek())) {
		m_reader->Read();
	}
	return !m_reader->IsEOF();
}

//------------------------------------------------------------------------------
bool XmlReader::ParseWhiteSpace()
{
	return false;
}

//------------------------------------------------------------------------------
bool XmlReader::IsAlphaNum(int ch)
{
	return (ch < 128) ? (isalnum(ch)!=0) : true;
}

//------------------------------------------------------------------------------
// text �� len+1 �̗̈悪���邱�� (�I�[�� \0 ���i�[����)
//------------------------------------------------------------------------------
void XmlReader::ExpandReservedEntities(TCHAR* text, int len)
{
	/*
		�\��ς� Entity �͑S�� 1 �����B
		�W�J������Ƀo�b�t�@�������邱�Ƃ͖����B
	*/

	TCHAR* rp = text;	// read pointer
	TCHAR* wp = text;	// write pointer
	TCHAR* end = text + len;
	while (rp < end)
	{
		if (*rp == '&')
		{
			int i = 0;
			for (; i < ReservedEntitiesCount; ++i)
			{
				if (StringTraits::StrNCmp(rp + 1, ReservedEntities[i].Pattern, ReservedEntities[i].Length) == 0 &&
					*(rp + ReservedEntities[i].Length + 1) == ';')
				{
					*wp = ReservedEntities[i].Value;
					++wp;
					rp += ReservedEntities[i].Length + 2;
					break;
				}
			}
			// �\��ς� Entity �ł͂Ȃ�����
			if (i == ReservedEntitiesCount)
			{
				*wp = *rp;
				++rp;
				++wp;
			}
		}
		else
		{
			*wp = *rp;
			++rp;
			++wp;
		}
	}
	*wp = '\0';
}

//==============================================================================
// XmlFileReader
//==============================================================================

//------------------------------------------------------------------------------
XmlFileReader::XmlFileReader(const PathName& filePath, Encoding* encoding)
	: XmlReader()
{
	m_filePath = filePath.CanonicalizePath();
	RefPtr<StreamReader> file(LN_NEW StreamReader(m_filePath, encoding), false);
	InitializeReader(file);
	m_errorInfo.filePath = filePath;
}

//------------------------------------------------------------------------------
XmlFileReader::~XmlFileReader()
{

}


LN_NAMESPACE_END
