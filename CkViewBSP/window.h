//
//	Author:	Michael Cameron
//	Email:	chronokun@hotmail.com
//

#pragma once

#ifndef __WINDOW_H__
#define __WINDOW_H__

// Preprocessor definitions
#define WINDOW_CLASS_NAME "CKENGINECLASS"

// Non-Member Function Prototypes
LRESULT CALLBACK WindowProc(HWND _hWnd, UINT _msg, WPARAM _wParam, LPARAM _lParam);
const HWND CreateAndRegisterWindow(const HINSTANCE _khInstance, const int32_t _kiWidth, const int32_t _kiHeight, const LPCSTR _kpcTitle);

// Data Prototypes
class CEngineCore;

class CWindow
{
	// Member Variables
protected:
	HINSTANCE m_hInstance;

public:
	HWND m_hWnd;
	int32_t m_iWidth;
	int32_t m_iHeight;

	// Member Functions
protected:
	//

public:
	~CWindow();
	const bool Initialize(const HINSTANCE _khApplicationInstance, const int32_t _kiWidth, const int32_t _kiHeight, const LPCSTR _kpcTitle);

};

#endif