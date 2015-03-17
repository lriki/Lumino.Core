
#pragma once

namespace Lumino
{
namespace Platform
{
class Application;

class Internal
{
public:
	/// 最初に作成された Application クラスのポインタ
	static Application* ApplicationInstance;
};

}
}
