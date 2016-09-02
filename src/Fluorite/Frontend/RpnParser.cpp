
#include "Cpp/CppLexer.h"
#include "../DiagnosticsManager.h"
#include "RpnParser.h"

LN_NAMESPACE_BEGIN
namespace parser
{
/*
	C++ �̉��Z�q�ƗD�揇��
	http://ja.cppreference.com/w/cpp/language/operator_precedence
	1+2+3	�� (1+2)+3	�c ������ (Left-to-right)
	a=b=1	�� a=(b=1)	�c �E���� (Right-to-left)
*/

/*
	�� �������Z�q�̉��

		1 ? 6 ? 7 : 8 : 3 ? 4 : 5		������₷�����ʂ�t����Ɓ� 1 ? (6 ? 7 : 8) : (3 ? 4 : 5)

	�Ƃ������́AParse �ł͈ȉ��̂悤�ɓW�J�ł���Ηǂ��B

		idx   >  0  1  2  3  4  5  6  7  8  9 10 11 12
		token >  1  ?  6  ?  7  :  8  :  3  ?  4  :  5
		goto  >     8     6    13    13    12    13

	Eval �ł́A
	�E�g�[�N���͍�����E�֓ǂ�ł����B
	�E? �� goto �́A������(�I�y����stack top)�� false �������ꍇ�ɃW�����v���� idx �������B
	�E: �� goto �́A�����ɂ��ǂ蒅�����Ƃ��ɃW�����v���� idx �������B(�ꗥ�A���̏I�[(��)�ł���)
	���u���̏I�[�v�Ƃ́A')' �܂��̓o�b�t�@�I�[�ł���B

	?: �ȉ��̗D��x�̉��Z�q�͑���n��throw�A, �����A�������l�����Ȃ��ꍇ��
	���Ƃ��ǂꂾ�� : �ŋ�؂��悤�Ƃ� : �ŃW�����v����͕̂K�����̏I�[�ł���B

*/

/*
	�� �֐��Ăяo���̉��

	"1+2" �̂悤�Ȋ֐��Ăяo���̖����P���Ȏ��ł��A��ԊO���ɂ̓_�~�[�̊֐��Ăяo����������̂Ƃ��ĉ�͂���B
	�C���[�W�Ƃ��Ă� "_(1+2)"�B

	���̏I�[�� ) �ł���ƍl���邱�ƂŁA�������Z�q�� , ���Z�q�Ƃ̌��ˍ����������y�ɂȂ�B�����B
	��
	, �͑S�āA�����̋�؂�ƍl����B�J���}���Z�q���B
	�����āA�_�~�[�֐��͈�Ԍ��̈����̕]�����ʂ�Ԃ��֐��Ƃ���B

	"A(1+1,2)" �̃p�[�X���ʂ́A
	
		1 1 + 2 A _

	�ƂȂ�A�g�[�N�� A �� _ �̎�ʂ́u�֐��v�B�v���p�e�B�Ƃ��āA�����̐������B(_ �̓_�~�[�֐��ł���_�ɒ���)
	�]���ł́A���̈����̐������I�y�����h(�]���ςݒ萔)�����o���A�֐��̏������s���B
	A �̓��[�U�[��`�B_ �͍Ō�̈�����Ԃ��B


	"A(1+2,3+4,5+6)" ���p�[�X����Ƃ��́E�E�E
	, �������邽�тɁAopStack �ɂ��� FuncCall �܂ł��o�̓��X�g�Ɉڂ��B
	) ������������AFuncCall �܂ł��ڂ��ς�����AFuncCall �������B
	
	�� �J���}���Z�q�̉��
	
	�O�q�̒ʂ�A�_�~�[�֐��𗘗p����B
	�_�~�[�֐��͈�Ԍ��̈����̕]�����ʂ�Ԃ��֐��Ƃ���B
*/


// LN_RPN_OPERATOR_DEFINE_xxxx �}�N���ŁARPN �g�[�N����ʂ̃e�[�u���ɓo�^���鉉�Z�q�̎���
// http://en.cppreference.com/w/cpp/language/expressions
// http://en.cppreference.com/w/cpp/language/operator_logical
struct RpnOperator
{
	template<typename T> static void UnaryPlus(T /*lhs*/, T rhs, RpnOperand* out) { out->Set(rhs); }
	template<typename T> static void UnaryMinus(T /*lhs*/, T rhs, RpnOperand* out) { out->Set(-rhs); }
	template<> static void UnaryMinus<uint32_t>(uint32_t /*lhs*/, uint32_t rhs, RpnOperand* out) { out->Set((uint32_t)-((int32_t)rhs)); }	// �x�����
	template<> static void UnaryMinus<uint64_t>(uint64_t /*lhs*/, uint64_t rhs, RpnOperand* out) { out->Set((uint64_t)-((int64_t)rhs)); }	// �x�����

	template<typename T> static void Multiply(T lhs, T rhs, RpnOperand* out)	{ out->Set(lhs * rhs); }
	template<typename T> static void Divide(T lhs, T rhs, RpnOperand* out)		{ out->Set(lhs / rhs); }
	template<typename T> static void Modulus(T lhs, T rhs, RpnOperand* out)		{ out->Set(lhs % rhs); }
	template<typename T> static void BinaryPlus(T lhs, T rhs, RpnOperand* out)	{ out->Set(lhs + rhs); }
	template<typename T> static void BinaryMinus(T lhs, T rhs, RpnOperand* out) { out->Set(lhs - rhs); }

	template<typename T> static void LeftShift(T lhs, T rhs, RpnOperand* out)	{ out->Set(lhs << rhs); }
	template<typename T> static void RightShift(T lhs, T rhs, RpnOperand* out)	{ out->Set(lhs >> rhs); }

	template<typename T> static void CompLessThan(T lhs, T rhs, RpnOperand* out)			{ out->Set(lhs < rhs); }
	template<typename T> static void CompLessThanEqual(T lhs, T rhs, RpnOperand* out)		{ out->Set(lhs <= rhs); }
	template<typename T> static void CompGreaterThen(T lhs, T rhs, RpnOperand* out)			{ out->Set(lhs > rhs); }
	template<typename T> static void CompGreaterThenEqual(T lhs, T rhs, RpnOperand* out)	{ out->Set(lhs >= rhs); }
	template<typename T> static void CompEqual(T lhs, T rhs, RpnOperand* out)				{ out->Set(lhs == rhs); }
	template<typename T> static void CompNotEqual(T lhs, T rhs, RpnOperand* out)			{ out->Set(lhs != rhs); }

	template<typename T> static void BitwiseNot(T /*lhs*/, T rhs, RpnOperand* out)	{ out->Set(~rhs); }
	template<typename T> static void BitwiseAnd(T lhs, T rhs, RpnOperand* out)		{ out->Set(lhs & rhs); }
	template<typename T> static void BitwiseXor(T lhs, T rhs, RpnOperand* out)		{ out->Set(lhs ^ rhs); }
	template<typename T> static void BitwiseOr(T lhs, T rhs, RpnOperand* out)		{ out->Set(lhs | rhs); }

	template<typename T> static void LogicalNot(T /*lhs*/, T rhs, RpnOperand* out)		{ out->Set(!(rhs != 0)); }
	template<typename T> static void LogicalAnd(T lhs, T rhs, RpnOperand* out)			{ out->Set((lhs != 0) && (rhs != 0)); }
	template<typename T> static void LogicalOr(T lhs, T rhs, RpnOperand* out)			{ out->Set((lhs != 0) || (rhs != 0)); }
	template<> static void LogicalNot<bool>(bool /*lhs*/, bool rhs, RpnOperand* out)	{ out->Set(!rhs); }
	template<> static void LogicalAnd<bool>(bool lhs, bool rhs, RpnOperand* out)		{ out->Set(lhs && rhs); }
	template<> static void LogicalOr<bool>(bool lhs, bool rhs, RpnOperand* out)			{ out->Set(lhs || rhs); }
};

typedef void(*NullOperator)(nullptr_t lhs, nullptr_t rhs, RpnOperand* out);
typedef void(*BooleanOperator)(bool lhs, bool rhs, RpnOperand* out);
typedef void(*Int32Operator)(int32_t lhs, int32_t rhs, RpnOperand* out);
typedef void(*UInt32Operator)(uint32_t lhs, uint32_t rhs, RpnOperand* out);
typedef void(*Int64Operator)(int64_t lhs, int64_t rhs, RpnOperand* out);
typedef void(*UInt64Operator)(uint64_t lhs, uint64_t rhs, RpnOperand* out);
typedef void(*FloatOperator)(float lhs, float rhs, RpnOperand* out);
typedef void(*DoubleOperator)(double lhs, double rhs, RpnOperand* out);

struct RpnTypedOperatorTableItem
{
	NullOperator	operaotrNullptr;
	BooleanOperator	operaotrBoolean;
	Int32Operator	operaotrInt32;
	UInt32Operator	operaotrUInt32;
	Int64Operator	operaotrInt64;
	UInt64Operator	operaotrUInt64;
	FloatOperator	operaotrFloat;
	DoubleOperator	operaotrDouble;
};

#define LN_RPN_OPERATOR_DEFINE_NONE \
	{ nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, }

#define LN_RPN_OPERATOR_DEFINE(op) \
	{ nullptr, nullptr, RpnOperator::op<int32_t>, RpnOperator::op<uint32_t>, RpnOperator::op<int64_t>, RpnOperator::op<uint64_t>, RpnOperator::op<float>, RpnOperator::op<double>, }

#define LN_RPN_OPERATOR_DEFINE_INTEGER(op) \
	{ nullptr, nullptr, RpnOperator::op<int32_t>, RpnOperator::op<uint32_t>, RpnOperator::op<int64_t>, RpnOperator::op<uint64_t>, nullptr, nullptr, }

#define LN_RPN_OPERATOR_DEFINE_LOGICAL(op) \
	{ nullptr, RpnOperator::op<bool>, RpnOperator::op<int32_t>, RpnOperator::op<uint32_t>, RpnOperator::op<int64_t>, RpnOperator::op<uint64_t>, RpnOperator::op<float>, RpnOperator::op<double>, }

struct TokenTypeTableItem
{
	RpnTokenGroup		tokenGroup;
	RpnOperatorGroup	operatorGroup;
	bool				isUnary;
	RpnTypedOperatorTableItem	eval;
};
static TokenTypeTableItem g_tokenTypeTable[] = 
{
	{ RpnTokenGroup::Unknown,		RpnOperatorGroup::Unknown,		false,	LN_RPN_OPERATOR_DEFINE_NONE, },		// RPN_TT_Unknown = 0,
	{ RpnTokenGroup::Identifier,	RpnOperatorGroup::Unknown,		false,	LN_RPN_OPERATOR_DEFINE_NONE, },		// RPN_TT_Identifier,				///< ���ʎq
	//{ RpnTokenGroup::Literal,		RpnOperatorGroup::Unknown,		false,	LN_RPN_OPERATOR_DEFINE_NONE, },		// RPN_TT_NumericLiteral,			///< ���l���e����
	{ RpnTokenGroup::Unknown,		RpnOperatorGroup::Unknown,		false,	LN_RPN_OPERATOR_DEFINE_NONE, },		// RPN_TT_OP_GroupStart,			// (	�� ���ʂ̓p�[�X�Ŏ�菜�����̂� Unknown
	{ RpnTokenGroup::Unknown,		RpnOperatorGroup::Unknown,		false,	LN_RPN_OPERATOR_DEFINE_NONE, },		// RPN_TT_OP_GroupEnd,				// )	�� ���ʂ̓p�[�X�Ŏ�菜�����̂� Unknown
	{ RpnTokenGroup::Operator,		RpnOperatorGroup::Arithmetic,	true,	LN_RPN_OPERATOR_DEFINE(UnaryPlus),},			// RPN_TT_OP_UnaryPlus,			// + (Unary)
	{ RpnTokenGroup::Operator,		RpnOperatorGroup::Arithmetic,	true,	LN_RPN_OPERATOR_DEFINE(UnaryMinus), },			// RPN_TT_OP_UnaryMinus,			// - (Unary)
	{ RpnTokenGroup::Operator,		RpnOperatorGroup::Arithmetic,	false,	LN_RPN_OPERATOR_DEFINE(Multiply), },		// RPN_TT_OP_Multiply,				// *
	{ RpnTokenGroup::Operator,		RpnOperatorGroup::Arithmetic,	false,	LN_RPN_OPERATOR_DEFINE(Divide), },		// RPN_TT_OP_Divide,				// /
	{ RpnTokenGroup::Operator,		RpnOperatorGroup::Arithmetic,	false,	LN_RPN_OPERATOR_DEFINE_INTEGER(Modulus), },		// RPN_TT_OP_Modulus,				// %
	{ RpnTokenGroup::Operator,		RpnOperatorGroup::Arithmetic,	false,	LN_RPN_OPERATOR_DEFINE(BinaryPlus), },		// RPN_TT_OP_BinaryPlus,			// + (Binary)
	{ RpnTokenGroup::Operator,		RpnOperatorGroup::Arithmetic,	false,	LN_RPN_OPERATOR_DEFINE(BinaryMinus), },		// RPN_TT_OP_BinaryMinus,			// - (Binary)
	{ RpnTokenGroup::Operator,		RpnOperatorGroup::Bitwise,		false,	LN_RPN_OPERATOR_DEFINE_INTEGER(LeftShift), },		// RPN_TT_OP_LeftShift,			// <<
	{ RpnTokenGroup::Operator,		RpnOperatorGroup::Bitwise,		false,	LN_RPN_OPERATOR_DEFINE_INTEGER(RightShift), },		// RPN_TT_OP_RightShift,			// >>
	{ RpnTokenGroup::Operator,		RpnOperatorGroup::Comparison,	false,	LN_RPN_OPERATOR_DEFINE_LOGICAL(CompLessThan), },		// RPN_TT_OP_CompLessThan,			// <
	{ RpnTokenGroup::Operator,		RpnOperatorGroup::Comparison,	false,	LN_RPN_OPERATOR_DEFINE_LOGICAL(CompLessThanEqual), },		// RPN_TT_OP_CompLessThanEqual,	// <=
	{ RpnTokenGroup::Operator,		RpnOperatorGroup::Comparison,	false,	LN_RPN_OPERATOR_DEFINE_LOGICAL(CompGreaterThen), },		// RPN_TT_OP_CompGreaterThen,		// >
	{ RpnTokenGroup::Operator,		RpnOperatorGroup::Comparison,	false,	LN_RPN_OPERATOR_DEFINE_LOGICAL(CompGreaterThenEqual), },		// RPN_TT_OP_CompGreaterThenEqual,	// >=
	{ RpnTokenGroup::Operator,		RpnOperatorGroup::Comparison,	false,	LN_RPN_OPERATOR_DEFINE_LOGICAL(CompEqual), },		// RPN_TT_OP_CompEqual,			// ==
	{ RpnTokenGroup::Operator,		RpnOperatorGroup::Comparison,	false,	LN_RPN_OPERATOR_DEFINE_LOGICAL(CompNotEqual), },		// RPN_TT_OP_CompNotEqual,			// !=
	{ RpnTokenGroup::Operator,		RpnOperatorGroup::Bitwise,		true,	LN_RPN_OPERATOR_DEFINE_INTEGER(BitwiseNot), },			// RPN_TT_OP_BitwiseNot,			// ~ (Unary)
	{ RpnTokenGroup::Operator,		RpnOperatorGroup::Bitwise,		false,	LN_RPN_OPERATOR_DEFINE_INTEGER(BitwiseAnd), },		// RPN_TT_OP_BitwiseAnd,			// &
	{ RpnTokenGroup::Operator,		RpnOperatorGroup::Bitwise,		false,	LN_RPN_OPERATOR_DEFINE_INTEGER(BitwiseXor), },		// RPN_TT_OP_BitwiseXor,			// ^
	{ RpnTokenGroup::Operator,		RpnOperatorGroup::Bitwise,		false,	LN_RPN_OPERATOR_DEFINE_INTEGER(BitwiseOr), },		// RPN_TT_OP_BitwiseOr,			// |
	{ RpnTokenGroup::Operator,		RpnOperatorGroup::Logical,		true,	LN_RPN_OPERATOR_DEFINE_LOGICAL(LogicalNot), },			// RPN_TT_OP_LogicalNot,			// ! (Unary)
	{ RpnTokenGroup::Operator,		RpnOperatorGroup::Logical,		false,	LN_RPN_OPERATOR_DEFINE_LOGICAL(LogicalAnd), },		// RPN_TT_OP_LogicalAnd,			// &&
	{ RpnTokenGroup::Operator,		RpnOperatorGroup::Logical,		false,	LN_RPN_OPERATOR_DEFINE_LOGICAL(LogicalOr), },		// RPN_TT_OP_LogicalOr,			// ||
	{ RpnTokenGroup::CondTrue,		RpnOperatorGroup::Conditional,	false,	LN_RPN_OPERATOR_DEFINE_NONE, },		// RPN_TT_OP_CondTrue,				// ? (�������Z�q)
	{ RpnTokenGroup::CondFalse,		RpnOperatorGroup::Conditional,	false,	LN_RPN_OPERATOR_DEFINE_NONE, },		// RPN_TT_OP_CondFalse,			// : (�������Z�q)
	{ RpnTokenGroup::Unknown,		RpnOperatorGroup::Unknown,		false,	LN_RPN_OPERATOR_DEFINE_NONE, },		// RPN_TT_OP_Comma,				// , (�J���}���Z�q or ��������؂蕶��)	�����Ή�
	{ RpnTokenGroup::FunctionCall,	RpnOperatorGroup::Unknown,		false,	LN_RPN_OPERATOR_DEFINE_NONE, },		// RPN_TT_OP_FuncCall,				///< �֐��Ăяo�� (���ʎq�������w��)
	
	{ RpnTokenGroup::Literal,		RpnOperatorGroup::Unknown,		false,	LN_RPN_OPERATOR_DEFINE_NONE, },		// RPN_TT_NumericLitaral_Null,
	{ RpnTokenGroup::Literal,		RpnOperatorGroup::Unknown,		false,	LN_RPN_OPERATOR_DEFINE_NONE, },		// RPN_TT_NumericLitaral_True,
	{ RpnTokenGroup::Literal,		RpnOperatorGroup::Unknown,		false,	LN_RPN_OPERATOR_DEFINE_NONE, },		// RPN_TT_NumericLitaral_False,
	{ RpnTokenGroup::Literal,		RpnOperatorGroup::Unknown,		false,	LN_RPN_OPERATOR_DEFINE_NONE, },		// RPN_TT_NumericLitaral_Int32,
	{ RpnTokenGroup::Literal,		RpnOperatorGroup::Unknown,		false,	LN_RPN_OPERATOR_DEFINE_NONE, },		// RPN_TT_NumericLitaral_UInt32,
	{ RpnTokenGroup::Literal,		RpnOperatorGroup::Unknown,		false,	LN_RPN_OPERATOR_DEFINE_NONE, },		// RPN_TT_NumericLitaral_Int64,
	{ RpnTokenGroup::Literal,		RpnOperatorGroup::Unknown,		false,	LN_RPN_OPERATOR_DEFINE_NONE, },		// RPN_TT_NumericLitaral_UInt64,
	{ RpnTokenGroup::Literal,		RpnOperatorGroup::Unknown,		false,	LN_RPN_OPERATOR_DEFINE_NONE, },		// RPN_TT_NumericLitaral_Float,	/**< 32bit */
	{ RpnTokenGroup::Literal,		RpnOperatorGroup::Unknown,		false,	LN_RPN_OPERATOR_DEFINE_NONE, },		// RPN_TT_NumericLitaral_Double,	/**< 64bit */
};

//=============================================================================
// RpnToken
//=============================================================================

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
RpnTokenGroup RpnToken::GetTokenGroup() const
{
	return g_tokenTypeTable[Type].tokenGroup;
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
RpnOperatorGroup RpnToken::GetOperatorGroup() const
{
	return g_tokenTypeTable[Type].operatorGroup;
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
bool RpnToken::IsUnary() const
{
	return g_tokenTypeTable[Type].isUnary;
}

//=============================================================================
// RpnParser
//=============================================================================
//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
//RefPtr<RpnTokenList> RpnParser::ParseCppConstExpression(Position exprBegin, Position exprEnd, DiagnosticsItemSet* diag)
//{
//	// �Ƃ肠���������Œǉ��Y��`�F�b�N
//	assert(LN_ARRAY_SIZE_OF(g_tokenTypeTable) == RPN_TT_Max);
//	RpnParser parser;
//	parser.TokenizeCppConst(exprBegin, exprEnd);
//	parser.Parse();
//	return parser.m_rpnTokenList;
//}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
ResultState RpnParser::ParseCppConstExpression2(Position exprBegin, Position exprEnd, DiagnosticsItemSet* diag)
{
	Initialize(diag);
	TokenizeCppConst(exprBegin, exprEnd);
	Parse();
	return ResultState::Success;
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void RpnParser::Initialize(DiagnosticsItemSet* diag)
{
	if (m_tokenList != nullptr) {
		m_tokenList->Clear();
	}
	else {
		m_tokenList.Attach(LN_NEW RpnTokenList());
	}
	if (m_rpnTokenList != nullptr) {
		m_rpnTokenList->Clear();
	}
	else {
		m_rpnTokenList.Attach(LN_NEW RpnTokenList());
	}
	m_diag = diag;
	m_tmpRPNTokenList.Clear();
	m_opStack.Clear();
	m_condStack.Clear();
	m_groupStack.Clear();
	m_lastToken = nullptr;
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void RpnParser::TokenizeCppConst(Position exprBegin, Position exprEnd)
{
	// �Ƃ肠�������̓g�[�N�����Ń������m�� (�X�y�[�X���܂܂�Ă���� Tokenize ��̎g�p�ʂ͏��Ȃ��Ȃ�͂�)
	m_tokenList->Reserve(exprEnd - exprBegin + 2);

	// ���������X�g��͏����� , ���Z�q�̉�͂����˂���悤�ɁA���X�g�擪�Ƀ_�~�[�� FuncCall �����Ă���
	RpnToken headToken;
	headToken.Type = RPN_TT_OP_FuncCall;
	m_tokenList->Add(headToken);

	Position pos = exprBegin;

	for (; pos != exprEnd; ++pos)
	{
		switch (pos->GetCommonType())
		{
		case CommonTokenType::SpaceSequence:
		case CommonTokenType::NewLine:
		case CommonTokenType::Comment:
		case CommonTokenType::Unknown:
			break;	// �����͋󔒈����B������������
		case CommonTokenType::Identifier:
		{
			RpnToken token;
			token.Type = RPN_TT_Identifier;
			token.SourceToken = &(*pos);
			m_tokenList->Add(token);
			break;
		}
		case CommonTokenType::ArithmeticLiteral:
		{
			RpnToken token;
			switch (pos->GetLangTokenType())
			{
			case TT_NumericLitaralType_Char:		token.Type = RPN_TT_NumericLitaral_Int32; break;
			case TT_NumericLitaralType_WideChar:	token.Type = RPN_TT_NumericLitaral_Int32; break;
			case TT_NumericLitaralType_Int32:		token.Type = RPN_TT_NumericLitaral_Int32; break;
			case TT_NumericLitaralType_UInt32:		token.Type = RPN_TT_NumericLitaral_UInt32; break;
			case TT_NumericLitaralType_Int64:		token.Type = RPN_TT_NumericLitaral_Int64; break;
			case TT_NumericLitaralType_UInt64:		token.Type = RPN_TT_NumericLitaral_UInt64; break;
			case TT_NumericLitaralType_Float:		token.Type = RPN_TT_NumericLitaral_Float; break;
			case TT_NumericLitaralType_Double:		token.Type = RPN_TT_NumericLitaral_Double; break;
			default:
				m_diag->Report(DiagnosticsCode::RpnEvaluator_InvalidNumericType);
				return;	// TODO: Result
			}
			token.SourceToken = &(*pos);
			m_tokenList->Add(token);
			break;
		}
		case CommonTokenType::Operator:
		{
			static const RpnTokenType CppTypeToRPNType[] =
			{
				/* TT_CppOP_SeparatorBegin		*/	RPN_TT_Unknown,
				/* TT_CppOP_SharpSharp  	##	*/	RPN_TT_Unknown,
				/* TT_CppOP_Sharp			#	*/	RPN_TT_Unknown,
				/* TT_CppOP_ArrowAsterisk	->*	*/	RPN_TT_Unknown,
				/* TT_CppOP_Arrow			->	*/	RPN_TT_Unknown,
				/* TT_CppOP_Comma			,	*/	RPN_TT_OP_Comma,
				/* TT_CppOP_Increment		++	*/	RPN_TT_Unknown,
				/* TT_CppOP_Decrement		--	*/	RPN_TT_Unknown,
				/* TT_CppOP_LogicalAnd		&&	*/	RPN_TT_OP_LogicalAnd,
				/* TT_CppOP_LogicalOr		||	*/	RPN_TT_OP_LogicalOr,
				/* TT_CppOP_LessThenEqual	<=	*/	RPN_TT_OP_CompLessThanEqual,
				/* TT_CppOP_GreaterThenEqual>=	*/	RPN_TT_OP_CompGreaterThenEqual,
				/* TT_CppOP_CmpEqual		==	*/	RPN_TT_OP_CompEqual,
				/* TT_CppOP_LeftShiftEqual	<<=	*/	RPN_TT_Unknown,
				/* TT_CppOP_RightShiftEqual	>>=	*/	RPN_TT_Unknown,
				/* TT_CppOP_PlusEqual		+=	*/	RPN_TT_Unknown,
				/* TT_CppOP_MinusEqual		-=	*/	RPN_TT_Unknown,
				/* TT_CppOP_MulEqual		*=	*/	RPN_TT_Unknown,
				/* TT_CppOP_DivEqual		/=	*/	RPN_TT_Unknown,
				/* TT_CppOP_ModEqual		%=	*/	RPN_TT_Unknown,
				/* TT_CppOP_AndEqual		&=	*/	RPN_TT_Unknown,
				/* TT_CppOP_OrEqual			|=	*/	RPN_TT_Unknown,
				/* TT_CppOP_NotEqual		!=	*/	RPN_TT_OP_CompNotEqual,
				/* TT_CppOP_Equal			=	*/	RPN_TT_Unknown,
				/* TT_CppOP_LeftShift		<<	*/	RPN_TT_OP_LeftShift,
				/* TT_CppOP_RightShift		>>	*/	RPN_TT_OP_RightShift,
				/* TT_CppOP_Plus			+	*/	RPN_TT_OP_BinaryPlus,
				/* TT_CppOP_Minul			-	*/	RPN_TT_OP_BinaryMinus,
				/* TT_CppOP_Asterisk		*	*/	RPN_TT_OP_Multiply,
				/* TT_CppOP_Slash			/	*/	RPN_TT_OP_Divide,
				/* TT_CppOP_Parseint		%	*/	RPN_TT_OP_Modulus,
				/* TT_CppOP_Ampersand		&	*/	RPN_TT_OP_BitwiseAnd,
				/* TT_CppOP_Pipe			|	*/	RPN_TT_OP_BitwiseOr,
				/* TT_CppOP_Tilde			~	*/	RPN_TT_OP_BitwiseNot,
				/* TT_CppOP_Caret,			^	*/	RPN_TT_OP_BitwiseXor,
				/* TT_CppOP_Exclamation		!	*/	RPN_TT_OP_LogicalNot,
				/* TT_CppOP_Ellipsis		...	*/	RPN_TT_Unknown,
				/* TT_CppOP_DotAsterisk		.*	*/	RPN_TT_Unknown,
				/* TT_CppOP_Dot				.	*/	RPN_TT_Unknown,
				/* TT_CppOP_DoubleColon		::	*/	RPN_TT_Unknown,
				/* TT_CppOP_Question		?	*/	RPN_TT_OP_CondTrue,
				/* TT_CppOP_Colon			:	*/	RPN_TT_OP_CondFalse,
				/* TT_CppOP_Semicolon		;	*/	RPN_TT_Unknown,
				/* TT_CppOP_LeftBrace		{	*/	RPN_TT_Unknown,
				/* TT_CppOP_RightBrace		}	*/	RPN_TT_Unknown,
				/* TT_CppOP_LeftBracket		[	*/	RPN_TT_Unknown,
				/* TT_CppOP_RightBracket	]	*/	RPN_TT_Unknown,
				/* TT_CppOP_LeftParen		(	*/	RPN_TT_OP_GroupStart,
				/* TT_CppOP_RightParen		)	*/	RPN_TT_OP_GroupEnd,
				/* TT_CppOP_LeftAngle		<	*/	RPN_TT_OP_CompLessThan,
				/* TT_CppOP_RightAngle		>	*/	RPN_TT_OP_CompGreaterThen,
			};
			assert(LN_ARRAY_SIZE_OF(CppTypeToRPNType) == (TT_CppOP_SeparatorEnd - TT_CppOP_SeparatorBegin));

			struct CppTokenInfo
			{
				int					Precedence;		///< �D�揇��
				OpeatorAssociation	Association;	///< ��������
			};
			static const CppTokenInfo TokenInfoTable[] =
			{
				{ 0,	OpeatorAssociation_Left },	// RPN_TT_Unknown,				// (Dummy)
				{ 0,	OpeatorAssociation_Left },	// RPN_TT_Identifier,			// (Dummy)
				//{ 0,	OpeatorAssociation_Left },	// RPN_TT_NumericLiteral,		// (Dummy)
				{ 2,	OpeatorAssociation_Left },	// RPN_TT_OP_GroupStart,		// (
				{ 2,	OpeatorAssociation_Left },	// RPN_TT_OP_GroupEnd,			// )
				{ 3,	OpeatorAssociation_Right },	// RPN_TT_OP_UnaryPlus,			// +
				{ 3,	OpeatorAssociation_Right },	// RPN_TT_OP_UnaryMinus,		// -
				{ 5,	OpeatorAssociation_Left },	// RPN_TT_OP_Multiply,			// *
				{ 5,	OpeatorAssociation_Left },	// RPN_TT_OP_Divide,			// /
				{ 5,	OpeatorAssociation_Left },	// RPN_TT_OP_IntegerDivide,		// %
				{ 6,	OpeatorAssociation_Left },	// RPN_TT_OP_BinaryPlus,		// + (Binary)
				{ 6,	OpeatorAssociation_Left },	// RPN_TT_OP_BinaryMinus,		// - (Binary)
				{ 7,	OpeatorAssociation_Left },	// RPN_TT_OP_LeftShift,			// <<
				{ 7,	OpeatorAssociation_Left },	// RPN_TT_OP_RightShift,		// >>
				{ 8,	OpeatorAssociation_Left },	// RPN_TT_OP_CompLessThan,		// <
				{ 8,	OpeatorAssociation_Left },	// RPN_TT_OP_CompLessThanEqual,	// <=
				{ 8,	OpeatorAssociation_Left },	// RPN_TT_OP_CompGreaterThen,	// >
				{ 8,	OpeatorAssociation_Left },	// RPN_TT_OP_CompGreaterThenEqual,// >=
				{ 9,	OpeatorAssociation_Left },	// RPN_TT_OP_CompEqual,			// ==
				{ 9,	OpeatorAssociation_Left },	// RPN_TT_OP_CompNotEqual,		// !=
				{ 3,	OpeatorAssociation_Right },	// RPN_TT_OP_BitwiseNot,		// ~
				{ 10,	OpeatorAssociation_Left },	// RPN_TT_OP_BitwiseAnd,		// &
				{ 11,	OpeatorAssociation_Left },	// RPN_TT_OP_BitwiseXor,		// ^
				{ 12,	OpeatorAssociation_Left },	// RPN_TT_OP_BitwiseOr,			// |
				{ 3,	OpeatorAssociation_Right },	// RPN_TT_OP_LogicalNot,		// !
				{ 13,	OpeatorAssociation_Left },	// RPN_TT_OP_LogicalAnd,		// &&
				{ 14,	OpeatorAssociation_Left },	// RPN_TT_OP_LogicalOr,			// ||
				{ 15,	OpeatorAssociation_Right },	// RPN_TT_OP_CondTrue,			// ? (�������Z�q)
				{ 15,	OpeatorAssociation_Left },	// RPN_TT_OP_CondFalse,			// : (�������Z�q)
				{ 17,	OpeatorAssociation_Left },	// RPN_TT_OP_Comma,				// , (�J���}���Z�q or ��������؂蕶��)
				{ 0,	OpeatorAssociation_Left },	// RPN_TT_OP_FuncCall,			// (Dummy)
				
				{ 0,	OpeatorAssociation_Left },	// RPN_TT_NumericLitaral_Null,// (Dummy)
				{ 0,	OpeatorAssociation_Left },	// RPN_TT_NumericLitaral_True,// (Dummy)
				{ 0,	OpeatorAssociation_Left },	// RPN_TT_NumericLitaral_False,// (Dummy)
				{ 0,	OpeatorAssociation_Left },	// RPN_TT_NumericLitaral_Int32,// (Dummy)
				{ 0,	OpeatorAssociation_Left },	// RPN_TT_NumericLitaral_UInt32,// (Dummy)
				{ 0,	OpeatorAssociation_Left },	// RPN_TT_NumericLitaral_Int64,// (Dummy)
				{ 0,	OpeatorAssociation_Left },	// RPN_TT_NumericLitaral_UInt64,// (Dummy)
				{ 0,	OpeatorAssociation_Left },	// RPN_TT_NumericLitaral_Float,// (Dummy)
				{ 0,	OpeatorAssociation_Left },	// RPN_TT_NumericLitaral_Double,// (Dummy)
			};
			assert(LN_ARRAY_SIZE_OF(TokenInfoTable) == RPN_TT_Max);

			RpnToken token;
			token.Type = CppTypeToRPNType[pos->GetLangTokenType() - TT_CppOP_SeparatorBegin];

			// ( ���ЂƂO�����ʎq�̏ꍇ�͊֐��Ăяo���Ƃ���
			if (token.Type == RPN_TT_OP_GroupStart &&
				!m_tokenList->IsEmpty() &&
				m_tokenList->GetLast().Type == RPN_TT_Identifier)
			{
				// Identifer �̎�ނ� FuncCall �ɕύX���A( �̓g�[�N���Ƃ��Ē��o���Ȃ�
				m_tokenList->GetLast().Type = RPN_TT_OP_FuncCall;
			}
			else
			{
				// + or - �̏ꍇ�͒P�����Z�q�ł��邩�������Ŋm�F����B
				// �ЂƂO�̗L���g�[�N�������Z�q�ł���ΒP�����Z�q�ł���B
				if (token.Type == RPN_TT_OP_BinaryPlus || token.Type == RPN_TT_OP_BinaryMinus)
				{
					if (!m_tokenList->IsEmpty() && m_tokenList->GetLast().IsOperator())
					{
						if (token.Type == RPN_TT_OP_BinaryPlus) {
							token.Type = RPN_TT_OP_UnaryPlus;
						}
						else {	// if (token.Type == RPN_TT_OP_BinaryMinus)
							token.Type = RPN_TT_OP_UnaryMinus;
						}
					}
				}

				token.Precedence = TokenInfoTable[token.Type].Precedence;
				token.Association = TokenInfoTable[token.Type].Association;
				token.SourceToken = &(*pos);
				m_tokenList->Add(token);
			}
			break;
		}
		case CommonTokenType::Keyword:
		{
			RpnToken token;
			if (pos->GetLangTokenType() == TT_CppKW_true)
			{
				token.Type = RPN_TT_NumericLitaral_True;
				token.SourceToken = &(*pos);
				m_tokenList->Add(token);
			}
			else if (pos->GetLangTokenType() == TT_CppKW_false)
			{
				token.Type = RPN_TT_NumericLitaral_False;
				token.SourceToken = &(*pos);
				m_tokenList->Add(token);
			}
			break;
		}
		default:
			// TODO:error
			break;
		}
	}

	// ���������X�g��͏����� , ���Z�q�̉�͂����˂���悤�ɁA���X�g�I�[�Ƀ_�~�[�� GroupEnd �����Ă���
	RpnToken tailToken;
	tailToken.Type = RPN_TT_OP_GroupEnd;
	m_tokenList->Add(tailToken);
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void RpnParser::Parse()
{
	m_tmpRPNTokenList.Reserve(m_tokenList->GetCount());
	m_lastToken = nullptr;

	for (RpnToken& token : *m_tokenList)
	{
		// ���݂� () �[����U���Ă���
		token.GroupLevel = m_groupStack.GetCount();

		// �萔�͏o�̓��X�g�֐ς�ł���
		if (token.GetTokenGroup() == RpnTokenGroup::Literal)
		{
			m_tmpRPNTokenList.Add(&token);
		}
		else if (token.IsOperator())
		{
			switch (token.Type)
			{
			case RPN_TT_OP_FuncCall:
				m_groupStack.Push(&token);
				m_opStack.Push(&token);
				break;
			case RPN_TT_OP_GroupStart:
				m_groupStack.Push(&token);
				m_opStack.Push(&token);
				break;
			case RPN_TT_OP_GroupEnd:			// m_tokenList �̍Ō�̓_�~�[�� ) �����Ă��邽�߁A�Ō��1�x�K���ʂ�
				if (m_lastToken->Type != RPN_TT_OP_GroupStart) {
					m_groupStack.GetTop()->ElementCount++;	// () ���̈����̐��𑝂₷�B�������A"Func()" �̂悤�Ɏ�������������Α��₳�Ȃ�
				}
				PopOpStackGroupEnd(false);		// opStack �� GroupStart �܂��� FuncCall �܂ł̓��e���o�̓��X�g�Ɉڂ��BGroupStart �܂��� FuncCall �͍폜����B
				CloseGroup(false);				// �����x�� () ���� ':' �S�Ă� CondGoto ��U��B�Ō�Ɍ��݂̃O���[�v�����폜����
				break;
			case RPN_TT_OP_CondTrue:
				PushOpStack(&token);
				m_tmpRPNTokenList.Add(&token);
				break;
			case RPN_TT_OP_CondFalse:
			{
				RpnToken* condTrue = PopOpStackCondFalse();
				if (!condTrue) { return; }		// Error : ':' �ɑΉ����� ? ��������Ȃ�����
				m_tmpRPNTokenList.Add(&token);
				m_condStack.Push(&token);
				condTrue->CondGoto = m_tmpRPNTokenList.GetCount();	// �W�����v��Ƃ��� ':' �̎����w��
				break;
			}
			case RPN_TT_OP_Comma:
				// ���݂̃O���[�v�����̎������̐����C���N�������g�B�ŏ��� , �̏ꍇ�� 1 �]����++����B
				//if (m_groupStack.GetTop()->ElementCount == 0) {	
				//	m_groupStack.GetTop()->ElementCount++;
				//}
				m_groupStack.GetTop()->ElementCount++;	

				PopOpStackGroupEnd(true);				// ( �` , �܂ł� opStack �̓��e���o�̓��X�g�Ɉڂ��B�������A( �͎c�����܂܂ɂ���
				CloseGroup(true);						// �O���[�v���̏������Z�q�̏������s���B�������A���̌�O���[�v���͍폜���Ȃ�
				break;
			// �ʏ�̉��Z�q
			default:
				PushOpStack(&token);
				break;
			}
		}

		m_lastToken = &token;
	}

	PopOpStackCondFalse();

	// �����x�� () ���� ':' �S�Ă� CondGoto ��U�� (��ԊO���� GroupEnd �����Ƃ����l��)
	//CloseGroup();

	// ���Z�q�p�X�^�b�N�Ɏc���Ă���v�f��S�ďo�̓��X�g�Ɉڂ�
	while (!m_opStack.IsEmpty())
	{
		RpnToken* top;
		m_opStack.Pop(&top);
		if (top->Type == RPN_TT_OP_GroupStart) {
			//TODO: error���ʂ����Ă��Ȃ�
			break;
		}
		else if (top->Type == RPN_TT_OP_CondTrue) {
			//TODO: error�������Z�q�����Ă��Ȃ�
			break;
		}
		m_tmpRPNTokenList.Add(top);
	}

	// �o�͗p�̃��X�g�ցA�g�[�N���̃R�s�[���쐬���ڂ�
	m_rpnTokenList->Reserve(m_tmpRPNTokenList.GetCount());
	for (RpnToken* token : m_tmpRPNTokenList)
	{
		m_rpnTokenList->Add(*token);
	}
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void RpnParser::PushOpStack(RpnToken* token)	// Operator �܂��� CondTrue ���� PushOpStack �����
{
	// �X�^�b�N�ɂ�����̂���菜����ƁB
	// ���ꂩ�����悤�Ƃ��Ă�����̂��Atop �̗D��x�̕���������Ύ�菜���B
	// �����ꍇ�͎�菜���Ȃ��B
	// �X�^�b�N�ɂ͗D��x�̒Ⴂ���̂��c�葱���邱�ƂɂȂ�B
	while (!m_opStack.IsEmpty())
	{
		RpnToken* top = m_opStack.GetTop();
		if (top->Type == RPN_TT_OP_GroupStart || top->Type == RPN_TT_OP_FuncCall) {
			// '(' �͓��ʈ����B�Ƃɂ������Z�q�X�^�b�N�̐擪�ɐςށB(���Z�q�̗D��x�łǂ������ł��Ȃ�)
			// �ʓr�A')' �����������Ƃ��A�Ή����� '(' �܂ł̃X�^�b�N�v�f��S�Ă��o�̓��X�g�ֈڂ��B
			break;
		}

		if (top->Precedence < token->Precedence)	// [+ * �� +] �ƌ�����ԂȂ�A*(5) +(6) �Ȃ̂� * ����菜��
		{
			// �o�̓��X�g��
			m_tmpRPNTokenList.Add(top);
			m_opStack.Pop();
		}
		else {
			break;
		}
	}
	m_opStack.Push(token);
}

//-----------------------------------------------------------------------------
// GroupEnd (')') �܂��� , ������������Ă΂��
//-----------------------------------------------------------------------------
RpnToken* RpnParser::PopOpStackGroupEnd(bool fromArgsSeparator)
{
	// �Ή����� GroupStart ('(') ��������܂ŃX�^�b�N�̉��Z�q���o�̓��X�g�ֈڂ��Ă����B
	RpnToken* top = NULL;
	while (!m_opStack.IsEmpty())
	{
		top = m_opStack.GetTop();
		if (top->Type == RPN_TT_OP_GroupStart)
		{
			m_opStack.Pop();	// GroupStart �͎̂Ă�
			break;
		}
		else if (top->Type == RPN_TT_OP_FuncCall)
		{
			// FuncCall �͏o�̓��X�g�̖����ɐς݁A�I������B
			// �������A, �̏ꍇ�͐ς܂Ȃ��B
			if (!fromArgsSeparator)
			{
				m_tmpRPNTokenList.Add(top);	
				m_opStack.Pop();
			}
			break;
		}

		// �o�̓��X�g��
		m_tmpRPNTokenList.Add(top);
		m_opStack.Pop();
		top = nullptr;
	}

	if (!top) {
		// TODO: error �K�� GroupStart ��������΂Ȃ�Ȃ�
		// ���~�B�Ō�ɂ�1�x�Ă΂�邽�߃G���[�ɂ��Ă͂����Ȃ�
		return nullptr;
	}
	return top;
}

//-----------------------------------------------------------------------------
// CondFalse (':') ������������Ă΂��
//-----------------------------------------------------------------------------
RpnToken* RpnParser::PopOpStackCondFalse()
{
	// �Ή����� CondStart ('?') ��������܂ŃX�^�b�N�̉��Z�q���o�̓��X�g�ֈڂ��Ă����B
	RpnToken* top = nullptr;
	while (!m_opStack.IsEmpty())
	{
		top = m_opStack.GetTop();
		if (top->Type == RPN_TT_OP_CondTrue) {
			m_opStack.Pop();	// CondTrue �͎̂Ă�
			break;
		}

		// �o�̓��X�g��
		m_tmpRPNTokenList.Add(top);
		m_opStack.Pop();
	}
	/* 
		�����O�� ? ��T���ɍs�������ŗǂ��B
		( ) �Ɠ����l�����B
		x    ?    x ? x : x    :    x ? x : x
	*/

	if (!top) {
		// TODO: error �K�� CondTrue ��������΂Ȃ�Ȃ�
		return nullptr;
	}
	return top;
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void RpnParser::CloseGroup(bool fromArgsSeparator)
{
	// ���݂� () ���x���� ':' �S�Ă� CondGoto ��U��A�X�^�b�N�����菜��
	while (!m_condStack.IsEmpty())
	{
		RpnToken* condFalse = m_condStack.GetTop();
		if (condFalse->GroupLevel < m_groupStack.GetCount()) {
			break;
		}
		condFalse->CondGoto = m_tmpRPNTokenList.GetCount();
		m_condStack.Pop();
	}

	// �O���[�v����1�폜����B�������A, �̏ꍇ�͎c���Ă����B
	if (!fromArgsSeparator) {
		m_groupStack.Pop();
	}
}


//=============================================================================
// RpnOperand
//=============================================================================

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
bool RpnOperand::IsIntager() const
{
	return
		type == RpnOperandType::Int32 ||
		type == RpnOperandType::UInt32 ||
		type == RpnOperandType::Int64 ||
		type == RpnOperandType::UInt64;
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
bool RpnOperand::IsFuzzyTrue() const
{
	switch (type)
	{
	case RpnOperandType::Boolean:	return valueBoolean;
	case RpnOperandType::Int32:		return valueInt32 != 0;
	case RpnOperandType::UInt32:	return valueUInt32 != 0;
	case RpnOperandType::Int64:		return valueInt64 != 0;
	case RpnOperandType::UInt64:	return valueUInt64 != 0;
	case RpnOperandType::Float:		return valueFloat != 0;
	case RpnOperandType::Double:	return valueDouble != 0;
	default:
		return false;
	}
}

//=============================================================================
// RpnEvaluator
//=============================================================================

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
ResultState RpnEvaluator::TryEval(const RpnTokenList* tokenList, DiagnosticsItemSet* diag, RpnOperand* outValue)
{
	if (tokenList == nullptr) ResultState::Error;
	if (diag == nullptr) ResultState::Error;
	m_diag = diag;

	Stack<RpnOperand> operandStack;
	Array<RpnOperand> funcCallArgs;
	RpnOperand operand, lhs, rhs;
	for (int iToken = 0; iToken < tokenList->GetCount(); ++iToken)
	{
		bool skipPush = false;
		const RpnToken& token = tokenList->GetAt(iToken);
		switch (token.GetTokenGroup())
		{
			case RpnTokenGroup::Literal:
				if (!MakeOperand(token, &operand)) return ResultState::Error;
				break;
			//case RpnTokenGroup::Constant:
			//case RpnTokenGroup::Identifier:
			case RpnTokenGroup::Operator:
				if (token.IsUnary() && operandStack.GetCount() >= 1)
				{
					operandStack.Pop(&rhs);
					lhs.type = RpnOperandType::Unknown;
				}
				else if (operandStack.GetCount() >= 2)
				{
					operandStack.Pop(&rhs);
					operandStack.Pop(&lhs);
				}
				else
				{
					// Error: ����������Ȃ�
					m_diag->Report(DiagnosticsCode::RpnEvaluator_InvalidOperatorSide);
					return ResultState::Error;
				}
				if (!EvalOperator(token, lhs, rhs, &operand)) return ResultState::Error;
				break;

			case RpnTokenGroup::CondTrue:
			case RpnTokenGroup::CondFalse:
				/*  0 ? 1 : 2 �̎��͂܂� CondTrue ��������A�I�y�����h�� 0 �Ȃ̂� 2 �փW�����v����B
                *  1 ? 1 : 2 �̎��͂܂� CondTrue ��������A�W�����v�����Ɍp������B
                *  ���̌�ACondFalse �� 2 �̌��փW�����v����B
                *  
                * 1 == 2 ? 3 + 4 : 5 + 6 �̃p�[�X���ʂ�
                * �� 1 2 == ? 3 4 + : 5 6 +
                *      ? �� goto �� 8 (5��)
                *      : �� goto �� 11 (�I�[(�z��O)��)
                */
				if (operandStack.GetCount() >= 0)
				{
					if (token.GetTokenGroup() == RpnTokenGroup::CondTrue)
					{
						operandStack.Pop(&lhs);		// ���̃I�y�����h�͒l�����邾���Ŏ̂ĂĂ��܂�
						if (!lhs.IsFuzzyTrue())
						{
							// : �̎��֍s��
							iToken = token.CondGoto - 1;
						}
						else
						{
							// ? �̎��֍s�� (���ɉ������Ȃ�)
						}
					}
					else // (token.GetTokenGroup() == RpnTokenGroup::CondFalse)
					{
						// : �̎��֍s��
						iToken = token.CondGoto - 1;
					}
				}
				else
				{
					// Error: ����������Ȃ�
					m_diag->Report(DiagnosticsCode::RpnEvaluator_InvalidOperatorSide);
					return ResultState::Error;
				}
				skipPush = true;	// �V�����I�y�����h�̍쐬�Ȃǂ͍s���Ă��Ȃ��̂� push ����K�v�͖���
				break;

			case RpnTokenGroup::FunctionCall:
				if (operandStack.GetCount() >= token.ElementCount)
				{
					funcCallArgs.Resize(token.ElementCount);
					for (int i = token.ElementCount - 1; i >= 0; --i)
					{
						operandStack.Pop(&funcCallArgs[i]);
					}
					if (!CallFunction(token, funcCallArgs, &operand)) return ResultState::Error;
				}
				else
				{
					// Error: ����������Ȃ�
					m_diag->Report(DiagnosticsCode::RpnEvaluator_InvalidFuncCallArgsCount);
					return ResultState::Error;
				}
				break;

			case RpnTokenGroup::Assignment:
				return ResultState::Error;
			default:
				m_diag->Report(DiagnosticsCode::RpnEvaluator_UnexpectedToken);
				return ResultState::Error;
		}

		if (!skipPush) {
			operandStack.Push(operand);
		}
	}

	// �S�Ẵg�[�N����ǂݏI����ƁA�X�^�b�N��1�������ʂ��o�Ă���
	if (operandStack.GetCount() != 1)
	{
		m_diag->Report(DiagnosticsCode::RpnEvaluator_InsufficientToken);
		return ResultState::Error;
	}
	*outValue = operandStack.GetTop();
	return ResultState::Success;
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
bool RpnEvaluator::MakeOperand(const RpnToken& token, RpnOperand* outOperand)
{
	// ���e����
	if (token.GetTokenGroup() == RpnTokenGroup::Literal)
	{
		const TokenChar* str = token.SourceToken->GetBegin();
		int len = token.SourceToken->GetLength();
		const TokenChar* dummy;
		NumberConversionResult r;
		switch (token.Type)
		{
			case RPN_TT_NumericLitaral_Null:
				outOperand->type = RpnOperandType::Null;
				r = NumberConversionResult::Success;
				break;
			case RPN_TT_NumericLitaral_True:
				outOperand->type = RpnOperandType::Boolean;
				outOperand->valueBoolean = true;
				r = NumberConversionResult::Success;
				break;
			case RPN_TT_NumericLitaral_False:
				outOperand->type = RpnOperandType::Boolean;
				outOperand->valueBoolean = false;
				r = NumberConversionResult::Success;
				break;
			case RPN_TT_NumericLitaral_Int32:
				outOperand->type = RpnOperandType::Int32;
				outOperand->valueInt32 = StringTraits::ToInt32(str, len, 0, &dummy, &r);
				break;
			case RPN_TT_NumericLitaral_UInt32:
				outOperand->type = RpnOperandType::UInt32;
				outOperand->valueUInt32 = StringTraits::ToInt32(str, len, 0, &dummy, &r);
				break;
			case RPN_TT_NumericLitaral_Int64:
				outOperand->type = RpnOperandType::Int64;
				outOperand->valueInt64 = StringTraits::ToInt64(str, len, 0, &dummy, &r);
				break;
			case RPN_TT_NumericLitaral_UInt64:
				outOperand->type = RpnOperandType::UInt64;
				outOperand->valueUInt64 = StringTraits::ToUInt64(str, len, 0, &dummy, &r);
				break;
			case RPN_TT_NumericLitaral_Float:
				outOperand->type = RpnOperandType::Float;
				outOperand->valueFloat = (float)StringTraits::ToDouble(str, len, &dummy, &r);
				break;
			case RPN_TT_NumericLitaral_Double:
				outOperand->type = RpnOperandType::Double;
				outOperand->valueDouble = StringTraits::ToDouble(str, len, &dummy, &r);
				break;
		}
		if (r == NumberConversionResult::Success)
		{
			return true;
		}
	}

	// Error: ���l�ɂł��Ȃ�����
	m_diag->Report(DiagnosticsCode::RpnEvaluator_InvalidNumericLiteral);
	return false;
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
bool RpnEvaluator::EvalOperator(const RpnToken& token, const RpnOperand& lhs_, const RpnOperand& rhs_, RpnOperand* outOperand)
{
	RpnOperand lhs, rhs;
	ExpandOperands(lhs_, rhs_, &lhs, &rhs);
	return EvalOperand(token.Type, lhs, rhs, outOperand);
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
bool RpnEvaluator::CallFunction(const RpnToken& token, Array<RpnOperand> args, RpnOperand* outOperand)
{
	// �Ή�����g�[�N����������΁A����̓J���}���Z�q��͗p�̃_�~�[�B��Ԍ��̈�����Ԃ������B
	if (token.SourceToken == nullptr)
	{
		*outOperand = args.GetLast();
		return true;
	}
	else
	{
		LN_THROW(0, NotImplementedException);
		return false;
	}
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
bool RpnEvaluator::EvalOperand(RpnTokenType tokenType, const RpnOperand& lhs, const RpnOperand& rhs, RpnOperand* outOperand)
{
	auto item = g_tokenTypeTable[tokenType].eval;
	outOperand->type = rhs.type;	// �P���̏ꍇ�� lhs �� Unknown �ɂȂ��Ă���̂� rhs �Ō���
	switch (rhs.type)
	{
	case RpnOperandType::Boolean:	if (item.operaotrBoolean != nullptr) { item.operaotrBoolean(lhs.valueBoolean, rhs.valueBoolean, outOperand); return true; } break;
	case RpnOperandType::Int32:		if (item.operaotrInt32 != nullptr) { item.operaotrInt32(lhs.valueInt32, rhs.valueInt32, outOperand); return true; } break;
	case RpnOperandType::UInt32:	if (item.operaotrUInt32 != nullptr) { item.operaotrUInt32(lhs.valueUInt32, rhs.valueUInt32, outOperand); return true; } break;
	case RpnOperandType::Int64:		if (item.operaotrInt64 != nullptr) { item.operaotrInt64(lhs.valueInt64, rhs.valueInt64, outOperand); return true; } break;
	case RpnOperandType::UInt64:	if (item.operaotrUInt64 != nullptr) { item.operaotrUInt64(lhs.valueUInt64, rhs.valueUInt64, outOperand); return true; } break;
	case RpnOperandType::Float:		if (item.operaotrFloat != nullptr) { item.operaotrFloat(lhs.valueFloat, rhs.valueFloat, outOperand); return true; } break;
	case RpnOperandType::Double:	if (item.operaotrDouble != nullptr) { item.operaotrDouble(lhs.valueDouble, rhs.valueDouble, outOperand); return true; } break;
	default:
		assert(0);
		break;
	}
	// Error: �w�肳�ꂽ�I�y�����h�̌^���s���B
	m_diag->Report(DiagnosticsCode::RpnEvaluator_OperatorInvalidType, rhs.type.ToString());
	return false;
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
RpnOperandType RpnEvaluator::ExpandOperands(const RpnOperand& lhs, const RpnOperand& rhs, RpnOperand* outlhs, RpnOperand* outrhs)
{
	RpnOperandType type = std::max(lhs.type, rhs.type);
	*outlhs = lhs;
	*outrhs = rhs;
	CastOperand(outlhs, type);
	CastOperand(outrhs, type);
	return type;
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void RpnEvaluator::CastOperand(RpnOperand* op, RpnOperandType to)
{
	if (op->type == RpnOperandType::Unknown || to == RpnOperandType::Unknown || op->type == to) return;
	RpnOperand t = *op;
	switch (op->type)
	{
		case RpnOperandType::Int32:
		{
			switch (to)
			{
			case RpnOperandType::Boolean:	op->valueBoolean = (t.valueInt32 != 0); break;
			case RpnOperandType::Int32:		op->valueInt32 = (int32_t)	t.valueInt32; break;
			case RpnOperandType::UInt32:	op->valueUInt32 = (uint32_t)t.valueInt32; break;
			case RpnOperandType::Int64:		op->valueInt64 = (int64_t)	t.valueInt32; break;
			case RpnOperandType::UInt64:	op->valueUInt64 = (uint64_t)t.valueInt32; break;
			case RpnOperandType::Float:		op->valueFloat = (float)	t.valueInt32; break;
			case RpnOperandType::Double:	op->valueDouble = (double)	t.valueInt32; break;
			}
			break;
		}
		case RpnOperandType::UInt32:
		{
			switch (to)
			{
			case RpnOperandType::Boolean:	op->valueBoolean = (t.valueUInt32 != 0); break;
			case RpnOperandType::Int32:		op->valueInt32 = (int32_t)	t.valueUInt32; break;
			case RpnOperandType::UInt32:	op->valueUInt32 = (uint32_t)t.valueUInt32; break;
			case RpnOperandType::Int64:		op->valueInt64 = (int64_t)	t.valueUInt32; break;
			case RpnOperandType::UInt64:	op->valueUInt64 = (uint64_t)t.valueUInt32; break;
			case RpnOperandType::Float:		op->valueFloat = (float)	t.valueUInt32; break;
			case RpnOperandType::Double:	op->valueDouble = (double)	t.valueUInt32; break;
			}
			break;
		}
		case RpnOperandType::Int64:
		{
			switch (to)
			{
			case RpnOperandType::Boolean:	op->valueBoolean = (t.valueInt64 != 0); break;
			case RpnOperandType::Int32:		op->valueInt32 = (int32_t)	t.valueInt64; break;
			case RpnOperandType::UInt32:	op->valueUInt32 = (uint32_t)t.valueInt64; break;
			case RpnOperandType::Int64:		op->valueInt64 = (int64_t)	t.valueInt64; break;
			case RpnOperandType::UInt64:	op->valueUInt64 = (uint64_t)t.valueInt64; break;
			case RpnOperandType::Float:		op->valueFloat = (float)	t.valueInt64; break;
			case RpnOperandType::Double:	op->valueDouble = (double)	t.valueInt64; break;
			}
			break;
		}
		case RpnOperandType::UInt64:
		{
			switch (to)
			{
			case RpnOperandType::Boolean:	op->valueBoolean = (t.valueUInt64 != 0); break;
			case RpnOperandType::Int32:		op->valueInt32 = (int32_t)	t.valueUInt64; break;
			case RpnOperandType::UInt32:	op->valueUInt32 = (uint32_t)t.valueUInt64; break;
			case RpnOperandType::Int64:		op->valueInt64 = (int64_t)	t.valueUInt64; break;
			case RpnOperandType::UInt64:	op->valueUInt64 = (uint64_t)t.valueUInt64; break;
			case RpnOperandType::Float:		op->valueFloat = (float)	t.valueUInt64; break;
			case RpnOperandType::Double:	op->valueDouble = (double)	t.valueUInt64; break;
			}
			break;
		}
		case RpnOperandType::Float:
		{
			switch (to)
			{
			case RpnOperandType::Boolean:	op->valueBoolean = (t.valueFloat != 0); break;
			case RpnOperandType::Int32:		op->valueInt32 = (int32_t)	t.valueFloat; break;
			case RpnOperandType::UInt32:	op->valueUInt32 = (uint32_t)t.valueFloat; break;
			case RpnOperandType::Int64:		op->valueInt64 = (int64_t)	t.valueFloat; break;
			case RpnOperandType::UInt64:	op->valueUInt64 = (uint64_t)t.valueFloat; break;
			case RpnOperandType::Float:		op->valueFloat = (float)	t.valueFloat; break;
			case RpnOperandType::Double:	op->valueDouble = (double)	t.valueFloat; break;
			}
			break;
		}
		case RpnOperandType::Double:
		{
			switch (to)
			{
			case RpnOperandType::Boolean:	op->valueBoolean = (t.valueDouble != 0); break;
			case RpnOperandType::Int32:		op->valueInt32 = (int32_t)	t.valueDouble; break;
			case RpnOperandType::UInt32:	op->valueUInt32 = (uint32_t)t.valueDouble; break;
			case RpnOperandType::Int64:		op->valueInt64 = (int64_t)	t.valueDouble; break;
			case RpnOperandType::UInt64:	op->valueUInt64 = (uint64_t)t.valueDouble; break;
			case RpnOperandType::Float:		op->valueFloat = (float)	t.valueDouble; break;
			case RpnOperandType::Double:	op->valueDouble = (double)	t.valueDouble; break;
			}
			break;
		}
	}

	op->type = to;
}

} // namespace parser
LN_NAMESPACE_END
