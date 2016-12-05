
#pragma once
#include <iterator>
#include <list>

LN_NAMESPACE_BEGIN
namespace tr {


//template<template<typename...> class Provider,
//	typename T,
//	typename... TemplateArgs,
//	typename... ConstructorArgs>
//	StreamProviderPtr<T> make_stream_provider(ConstructorArgs&&... args) {
//	return StreamProviderPtr<T>(
//		new Provider<T, TemplateArgs...>(
//		std::forward<ConstructorArgs>(args)...));
//}
//



template<typename Itr>
using IteratorType = typename std::iterator_traits<Itr>::value_type;





template<typename T>
struct StreamProvider
{
public:
	virtual ~StreamProvider() = default;
	struct Iterator;

	virtual T* Get() = 0;

	bool Advance()
	{
		//try {
		return AdvanceImpl();
		//}
		//catch (stream::StopStream& stop) {
		//	return false;
		//}
		//catch (...) {
		//	throw;
		//}
	}

	Iterator begin();
	Iterator end();

protected:
	virtual bool AdvanceImpl() = 0;
};



template<typename T>
using StreamProviderPtr = std::shared_ptr<StreamProvider<T>>;







template<typename T>
struct StreamProvider<T>::Iterator {

public:
	T& operator* ();
	Iterator& operator++ ();
	Iterator operator++ (int);
	bool operator== (Iterator& other);
	bool operator!= (Iterator& other);

	friend struct StreamProvider<T>;

private:
	enum class State {
		Initial,
		Consumed,
		Iterating,
		End
	};

	static Iterator begin(StreamProvider<T>* source);
	static Iterator iterating(StreamProvider<T>* source, std::shared_ptr<T> value);
	static Iterator end(StreamProvider<T>* source);

	Iterator(StreamProvider<T>* source, State state, /*std::shared_ptr<T>*/T* value);

	void update_initial();
	void check_consumed(const std::string& op) const;

	StreamProvider<T>* source_;
	State state_;
	//std::shared_ptr<T> current_;
	T* current_ = nullptr;
};

template<typename T>
typename StreamProvider<T>::Iterator
StreamProvider<T>::Iterator::begin(StreamProvider<T>* source) {
	return Iterator(source, State::Initial, nullptr);
}

template<typename T>
typename StreamProvider<T>::Iterator
StreamProvider<T>::Iterator::iterating(StreamProvider<T>* source,
std::shared_ptr<T> value) {
	return Iterator(source, State::Iterating, value);
}

template<typename T>
typename StreamProvider<T>::Iterator
StreamProvider<T>::Iterator::end(StreamProvider<T>* source) {
	return Iterator(source, State::End, nullptr);
}

template<typename T>
StreamProvider<T>::Iterator::Iterator(StreamProvider<T>* source, State state,
	/*std::shared_ptr<T>*/T* value)
	: source_(source), state_(state), current_(value) {}

template<typename T>
T& StreamProvider<T>::Iterator::operator* () {
	update_initial();
	return *current_;
}

template<typename T>
typename StreamProvider<T>::Iterator&
StreamProvider<T>::Iterator::operator++() {
	update_initial();
	check_consumed("prefix increment");
	if (source_->Advance()) {
		current_ = source_->Get();
	}
	else {
		state_ = State::End;
		//current_.reset();
		current_ = nullptr;
	}
	return *this;
}

template<typename T>
typename StreamProvider<T>::Iterator
StreamProvider<T>::Iterator::operator++(int) {
	update_initial();
	check_consumed("postfix increment");
	Iterator result(nullptr, State::Consumed, current_);
	if (source_->advance()) {
		current_ = source_->get();
	}
	else {
		state_ = State::End;
		current_.reset();
	}
	return result;
}

template<typename T>
bool StreamProvider<T>::Iterator::operator== (Iterator& other) {
	update_initial();
	check_consumed("equality check");
	other.update_initial();
	other.check_consumed();
	if (state_ == State::End) {
		return other.state_ == State::End;
	}
	if (other.state_ == State::End) {
		return false;
	}
	return source_ == other.source_ && current_ == other.current_;
}

template<typename T>
bool StreamProvider<T>::Iterator::operator!= (Iterator& other) {
	update_initial();
	check_consumed("inequality check");
	other.update_initial();
	other.check_consumed("inequality check");
	if (state_ == State::End) {
		return other.state_ != State::End;
	}
	if (other.state_ == State::End) {
		return true;
	}
	return source_ != other.source_ || current_ != other.current_;
}

template<typename T>
void StreamProvider<T>::Iterator::update_initial() {
	if (state_ == State::Initial) {
		if (source_->Advance()) {
			current_ = source_->Get();
			state_ = State::Iterating;
		}
		else {
			//current_.reset();
			current_ = nullptr;
			state_ = State::End;
		}
	}
}

template<typename T>
void StreamProvider<T>::Iterator::check_consumed(const std::string& op) const {
	if (state_ == State::Consumed) {
		assert(0);
		//throw stream::ConsumedIteratorException(op);
	}
}












template<typename T>
typename StreamProvider<T>::Iterator
StreamProvider<T>::begin() {
	return Iterator::begin(this);
}

template<typename T>
typename StreamProvider<T>::Iterator
StreamProvider<T>::end() {
	return Iterator::end(this);
}





//==============================================================================
template<typename T, typename Itr>
class IteratorProvider : public StreamProvider<T>
{
public:
	IteratorProvider(Itr begin, Itr end)
		: current_(begin), end_(end) {}

	virtual T* Get() override
	{
		return &(*current_);
	}

	bool AdvanceImpl() override
	{
		if (first_) {
			first_ = false;
			return current_ != end_;
		}
		++current_;
		return current_ != end_;
	}

private:
	bool first_ = true;
	Itr current_;
	Itr end_;

};



//==============================================================================
template<typename T, typename Predicate>
class Filter : public StreamProvider<T> {

public:
	Filter(const StreamProviderPtr<T>& source, const Predicate& predicate/*Predicate&& predicate*/)
		: source_(std::move(source)), predicate_(predicate) {}

	T* Get() override {
		return current_;
	}

	bool AdvanceImpl() override {
		while (source_->Advance()) {
			current_ = source_->Get();
			if (predicate_(*current_)) {
				return true;
			}
		}
		current_ = nullptr;
		return false;
	}

private:
	StreamProviderPtr<T> source_;
	Predicate predicate_;
	T* current_ = nullptr;
};



//==============================================================================
template<typename T>
class Concatenate : public StreamProvider<T> {

public:
	template<typename Iterator>
	Concatenate(Iterator begin, Iterator end) : sources_(begin, end) {}

	Concatenate(const StreamProviderPtr<T>& first, const StreamProviderPtr<T>& second)
	{
		sources_.push_back(first);
		sources_.push_back(second);
	}

	T* Get() override { return current_; }

	bool AdvanceImpl() override
	{
		while (!sources_.empty()) {
			auto& provider = sources_.front();
			if (provider->Advance()) {
				current_ = provider->Get();
				return true;
			}
			sources_.pop_front();
		}
		current_ = nullptr;
		return false;
	}

	void concat(StreamProviderPtr<T>&& source) {
		sources_.push_back(std::move(source));
	}

private:
	std::list<StreamProviderPtr<T>> sources_;
	T* current_;

};

//==============================================================================
template<typename T, typename Transform, typename In>
class Map : public StreamProvider<T> {

public:
	Map(StreamProviderPtr<In> source, Transform transform)
		: source_(std::move(source)), transform_(transform) {}

	T* Get() override {
		return &current_;
	}

	bool AdvanceImpl() override {
		if (source_->Advance()) {
			current_ = transform_(*source_->Get());
			return true;
		}
		current_ = nullptr;
		return false;
	}

private:
	StreamProviderPtr<In> source_;
	Transform transform_;
	T current_;

};

/**
	@brief		
*/
template<typename T>
class Enumerator
{
public:
	using iterator = typename StreamProvider<T>::Iterator;
	
	iterator begin() { return m_source->begin(); }

	iterator end() { return m_source->end(); }

	template<typename Iterator>
	Enumerator(Iterator begin, Iterator end)
		: m_source(StreamProviderPtr<T>( LN_NEW IteratorProvider<T, Iterator>(begin, end)))
	{}

	
	//template<typename TPredicate>
	//Enumerator<T> Where(TPredicate pred)
	//{
	//	return Enumerator<T>(new Filter(m_source, pred));
	//}

	template<typename TPredicate>
	Enumerator<T>& Where(TPredicate pred)
	{
		m_source.reset(LN_NEW Filter<T, TPredicate>(m_source, pred));
		return *this;
	}

	Enumerator<T>& Join(Enumerator<T>&& tail)
	{
		m_source.reset(LN_NEW Concatenate<T>(m_source, tail.m_source));
		return *this;
	}

	template<typename TTransform, typename TResult = std::result_of_t<TTransform(T&&)>>
	Enumerator<TResult> Select(TTransform transform)
	{
		Enumerator<TResult> e(std::make_shared<Map<TResult, TTransform, T>>(m_source, transform));
		//e.m_source = (LN_NEW Map<TResult, TTransform, T>(m_source, transform));
		return e;
	}

	Enumerator(Enumerator<T>&& other)	// vs2013 default ‚É‚Å‚«‚È‚¢
		: m_source(std::move(other.m_source))
	{}

	Enumerator(const Enumerator<T>& other) = default;

	Enumerator<T>& operator= (const Enumerator<T>& other) = default;


	Enumerator()
	{}


	Enumerator(StreamProviderPtr<T> source)
		: m_source(std::move(source))
	{}
	
private:

	StreamProviderPtr<T> m_source;
};

/**
	@brief		
*/
class MakeEnumerator
{
public:

	template<typename Iterator>
	static Enumerator<IteratorType<Iterator>> from(Iterator begin, Iterator end)
	{
		return Enumerator<IteratorType<Iterator>>(begin, end);
	}
};

namespace q
{

}

} // namespace tr
LN_NAMESPACE_END
