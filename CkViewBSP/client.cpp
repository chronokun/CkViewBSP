//
//	Author:	Michael Cameron
//	Email:	chronokun@hotmail.com
//

// Libraries Include
#include "libraries.h"
// This Include
#include "client.h"
// Local Includes
#include "enginecore.h"
#include "renderer.h"
#include "input.h"
#include "utilities.h"

const bool CClient::Initialize()
{
	this->m_bDoPVS = false;
	this->m_bWireframe = false;

	this->m_fElapsedTime = 0.0f;

	this->m_CameraPosition = TVector3f{0.0f, 56.0f, 0.0f};
	this->m_CameraOrientation = IdentityQuaternion(TVector4f());

	return(true);
}

bool CClient::ProcessCommandLine(const char* _kpCmdLine)
{
	if(!LoadQBSPFile(_kpCmdLine, this->m_BSP))
	{
		return(false);
	}
	this->filename = _kpCmdLine;

	CEngineCore::GetInstance().m_pRenderer->LoadMap();

	return true;
}

void CClient::Reload()
{
	LoadQBSPFile(this->filename.c_str(), this->m_BSP);
	CEngineCore::GetInstance().m_pRenderer->LoadMap();
}

const float CClient::Update(const float _kfDeltaTick)
{
	// Restrict tilt
	if(CEngineCore::GetInstance().m_pInput->m_fViewY > s_kfPi/2.0f)
	{
		CEngineCore::GetInstance().m_pInput->m_fViewY = s_kfPi/2.0f;
	}
	if(CEngineCore::GetInstance().m_pInput->m_fViewY < -s_kfPi/2.0f)
	{
		CEngineCore::GetInstance().m_pInput->m_fViewY = -s_kfPi/2.0f;
	}

	// Restrict pan
	if(CEngineCore::GetInstance().m_pInput->m_fViewY > s_kfPi)
	{
		CEngineCore::GetInstance().m_pInput->m_fViewY -= s_kfPi;
	}
	if(CEngineCore::GetInstance().m_pInput->m_fViewY < -s_kfPi)
	{
		CEngineCore::GetInstance().m_pInput->m_fViewY += s_kfPi;
	}

	// Rotate Camera
	TVector4f Tilt = AxisAngleQuaternion(TVector4f(), TVector3f{1.0f, 0.0f, 0.0f}, -CEngineCore::GetInstance().m_pInput->m_fViewY);
	TVector4f Pan = AxisAngleQuaternion(TVector4f(), TVector3f{0.0f, 1.0f, 0.0f}, -CEngineCore::GetInstance().m_pInput->m_fViewX);

	this->m_CameraOrientation = QuaternionProduct(TVector4f(), Pan, Tilt);

	// Move Camera
	TVector3f Forward = QuaternionRotate(TVector3f(), TVector3f{0.0f, 0.0f, 1.0f}, this->m_CameraOrientation);
	TVector3f Right = QuaternionRotate(TVector3f(), TVector3f{1.0f, 0.0f, 0.0f}, this->m_CameraOrientation);
	if(CEngineCore::GetInstance().m_pInput->m_bKeys['W'])
	{
		this->m_CameraPosition = Add(TVector3f(), this->m_CameraPosition, ScalarMultiply(TVector3f(), Forward, -_kfDeltaTick * 320.0f));
	}
	if(CEngineCore::GetInstance().m_pInput->m_bKeys['S'])
	{
		this->m_CameraPosition = Add(TVector3f(), this->m_CameraPosition, ScalarMultiply(TVector3f(), Forward, _kfDeltaTick * 320.0f));
	}
	if(CEngineCore::GetInstance().m_pInput->m_bKeys['D'])
	{
		this->m_CameraPosition = Add(TVector3f(), this->m_CameraPosition, ScalarMultiply(TVector3f(), Right, _kfDeltaTick * 320.0f));
	}
	if(CEngineCore::GetInstance().m_pInput->m_bKeys['A'])
	{
		this->m_CameraPosition = Add(TVector3f(), this->m_CameraPosition, ScalarMultiply(TVector3f(), Right, -_kfDeltaTick * 320.0f));
	}

	if(CEngineCore::GetInstance().m_pInput->m_bPresses['1'])
	{
		this->m_bDoPVS = !this->m_bDoPVS;
	}
	if(CEngineCore::GetInstance().m_pInput->m_bPresses['2'])
	{
		this->m_bWireframe = !this->m_bWireframe;
	}
	if(CEngineCore::GetInstance().m_pInput->m_bPresses['R'])
	{
		this->Reload();
	}

	this->m_fElapsedTime += _kfDeltaTick;

	return(0.0f);
}

void CClient::Terminate()
{
	//
}
