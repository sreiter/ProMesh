//	created by Sebastian Reiter
//	s.b.reiter@googlemail.com
//	y09 m09 d29

#ifndef __H__CAM__VEC_MATH__
#define __H__CAM__VEC_MATH__

#include <cmath>
#include "common/math/ugmath.h"

namespace cam
{

const float SMALL = 1.e-12;
typedef ug::vector2 vector2;
typedef ug::vector3 vector3;
/*
///	a simple type for a 2-component math-vector.
struct vector2
{
	vector2()	{}
	vector2(float nx, float ny) : x(nx), y(ny)	{}
	vector2(const vector2& v)					{assign(v);}
	inline vector2& operator=(const vector2& v)	{assign(v); return *this;}
	inline bool operator==(const vector2& v)	{return (x == v.x()) && (y == v.y());}
	inline void assign(const vector2& v)		{x = v.x(); y = v.y();}
	inline const float& operator[](int index) const		{return m_data[index];}
	inline float& operator[](int index)					{return m_data[index];}
	
	union
	{
		struct
		{
			float x;
			float y;
		};

		float m_data[2];
	};
	
};

///	a simple type for a 3-component math-vector.
struct vector3
{
	vector3()	{}
	vector3(float nx, float ny, float nz) : x(nx), y(ny), z(nz)	{}
	vector3(const vector3& v)					{assign(v);}
	inline vector3& operator=(const vector3& v)	{assign(v); return *this;}
	inline bool operator==(const vector3& v)	{return (x == v.x()) && (y == v.y()) && (z == v.z());}
	inline void assign(const vector3& v)		{x = v.x(); y = v.y(); z = v.z();}
	inline const float& operator[](int index) const		{return m_data[index];}
	inline float& operator[](int index)					{return m_data[index];}
	
	union
	{
		struct
		{
			float x;
			float y;
			float z;
		};

		float m_data[3];
	};
};
*/
////////////////////////////////////////////////////////////////////////////////////////////////
//	Vec2 methods
template <class Vector>
inline void Vec2Scale(Vector& vOut, Vector& v, double fScale)
{
	vOut.x() = v.x*fScale;
	vOut.y() = v.y*fScale;
}

template <class Vector>
inline double Vec2DistanceSq(Vector& v1, Vector& v2)
{
	double a = v1.x() - v2.x();
	double b = v1.y() - v2.y();
	return a*a + b*b ;
}

template <class Vector>
inline double Vec2Distance(Vector& v1, Vector& v2)
{
	return sqrt(Vec2DistanceSq(v1, v2));
}


////////////////////////////////////////////////////////////////////////////////////////////////
//	Vec3 methods
template <class Vector>
inline double Vec3Length(const Vector& Vec)
{
	return(sqrt(Vec.x() * Vec.x() + Vec.y() * Vec.y() + Vec.z() * Vec.z()));
}

template <class Vector>
inline double Vec3LengthSq(const Vector& Vec)
{
	return(Vec.x() * Vec.x() + Vec.y() * Vec.y() + Vec.z() * Vec.z());
}

template <class Vector>
inline double Vec3DistanceSq(const Vector& v1, const Vector& v2)
{
	double a = v1.x() - v2.x();
	double b = v1.y() - v2.y();
	double c = v1.z() - v2.z();
	return a*a + b*b + c*c;
}

template <class Vector>
inline double Vec3Distance(const Vector& v1, const Vector& v2)
{
	return sqrt(Vec3DistanceSq(v1, v2));
}

template <class Vector>
void Vec3Add(Vector& VOut, const Vector& V1, const Vector& V2)
{
	VOut.x() = V1.x() + V2.x();
	VOut.y() = V1.y() + V2.y();
	VOut.z() = V1.z() + V2.z();
}

template <class Vector>
void Vec3Subtract(Vector& VOut, const Vector& V1, const Vector& V2)
{
	VOut.x() = V1.x() - V2.x();
	VOut.y() = V1.y() - V2.y();
	VOut.z() = V1.z() - V2.z();
}

template <class Vector>
void Vec3Scale(Vector& VOut, const Vector& V1, double fScale)
{
	VOut.x() = V1.x() * fScale;
	VOut.y() = V1.y() * fScale;
	VOut.z() = V1.z() * fScale;
}

template <class Vector>
double Vec3Dot(const Vector& V1, const Vector& V2)
{
	return (V1.x() * V2.x() + V1.y() * V2.y() + V1.z() * V2.z());
}

template <class Vector>
void Vec3Normalize(Vector& VOut, const Vector& Vec)
{
	double VecLength;
	VecLength=Vec3Length(Vec);

	if(fabs(VecLength) < SMALL){
		VOut.x() = 0;
		VOut.y() = 0;
		VOut.z() = 0;
		return;
	}

	VOut.x() =Vec.x()/VecLength;
	VOut.y() =Vec.y()/VecLength;
	VOut.z() =Vec.z()/VecLength;
}

template <class Vector>
void Vec3Cross(Vector& VOut, const Vector& V1, const Vector& V2)
{
	VOut.x() =V1.y() * V2.z() - V1.z() * V2.y();
	VOut.y() =V1.z() * V2.x() - V1.x() * V2.z();
	VOut.z() =V1.x() * V2.y() - V1.y() * V2.x();
}

};

#endif
