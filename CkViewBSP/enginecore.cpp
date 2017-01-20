//
//	Author:	Michael Cameron
//	Email:	chronokun@hotmail.com
//

// Libraries include
#include "libraries.h"
// This include
#include "enginecore.h"
// Local includes
#include "window.h"
#include "timer.h"
#include "renderer.h"
#include "client.h"
#include "input.h"

// Static Variables
CEngineCore CEngineCore::s_EngineCore;

CEngineCore::CEngineCore()
{
	//
}

CEngineCore& CEngineCore::GetInstance()
{
	return(s_EngineCore);
}

CEngineCore::~CEngineCore()
{
	delete this->m_pWindow;
	delete this->m_pTimer;
	delete this->m_pRenderer;
	delete this->m_pClient;
	delete this->m_pInput;
}

const int CEngineCore::Start(const HINSTANCE _khInstance)
{
	MSG msg;
	bool bDone = false;
	float fTimeLeft = 0.0f;

	if(this->Initialize())
	{
		this->m_pTimer->Reset();
		while(!bDone)
		{
			if(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
			{
				// Process Message Queue
				if(msg.message == WM_QUIT)
				{
					bDone = true;
				}
				else
				{
					TranslateMessage(&msg);
					DispatchMessage(&msg);
				}
			}
			else
			{
				if(this->m_bActive)
				{
					// Execute Main Gameloop

					// Update
					fTimeLeft = this->Update(fTimeLeft);

					// Draw
					this->m_pRenderer->Draw(fTimeLeft);

					// Swap Buffers
					this->m_pRenderer->BufferSwap();

					// Done
					Sleep(0);
				}
			}
		}
		this->Terminate();
	}
	else
	{
		return(1);
	}

	return(static_cast<int>(msg.wParam));
}

void CEngineCore::Terminate()
{
	this->m_pClient->Terminate();
	this->m_pRenderer->Terminate();
}

const float CEngineCore::Update(float _fStartTime)
{
	float fTimeLeft = _fStartTime;

	this->m_pTimer->Process();
	fTimeLeft += (float)this->m_pTimer->GetDeltaTime();

	fTimeLeft = std::min(fTimeLeft, 0.1f);

	fTimeLeft = this->m_pClient->Update(fTimeLeft);

	this->m_pInput->Update();

	return(fTimeLeft);
}

const bool CEngineCore::Initialize()
{
	// Window
	this->m_pWindow = new CWindow();
	if(this->m_pWindow != nullptr)
	{
		if(!this->m_pWindow->Initialize(this->m_hApplicationInstance, 1280, 720, "CkViewBSP"))
		{
			return(false);
		}
	}
	else
	{
		return(false);
	}

	// Timer
	this->m_pTimer = new CTimer();
	if(this->m_pTimer != nullptr)
	{
		if(!this->m_pTimer->Initialize())
		{
			return(false);
		}
	}
	else
	{
		return(false);
	}

	// Renderer
	this->m_pRenderer = new CRenderer;
	if(this->m_pRenderer != nullptr)
	{
		if(!this->m_pRenderer->Initialize(this->m_pWindow->m_hWnd))
		{
			return(false);
		}
	}
	else
	{
		return(false);
	}

	// Input
	this->m_pInput = new CInput();
	if(this->m_pInput != nullptr)
	{
		if(!this->m_pInput->Initialize())
		{
			return(false);
		}
	}
	else
	{
		return(false);
	}

	// Client
	this->m_pClient = new CClient;
	if(this->m_pClient != nullptr)
	{
		if(!this->m_pClient->Initialize())
		{
			return(false);
		}
	}
	else
	{
		return(false);
	}

	// Set variable values
	this->m_bActive = true;

	return(true);
}

void CEngineCore::Pause()
{
	if(this->m_pTimer != nullptr)
	{
		this->m_pTimer->Pause();
	}
	this->m_bActive = false;
}

void CEngineCore::Unpause()
{
	if(this->m_pTimer != nullptr)
	{
		this->m_pTimer->Unpause();
	}
	this->m_bActive = true;
}