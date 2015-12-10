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

#ifndef __H__CAM__ARC_BALL__
#define __H__CAM__ARC_BALL__

#include "vec_math.h"
#include "quaternion.h"
#include "matrix44.h"

namespace cam
{

class CArcBall
{
	public:
		CArcBall();
		void set_window(int nWidth, int nHeight, float fRadius = 0.9f, int OffsetX = 0, int OffsetY = 0);

		matrix44* get_rotation_matrix();
		CQuaternion* get_rotation_quaternion();
		void set_rotation_quaternion(CQuaternion* pQuaternion);

		void begin_drag(int x, int y);
		void drag_to(int x, int y);
		void end_drag();

	private:
		vector3 get_ball_point_from_screen_coords(int nx, int ny);
		CQuaternion get_quat_from_ball_points(vector3& vFrom, vector3& vTo);

		bool		m_bDrag;

		matrix44	m_matRotation;
		CQuaternion	m_quatRotation;

		CQuaternion	m_quatDown;
		vector3		m_vDown;

		vector2		m_Screen;
		vector2		m_Offset;

		float		m_fRadius;

		int			m_iMouseLastX;
		int			m_iMouseLastY;
};

}//	end of namespace

#endif
