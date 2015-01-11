
#pragma once

#include "../Base/RefObject.h"
#include "../Base/Size.h"

namespace Lumino
{
namespace Platform
{
class Window;

class Application
	: public RefObject
{
public:

	static Application* Create();

public:
	
	/**
		@brief		メインウィンドウを作成します。
		@details	メインウィンドウは 1 つのアプリケーションにつき 1 つだけ作成できます。
					作成したウィンドウは GetMainWindow() で取得できます。
	*/
	virtual void CreateMainWindow(const TCHAR* title, Size clienSize, bool windowed = true, bool resizable = true) = 0;

	/**
		@brief		メインウィンドウを取得します。
	*/
	virtual Window* GetMainWindow() = 0;

	/**
		@brief	メッセージキューに現在あるメッセージをすべて処理します。
	*/
	virtual bool DoEvents() = 0;

	/**
		@brief	メッセージキューにアプリケーションの終了を通知します。
	*/
	virtual void Exit() = 0;

protected:
	Application() {}
	virtual ~Application() {}
};

} // namespace Platform
} // namespace Lumino
