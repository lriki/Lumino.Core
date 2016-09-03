

#define LN_PARSE(result, parser) \
	auto result##_ = (parser).Call(input); \
	if (result##_.IsFailed()) return input.Fail(); \
	input.Next(result##_); \
	auto result = result##_.GetValue();

#define LN_PARSE_RESULT(result, parser) \
	auto result = (parser).Call(input); \
	if (result.IsFailed()) return input.Fail(); \
	input.Next(result);

namespace ln
{
namespace combinators
{
namespace detail
{

template<typename TCursor>
class ParserFailure
{
public:
	TCursor remainder;
};

} // namespace detail


// ParserResult の決まりごと
//	- 成否 (true/false) を持つ
//	- 値を持つ (失敗の場合は不正値)
//	- 次の読み取り位置を持つ (remainder)
// T はパーサ関数が返した値
template<typename T, typename TCursor>
class GenericParserResult
{
public:
	static GenericParserResult<T, TCursor> Success(const T& value, int matchBegin, int matchEnd, const TCursor& remainder)
	{
		return GenericParserResult<T, TCursor>(value, matchBegin, matchEnd, remainder, true);
	}
	static GenericParserResult<T, TCursor> Fail(const TCursor& remainder)
	{
		return GenericParserResult<T, TCursor>(T(), 0, 0, remainder, false);
	}

	const T& GetValue() const { return m_value; }
	const TCursor& GetRemainder() const { return m_remainder; }	// 評価後の次の読み取り位置
	int GetRemainderPosition() const { return m_remainder.GetPosition(); }
	bool IsSucceed() const { return m_isSuccess; }
	bool IsFailed() const { return !m_isSuccess; }
	int GetMatchBegin() const { return m_matchBegin; }
	int GetMatchEnd() const { return m_matchEnd; }


	GenericParserResult(const detail::ParserFailure<TCursor>& failer)
		: m_value()
		, m_matchBegin(0)
		, m_matchEnd(0)
		, m_remainder(failer.remainder)
		, m_isSuccess(false)
	{
	}
	
private:
	GenericParserResult(const T& value, int matchBegin, int matchEnd, const TCursor& remainder, bool isSuccess)
		: m_value(value)
		, m_matchBegin(matchBegin)
		, m_matchEnd(matchEnd)
		, m_remainder(remainder)
		, m_isSuccess(isSuccess)
	{
	}

	T			m_value;
	int			m_matchBegin;
	int			m_matchEnd;
	TCursor		m_remainder;
	bool		m_isSuccess;
};

template<typename T>
class Option
{
public:
	// TODO: move

	static Option Some(const T& value) { return Option(value); }
	static Option None() { return Option(); }

	bool IsEmpty() const { return m_empty; }
	const T& GetValue() const { return m_value; }

private:
	Option(const T& value)
		: m_vale(value)
		, m_empty(false)
	{}
	Option()
		: m_vale()
		, m_empty(true)
	{}

	T		m_vale;
	bool	m_empty;
};

struct ParserCursorConditional
{
	struct Always
	{
		template<typename T>
		bool operator()(const T& value) const
		{
			return true;
		}
	};
};

template<typename TConditional = ParserCursorConditional::Always>
class GenericParserCursor
{
public:
	typedef TConditional FilterType;

public:
	GenericParserCursor()
		: m_tokenList(nullptr)
		, m_position(0)
		, m_cond()
	{}

	// パース開始時の初期化用
	GenericParserCursor(const ln::parser::TokenList* tokenList)
		: m_tokenList(tokenList)
		, m_position(0)
		, m_cond()
	{
	}

	// パース開始時の初期化用
	GenericParserCursor(const ln::parser::TokenList* tokenList, int position)
		: m_tokenList(tokenList)
		, m_position(position)
		, m_cond()
	{
	}

	GenericParserCursor(const GenericParserCursor& obj)
		: m_tokenList(obj.m_tokenList)
		, m_position(obj.m_position)
		, m_cond(obj.m_cond)
	{
	}

	GenericParserCursor& operator=(const GenericParserCursor& obj)
	{
		m_tokenList = obj.m_tokenList;
		m_position = obj.m_position;
		m_cond = obj.m_cond;
		return *this;
	}

	bool IsEof() const
	{
		return GetCurrentValue().IsEof();
	}

	const ln::parser::Token& GetCurrentValue() const	// TODO: Current Token のほうがいいかな。Result の value とかぶって見える
	{
		return m_tokenList->GetAt(m_position);
	}

	int GetPosition() const
	{
		return m_position;
	}

	GenericParserCursor Cuing() const
	{
		int pos = m_position;
		while (!m_cond(m_tokenList->GetAt(pos)))
		{
			++pos;
		};
		return GenericParserCursor(m_tokenList, pos);
	}

	GenericParserCursor Advance() const
	{
		if (m_position == m_tokenList->GetCount())
		{
			LN_THROW(0, ln::InvalidOperationException, "end of source.");
		}

		int pos = m_position;
		do
		{
			++pos;
		} while (pos < m_tokenList->GetCount() && !m_cond(m_tokenList->GetAt(pos)));

		return GenericParserCursor(m_tokenList, pos);
	}

private:
	const ln::parser::TokenList*	m_tokenList;
	int								m_position;
	TConditional					m_cond;

	template<class TCursor>
	friend class GenericParserContext;
};

// 1つのパーサ関数の実行状態を管理する。
// インスタンスはパーサ関数の呼び出し時にコピーされる。
template<typename TCursor>
class GenericParserContext
{
public:
	GenericParserContext(const TCursor& input)
		: m_start(input)
		, m_current(input)
		, filter(m_current.m_cond)
	{
	}

	GenericParserContext(const GenericParserContext& obj)
		: m_start(obj.m_current)
		, m_current(obj.m_current)
		, filter(m_current.m_cond)
	{
	}

	TCursor GetNext() const
	{
		return m_current.Advance();
	}

	const parser::Token& GetCurrentValue() const
	{
		return m_current.GetCurrentValue();
	}

	const TCursor& GetStartCursor() const
	{
		return m_start;
	}

	int GetStartPosition() const
	{
		return m_start.GetPosition();
	}

	int GetLastMatchEndPosition() const
	{
		return m_last.GetPosition();
	}

	const TCursor& GetCurrentCursor() const
	{
		return m_current;
	}

	template<typename T>
	void Next(const GenericParserResult<T, TCursor>& result/*const TCursor& newPos*/)
	{
		m_last = m_current;
		m_current = result.GetRemainder();
	}

	template<typename T>
	GenericParserResult<T, TCursor> Success(const T& value)
	{
		return GenericParserResult<T, TCursor>::Success(value, GetStartPosition(), GetLastMatchEndPosition(), m_current);
	}

	detail::ParserFailure<TCursor> Fail()
	{
		detail::ParserFailure<TCursor> failer;
		failer.remainder = GetCurrentCursor();
		return failer;
	}

private:
	TCursor		m_start;
	TCursor		m_current;
	TCursor		m_last;

public:
	typename TCursor::FilterType&	filter;
};


template<typename TValue, typename TCursor, typename TContext>
class GenericParser : public ln::Delegate<GenericParserResult<TValue, TCursor>(TContext)>
{
public:
	template<typename TParserFunc>
	GenericParser(TParserFunc func)
		: ln::Delegate<GenericParserResult<TValue, TCursor>(TContext)>(func)
	{
	}

	GenericParser operator||(const GenericParser& second) const
	{
		return ParseLib<TCursor>::Or(*this, second);
	}
};




template<typename TParserCursor>
class ParseLib
{
public:
	using ValueT = ln::parser::Token;	// TODO: TokenT のほうがいいかな

	using ParserContext = GenericParserContext<TParserCursor>;

	template<typename TValue>
	using Parser = GenericParser<TValue, TParserCursor, ParserContext>;

	template<typename TValue>
	using ParserResult = GenericParserResult<TValue, TParserCursor>;

	using ParserCursor = TParserCursor;



	static Parser<ValueT> TokenChar(char ch)
	{
		return [ch](ParserContext input)
		{
			if (input.GetCurrentValue().EqualChar(ch))
				return ParserResult<ValueT>::Success(input.GetCurrentValue(), input.GetStartPosition(), input.GetStartPosition() + 1, input.GetNext());
			return ParserResult<ValueT>::Fail(input.GetCurrentCursor());	// TODO: メッセージあるとよい
		};
	}

	static Parser<ValueT> TokenString(const char* str_)
	{
		String str = str_;
		return [str](ParserContext input)
		{
			if (input.GetCurrentValue().EqualString(str.c_str(), str.GetLength()))
				return ParserResult<ValueT>::Success(input.GetCurrentValue(), input.GetStartPosition(), input.GetStartPosition() + 1, input.GetNext());
			return ParserResult<ValueT>::Fail(input.GetCurrentCursor());	// TODO: メッセージあるとよい
		};
	}

	static Parser<ValueT> Token(ln::parser::CommonTokenType type)
	{
		return [type](ParserContext input)
		{
			if (input.GetCurrentValue().GetCommonType() == type)
				return ParserResult<ValueT>::Success(input.GetCurrentValue(), input.GetStartPosition(), input.GetStartPosition() + 1, input.GetNext());
			return ParserResult<ValueT>::Fail(input.GetCurrentCursor());	// TODO: メッセージあるとよい
		};
	}

	static Parser<ValueT> Token(ln::parser::CommonTokenType type, char ch)
	{
		return [type, ch](ParserContext input)
		{
			if (input.GetCurrentValue().GetCommonType() == type && input.GetCurrentValue().EqualChar(ch))
				return ParserResult<ValueT>::Success(input.GetCurrentValue(), input.GetStartPosition(), input.GetStartPosition() + 1, input.GetNext());
			return ParserResult<ValueT>::Fail(input.GetCurrentCursor());
		};
	}

	static Parser<ValueT> Token(ln::parser::CommonTokenType type, const char* string, int len)
	{
		return [type, string, len](ParserContext input)
		{
			if (input.GetCurrentValue().GetCommonType() == type && input.GetCurrentValue().EqualString(string, len))
				return ParserResult<ValueT>::Success(input.GetCurrentValue(), input.GetStartPosition(), input.GetStartPosition() + 1, input.GetNext());
			return ParserResult<ValueT>::Fail(input.GetCurrentCursor());
		};
	}

	// 0回以上の繰り返し
	template<typename T>
	static Parser<ln::Array<T>> Many(const Parser<T>& parser)
	{
		return [parser](ParserContext input)
		{
			ln::Array<T> list;
			auto r = parser.Call(input);
			//input = r.GetRemainder();

			while (r.IsSucceed())
			{
				list.Add(r.GetValue());
				r = parser.Call(r.GetRemainder());
				//input = r.GetRemainder();
			}
			return ParserResult<ln::Array<T>>::Success(list, input.GetStartPosition(), r.GetMatchEnd(), r.GetRemainder());
		};
	}

	template<typename T>
	static Parser<T> Or(const Parser<T>& first, const Parser<T>& second)
	{
		//Parser<T> second(second_);
		return [first, second](ParserContext input)
		{
			auto fr = first.Call(input);
			if (fr.IsFailed())
			{
				return second.Call(input);
			}
			return fr;
		};
	}

	template<typename T>
	static Parser<Option<T>> Optional(const Parser<T>& parser)
	{
		return [parser](ParserContext input)
		{
			auto r = parser.Call(input);
			if (r.IsSucceed())
			{
				return ParserResult<Option<T>>::Success(Option<T>::Some(r.GetValue()), input.GetStartPosition(), r.GetMatchEnd(), r.GetRemainder());
			}
			return ParserResult<Option<T>>::Success(Option<T>::None(), input.GetStartPosition(), input.GetStartPosition(), input.GetStartCursor());
		};
	}

	// term までをマッチの範囲とし、
	// ターミネータを result に含む
	//template<typename T>
	//static Parser<ln::Array<T>> UntilPrev(const Parser<T>& term)
	//{
	//	return [term](ParserContext input)
	//	{
	//		ln::Array<T> list;
	//		auto lastPos = input.GetCurrentCursor();
	//		int lastMatchEnd = input.GetStartPosition();
	//		auto r = term.Call(input);
	//		auto lastResult = r;

	//		while (r.IsFailed())
	//		{
	//			list.Add(r.GetValue());
	//			lastResult = r;
	//			lastPos = r.GetRemainder();
	//			lastMatchEnd = r.GetMatchEnd();

	//			auto next = r.GetRemainder().Advance();
	//			if (next.IsEof()) return ParserResult<ln::Array<T>>::Fail(list, input.GetCurrentCursor());

	//			r = term.Call(next);
	//		}

	//		// TODO: ストリーム末尾までfailedだったらパース失敗

	//		// 次は lastPos から読み取り始める
	//		return ParserResult<ln::Array<T>>::Success(list, input.GetStartPosition(), lastMatchEnd, lastPos.Advance());
	//	};
	//}

	// term までをマッチの範囲とし、
	// ターミネータを result に含む
	template<typename T>
	static Parser<ln::Array<T>> UntilMore(const Parser<T>& term)
	{
		return [term](ParserContext input)
		{
			ln::Array<T> list;
			auto r = term.Call(input);
			auto lastResult = r;

			while (r.IsFailed())
			{
				list.Add(r.GetValue());
				lastResult = r;

				auto next = r.GetRemainder().Advance();
				if (next.IsEof()) return ParserResult<ln::Array<T>>::Fail(input.GetCurrentCursor());

				r = term.Call(next);
			}

			return ParserResult<ln::Array<T>>::Success(list, input.GetStartPosition(), r.GetMatchEnd(), r.GetRemainder());
		};
	}

	//template<typename T>
	//static Parser<ValueT> Except(const Parser<T>& except)
	//{
	//	return [except](ParserContext input)
	//	{
	//		auto r = except.Call(input);
	//		if (r.IsSucceed())
	//			return ParserResult<ValueT>::Fail(input.GetCurrentCursor());	// TODO: メッセージあるとよい
	//		return ParserResult<ValueT>::Success(input.GetCurrentValue(), input.GetStartPosition(), input.GetStartPosition() + 1, input.GetNext());
	//	};
	//}
	

	template<typename T>
	static Parser<ln::Array<ValueT>> Until(const Parser<T>& term)
	{
		return [term](ParserContext input)
		{
			ln::Array<ValueT> list;
			auto r = term.Call(input);

			while (r.IsFailed())
			{
				list.Add(r.GetRemainder().GetCurrentValue());

				auto next = r.GetRemainder().Advance();
				if (next.IsEof()) return ParserResult<ln::Array<T>>::Fail(input.GetCurrentCursor());
				r = term.Call(next);
			}

			return ParserResult<ln::Array<ValueT>>::Success(list, input.GetStartPosition(), r.GetMatchEnd(), r.GetRemainder());
		};
	}

	// term までをマッチの範囲とし、
	// term の結果を返す。
	// それまでのトークンは破棄する。
	template<typename T>
	static Parser<T> SkipTill(const Parser<T>& term)
	{
		return [term](ParserContext input)
		{
			auto r = term.Call(input);
			while (r.IsFailed())
			{
				auto next = r.GetRemainder().Advance();
				if (next.IsEof()) return ParserResult<T>::Fail(input.GetCurrentCursor());
				r = term.Call(next);
			}

			return ParserResult<T>::Success(r.GetValue(), input.GetStartPosition(), r.GetMatchEnd(), r.GetRemainder());
		};
	}

	template<typename T>
	static ParserResult<T> TryParse(const Parser<T>& parser, const ln::parser::TokenList* tokenList)
	{
		TParserCursor input(tokenList);
		ParserResult<T> result = parser.Call(input.Cuing());
		if (result.IsFailed())
			LN_THROW(0, ln::InvalidFormatException);
		return result;
	}
};

} // namespace combinators
} // namespace ln
