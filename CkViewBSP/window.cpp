//
//	Author:	Michael Cameron
//	Email:	chronokun@hotmail.com
//

// Libraries Include
#include "libraries.h"
// This Include
#include "window.h"
// Local Includes
#include "enginecore.h"
#include "input.h"

// Static Variables
static bool s_bGrabedCursor = false;
static POINTS s_MouseReturnPoint;

LRESULT CALLBACK WindowProc(HWND _hWnd, UINT _msg, WPARAM _wParam, LPARAM _lParam)
{
	switch(_msg)
	{
		case WM_ACTIVATE:
			{
				if(!HIWORD(_wParam))
				{
					CEngineCore::GetInstance().Unpause();
				}
				else
				{
					CEngineCore::GetInstance().Pause();
				}
				return(0);
			}
		case WM_SYSCOMMAND:
			{
				switch(_wParam)
				{
					case SC_SCREENSAVE:
					case SC_MONITORPOWER:
					return(0);
				}
				break;
			}
		case WM_CLOSE:
			{
				DestroyWindow(_hWnd);
				return(0);
			}
		case WM_DESTROY:
			{
				PostQuitMessage(0);
				return(0);
			}
			case WM_CHAR:
			{
				char c = (char)(_wParam);
				if(c > 0 && c <= 255)
				{
					//ImGui::GetIO().AddInputCharacter(c);
				}
				return(0);
			}

		case WM_KEYDOWN:
			{
				CEngineCore::GetInstance().m_pInput->m_bKeys[_wParam] = true;
				CEngineCore::GetInstance().m_pInput->m_bPresses[_wParam] = true;
				//ImGui::GetIO().KeysDown[_wParam] = true;
				return(0);
			}
		case WM_KEYUP:
			{
				CEngineCore::GetInstance().m_pInput->m_bKeys[_wParam] = false;
				//ImGui::GetIO().KeysDown[_wParam] = false;
				return(0);
			}
		case WM_LBUTTONDOWN:
			{
				CEngineCore::GetInstance().m_pInput->m_bLeftButton = true;
				SetCapture(_hWnd);
				return(0);
			}
		case WM_LBUTTONUP:
			{
				CEngineCore::GetInstance().m_pInput->m_bLeftButton = false;
				ReleaseCapture();
				return(0);
			}

		case WM_RBUTTONDOWN:
			{
				CEngineCore::GetInstance().m_pInput->m_bRightButton = true;
				if(!s_bGrabedCursor)
				{
					s_MouseReturnPoint = MAKEPOINTS(_lParam);

					RECT WindowRect;
					GetClientRect(_hWnd, &WindowRect);
					POINT PT1 = { WindowRect.left, WindowRect.top };
					POINT PT2 = { WindowRect.right, WindowRect.bottom };
					ClientToScreen(_hWnd, &PT1);
					ClientToScreen(_hWnd, &PT2);
					SetRect(&WindowRect, PT1.x, PT1.y, PT2.x, PT2.y);
					ClipCursor(&WindowRect);

					ShowCursor(false);

					s_bGrabedCursor = true;
				}
				return(0);
			}

		case WM_RBUTTONUP:
			{
				CEngineCore::GetInstance().m_pInput->m_bRightButton = false;
				if(s_bGrabedCursor)
				{
					ClipCursor(nullptr);

					POINT MousePoint = { s_MouseReturnPoint.x, s_MouseReturnPoint.y };

					ClientToScreen(_hWnd, &MousePoint);

					SetCursorPos(MousePoint.x, MousePoint.y);

					ShowCursor(true);

					s_bGrabedCursor = false;
				}
				return(0);
			}

		case WM_MOUSEMOVE:
			{
				POINTS NewMousePts = MAKEPOINTS(_lParam);
				
				if(!s_bGrabedCursor)
				{
					CEngineCore::GetInstance().m_pInput->m_iMouseX = NewMousePts.x;
					CEngineCore::GetInstance().m_pInput->m_iMouseY = NewMousePts.y;
				}
				return(0);
			}
		case WM_INPUT:
			{
				UINT dwSize;

				GetRawInputData((HRAWINPUT)_lParam, RID_INPUT, NULL, &dwSize, sizeof(RAWINPUTHEADER));
				LPBYTE lpb = new BYTE[dwSize];
				if(lpb == nullptr)
				{
					return(0);
				}

				if( GetRawInputData((HRAWINPUT)_lParam, RID_INPUT, lpb, &dwSize, sizeof(RAWINPUTHEADER)) != dwSize )
				{
					return(0);
				}

				RAWINPUT* pRaw = (RAWINPUT*)lpb;

				if (pRaw->header.dwType == RIM_TYPEMOUSE) 
				{
					if(s_bGrabedCursor)
					{
						CEngineCore::GetInstance().m_pInput->m_fViewX += (float)(pRaw->data.mouse.lLastX) / 300.0f;
						CEngineCore::GetInstance().m_pInput->m_fViewY += (float)(pRaw->data.mouse.lLastY) / 300.0f;
					}
				}

				delete[] lpb;

				return(0);
			}
		case WM_SIZE:
			{
				CEngineCore::GetInstance().m_pWindow->m_iWidth = LOWORD(_lParam);
				CEngineCore::GetInstance().m_pWindow->m_iHeight = HIWORD(_lParam);
				return(0);
			}
	}
	return DefWindowProc(_hWnd, _msg, _wParam, _lParam);
}

const HWND CreateAndRegisterWindow(const HINSTANCE _khInstance, const int32_t _kiWidth, const int32_t _kiHeight, const LPCSTR _kpcTitle)
{
	WNDCLASSEX winclass;

	winclass.cbSize = sizeof(WNDCLASSEX);
	winclass.style = CS_HREDRAW | CS_VREDRAW;
	winclass.lpfnWndProc = WindowProc;
	winclass.cbClsExtra = 0;
	winclass.cbWndExtra = 0;
	winclass.hInstance = _khInstance;
	winclass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	winclass.hCursor = LoadCursor(NULL, IDC_ARROW);
	winclass.hbrBackground = static_cast<HBRUSH>(GetStockObject(NULL_BRUSH));
	winclass.lpszMenuName = NULL;
	winclass.lpszClassName = WINDOW_CLASS_NAME;
	winclass.hIconSm = LoadIcon(NULL, IDI_APPLICATION);

	if(!RegisterClassEx(&winclass))
	{
		// Failed to register.
		return(0);
	}

	// Compute window rectangle dimensions based on requested client area dimensions.
	const DWORD kdwStyle = WS_BORDER | WS_CAPTION | WS_SYSMENU | WS_VISIBLE | WS_SIZEBOX | WS_MAXIMIZEBOX | WS_MINIMIZEBOX;
	RECT R = { 0, 0, _kiWidth, _kiHeight };
    AdjustWindowRect(&R, kdwStyle, false);
	int iWidth  = R.right - R.left;
	int iHeight = R.bottom - R.top;

	HWND hwnd;
	hwnd = CreateWindowEx(	NULL,
							WINDOW_CLASS_NAME,
							_kpcTitle,
							kdwStyle,
							CW_USEDEFAULT, CW_USEDEFAULT,
							iWidth, iHeight,
							NULL,
							NULL,
							_khInstance,
							NULL);

	if(!hwnd)
	{
		// Failed to create.
		return(0);
	}

	return(hwnd);
}

CWindow::~CWindow()
{
	DestroyWindow(this->m_hWnd);
	UnregisterClass(WINDOW_CLASS_NAME, m_hInstance);
}

const bool CWindow::Initialize(const HINSTANCE _khApplicationInstance, const int32_t _kiWidth, const int32_t _kiHeight, const LPCSTR _kpcTitle)
{
	bool bSuccess = false;
	this->m_hInstance = _khApplicationInstance;
	this->m_hWnd = CreateAndRegisterWindow(_khApplicationInstance, _kiWidth, _kiHeight, _kpcTitle);
	if(this->m_hWnd != NULL)
	{
		bSuccess = true;
		m_iWidth = _kiWidth;
		m_iHeight = _kiHeight;
	}
	return(bSuccess);
}