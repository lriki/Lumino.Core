
#pragma once
#include <Lumino/Base/Stack.h>
#include <Lumino/IO/PathName.h>
#include "../Common.h"
#include "../Token.h"
#include "../TokenList.h"

LN_NAMESPACE_BEGIN
namespace parser
{
enum RpnTokenType
{
	RPN_TT_Unknown = 0,

	RPN_TT_Identifier,				///< ���ʎq
	//RPN_TT_NumericLiteral,			///< ���l���e����

	RPN_TT_OP_GroupStart,			// (
	RPN_TT_OP_GroupEnd,				// )

	RPN_TT_OP_UnaryPlus,			// + (Unary)
	RPN_TT_OP_UnaryMinus,			// - (Unary)
	RPN_TT_OP_Multiply,				// *
	RPN_TT_OP_Divide,				// /
	RPN_TT_OP_Modulus,				// %
	RPN_TT_OP_BinaryPlus,			// + (Binary)
	RPN_TT_OP_BinaryMinus,			// - (Binary)

	RPN_TT_OP_LeftShift,			// <<
	RPN_TT_OP_RightShift,			// >>

	RPN_TT_OP_CompLessThan,			// <
	RPN_TT_OP_CompLessThanEqual,	// <=
	RPN_TT_OP_CompGreaterThen,		// >
	RPN_TT_OP_CompGreaterThenEqual,	// >=
	RPN_TT_OP_CompEqual,			// ==
	RPN_TT_OP_CompNotEqual,			// !=

	RPN_TT_OP_BitwiseNot,			// ~ (Unary)
	RPN_TT_OP_BitwiseAnd,			// &
	RPN_TT_OP_BitwiseXor,			// ^
	RPN_TT_OP_BitwiseOr,			// |

	RPN_TT_OP_LogicalNot,			// ! (Unary)
	RPN_TT_OP_LogicalAnd,			// &&
	RPN_TT_OP_LogicalOr,			// ||

	RPN_TT_OP_CondTrue,				// ? (�������Z�q)
	RPN_TT_OP_CondFalse,			// : (�������Z�q)

	RPN_TT_OP_Comma,				// , (�J���}���Z�q or ��������؂蕶��)

	RPN_TT_OP_FuncCall,				///< �֐��Ăяo�� (���ʎq�������w��)

	RPN_TT_NumericLitaral_Null,
	RPN_TT_NumericLitaral_True,
	RPN_TT_NumericLitaral_False,
	RPN_TT_NumericLitaral_Int32,	/**< C/C++ �� char/wchar_t ������ɂȂ� */
	RPN_TT_NumericLitaral_UInt32,
	RPN_TT_NumericLitaral_Int64,
	RPN_TT_NumericLitaral_UInt64,
	RPN_TT_NumericLitaral_Float,	/**< 32bit */
	RPN_TT_NumericLitaral_Double,	/**< 64bit */

	RPN_TT_Max,	
};

enum class RpnTokenGroup
{
	Unknown,
	Literal,
	//Constant,
	Identifier,
	Operator,
	CondTrue,
	CondFalse,
	FunctionCall,
	Assignment,
};

/** ���Z�q�̃O���[�v (�G���[�����Ȃǂ�����) */
enum class RpnOperatorGroup
{
	Unknown,
	Arithmetic,
	Comparison,
	Logical,
	Bitwise,
	Conditional,
	Assignment,
};

/// ���Z�q�̌�������
enum OpeatorAssociation
{
	OpeatorAssociation_Left,
	OpeatorAssociation_Right,
};

/**
	@brief	�t�|�[�����h�����\������v�f�ł��B
*/
class RpnToken
{
public:
	RpnToken()
	{
		SourceToken = NULL;
		CondGoto = 0;
		ElementCount = 0;
	}

public:
	bool IsOperator() const { return RPN_TT_OP_GroupStart <= Type && Type <= RPN_TT_OP_FuncCall; }

	RpnTokenGroup GetTokenGroup() const;
	RpnOperatorGroup GetOperatorGroup() const;
	bool IsUnary() const;

public:
	RpnTokenType		Type;
	int					Precedence;		///< �D�揇��
	OpeatorAssociation	Association;	///< ��������
	const Token*		SourceToken;
	int					GroupLevel;		///< () �l�X�g�̐[���B���[�g�� 0

	int					CondGoto;		///< (Type �� CondTrue �܂��� CondFalse �̂Ƃ��Ɏg�p����)
	int					ElementCount;	///< , �ŋ�؂�ꂽ�v�f�� (Type �� GroupStart �܂��� FuncCall �̂Ƃ��Ɏg�p����)


};

class RpnTokenList
	: public RefObject
	, public Collection<RpnToken>
{
};

/*
	�����������̂��߁A�ė��p�����
*/
class RpnParser
{
public:
	typedef Collection<Token>::const_iterator Position;

public:
	// TODO: RefPtr
	//static RefPtr<RPNTokenList> ParseCppConstExpression(Position exprBegin, Position exprEnd, DiagnosticsItemSet* diag);

	ResultState ParseCppConstExpression2(Position exprBegin, Position exprEnd, DiagnosticsItemSet* diag);
	const RpnTokenList* GetTokenList() const { return m_rpnTokenList; }

private:
	void Initialize(DiagnosticsItemSet* diag);
	void TokenizeCppConst(Position exprBegin, Position exprEnd);
	void Parse();
	void PushOpStack(RpnToken* token);
	RpnToken* PopOpStackGroupEnd(bool fromArgsSeparator);
	RpnToken* PopOpStackCondFalse();
	void CloseGroup(bool fromArgsSeparator);

private:
	RefPtr<RpnTokenList>	m_tokenList;
	RefPtr<RpnTokenList>	m_rpnTokenList;
	DiagnosticsItemSet*		m_diag;

	Array<RpnToken*>		m_tmpRPNTokenList;
	Stack<RpnToken*>		m_opStack;			// ���Z�q�p�̍�ƃX�^�b�N
	Stack<RpnToken*>		m_condStack;		// �������Z�q�p�̍�ƃX�^�b�N�B: ���i�[���Ă���
	Stack<RpnToken*>		m_groupStack;		// () �̍�ƃX�^�b�N�B( �܂��� FuncCall ���i�[���Ă���
	RpnToken*				m_lastToken;
};



/** RPN �]���Ɏg�p����I�y�����h�̌^ */
LN_ENUM(RpnOperandType)
{
	Unknown,
	Null,
	Boolean,
	Int32,		/* uint8 ���͂���Ɋg�������B*/
	UInt32,
	Int64,
	UInt64,
	Float,
	Double,

	/* ���֍s���قǗD��x������ */
};
LN_ENUM_REFLECTION(RpnOperandType, Null, Boolean, Int32, UInt32, Int64, UInt64, Float, Double);
LN_ENUM_DECLARE(RpnOperandType);

/** RPN �]���Ɏg�p����I�y�����h */
class RpnOperand
{
public:
	RpnOperandType	type;

	union
	{
		bool		valueBoolean;
		int32_t		valueInt32;
		uint32_t	valueUInt32;
		int64_t		valueInt64;
		uint64_t	valueUInt64;
		float		valueFloat;
		double		valueDouble;
	};

	void Set(bool value) { valueBoolean = value; type = RpnOperandType::Boolean; }
	void Set(int32_t value) { valueInt32 = value; type = RpnOperandType::Int32; }
	void Set(uint32_t value) { valueUInt32 = value; type = RpnOperandType::UInt32; }
	void Set(int64_t value) { valueInt64 = value; type = RpnOperandType::Int64; }
	void Set(uint64_t value) { valueUInt64 = value; type = RpnOperandType::UInt64; }
	void Set(float value) { valueFloat = value; type = RpnOperandType::Float; }
	void Set(double value) { valueDouble = value; type = RpnOperandType::Double; }

	/** �����^�ł��邩 (Boolean �͊܂܂Ȃ�) */
	bool IsIntager() const;
	
	bool IsFuzzyTrue() const;
};

/**
	@brief	RPN �g�[�N���̃��X�g��]�����܂��B
*/
class RpnEvaluator
{
public:

	/**
		@brief	�w�肳�ꂽ RPN �g�[�N���̃��X�g��]�����A�l���쐬���܂��B
		@return	���������ꍇ true�B
	*/
	ResultState TryEval(const RpnTokenList* tokenList, DiagnosticsItemSet* diag, RpnOperand* outValue);

private:
	bool MakeOperand(const RpnToken& token, RpnOperand* outOperand);
	bool EvalOperator(const RpnToken& token, const RpnOperand& lhs, const RpnOperand& rhs, RpnOperand* outOperand);
	bool CallFunction(const RpnToken& token, Array<RpnOperand> args, RpnOperand* outOperand);
	bool EvalOperand(RpnTokenType tokenType, const RpnOperand& lhs, const RpnOperand& rhs, RpnOperand* outOperand);

	static RpnOperandType ExpandOperands(const RpnOperand& lhs, const RpnOperand& rhs, RpnOperand* outlhs, RpnOperand* outrhs);
	static void CastOperand(RpnOperand* operand, RpnOperandType to);

private:
	DiagnosticsItemSet*	m_diag;
};

} // namespace parser
LN_NAMESPACE_END
