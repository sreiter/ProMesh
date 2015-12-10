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
