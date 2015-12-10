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

#include "plane_sphere.h"

namespace ug
{

RelativePositionIndicator PlaneSphereTest(const Plane& plane, const Sphere3& sphere)
{
//	perform a dot-product between the normal of the plane and the vector
//	pointing from the planes position-vec to the center of the sphere.
	vector3 dir;
	VecSubtract(dir, sphere.get_center(), plane.get_p());

	number d = VecDot(plane.get_n(), dir);

//	d equals the distance of the spheres center to the plane
	if(d >= 0)
	{
		if(d > sphere.get_radius())
			return RPI_OUTSIDE;
		else if(d == sphere.get_radius())
			return RPI_OUTSIDE_TOUCHES;
	}
	else
	{
		d *= -1;
		if(d > sphere.get_radius())
			return RPI_INSIDE;
		else if(d == sphere.get_radius())
			return RPI_INSIDE_TOUCHES;
	}

	return RPI_CUT;
}

RelativePositionIndicator PlanePointTest(const Plane& plane, const vector3& point)
{
	const vector4& equ = plane.get_equation();
	number d = equ.x() * point.x() + equ.y() * point.y() + equ.z() * point.z() + equ.w();

	if(d < 0)
		return RPI_INSIDE;
	else if(d > 0)
		return RPI_OUTSIDE;

	return RPI_CUT;
}

number PlanePointDistance(const Plane& plane, const vector3& point)
{
	vector3 v;
	VecSubtract(v, point, plane.get_p());
	return fabs(VecDot(v, plane.get_n()));
}

}//	end of namespace
