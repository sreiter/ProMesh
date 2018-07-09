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

#include <QtWidgets>
#include <iostream>
#include "gl_includes.h"
#include "view3d.h"
#include "renderer3d_interface.h"

using namespace std;

View3D::View3D(QWidget *parent) :
	QGLWidget(parent),
	m_orthoPerspective(false)
{
	m_viewWidth = 100;
	m_viewHeight = 100;
	m_aspectRatio = 1.f;
	m_fovy = 30;
	m_zNear = 0.01;
	m_zFar = 1000.f;

	m_bDrawSelRect = false;

	m_pRenderer = NULL;
	setFormat(QGLFormat(QGL::DoubleBuffer | QGL::DepthBuffer));

	glViewport(0, 0, m_viewWidth, m_viewHeight);

	this->setFocusPolicy(Qt::StrongFocus);

	m_camInterpDur = 250.f;
	//m_drawMode = DM_SOLID;
	m_bgColor = Qt::black;

//	create the timer that is used during camera interpolation.
	m_pTimer = new QTimer(this);
	connect(m_pTimer, SIGNAL(timeout()), this, SLOT(interpolate_cam_states()));
}

View3D::~View3D()
{
}

void View3D::set_renderer(IRenderer3D* renderer)
{
	m_pRenderer = renderer;
	if(m_pRenderer)
		m_pRenderer->set_perspective(m_fovy, m_viewWidth, m_viewHeight,
									 m_zNear, m_zFar);
	updateGL();
}

void View3D::set_background_color(const QColor& color)
{
	m_bgColor = color;
	update();
}

/*
void View3D::set_draw_mode(unsigned int drawMode)
{
	m_drawMode = drawMode;
	updateGL();
}
*/
void View3D::set_projection(float fovy, float zNear, float zFar)
{
	m_fovy = fovy;
	m_zNear = zNear;
	m_zFar = zFar;
	if(m_pRenderer)
		m_pRenderer->set_perspective(m_fovy, m_viewWidth, m_viewHeight,
									 m_zNear, m_zFar);
	updateGL();
}

void View3D::initializeGL()
{
}

void View3D::resizeGL(int width, int height)
{
	m_viewWidth = width;
	m_viewHeight = height;

	glViewport(0, 0, width, height);
	//glMatrixMode(GL_PROJECTION);
	//glLoadIdentity();
	m_aspectRatio = float(width) / height;
	//gluPerspective(m_fovy, m_aspectRatio, m_zNear, m_zFar);
	m_camera.set_window(width, height);
	//glMatrixMode(GL_MODELVIEW);
	if(m_pRenderer)
		m_pRenderer->set_perspective(m_fovy, m_viewWidth, m_viewHeight,
									 m_zNear, m_zFar);
}

void View3D::paintGL()
{
//	setup gl
	qglClearColor(m_bgColor);
	glShadeModel(GL_FLAT);
	glEnable(GL_DEPTH_TEST);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	cam::matrix mat = *m_camera.get_camera_transform();

	if(m_pRenderer)
	{
	//	update near and far clipping planes
		const cam::vector3* vFrom = m_camera.get_from();
		const cam::vector3* vTo = m_camera.get_to();
		cam::vector3 camDir = m_camera.get_to_dir();
		cam::vector3 camUp = m_camera.get_up_dir();
		m_pRenderer->set_camera_parameters(vFrom->x(), vFrom->y(), vFrom->z(),
										   camDir.x(), camDir.y(), camDir.z(),
										   camUp.x(), camUp.y(), camUp.z());

		const cam::vector3& ws = m_camera.world_scale();
		m_pRenderer->set_world_scale(ws.x(), ws.y(), ws.z());

		m_pRenderer->get_clip_distance_estimate(m_zNear, m_zFar,
												vFrom->x(), vFrom->y(), vFrom->z(),
												vTo->x(), vTo->y(), vTo->z());
	//	set render-attributes
		//m_pRenderer->set_draw_mode(m_drawMode);
		m_pRenderer->set_transform((float*)&mat);
		
		if(!m_orthoPerspective){
			m_pRenderer->set_perspective(m_fovy, m_viewWidth, m_viewHeight,
										 m_zNear, m_zFar);
		}
		else{
		//todo: This is only a very rough implentation and is in no way ready for broad use
			float zoom = VecDistance(*vFrom, *vTo);
			float aspectInv = float(m_viewWidth) / float(m_viewHeight);
			// float fromDist = VecLength(*vFrom);
			UG_LOG("zoom: " << zoom << endl);
			m_pRenderer->set_ortho_perspective(-zoom*aspectInv, zoom*aspectInv,
											   -zoom, zoom, -100, 100);
		}

	//	draw the scene
		m_pRenderer->draw();

	//	draw the coordinate system
	//	projection matrix for the coordinate system in the lower left corner
		glMatrixMode(GL_PROJECTION);
		glPushMatrix();
		glLoadIdentity();
		glOrtho(0, 100.f, 0, 100.f / m_aspectRatio, 0.01, 100);
		glMatrixMode(GL_MODELVIEW);
		glPushMatrix();
		glLoadIdentity();
	//	apply camera orientation only
		cam::CQuaternion* pQOrientation = m_camera.get_orientation();
		cam::matrix matOrientation;
		cam::matrix_from_quaternion(&matOrientation, pQOrientation);
		cam::matrix axMat;
		cam::MatTranslation(axMat, 6, 6, -6);
		//cam::MatTranspose(mat, mat);
		cam::MatMultiply(axMat, matOrientation, axMat);
		glMultMatrixf((GLfloat*)&axMat);
//
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glEnable(GL_BLEND);
		glEnable(GL_LINE_SMOOTH);
		glLineWidth(2.f);
		glPointSize(2.f);
		glDisable(GL_LIGHTING);
//
//	//	render text
		const GLfloat axlen = 5;
//CAUSES PROBLEMS ON SOME SYSTEMS (mscherer, mknodel)
		glColor3f(1, 0, 0);
		renderText(axlen, 0, 0, "X");
		glColor3f(0, 1, 0);
		renderText(0, axlen, 0, "Y");
		glColor3f(0, 0, 1);
		renderText(0, 0, axlen, "Z");

		glBegin(GL_LINES);
		//	x
			glColor3f(1, 0, 0);
			glVertex3f(0, 0, 0);
			glVertex3f(axlen, 0, 0);
		//	y
			glColor3f(0, 1, 0);
			glVertex3f(0, 0, 0);
			glVertex3f(0, axlen, 0);
		//	z
			glColor3f(0, 0, 1);
			glVertex3f(0, 0, 0);
			glVertex3f(0, 0, axlen);
		glEnd();

	//	draw selection rect
		if(m_bDrawSelRect){
			glDisable(GL_DEPTH_TEST);
			glMatrixMode(GL_PROJECTION);
			glLoadIdentity();
			glOrtho(0, m_viewWidth, 0, m_viewHeight, -1, 1);
			glMatrixMode(GL_MODELVIEW);
			glLoadIdentity();

			glColor3f(0, 0, 0);
			glBegin(GL_POINTS);
			glVertex3f(m_selRectMin.x(), m_selRectMin.y(), 0);
			glVertex3f(m_selRectMax.x(), m_selRectMin.y(), 0);
			glVertex3f(m_selRectMax.x(), m_selRectMax.y(), 0);
			glVertex3f(m_selRectMin.x(), m_selRectMax.y(), 0);
			glEnd();

			glBegin(GL_LINES);
			glVertex3f(m_selRectMin.x(), m_selRectMin.y(), 0);
			glVertex3f(m_selRectMax.x(), m_selRectMin.y(), 0);
			glVertex3f(m_selRectMax.x(), m_selRectMin.y(), 0);
			glVertex3f(m_selRectMax.x(), m_selRectMax.y(), 0);
			glVertex3f(m_selRectMax.x(), m_selRectMax.y(), 0);
			glVertex3f(m_selRectMin.x(), m_selRectMax.y(), 0);
			glVertex3f(m_selRectMin.x(), m_selRectMax.y(), 0);
			glVertex3f(m_selRectMin.x(), m_selRectMin.y(), 0);
			glEnd();
		}

		glMatrixMode(GL_MODELVIEW);
		glPopMatrix();
		glMatrixMode(GL_PROJECTION);
		glPopMatrix();

//	//	reset renderer matrix
//		m_pRenderer->set_transform((float*)&mat);
//		m_pRenderer->set_perspective(m_fovy, m_aspectRatio,
//									 m_zNear, m_zFar);
	}

}

void  View3D::
drawSelectionRect(bool bDrawIt, float xMin, float yMin,
				  float xMax, float yMax)
{
	m_bDrawSelRect = bDrawIt;
	m_selRectMin = cam::vector2(xMin, m_viewHeight - yMin);
	m_selRectMax = cam::vector2(xMax, m_viewHeight - yMax);
}

unsigned int View3D::get_camera_drag_flags()
{
	const Qt::KeyboardModifiers keys = QApplication::keyboardModifiers();
	const bool shiftPressed = bool(keys & Qt::ShiftModifier);
	const bool ctrlPressed = bool(keys & Qt::ControlModifier);
	unsigned int dragFlags = cam::CDF_NONE;
	
	if(shiftPressed)
		dragFlags |= cam::CDF_ZOOM;
	if(ctrlPressed)
		dragFlags |= cam::CDF_MOVE;

	return dragFlags;
}

void View3D::fly_to(const cam::vector3& destTo, float distance)
{
//	fly and zoom as required.
	m_csOld = m_camera.get_camera_state();
	m_csNew = m_camera.get_camera_state();
	m_csNew.vTo = destTo;
	m_csNew.fDistance = distance;

	// cam::vector3 v;
	// VecSubtract(v, m_csOld.vFrom, m_csOld.vTo);
	// VecNormalize(v, v);
	// VecScale(v, v, distance);
	// VecAdd(m_csNew.vFrom, m_csNew.vTo, v);

	start_interpolation();
}

void  View3D::
get_ray(cam::vector3& vFromOut, cam::vector3& vToOut,
		float screenX, float screenY)
{
	//QSize winSize = size();
	QSize winSize = size()*this->windowHandle()->devicePixelRatio();
	cam::matrix44 tMat = *m_camera.get_camera_transform();
	GLdouble modelMat[16];
	GLdouble projMat[16];
	GLint viewport[4];

	for(int i = 0; i < 4; ++i)
	{
			for(int j = 0; j < 4; ++j)
					modelMat[4*i + j] = tMat[i][j];
	}

	glGetDoublev(GL_PROJECTION_MATRIX, projMat);
	glGetIntegerv(GL_VIEWPORT, viewport);

	GLdouble vx, vy, vz;
	gluUnProject(screenX, winSize.height() - screenY, 0,
				 (GLdouble*)modelMat, projMat, viewport,
				 &vx, &vy, &vz);

	vFromOut = cam::vector3(vx, vy, vz);

	gluUnProject(screenX, winSize.height() - screenY, 1,
				 (GLdouble*)modelMat, projMat, viewport,
				 &vx, &vy, &vz);

	vToOut = cam::vector3(vx, vy, vz);
}

bool View3D::
get_ray_to_geometry(cam::vector3& vFromOut,
					 cam::vector3& vToOut,
					 float screenX, float screenY)
{
	//QSize winSize = size();
	QSize winSize = size()*this->windowHandle()->devicePixelRatio();

	GLfloat depthVal;
	glReadPixels(screenX, winSize.height() - screenY, 1, 1,
				GL_DEPTH_COMPONENT, GL_FLOAT, &depthVal);

	cam::matrix44 tMat = *m_camera.get_camera_transform();
	GLdouble modelMat[16];
	GLdouble projMat[16];
	GLint viewport[4];

	for(int i = 0; i < 4; ++i){
		// UG_LOG("dbg - trans: ");
		for(int j = 0; j < 4; ++j){
			// UG_LOG(tMat[i][j] << ", ");
			modelMat[4*i + j] = tMat[i][j];
		}
		// UG_LOG(std::endl);
	}

	glGetDoublev(GL_PROJECTION_MATRIX, projMat);
	glGetIntegerv(GL_VIEWPORT, viewport);


	// for(int i = 0; i < 4; ++i){
	// 	UG_LOG("dbg - proj: ");
	// 	for(int j = 0; j < 4; ++j){
	// 		UG_LOG(projMat[4*i + j] << ", ");
	// 	}
	// 	UG_LOG(std::endl);
	// }

	GLdouble vx, vy, vz;
	gluUnProject(screenX, winSize.height() - screenY, 0,
				 (GLdouble*)modelMat, projMat, viewport,
				 &vx, &vy, &vz);

	vFromOut = cam::vector3(vx, vy, vz);

	gluUnProject(screenX, winSize.height() - screenY, depthVal,
				 (GLdouble*)modelMat, projMat, viewport,
				 &vx, &vy, &vz);

	vToOut = cam::vector3(vx, vy, vz);

	if(depthVal > 0.999999)
		return false;
	return true;
}

void View3D::refocus_by_screen_coords(int screenX, int screenY)
{
	//QSize winSize = size();
	QSize winSize = size()*this->windowHandle()->devicePixelRatio();

	GLfloat depthVal;
	glReadPixels(screenX, winSize.height() - screenY, 1, 1,
				GL_DEPTH_COMPONENT, GL_FLOAT, &depthVal);

	if(depthVal < 1.f)
	{
		cam::matrix44 tMat = *m_camera.get_camera_transform();
		GLdouble modelMat[16];
		GLdouble projMat[16];
		GLint viewport[4];
		GLdouble vx, vy, vz;

		for(int i = 0; i < 4; ++i)
		{
				for(int j = 0; j < 4; ++j)
						modelMat[4*i + j] = tMat[i][j];
		}

		glGetDoublev(GL_PROJECTION_MATRIX, projMat);
		glGetIntegerv(GL_VIEWPORT, viewport);

		gluUnProject(screenX, winSize.height() - screenY, depthVal,
					 (GLdouble*)modelMat, projMat, viewport, &vx, &vy, &vz);

	//	set new camera state:
		//m_csFrom = m_camera.get_camera_state();
		cam::vector3 nvTo(vx, vy, vz);
		// cam::vector3 nvTo(vx * m_camera.world_scale().x(),
		//                   vy * m_camera.world_scale().y(),
		//                   vz * m_camera.world_scale().z());
		// UG_LOG(">>>\n");
		// UG_LOG("old from at: " << *m_camera.get_from() << endl);
		// UG_LOG("old to at: " << *m_camera.get_to() << endl);
		UG_LOG("new focus at: " << nvTo << endl);

		m_csOld = m_camera.get_camera_state();
		m_csNew = m_csOld;
		m_csNew.vTo = nvTo;
		// m_csNew = m_camera.calculate_camera_state(m_csOld, m_camera.get_from(), &nvTo);
		start_interpolation();
	}
}

void View3D::start_interpolation()
{
//	starts the timer and resets m_interpDur
	m_time.start();
	m_pTimer->start(20);
}

void View3D::interpolate_cam_states()
{
	float ia = (float)m_time.elapsed() / m_camInterpDur;
	if(ia > 1.f)
	{
		ia = 1.f;
		m_pTimer->stop();
	}

	cam::SCameraState cs = m_camera.interpolate_camera_states(m_csOld, m_csNew, ia);
	m_camera.set_camera_state(cs);
	updateGL();
}

////////////////////////////////////////////////////////////////////////
//	event handlers
void View3D::mousePressEvent(QMouseEvent *event)
{
	QMouseEvent* scaledEvent = new QMouseEvent(QEvent::MouseButtonPress,
									QPoint(event->x()*this->windowHandle()->devicePixelRatio(),event->y()*this->windowHandle()->devicePixelRatio()),
									event->button(), event->buttons(), event->modifiers());

//	if alt is pressed, we'll refocus the clicked geometry.
//	if not, we'll start dragging.
//	if(event->button() == Qt::LeftButton){
//		m_camera.begin_drag(event->x(), event->y(),
//							get_camera_drag_flags());
//	}
//	else if(event->button() == Qt::MidButton){
//		m_camera.begin_drag(event->x(), event->y(), cam::CDF_MOVE);
//	}

	if(scaledEvent->button() == Qt::LeftButton){
		m_camera.begin_drag(scaledEvent->x(), scaledEvent->y(),
							get_camera_drag_flags());
	}
	else if(scaledEvent->button() == Qt::MidButton){
		m_camera.begin_drag(scaledEvent->x(), scaledEvent->y(), cam::CDF_MOVE);
	}

//	emit mousePress
	//emit View3D::mousePressed(event);
	emit View3D::mousePressed(scaledEvent);
}

void View3D::mouseMoveEvent(QMouseEvent *event)
{
	QMouseEvent* scaledEvent = new QMouseEvent(QEvent::MouseButtonPress,
									QPoint(event->x()*this->windowHandle()->devicePixelRatio(),event->y()*this->windowHandle()->devicePixelRatio()),
									event->button(), event->buttons(), event->modifiers());

	unsigned int cdf;
	//if(event->buttons().testFlag(Qt::MidButton))
	if(scaledEvent->buttons().testFlag(Qt::MidButton))
		cdf = cam::CDF_MOVE;
	else
		cdf = get_camera_drag_flags();

	if(m_camera.dragging())
	{
		m_camera.drag_to(scaledEvent->x(), scaledEvent->y(), cdf);
		updateGL();
	}

	emit View3D::mouseMoved(scaledEvent);
}

void View3D::mouseReleaseEvent(QMouseEvent *event)
{
	QMouseEvent* scaledEvent = new QMouseEvent(QEvent::MouseButtonPress,
									QPoint(event->x()*this->windowHandle()->devicePixelRatio(),event->y()*this->windowHandle()->devicePixelRatio()),
									event->button(), event->buttons(), event->modifiers());

	unsigned int cdf;
	//if(event->button() == Qt::MidButton)
	if(scaledEvent->button() == Qt::MidButton)
		cdf = cam::CDF_MOVE;
	else
		cdf = get_camera_drag_flags();

	if(m_camera.dragging())
	{
		m_camera.end_drag(scaledEvent->x(), scaledEvent->y(), cdf);
		updateGL();
	}
	emit View3D::mouseReleased(scaledEvent);
}

void View3D::wheelEvent(QWheelEvent *event)
{
	float numDegrees = event->delta() / 8.;
	float numSteps = numDegrees / 15.;
    m_camera.scroll(-numSteps * 0.1, get_camera_drag_flags());
	event->accept();
	updateGL();
}

void View3D::mouseDoubleClickEvent(QMouseEvent *event)
{
	QMouseEvent* scaledEvent = new QMouseEvent(QEvent::MouseButtonPress,
									QPoint(event->x()*this->windowHandle()->devicePixelRatio(),event->y()*this->windowHandle()->devicePixelRatio()),
									event->button(), event->buttons(), event->modifiers());

	//refocus_by_screen_coords(event->x(), event->y());
	refocus_by_screen_coords(scaledEvent->x(), scaledEvent->y());
}

void View3D::keyReleaseEvent(QKeyEvent * event)
{
	QGLWidget::keyReleaseEvent(event);
	emit View3D::keyReleased(event);
}
