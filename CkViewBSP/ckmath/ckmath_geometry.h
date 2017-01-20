//
//	Author:	Michael Cameron
//	Email:	chronokun@hotmail.com
//

#pragma once

#ifndef __MATH_GEOMETRY_H__
#define __MATH_GEOMETRY_H__

// Local Includes
#include "ckmath_types.h"


const bool IsIntersection(	const TPlane3d& _krA,
							const TPlane3d& _krB,
							const TPlane3d& _krC);

const bool IsIntersection(	const TPlane3f& _krA,
							const TPlane3f& _krB,
							const TPlane3f& _krC);

const TVector3d& GetIntersection(	TVector3d& _rResult,
									const TPlane3d& _krA,
									const TPlane3d& _krB,
									const TPlane3d& _krC);

const TVector3f& GetIntersection(	TVector3f& _rResult,
									const TPlane3f& _krA,
									const TPlane3f& _krB,
									const TPlane3f& _krC);

const TVector3d& GetPolygonNormal(	TVector3d& _rResult,
									const TVector3d* _kpVertices,
									const size_t _kVertexCount);

const TVector3f& GetPolygonNormal(	TVector3f& _rResult,
									const TVector3f* _kpVertices,
									const size_t _kVertexCount);

const float DistanceFromTetrahedronSide(const TVector3f& _krV0, const TVector3f& _krV1, const TVector3f& _krV2, const TVector3f& _krV3, const TVector3f& _krPoint);

const float DistanceFromTetrahedron(const TVector3f& _krV0, const TVector3f& _krV1, const TVector3f& _krV2, const TVector3f& _krV3, const TVector3f& _krPoint);

const bool IsPointInsideTetrahedron(const TVector3f& _krV0, const TVector3f& _krV1, const TVector3f& _krV2, const TVector3f& _krV3, const TVector3f& _krPoint);

#endif