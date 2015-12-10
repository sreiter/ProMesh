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

#ifndef __H__VIEW3D__
#define __H__VIEW3D__

//	includes
#include <QGLWidget>
#include <QTime>
#include <QColor>
#include "camera/camera.h"

//	predeclarations
class IRenderer3D;
class QTimer;
class QTime;

class View3D : public QGLWidget
{
	Q_OBJECT
	
	public:
		View3D(QWidget *parent = 0);
		virtual ~View3D();

	///	set the renderer.
		virtual void set_renderer(IRenderer3D* renderer);
	///	use this method to set the draw mode for rendering.
		//virtual void set_draw_mode(unsigned int drawMode);
	///	set projection parameters
		virtual void set_projection(float fovy, float zNear, float zFar);

	///	set background color
		void set_background_color(const QColor& color);

		inline cam::CModelViewerCamera& camera()				{return m_camera;}
		inline const cam::CModelViewerCamera& camera() const	{return m_camera;}

		void fly_to(const cam::vector3& destTo, float distance);

	///	calculated the ray from the screen-position to the back-plane.
		void get_ray(cam::vector3& vFromOut, cam::vector3& vToOut,
					float screenX, float screenY);

	///	calculated the ray from the screen-position to the geometry.
	/**	if the geometry is hit, true is returned and vToOut approximately
	 *	contains the intersection point.
	 *	If no intersection has been found, vToOut will be located at the
	 *	far clipping plane.*/
		bool get_ray_to_geometry(cam::vector3& vFromOut,
								 cam::vector3& vToOut,
								 float screenX, float screenY);

	///	if bDrawIt is true, the view will draw a the given rect until the method is called with bDrawIt == false.
		void drawSelectionRect(bool bDrawIt, float xMin = 0, float yMin = 0,
								 float xMax = 0, float yMax = 0);
	signals:
		void mousePressed(QMouseEvent* event);
		void mouseMoved(QMouseEvent* event);
		void mouseReleased(QMouseEvent* event);
		void keyReleased(QKeyEvent* event);

	protected:
	//	derived from QGLWidget
		void initializeGL();
		void resizeGL(int width, int height);
		void paintGL();
		void mousePressEvent(QMouseEvent *event);
		void mouseMoveEvent(QMouseEvent *event);
		void mouseReleaseEvent(QMouseEvent *event);
		void wheelEvent(QWheelEvent *event);
		void mouseDoubleClickEvent(QMouseEvent *event);
		void keyReleaseEvent(QKeyEvent * event);

	//	helper methods
		unsigned int get_camera_drag_flags();
		void refocus_by_screen_coords(int screenX, int screenY);
		void start_interpolation();

	//	slots
	protected slots:
		void interpolate_cam_states();

	protected:
	//	camera
		cam::CModelViewerCamera	m_camera;
		float	m_fovy;
		float	m_aspectRatio;
		float	m_zNear;
		float	m_zFar;
		int		m_viewWidth;
		int		m_viewHeight;

	//	camera interpolation
		cam::SCameraState	m_csOld;
		cam::SCameraState	m_csNew;
		float				m_camInterpDur;//duration of interpolation since start.

		IRenderer3D*		m_pRenderer;
		//unsigned int		m_drawMode;
		QColor				m_bgColor;
		QTimer*				m_pTimer;
		QTime				m_time;

	//	selection rect
		bool m_bDrawSelRect;
		cam::vector2 m_selRectMin;
		cam::vector2 m_selRectMax;
};

#endif
