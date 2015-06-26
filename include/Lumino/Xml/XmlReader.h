
#pragma once
#include "../IO/TextReader.h"
#include "../IO/MemoryStream.h"
#include "XmlError.h"

namespace Lumino
{

/// �m�[�h�̎��
enum XmlNodeType
{
	/// Read ���\�b�h���Ăяo����Ȃ������ꍇ�ɁAXmlReader �ɂ���ĕԂ���܂��B
	XmlNodeType_None = 0,

	/// �v�f
	XmlNodeType_Element,

	/// �v�f�̏I���^�O
	XmlNodeType_EndElement,

	/// ����
	XmlNodeType_Attribute,

	/// �R�����g (�� : <!-- comment -->)
	XmlNodeType_Comment,

	/// �}�[�N�A�b�v�Ԃ̋�
	XmlNodeType_Whitespace,

	/// �m�[�h�̃e�L�X�g�̓��e�B
	XmlNodeType_Text,

	/// CDATA �Z�N�V���� (�� : <![CDATA[�e�L�X�g]]>)�B
	XmlNodeType_CDATA,

	/// �G���e�B�e�B�ւ̎Q�� (�� : &book;)
	XmlNodeType_EntityReference,

	/// �h�L�������g�^�錾 (�� : <!DOCTYPE...>)
	XmlNodeType_DocumentType,

	/// �G���e�B�e�B�錾 (�� : <!ENTITY...>)
	XmlNodeType_Entity,

	/// �L�@�錾 (�� : <!NOTATION...>)
	XmlNodeType_Notation,

	/// �������� (�� : <?pi test?>)
	XmlNodeType_ProcessingInstruction,

	/// XML �錾 (�� : <?xml version='1.0'?>)
	XmlNodeType_XmlDeclaration,

};

/**
	@brief		SAX �X�^�C���� XML �p�[�T�ł��B
	@details	�uExtensible Markup Language (XML) 1.1 (����)�v���x�[�X�Ɏ�������Ă��܂��B
				http://www.eplusx.net/translation/W3C/REC-xml11-20060816/#NT-elementdecl <br>
				�������A���ݑS�Ă̎d�l�ɂ͑Ή�����Ă��܂���B
				�Ή����Ă���d�l�́A��L�����N��̖ڎ��u2 �����v�A�u4.3.1 �e�L�X�g�錾�v����сu4.6 ��`�ςݎ��́v�̓��e�ł��B
				����ȊO�̓m�[�h�̎�ʂ̂ݔF�����܂��B<br>
				�Ⴆ�� <!DOCTYPE...> �̓m�[�h��ʂƂ��ĔF�����܂����A���e�̓ǂݎ��̓X�L�b�v���܂��B
*/
class XmlReader
{
public:
	XmlReader(const String& str);
	XmlReader(TextReader* textReader);
	~XmlReader();

	/**
		@brief		���̃m�[�h�ǂݎ��܂��B
		@return		����ɓǂݎ��ꂽ�ꍇ�� true�B����ȏ�ǂݎ��m�[�h�����݂��Ȃ��ꍇ�� false�B
	*/
	bool Read();

	/**
		@brief		���݂̃m�[�h�̎�ނ��擾���܂��B
	*/
	XmlNodeType GetNodeType() const;

	/**
		@brief		���݂̃m�[�h�̖��O���擾���܂��B
	*/
	const String& GetName();

	/**
		@brief		���݂̃m�[�h�̒l (������`��) ���擾���܂��B
		@details	�l�������ꍇ�͋󕶎����Ԃ��܂��B
	*/
	const String& GetValue();

	/**
		@brief		���݂̃m�[�h����v�f(<book/> ��) ���ǂ������m�F���܂��B
	*/
	bool IsEmptyElement() const;

	/**
		@brief		���݂̃m�[�h�̑��������擾���܂��B
	*/
	int GetAttributeCount() const;

	/**
		@brief		���݂̃m�[�h�̍ŏ��̑����Ɉړ����܂��B
		@return		���������݂���ꍇ�� true�B����ȊO�̏ꍇ�� false�B
		@details	���������݂��Ȃ��ꍇ�A���݈ʒu�͕ύX����܂���B
	*/
	bool MoveToFirstAttribute();

	/**
		@brief		���̑����Ɉړ����܂��B
		@return		���̑��������݂���ꍇ�� true�B����ȏ㑮�������݂��Ȃ��ꍇ�� false�B
		@details	���̑��������݂��Ȃ��ꍇ�A���݈ʒu�͕ύX����܂���B
	*/
	bool MoveToNextAttribute();

	/**
		@brief		���݂̑����m�[�h���܂ޗv�f�Ɉړ����܂��B
		@return		���݈ʒu�������ł������ꍇ�� true�B����ȊO�̏ꍇ�� false�B
		@details	���݈ʒu�������łȂ��ꍇ�A���݈ʒu�͕ύX����܂���B
	*/
	bool MoveToElement();

private:
	bool ParseElementInner();	// '<' ����n�܂�
	bool ParseElementOuter();	// '<' �ȊO����n�܂�

	bool ParseComment();

	bool ParseName(int* startPos, int* length);

	bool ParseElement(int nameStart, int nameLength, bool isEnd);
	bool ParseAttribute();

	bool ParseSkipElement();
	//bool ParseDocumentType();



	bool IsTextChar(int ch);


	bool IsWhiteSpace(int ch);
	bool SkipWhitespace();

	bool IsReservedEntity(const TCHAR* text, int len);	// & �� ; �͊܂܂Ȃ�����

	bool ParseWhiteSpace();

	bool IsAlphaNum(int ch);

	void ExpandReservedEntities(TCHAR* text, int len);

private:
	struct NodeData
	{
		XmlNodeType	Type;
		int			NameStartPos;
		int			NameLen;
		int			ValueStartPos;	///< m_textCache ��̃o�C�g�C���f�b�N�X�B-1 �̏ꍇ�͒l�����݂��Ȃ�
		int			ValueLen;		///< m_textCache ��̃o�C�g��
		int			AttrCount;		///< Element �̏ꍇ�Ɏ��q Attribute �̐�
		bool		IsSubData;		///< Attribute ���A���̗v�f�̎q�ł��邩
		bool		IsEmptyElement;	///< <a />�@�̂悤�ȋ�v�f�ł��邩 

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
	int						m_currentAttrIndex;		///< �����m�[�h���w���Ă���ꍇ�A���̑����C���f�b�N�X�B�w���Ă��Ȃ���� -1
	int						m_line;
	int						m_col;
	XmlError				m_errorInfo;
	int						m_stockElementCount;
	String					m_tmpName;
	int						m_currentAttrCount;		///< ���݂̃m�[�h�̑�����
};

} // namespace Lumino
