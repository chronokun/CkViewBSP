//
//	Author:	Michael Cameron
//	Email:	chronokun@hotmail.com
//

#pragma once

#ifndef __SURFACE_H__
#define __SURFACE_H__

// Local Includes
#include "ckmath/ckmath_types.h"

struct TSurface
{
	std::vector<TVector3f> m_Positions;
	std::vector<TVector2f> m_TexCoords;
	TVector3f m_Tangent;
	TVector3f m_BiTangent;
	size_t m_Material;
	size_t m_DrawObject;
};

#endif