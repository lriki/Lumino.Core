
#pragma once

#include "JsonValue.h"

namespace Lumino
{
namespace Json
{

/*
	@brief	JSON �f�[�^�̃��[�g�v�f�ł��B
*/
template<typename TChar>
class JsonDocument
	: public JsonValue
{
public:
	void Parse();
	//void Load();

private:
};

} // namespace Json
} // namespace Lumino
