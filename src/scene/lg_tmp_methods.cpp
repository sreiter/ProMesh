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
//	Methods implemented here will most likely be moved to lib_grid
//	at some point in the near future.
////////////////////////////////////////////////////////////////////////

#include "lg_include.h"

namespace ug
{

////////////////////////////////////////////////////////////////////////
// CalculateBoundingSphere
void CalculateBoundingSphere(Sphere3& sphereOut, Face* f,
								Grid::VertexAttachmentAccessor<APosition>& aaPos)
{
//	calculate center
	vector3 center(0, 0, 0);

	uint numVrts = f->num_vertices();
	assert(numVrts > 0 && "face does not contain any vertex!");

	for(uint i = 0; i < numVrts; ++i)
		VecAdd(center, center, aaPos[f->vertex(i)]);

	VecScale(center, center, 1./(number)numVrts);

//	find the point that is the farthest away from center.
	number maxDistSq = VecDistanceSq(center, aaPos[f->vertex(0)]);
	for(uint i = 1; i < numVrts; ++i)
	{
		number nDistSq = VecDistanceSq(center, aaPos[f->vertex(i)]);
		if(nDistSq > maxDistSq)
			maxDistSq = nDistSq;
	}

	sphereOut.set_center(center);
	sphereOut.set_radius(sqrt(maxDistSq));
}

////////////////////////////////////////////////////////////////////////
// CalculateBoundingSphere
void CalculateBoundingSphere(Sphere3& sphereOut, Volume* v,
							Grid::VertexAttachmentAccessor<APosition>& aaPos)
{
//	calculate center
	vector3 center(0, 0, 0);

	uint numVrts = v->num_vertices();
	assert(numVrts > 0 && "face does not contain any vertex!");

	for(uint i = 0; i < numVrts; ++i)
		VecAdd(center, center, aaPos[v->vertex(i)]);

	VecScale(center, center, 1./(number)numVrts);

//	find the point that is the farthest away from center.
	number maxDistSq = VecDistanceSq(center, aaPos[v->vertex(0)]);
	for(uint i = 1; i < numVrts; ++i)
	{
		number nDistSq = VecDistanceSq(center, aaPos[v->vertex(i)]);
		if(nDistSq > maxDistSq)
			maxDistSq = nDistSq;
	}

	sphereOut.set_center(center);
	sphereOut.set_radius(sqrt(maxDistSq));
}


////////////////////////////////////////////////////////////////////////
//	ClipEdge
bool ClipEdge(Edge* e, Plane& clipPlane,
			  Grid::VertexAttachmentAccessor<APosition>& aaPos)
{
//	check whether one of the vertices lies outside of the plane.
//	if so, the whole edge will be clipped.
	Edge::ConstVertexArray vrts = e->vertices();
	if((PlanePointTest(clipPlane, aaPos[vrts[0]]) == RPI_OUTSIDE)
		|| (PlanePointTest(clipPlane, aaPos[vrts[1]]) == RPI_OUTSIDE))
	{
		return true;
	}

//	they were all inside.
	return false;
}


////////////////////////////////////////////////////////////////////////
//	ClipFace
bool ClipFace(Face* f, const Sphere3& boundingSphere, Plane& clipPlane,
				Grid::VertexAttachmentAccessor<APosition>& aaPos)
{
	RelativePositionIndicator rpi = PlaneSphereTest(clipPlane, boundingSphere);

	if(rpi == RPI_INSIDE || rpi == RPI_INSIDE_TOUCHES)
		return false;

	if(rpi == RPI_CUT)
	{
	//	check whether one of the vertices lies outside of the plane.
	//	if so, the whole face will be clipped.
		uint numVrts = f->num_vertices();
		Face::ConstVertexArray vrts = f->vertices();
		for(uint i = 0; i < numVrts; ++i)
		{
			if(PlanePointTest(clipPlane, aaPos[vrts[i]]) == RPI_OUTSIDE)
				return true;
		}

	//	they were all inside.
		return false;
	}

//	the face is completely outside.
	return true;
}

////////////////////////////////////////////////////////////////////////
//	ClipVolume
bool ClipVolume(Volume* v, const ug::Sphere3& boundingSphere, ug::Plane& clipPlane,
				Grid::VertexAttachmentAccessor<APosition>& aaPos)
{
	RelativePositionIndicator rpi = PlaneSphereTest(clipPlane, boundingSphere);

	if(rpi == RPI_INSIDE || rpi == RPI_INSIDE_TOUCHES)
		return false;

	if(rpi == RPI_CUT)
	{
	//	check whether one of the vertices lies outside of the plane.
	//	if so, the whole face will be clipped.
		uint numVrts = v->num_vertices();
		for(uint i = 0; i < numVrts; ++i)
		{
			if(PlanePointTest(clipPlane, aaPos[v->vertex(i)]) == RPI_OUTSIDE)
				return true;
		}

	//	they were all inside.
		return false;
	}

//	the triangle is completly outside.
	return true;
}


}
