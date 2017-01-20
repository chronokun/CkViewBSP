// Libraries Include
#include "libraries.h"
// This Include
#include "input.h"
// Local Includes
#include "enginecore.h"
#include "window.h"

CInput::CInput()
{
	for(int i = 0; i < 256; ++i)
	{
		this->m_bKeys[i] = false;
		this->m_bPresses[i] = false;
	}
	this->m_fViewX = 0.0f;
	this->m_fViewY = 0.0f;

	this->m_bLeftButton = false;
	this->m_bRightButton = false;
}

const bool CInput::Initialize()
{
	// http://msdn.microsoft.com/en-us/library/ff543477%28v=vs.85%29.aspx
	RAWINPUTDEVICE RawInputDevice;
	RawInputDevice.usUsagePage = 0x01; // Input?
	RawInputDevice.usUsage = 0x02; // Mouse
	RawInputDevice.dwFlags = 0;
	RawInputDevice.hwndTarget = 0;

	if(!RegisterRawInputDevices(&RawInputDevice, 1, sizeof(RAWINPUTDEVICE)))
	{
		return(false);
	}

	return(true);
}

void CInput::Update()
{
	for(unsigned int i = 0; i < 256; ++i)
	{
		this->m_bPresses[i] = false;
	}
}