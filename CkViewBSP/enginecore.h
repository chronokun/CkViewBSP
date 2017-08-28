//
//	Author:	Michael Cameron
//	Email:	chronokun@hotmail.com
//

#pragma once

#ifndef __ENGINECORE_H__
#define __ENGINECORE_H__

// Prototypes
class CWindow;
class CRenderer;
class CClient;
class CTimer;
class CInput;

class CEngineCore
{
	// Member Variables
protected:
	static CEngineCore s_EngineCore;
	HINSTANCE m_hApplicationInstance;

public:
	CWindow* m_pWindow;
	CTimer* m_pTimer;
	CRenderer* m_pRenderer;
	CClient* m_pClient;
	CInput* m_pInput;

	bool m_bActive;

	// Member Functions
protected:
	CEngineCore();

public:
	static CEngineCore& GetInstance();
	~CEngineCore();
	const bool Initialize();
	const int Start(const HINSTANCE _khInstance, const char* _kpCmdLine);
	void Pause();
	void Unpause();
	const float Update(float _fStartTime);
	void Terminate();

};

#endif