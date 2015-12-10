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

#ifndef _BASICCAMERA_H_
#define _BASICCAMERA_H_

#include "vec_math.h"
#include "quaternion.h"
#include "matrix44.h"
#include "arc_ball.h"

namespace cam
{

enum EInterpolation
{
	I_NONE,
	I_LERP,
	I_SLERP
};

struct SCameraState
{
	vector3		vFrom;
	vector3		vTo;
	float		fDistance;
	CQuaternion	quatOrientation;
};

enum ECameraDragFlags
{
	CDF_NONE = 0,
	CDF_MOVE = 1,
	CDF_ZOOM = 1<<1,
};


SCameraState calculate_camera_state(SCameraState& OldState, vector3* vFrom, vector3* vTo);
SCameraState interpolate_camera_states(SCameraState& state1, SCameraState& state2, float IA);

class CBasicCamera
{
	public:
		CBasicCamera();
		virtual ~CBasicCamera()	{}

		void reset();

		virtual void begin_drag(int x, int y, unsigned int cdf)	{}	///<	cdf has to be an or-combination of consts enumerated in ECameraDragFlags.
		virtual void drag_to(int x, int y, unsigned int cdf)	{}	///<	cdf has to be an or-combination of consts enumerated in ECameraDragFlags.
		virtual void end_drag(int x, int y, unsigned int cdf)	{}	///<	cdf has to be an or-combination of consts enumerated in ECameraDragFlags.
		virtual void scroll(float scrollAmount, unsigned int cdf)	{}
		virtual SCameraState get_camera_state();
		virtual void set_camera_state(SCameraState& CameraState);

		void move_object_space(float dx, float dy, float dz);
		void move_world_space(float dx, float dy, float dz);

		void rotate_object_space_x(float drads, vector3* pCenter);
		void rotate_object_space_y(float drads, vector3* pCenter);
		void rotate_object_space_z(float drads, vector3* pCenter);

		void rotate(CQuaternion& q, vector3* pCenter);

		void scale_from_to(float scale, const vector3* pCenter);

		virtual matrix44* get_camera_transform();
		virtual CQuaternion* get_orientation();

		vector3* get_from()		{return &m_vFrom;};
		vector3* get_to()		{return &m_vTo;};
		vector3* get_dir()		{return &m_vZ;};
		float get_distance()	{return m_fDistance;};

		const vector3& get_up_dir()		{return m_vY;}
		const vector3& get_right_dir()	{return m_vX;}
		const vector3& get_to_dir()		{return m_vZ;}
	protected:
		matrix44	m_matTransform;
		CQuaternion	m_quatOrientation;

		vector3 m_vFrom;
		vector3 m_vTo;

		float	m_fDistance;

		vector3 m_vX;	//strafe dir
		vector3 m_vY;	//up dir
		vector3 m_vZ;	//look dir
};

class CModelViewerCamera : public CBasicCamera
{
	public:
		CModelViewerCamera();
		virtual ~CModelViewerCamera()	{}

		virtual void begin_drag(int x, int y, unsigned int cdf);///<	cdf has to be an or-combination of consts enumerated in ECameraDragFlags.
		virtual void drag_to(int x, int y, unsigned int cdf);///<	cdf has to be an or-combination of consts enumerated in ECameraDragFlags.
		virtual void end_drag(int x, int y, unsigned int cdf);///<	cdf has to be an or-combination of consts enumerated in ECameraDragFlags.
		virtual void scroll(float scrollAmount, unsigned int cdf);

		bool dragging()	{return m_bDragging;}
		SCameraState get_camera_state();
		void set_camera_state(SCameraState& CameraState);

		void set_window(int nWidth, int nHeight, float fRadius = 0.9f, int OffsetX = 0, int OffsetY = 0);

		matrix44* get_camera_transform();
		CQuaternion* get_orientation();

	protected:
		CArcBall	m_ArcBall;

//		MovementControl
		bool		m_lastCDF;
		bool		m_bDragging;
		int			m_iLastMouseX;
		int			m_iLastMouseY;
};

}
#endif /* _BASICCAMERA_H_ */
