// created by Sebastian Reiter
// s.b.reiter@googlemail.com
// y09 m07 d17

////////////////////////////////////////////////////////////////////////
//	Methods implemented here will most likely be moved to lib_grid
//	at some point in the near future.
////////////////////////////////////////////////////////////////////////

#include "lg_include.h"

namespace ug
{

////////////////////////////////////////////////////////////////////////
// CalculateBoundingSphere
void CalculateBoundingSphere(Sphere& sphereOut, Face* f,
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
void CalculateBoundingSphere(Sphere& sphereOut, Volume* v,
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
//	ClipFace
bool ClipFace(Face* f, const Sphere& boundingSphere,
				Plane& clipPlane,
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
		for(uint i = 0; i < numVrts; ++i)
		{
			if(PlanePointTest(clipPlane, aaPos[f->vertex(i)]) == RPI_OUTSIDE)
				return true;
		}

	//	they were all inside.
		return false;
	}

//	the triangle is completly outside.
	return true;
}

////////////////////////////////////////////////////////////////////////
//	ClipVolume
bool ClipVolume(Volume* v, const ug::Sphere& boundingSphere,
								ug::Plane& clipPlane,
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
