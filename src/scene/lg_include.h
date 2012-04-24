// created by Sebastian Reiter
// s.b.reiter@googlemail.com
// y09 m07 d17

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

typedef Attachment<ug::Sphere> ASphere;


////////////////////////////////////////////////////////////////////////
//	Methods
///	calculates the a bounding sphere of a face.
void CalculateBoundingSphere(ug::Sphere& sphereOut, ug::Face* f,
							Grid::VertexAttachmentAccessor<APosition>& aaPos);

///	calculates the a bounding sphere of a volume.
void CalculateBoundingSphere(ug::Sphere& sphereOut, ug::Volume* v,
							Grid::VertexAttachmentAccessor<APosition>& aaPos);

///	if at least one point of the face lies outside of the plane, the method returns true.
bool ClipFace(Face* f, const ug::Sphere& boundingSphere,
                                ug::Plane& clipPlane,
				Grid::VertexAttachmentAccessor<APosition>& aaPos);

///	if at least one point of the volume lies outside of the plane, the method returns true.
bool ClipVolume(Volume* v, const ug::Sphere& boundingSphere,
								ug::Plane& clipPlane,
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
