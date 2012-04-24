//	created by Sebastian Reiter
//	s.b.reiter@googlemail.com
//	copyright Sebastian Reiter
//	y09 m03 d20

#ifndef __H__LGMATH__PLANE_SPHERE__
#define __H__LGMATH__PLANE_SPHERE__

#include "common/math/ugmath.h"

namespace ug
{
////////////////////////////////////////////////////////////////////////
//	Sphere
///	a simple sphere, described by a center and a radius.
class Sphere
{
	public:
		Sphere()	{}
		Sphere(const vector3& center, number radius) : m_center(center), m_radius(radius)	{}

		inline const vector3& get_center() const		{return m_center;}
		inline void set_center(const vector3& center)	{m_center = center;}
		inline number get_radius() const				{return m_radius;}
		inline void set_radius(number radius)			{m_radius = radius;}

	protected:
		vector3	m_center;
		number	m_radius;
};

////////////////////////////////////////////////////////////////////////
//	Plane
///	a simple plane, described by a point (p) and a normal (n)
/**
 *	n is normized automatically.
 */
class Plane
{
	public:
		Plane()	{}
		Plane(const vector3& p, const vector3& n)
			{
				set_p(p);
				set_n(n);
			}

		inline const vector3& get_p() const		{return m_p;}
		inline void set_p(const vector3& p)
			{
				m_p = p;
				m_equ = vector4(m_n.x, m_n.y, m_n.z, -VecDot(m_n, m_p));
			}

		inline const vector3& get_n() const		{return m_n;}
		inline void set_n(const vector3& n)
			{
				VecNormalize(m_n, n);
				m_equ = vector4(m_n.x, m_n.y, m_n.z, -VecDot(m_n, m_p));
			}

	///	equation: equ.x*x + equ.y*y + equ.z*z + equ.w = 0
		inline const vector4& get_equation() const 	{return m_equ;}

	protected:
		vector3 m_p;
		vector3 m_n;
		vector4 m_equ;
};

////////////////////////////////////////////////////////////////////////
//	RelativePositionIndicator
///	used to classify the position of one object relative to another.
enum RelativePositionIndicator
{
	RPI_INSIDE = -2,
	RPI_INSIDE_TOUCHES = -1,
	RPI_CUT = 0,
	RPI_OUTSIDE_TOUCHES = 1,
	RPI_OUTSIDE = 2
};

////////////////////////////////////////////////////////////////////////
//	PlaneSphereTest
///	returns whether a sphere lies behind (inside), in front (outside) a plane or cuts it.
RelativePositionIndicator PlaneSphereTest(const Plane& plane, const Sphere& sphere);

////////////////////////////////////////////////////////////////////////
//	PlanePointTest
///	returns whether a point lies behind (inside), in front (outside) a plane or lies on it (cuts it).
RelativePositionIndicator PlanePointTest(const Plane& plane, const vector3& point);

////////////////////////////////////////////////////////////////////////
//	PlanePointDistance
///	returns the distance of a point to a plane
number PlanePointDistance(const Plane& plane, const vector3& point);
}//	end of namespace

#endif
