//
//	Author:	Michael Cameron
//	Email:	chronokun@hotmail.com
//

#pragma once

#ifndef __DRAWOBJECT_SURFACE_H__
#define __DRAWOBJECT_SURFACE_H__

// Prototypes
struct TSurface;

class CDrawObject_Surface
{
	// Member Variables
protected:
	GLuint m_uiPositionsVBO;
	GLuint m_uiTexCoordsVBO;
	GLuint m_uiVAO;
	//
	GLuint m_uiVertexCount;
	//
	GLuint m_uiTexName_Diffuse;
	GLuint m_uiTexName_Normal;
	//
	TVector3f m_Tangent;
	TVector3f m_BiTangent;
	//
	bool m_bReady;

public:
	size_t m_ID;

	// Member Functions
protected:
	//

public:
	CDrawObject_Surface();
	const bool Initialize(const TSurface& _krSurface, const size_t _ID);
	void Terminate();
	void Draw();

};

#endif