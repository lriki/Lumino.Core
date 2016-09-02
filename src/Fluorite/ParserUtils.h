
#pragma once
#include "Common.h"
#include "TokenList.h"

LN_NAMESPACE_BEGIN
namespace parser
{

class ParserUtils
{
public:

	// begin �̎�����n�߂āA���̋󔒈ȊO�̃g�[�N���̈ʒu�܂��� end ��Ԃ�
	template<typename TItr>
	static TItr SkipNextSpaceOrComment(TItr begin, TItr end)
	{
		++begin;
		while (begin < end && begin->IsSpaceOrComment()) ++begin;
		return begin;
	}

	// end �̑O����n�߂āA�O�̋󔒈ȊO�̃g�[�N���̈ʒu�܂��� begin ��Ԃ�
	template<typename TItr>
	static TItr SkipPrevSpaceOrComment(TItr begin, TItr end)
	{
		--end;
		while (begin < end && end->IsSpaceOrComment()) --end;
		return end;
	}


};

} // namespace Parser
LN_NAMESPACE_END

