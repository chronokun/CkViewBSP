//
//	Author:	Michael Cameron
//	Email:	chronokun@hotmail.com
//

// Libraries include
#include "libraries.h"
// Local includes
#include "enginecore.h"

int WINAPI WinMain(	HINSTANCE	_hInstance,
					HINSTANCE	_hPrevInstance,
					LPSTR		_lpCmdLine,
					int			_iCmdShow)
{
	return(CEngineCore::GetInstance().Start(_hInstance));
}