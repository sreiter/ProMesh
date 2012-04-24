//	created by Sebastian Reiter in 2006
//	s.b.reiter@googlemail.com
//	This code may only be used with explicit permission by Sebastian Reiter.

#ifndef __QUATERNION__
#define __QUATERNION__

#include "vec_math.h"
#include "matrix44.h"
namespace cam
{

///	a quaternion can be used to represent rotations
class CQuaternion
{
	public:
		float x;
		float y;
		float z;
		float w;

		CQuaternion();
		CQuaternion(float x, float y, float z, float angle);

		CQuaternion inverse();

		void set_values(float angle, vector3& axis);
		void set_values(float x, float y, float z, float angle);
		void set_angle_of_rotation(float angle);
		vector3 axis_of_rotation();
		float angle_of_rotation();

		float length();
		float length_sqr();
		void normalize();

		CQuaternion operator * (const float f);
		CQuaternion operator * (const CQuaternion& q);
};


void matrix_from_quaternion(matrix44* matOut, CQuaternion* quat);
void quaternion_from_matrix(CQuaternion* quatOut, matrix44* mat);
void axis_from_quaternion(vector3* vXout, vector3* vYout, vector3* vZout, CQuaternion* quat);
void quaternion_from_axis(CQuaternion* quatOut, vector3* vX, vector3* vY, vector3* vZ);
void quaternion_from_ball_points(CQuaternion* quatOut, vector3* vFrom, vector3* vTo);
void quaternion_lerp(CQuaternion* quatOut, CQuaternion* quat1, CQuaternion* quat2, float t);
void quaternion_slerp(CQuaternion* quatOut, CQuaternion* quat1, CQuaternion* quat2, float t);

}

#endif
