//
//	Author:	Michael Cameron
//	Email:	chronokun@hotmail.com
//

#pragma once

#ifndef __RENDERER_H__
#define __RENDERER_H__

// Local Includes
#include "shader.h"
#include "drawobject_surface.h"

// Prototypes
struct TSurface;

// Types
struct TPatchData
{
	float position	[3][3][4];
	float texcoord	[3][3][4];
	float normal	[3][3][4];
	float lightmap	[3][3][4];
	float color		[3][3][4];
};

class CRenderer
{
	// Member Variables
protected:
	GLuint m_uiVAO;
	GLuint m_uiPatchBufferSSBO;
	GLuint m_uiPatchVAO;
	//
	std::vector<int32_t> m_VisibleLeafs;
	int32_t m_iVisibleLeafCount;
	std::vector<int32_t> m_VisibleFaces;
	int32_t m_iVisibleFaceCount;
	int32_t m_iCurrentLeaf;
	std::vector<uint8_t> m_FaceVisibility;
	std::vector<uint8_t> m_LeafVisibility;
	std::vector<GLuint> m_LightmapTextures;
	//
	int m_iNotCulled;
	int m_iCulledByFrustum;
	int m_iCulledByPVS;
	//
	NVGcontext* m_pNVG;
	//
	std::vector<float> m_FrameTimes;
	size_t m_FrameNumber;
	float m_fLastFrameTime;
	//
	GLuint m_BufferObjects[7];
	bool m_bInitializedBufferObjects;

public:
	HDC m_hDC;
	HGLRC m_hGLRC;
	HWND m_hWnd;
	//
	std::map<std::string, CShader> m_Shaders;
	std::vector<CDrawObject_Surface> m_DrawObject_Surfaces;

	// Member Functions
protected:
	const bool SetupDC(const HWND _khWnd);
	const bool SetupGLRC();
	const bool SetupGLFunctions();
	const bool SetupShaders();
	//
	void DrawSurfaces();
	//
	void GetLeafFaces(const int32_t _kiLeaf);
	void FindVisibleLeafs(const int32_t _kiCurrentLeaf);

public:
	~CRenderer();
	const bool Initialize(const HWND _khWnd);
	void Draw(const float _kfExtrapolationTime);
	void BufferSwap();
	void Terminate();
	void LoadMap();

};

int32_t FindLeaf(const TVector3f& _krPoint);

bool IsVisible(const int32_t _kiVisCluster, const int32_t _kiTestCluster);

bool IsVisibleFromFrustum(const int32_t _kiLeaf);

void FindVisibleLeafs(const int32_t _kiCurrentLeaf, std::vector<int32_t>& _rVisibleLeafs, int32_t& _riVisibleLeafCount);

#endif
