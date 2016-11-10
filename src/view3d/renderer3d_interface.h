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

#ifndef __H__RENDERER3D_INTERFACE__
#define __H__RENDERER3D_INTERFACE__

///	constants that define the draw mode of the renderer.
enum DrawMode
{
	DM_NONE = 0,
	DM_WIRE = 1,
	DM_SOLID = 1 << 1,
	DM_SOLID_WIRE = DM_SOLID | DM_WIRE
};

///	interface for classes that can draw their content using openGL.
class IRenderer3D
{
	public:
		virtual ~IRenderer3D()	{}

	///	this method is called when the renderer shall draw its content.
		virtual void draw() = 0;

	///	use this method to set the front draw mode of the renderer
		virtual void set_draw_mode_front(unsigned int drawMode) = 0;

	///	use this method to set the back draw mode of the renderer
		virtual void set_draw_mode_back(unsigned int drawMode) = 0;

	///	the camara transform
		virtual void set_transform(float* mat) = 0;
		
	///	the camera parameters
		virtual void set_camera_parameters(float fromX, float fromY, float fromZ,
										   float dirX, float dirY, float dirZ,
										   float upX, float upY, float upZ) = 0;

	///	the perspective transform
		virtual void set_perspective(float fovy, int viewWidth, int viewHeight,
									 float zNear, float zFar) = 0;

		virtual void set_ortho_perspective(float left, float right, float bottom,
										   float top, float zNear, float zFar) = 0;

	///	returns the distance of the near and far clipping plane.
		virtual void get_clip_distance_estimate(float& nearOut, float& farOut,
												float fromX, float fromY, float fromZ,
												float toX, float toY, float toZ) = 0;
};

#endif // __H__RENDERER3D_INTERFACE__
