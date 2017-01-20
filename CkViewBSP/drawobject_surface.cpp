//
//	Author:	Michael Cameron
//	Email:	chronokun@hotmail.com
//

// Libraries Include
#include "libraries.h"
// This Include
#include "drawobject_surface.h"
// Local Includes
#include "enginecore.h"
#include "renderer.h"
#include "surface.h"

CDrawObject_Surface::CDrawObject_Surface()
	: m_bReady(false)
{
	//
}

const bool CDrawObject_Surface::Initialize(const TSurface& _krSurface, const size_t _ID)
{
	if(_krSurface.m_Positions.size() != _krSurface.m_TexCoords.size())
	{
		return(false);
	}

	this->m_uiVertexCount = (GLuint)_krSurface.m_Positions.size();

	// Positions
	this->m_uiPositionsVBO = 0;
	glGenBuffers(1, &this->m_uiPositionsVBO);
	glBindBuffer(GL_ARRAY_BUFFER, this->m_uiPositionsVBO);
	glBufferData(	GL_ARRAY_BUFFER, (this->m_uiVertexCount * 3 * sizeof(GLfloat)),
					_krSurface.m_Positions.data(), GL_STATIC_DRAW);

	// TexCoords
	this->m_uiTexCoordsVBO = 0;
	glGenBuffers(1, &this->m_uiTexCoordsVBO);
	glBindBuffer(GL_ARRAY_BUFFER, this->m_uiTexCoordsVBO);
	glBufferData(	GL_ARRAY_BUFFER, (this->m_uiVertexCount * 2 * sizeof(GLfloat)),
					_krSurface.m_TexCoords.data(), GL_STATIC_DRAW);

	this->m_uiVAO = 0;
	glGenVertexArrays(1, &this->m_uiVAO);
	glBindVertexArray(this->m_uiVAO);
	// Positions
	glBindBuffer(GL_ARRAY_BUFFER, this->m_uiPositionsVBO);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
	glEnableVertexAttribArray(0);
	// TexCoords
	glBindBuffer(GL_ARRAY_BUFFER, this->m_uiTexCoordsVBO);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, nullptr);
	glEnableVertexAttribArray(1);

	this->m_Tangent = _krSurface.m_Tangent;
	this->m_BiTangent = _krSurface.m_BiTangent;

	this->m_uiTexName_Diffuse = 0;
	this->m_uiTexName_Normal = 0;

	this->m_bReady = true;

	this->m_ID = _ID;

	return(true);
}

void CDrawObject_Surface::Terminate()
{
	glDeleteBuffers(1, &this->m_uiPositionsVBO);
	glDeleteBuffers(1, &this->m_uiTexCoordsVBO);
	glDeleteVertexArrays(1, &this->m_uiVAO);
	this->m_bReady = false;
}

void CDrawObject_Surface::Draw()
{
	if(this->m_bReady)
	{
		glBindVertexArray(this->m_uiVAO);
		glDrawArrays(GL_TRIANGLES, 0, this->m_uiVertexCount);
	}
}
