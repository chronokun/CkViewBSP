//
//	Author:	Michael Cameron
//	Email:	chronokun@hotmail.com
//

#pragma once

#ifndef __CLIENT_H__
#define __CLIENT_H__

// Constants
static const int s_kiFrameRate = 300;
static const float s_kfFrameInterval = 1.0f/(float)s_kiFrameRate;

// Local Includes
#include "surface.h"
#include "qbsp.h"

class CClient
{
	// Member Variables
protected:
	//

public:
	float m_fElapsedTime;
	std::vector<TSurface> m_Surfaces;

	TVector3f m_CameraPosition;
	TVector4f m_CameraOrientation;

	TQBSP m_BSP;

	bool m_bDoPVS;
	bool m_bWireframe;

	std::string filename;

	// Member Functions
protected:
	//

public:
	const bool Initialize();
	bool ProcessCommandLine(const char* _kpCmdLine);
	void Reload();
	const float Update(const float _kfDeltaTick);
	void Terminate();

};

#endif