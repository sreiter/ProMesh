//	created by Sebastian Reiter
//	s.b.reiter@googlemail.com
//	copyright Sebastian Reiter
//	y09 m03 d20

#include "plane_sphere.h"

namespace ug
{

RelativePositionIndicator PlaneSphereTest(const Plane& plane, const Sphere& sphere)
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
