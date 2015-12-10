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

#include "camera.h"

namespace cam
{

CBasicCamera::CBasicCamera()
{
	reset();
}

void CBasicCamera::reset()
{
	m_vFrom = vector3(0, 0, 0);
	m_vTo = vector3(0, 0, 1.f);
	m_vX = vector3(1.f, 0, 0);
	m_vY = vector3(0, 1.f, 0);
	m_vZ = vector3(0, 0, 1.f);
	m_fDistance = 5.f;
}

SCameraState CBasicCamera::get_camera_state()
{
	SCameraState CameraState;

	get_orientation();

	CameraState.vFrom = m_vFrom;
	CameraState.vTo = m_vTo;
	CameraState.fDistance = Vec3Distance(m_vFrom, m_vTo);
	CameraState.quatOrientation = m_quatOrientation;

	return CameraState;
}

void CBasicCamera::set_camera_state(SCameraState& CameraState)
{
	axis_from_quaternion(&m_vX, &m_vY, &m_vZ, &CameraState.quatOrientation);
	Vec3Normalize(m_vX, m_vX);
	Vec3Normalize(m_vY, m_vY);
	Vec3Normalize(m_vZ, m_vZ);

	m_quatOrientation = CameraState.quatOrientation;
	m_vTo = CameraState.vTo;
	m_vFrom = CameraState.vFrom;
	m_fDistance = CameraState.fDistance;

	get_camera_transform();
}

void CBasicCamera::move_object_space(float dx, float dy, float dz)
{
	vector3 vT, vTmp;
	Vec3Scale(vT, m_vX, dx);
	Vec3Scale(vTmp, m_vY, dy);
	Vec3Add(vT, vT, vTmp);
	Vec3Scale(vTmp, m_vZ, dz);
	Vec3Add(vT, vT, vTmp);

	Vec3Add(m_vFrom, m_vFrom, vT);
	Vec3Add(m_vTo, m_vTo, vT);
}

void CBasicCamera::move_world_space(float dx, float dy, float dz)
{
	vector3 vT(dx, dy, dz);
	Vec3Add(m_vFrom, m_vFrom, vT);
	Vec3Add(m_vTo, m_vTo, vT);
}

void CBasicCamera::rotate(CQuaternion& q, vector3* pCenter)
{
	CQuaternion qi = q.inverse();

	CQuaternion v(m_vX.x(), m_vX.y(), m_vX.z(), 0);
	v = q * v * qi;
	m_vX = vector3(v.x, v.y, v.z);

	v = CQuaternion(m_vY.x(), m_vY.y(), m_vY.z(), 0);
	v = (q * v) * qi;
	m_vY = vector3(v.x, v.y, v.z);

	v = CQuaternion(m_vZ.x(), m_vZ.y(), m_vZ.z(), 0);
	v = (q * v) * qi;
	m_vZ = vector3(v.x, v.y, v.z);

//	rotate vFrom / vTo
	v = CQuaternion(m_vFrom.x() - pCenter->x(), m_vFrom.y() - pCenter->y(), m_vFrom.z() - pCenter->z(), 0);
	v = (q * v) * qi;
	Vec3Add(m_vFrom, *pCenter, vector3(v.x, v.y, v.z));

	v = CQuaternion(m_vTo.x() - pCenter->x(), m_vTo.y() - pCenter->y(), m_vTo.z() - pCenter->z(), 0);
	v = (q * v) * qi;
	Vec3Add(m_vTo, *pCenter, vector3(v.x, v.y, v.z));
}

void CBasicCamera::rotate_object_space_x(float drads, vector3* pCenter)
{
	CQuaternion q;
	q.set_values(drads, m_vX);
	q.normalize();

	CQuaternion qi = q.inverse();

	CQuaternion v(m_vY.x(), m_vY.y(), m_vY.z(), 0);
	v = (q * v) * qi;
	m_vY = vector3(v.x, v.y, v.z);

	v = CQuaternion(m_vZ.x(), m_vZ.y(), m_vZ.z(), 0);
	v = (q * v) * qi;
	m_vZ = vector3(v.x, v.y, v.z);

//	rotate vFrom / vTo
	v = CQuaternion(m_vFrom.x() - pCenter->x(), m_vFrom.y() - pCenter->y(), m_vFrom.z() - pCenter->z(), 0);
	v = (q * v) * qi;
	Vec3Add(m_vFrom, *pCenter, vector3(v.x, v.y, v.z));

	v = CQuaternion(m_vTo.x() - pCenter->x(), m_vTo.y() - pCenter->y(), m_vTo.z() - pCenter->z(), 0);
	v = (q * v) * qi;
	Vec3Add(m_vTo, *pCenter, vector3(v.x, v.y, v.z));

}

void CBasicCamera::rotate_object_space_y(float drads, vector3* pCenter)
{
	CQuaternion q;
	q.set_values(drads, m_vY);
	q.normalize();

	CQuaternion qi = q.inverse();

	CQuaternion v(m_vX.x(), m_vX.y(), m_vX.z(), 0);
	v = q * v * qi;
	m_vX = vector3(v.x, v.y, v.z);

	v = CQuaternion(m_vZ.x(), m_vZ.y(), m_vZ.z(), 0);
	v = q * v * qi;
	m_vZ = vector3(v.x, v.y, v.z);

//	rotate vFrom / vTo
	v = CQuaternion(m_vFrom.x() - pCenter->x(), m_vFrom.y() - pCenter->y(), m_vFrom.z() - pCenter->z(), 0);
	v = (q * v) * qi;
	Vec3Add(m_vFrom, *pCenter, vector3(v.x, v.y, v.z));

	v = CQuaternion(m_vTo.x() - pCenter->x(), m_vTo.y() - pCenter->y(), m_vTo.z() - pCenter->z(), 0);
	v = (q * v) * qi;
	Vec3Add(m_vTo, *pCenter, vector3(v.x, v.y, v.z));

}

void CBasicCamera::rotate_object_space_z(float drads, vector3* pCenter)
{
	CQuaternion q;
	q.set_values(drads, m_vZ);
	q.normalize();
	CQuaternion qi = q.inverse();

	CQuaternion v(m_vX.x(), m_vX.y(), m_vX.z(), 0);
	v = q * v * qi;
	m_vX = vector3(v.x, v.y, v.z);

	v = CQuaternion(m_vY.x(), m_vY.y(), m_vY.z(), 0);
	v = q * v * qi;
	m_vY = vector3(v.x, v.y, v.z);

//	rotate vFrom / vTo
	v = CQuaternion(m_vFrom.x() - pCenter->x(), m_vFrom.y() - pCenter->y(), m_vFrom.z() - pCenter->z(), 0);
	v = (q * v) * qi;
	Vec3Add(m_vFrom, *pCenter, vector3(v.x, v.y, v.z));

	v = CQuaternion(m_vTo.x() - pCenter->x(), m_vTo.y() - pCenter->y(), m_vTo.z() - pCenter->z(), 0);
	v = (q * v) * qi;
	Vec3Add(m_vTo, *pCenter, vector3(v.x, v.y, v.z));

}

void CBasicCamera::scale_from_to(float scale, const vector3* pCenter)
{
	vector3 v;
	Vec3Subtract(v, m_vFrom, *pCenter);
	Vec3Scale(v, v, scale);
	Vec3Add(m_vFrom, *pCenter, v);

	Vec3Subtract(v, m_vTo, *pCenter);
	Vec3Scale(v, v, scale);
	Vec3Add(m_vTo, *pCenter, v);
	m_fDistance = Vec3Distance(m_vFrom, m_vTo);
}

matrix44* CBasicCamera::get_camera_transform()
{
	Vec3Normalize(m_vX, m_vX);
	Vec3Normalize(m_vY, m_vY);
	Vec3Normalize(m_vZ, m_vZ);

	matrix44 matRot(m_vX.x(), m_vY.x(), m_vZ.x(), 0,
					m_vX.y(), m_vY.y(), m_vZ.y(), 0,
					m_vX.z(), m_vY.z(), m_vZ.z(), 0,
					0, 0, 0, 1.f);

	MatTranslation(m_matTransform, -m_vFrom.x(), -m_vFrom.y(), -m_vFrom.z());
	MatMultiply(m_matTransform, matRot, m_matTransform);
	return &m_matTransform;
}

CQuaternion* CBasicCamera::get_orientation()
{
	get_camera_transform();
	quaternion_from_matrix(&m_quatOrientation, &m_matTransform);
	return &m_quatOrientation;
}
/*
SCameraState calculate_camera_state(SCameraState& OldState, vector3* vFrom, vector3* vTo)
{
	SCameraState cs;

	vector3 vX, vY, vZ;
	axis_from_quaternion(&vX, &vY, &vZ, &OldState.quatOrientation);

	vector3 vDir;
	Vec3Subtract(vDir, *vFrom, *vTo);
	Vec3Normalize(vDir, vDir);
	float DotRight = Vec3Dot(vDir, vX);
	float DotUp = Vec3Dot(vDir, vY);

	if(fabs(DotUp) < fabs(DotRight))
	{
	//	first calculate m_vX = vDir x m_vY		then m_vY = vDir x m_vX

		Vec3Cross(vX, vY, vDir);
		Vec3Normalize(vX, vX);
		Vec3Cross(vY, vDir, vX);

	}
	else
	{
	//	first calculate m_vY = vDir x m_vX		then m_vX = vDir x m_vY
		Vec3Cross(vY, vDir, vX);
		Vec3Normalize(vY, vY);
		Vec3Cross(vX, vY, vDir);
	}
	vZ = vDir;

	Vec3Normalize(vX, vX);
	Vec3Normalize(vY, vY);
	Vec3Normalize(vZ, vZ);

	quaternion_from_axis(&cs.quatOrientation, &vX, &vY, &vZ);
	cs.quatOrientation.normalize();

	cs.vTo = *vTo;
	cs.vFrom = *vFrom;
	cs.fDistance = Vec3Distance(*vTo, *vFrom);

	return cs;
}*/

SCameraState calculate_camera_state(SCameraState& OldState, vector3* vFrom, vector3* vTo)
{//	calculates the rotation-transformation.
	SCameraState cs;

	vector3 dirOld, dirNew;
	Vec3Subtract(dirOld, OldState.vTo, OldState.vFrom);
	Vec3Subtract(dirNew, *vTo, *vFrom);

	Vec3Normalize(dirOld, dirOld);
	Vec3Normalize(dirNew, dirNew);

	float fDot = Vec3Dot(dirOld, dirNew);
	vector3 vPart;
	Vec3Cross(vPart, dirOld, dirNew);

	CQuaternion q1(vPart.x(), vPart.y(), vPart.z(), fDot);
	q1.normalize();

/*
	vector3 dirNew;
	Vec3Subtract(dirNew, *vTo, *vFrom);
	Vec3Normalize(dirNew, dirNew);

	vector3 vX, vY, vZ;
	axis_from_quaternion(&vX, &vY, &vZ, &OldState.quatOrientation);

//	calculate the quaternion for x rotation
//	project it into the yz plane first
	float d = Vec3Dot(vX, dirNew);
	vector3 v;
	Vec3Scale(v, vX, d);
	Vec3Subtract(v, dirNew, v);
	Vec3Normalize(v, v);
	Vec3Scale(v, v, -1);
//	now calculate the angle and the quaternion.
	d = Vec3Dot(vZ, v);
	CQuaternion q1;
	q1.set_values(vX.x(), vX.y(), vX.z(), 0);
	q1.normalize();
*/
	cs.vTo = *vTo;
	cs.vFrom = *vFrom;
	cs.fDistance = Vec3Distance(*vTo, *vFrom);
	cs.quatOrientation = OldState.quatOrientation * q1;
	cs.quatOrientation.normalize();

	return cs;
}

SCameraState interpolate_camera_states(SCameraState& state1, SCameraState& state2, float IA)
{
	SCameraState cs;
	float w1 = 1.f - IA;
	float w2 = IA;

	vector3 vTmp;
	Vec3Scale(cs.vFrom, state1.vFrom, w1);
	Vec3Scale(vTmp, state2.vFrom, w2);
	Vec3Add(cs.vFrom, cs.vFrom, vTmp);

	Vec3Scale(cs.vTo, state1.vTo, w1);
	Vec3Scale(vTmp, state2.vTo, w2);
	Vec3Add(cs.vTo, cs.vTo, vTmp);

	cs.fDistance = w1 * state1.fDistance + w2 * state2.fDistance;
	quaternion_slerp(&cs.quatOrientation, &state1.quatOrientation, &state2.quatOrientation, IA);
	return cs;
}

}
