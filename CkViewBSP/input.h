#pragma once

#ifndef __INPUT_H__
#define __INPUT_H__

class CInput
{
	// Member Variables
protected:
	//

public:
	bool m_bKeys[256];
	bool m_bPresses[256];
	float m_fViewX;
	float m_fViewY;

	int m_iMouseX;
	int m_iMouseY;

	bool m_bLeftButton;
	bool m_bRightButton;

	// Member Functions
protected:
	//

public:
	CInput();
	const bool Initialize();
	void Update();

};

#endif