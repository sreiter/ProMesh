/*
 * Copyright (c) 2008-2015:  G-CSC, Goethe University Frankfurt
 * Copyright (c) 2006-2008:  Steinbeis Forschungszentrum (STZ Ölbronn)
 * Copyright (c) 2006-2015:  Sebastian Reiter
 * Author: Sebastian Reiter
 *
 * This file is part of ProMesh.
 * 
 * ProMesh is free software: you can redistribute it and/or modify it under the
 * terms of the GNU Lesser General Public License version 3 (as published by the
 * Free Software Foundation) with the following additional attribution
 * requirements (according to LGPL/GPL v3 §7):
 * 
 * (1) The following notice must be displayed in the Appropriate Legal Notices
 * of covered and combined works: "Based on ProMesh (www.promesh3d.com)".
 * 
 * (2) The following bibliography is recommended for citation and must be
 * preserved in all covered files:
 * "Reiter, S. and Wittum, G. ProMesh -- a flexible interactive meshing software
 *   for unstructured hybrid grids in 1, 2, and 3 dimensions. In preparation."
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU Lesser General Public License for more details.
 */

////////////////////////////////////////////////////////////////////////
//	Instead of directly including "lib_grid/lib_grid.h", one should
//	include this file.
//	The purpose of this header is to easily define and use new methods,
//	that will later be moved to lib_grid.
////////////////////////////////////////////////////////////////////////

#ifndef __H__PM__LG_INCLUDE__
#define __H__PM__LG_INCLUDE__

#include <algorithm>
#include "lib_grid/lib_grid.h"


////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////
//	everything below is temporarily included here and will be moved to
//	lib_grid at some point in the future.
////////////////////////////////////////////////////////////////////////

#include "plane_sphere.h"

////////////////////////////////////////////////////////////////////////
//	Attachments
namespace ug
{

typedef Attachment<ug::Sphere3> ASphere;


////////////////////////////////////////////////////////////////////////
//	Methods
///	calculates the a bounding sphere of a face.
void CalculateBoundingSphere(ug::Sphere3& sphereOut, ug::Face* f,
							Grid::VertexAttachmentAccessor<APosition>& aaPos);

///	calculates the a bounding sphere of a volume.
void CalculateBoundingSphere(ug::Sphere3& sphereOut, ug::Volume* v,
							Grid::VertexAttachmentAccessor<APosition>& aaPos);

///	if at least one point of the edge lies outside of the plane, the method returns true.
bool ClipEdge(Edge* e, ug::Plane& clipPlane,
			  Grid::VertexAttachmentAccessor<APosition>& aaPos);

///	if at least one point of the face lies outside of the plane, the method returns true.
bool ClipFace(Face* f, const ug::Sphere3& boundingSphere, ug::Plane& clipPlane,
			  Grid::VertexAttachmentAccessor<APosition>& aaPos);

///	if at least one point of the volume lies outside of the plane, the method returns true.
bool ClipVolume(Volume* v, const ug::Sphere3& boundingSphere, ug::Plane& clipPlane,
				Grid::VertexAttachmentAccessor<APosition>& aaPos);

}
////////////////////////////////////////////////////////////////////////
//	math
namespace ug
{

template <typename vector_t>
void VecCompMin(vector_t& vOut, vector_t& v1, vector_t & v2)
{
	for(size_t i = 0; i < vOut.size(); ++i)
		vOut[i] = std::min(v1[i], v2[i]);
}

template <typename vector_t>
void VecCompMax(vector_t& vOut, vector_t& v1, vector_t & v2)
{
	for(size_t i = 0; i < vOut.size(); ++i)
		vOut[i] = std::max(v1[i], v2[i]);
}

}

#endif
