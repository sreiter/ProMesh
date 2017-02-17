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
#include <QApplication>
#include "main_window.h"
#include "app.h"
#include "lib_grid/algorithms/selection_util.h"
#include "tools/tool_manager.h"
#include "options/options.h"
#include "modules/module_interface.h"

using namespace std;
using namespace ug;

void MainWindow::beginMouseMoveAction(MouseMoveAction mma)
{
	m_mouseMoveActionObject = app::getActiveObject();
	if(m_mouseMoveActionObject){
		m_activeAxis = X_AXIS | Y_AXIS | Z_AXIS;
		m_mouseMoveAction = mma;
		m_mouseMoveActionStart = QCursor::pos();
		setMouseTracking(true);
		grabMouse();

		switch(mma){
			case MMA_GRAB:
				m_mouseMoveActionObject->begin_transform(TT_GRAB);
				break;
			case MMA_SCALE:
				m_mouseMoveActionObject->begin_transform(TT_SCALE);
				break;
			default:
				break;
		}
	}
}

void MainWindow::updateMouseMoveAction()
{
	LGObject* obj = m_mouseMoveActionObject;
	if(!obj)
		return;

//	calculate the transform in world coordinates
//	get the current position and calculate the offset from the initial position
	QPoint pos = QCursor::pos();
	number dx = pos.x() - m_mouseMoveActionStart.x();
	number dy = pos.y() - m_mouseMoveActionStart.y();

//	get horizontal and vertical camera axis
	cam::CModelViewerCamera& cam = m_pView->camera();

	vector3 right = cam.get_right_dir();
	vector3 up = cam.get_up_dir();

//	speed of grab depends on the distance of the grab-center to
//	the camera.
	vector3 from = *cam.get_from();
	vector3 grabCenter = obj->transform_center();

	number camDist = VecDistance(from, grabCenter);

	switch(m_mouseMoveAction){
		case MMA_GRAB:
		{
		//	calculate the offset and perform the grab
			vector3 dir;
			VecScaleAdd(dir, dx, right, - dy, up);
			if(m_activeAxis == X_AXIS)
				dir.y() = dir.z() = 0;
			if(m_activeAxis == Y_AXIS)
				dir.x() = dir.z() = 0;
			if(m_activeAxis == Z_AXIS)
				dir.x() = dir.y() = 0;

			VecScale(dir, dir, camDist / 1000.f);
			obj->grab(dir);
		}break;

		case MMA_SCALE:
		{
		//	calculate the scale-facs
			number scale = 1. + (dx-dy) / 250.;
			vector3 scaleFacs(1,1,1);
			if(m_activeAxis & X_AXIS)
				scaleFacs.x() = scale;
			if(m_activeAxis & Y_AXIS)
				scaleFacs.y() = scale;
			if(m_activeAxis & Z_AXIS)
				scaleFacs.z() = scale;

			obj->scale(scaleFacs);
		}break;

		default: break;
	}
}

void MainWindow::endMouseMoveAction(bool bApply)
{
	if(m_mouseMoveAction != MMA_DEFAULT){
		m_mouseMoveActionObject->end_transform(bApply);
		m_mouseMoveAction = MMA_DEFAULT;
		setMouseTracking(false);
		releaseMouse();
	}
}


// BEGIN: QMainWindow events
void MainWindow::mousePressEvent(QMouseEvent* event)
{
	if(m_mouseMoveAction != MMA_DEFAULT){
		if(event->button() == Qt::LeftButton)
			endMouseMoveAction(true);
		else if(event->button() == Qt::RightButton)
			endMouseMoveAction(false);
	}
}

void MainWindow::mouseMoveEvent(QMouseEvent* event)
{
	if(m_mouseMoveAction != MMA_DEFAULT)
		updateMouseMoveAction();
}

void MainWindow::mouseReleaseEvent(QMouseEvent* event)
{
	if(m_mouseMoveAction != MMA_DEFAULT){
		endMouseMoveAction(false);
	}
}

void MainWindow::keyPressEvent(QKeyEvent* event)
{
//	if a modifier key is pressed, we don't want the standard keys to be checked.
//	We thus set key to 0 if a modifier is pressed.
	int modifiedKey = event->key();
	Qt::KeyboardModifiers qtMods = QApplication::keyboardModifiers();

	if(!qtMods.testFlag(Qt::NoModifier))
		modifiedKey = 0;

	switch(modifiedKey)
	{
/*		case Qt::Key_Space:
			m_toolsMenu->popup(QCursor::pos());
			break;
*/
		case Qt::Key_A:
		{
		//	Get the current object. If the selection is not empty, then
		//	we'll deselect everything, if it is empty, then we'll select
		//	all elements.
			LGObject* obj = app::getActiveObject();
			if(obj){
				Selector& sel = obj->selector();
				if(!sel.empty())
					sel.clear();
				else{
					Grid& g = obj->grid();
					sel.select(g.vertices_begin(), g.vertices_end());
					sel.select(g.edges_begin(), g.edges_end());
					sel.select(g.faces_begin(), g.faces_end());
					sel.select(g.volumes_begin(), g.volumes_end());
				}
				obj->selection_changed();
			}
		}break;

		case Qt::Key_G:
			beginMouseMoveAction(MMA_GRAB);
			break;

		case Qt::Key_S:
			beginMouseMoveAction(MMA_SCALE);
			break;

		case Qt::Key_V:{
				QPoint p = m_pView->mapFromGlobal(QCursor::pos());
				insertVertexAtScreenCoord(p.x(), p.y());
			}break;

		case Qt::Key_X:
			if(m_activeAxis == X_AXIS)
				m_activeAxis = X_AXIS | Y_AXIS | Z_AXIS;
			else
				m_activeAxis = X_AXIS;
			break;

		case Qt::Key_Y:
			if(m_activeAxis == Y_AXIS)
				m_activeAxis = X_AXIS | Y_AXIS | Z_AXIS;
			else
				m_activeAxis = Y_AXIS;
			break;

		case Qt::Key_Z:
			if(m_activeAxis == Z_AXIS)
				m_activeAxis = X_AXIS | Y_AXIS | Z_AXIS;
			else
				m_activeAxis = Z_AXIS;
			break;

		case Qt::Key_Escape:
			endMouseMoveAction(false);
			break;

		default:
			m_activeModule->keyPressEvent(event);
			break;
	}

	if(m_mouseMoveAction != MMA_DEFAULT)
		updateMouseMoveAction();

	QMainWindow::keyReleaseEvent(event);
}

// END QMainWindow events


template <class TElem>
void MainWindow::
selectElement(LGObject* obj, TElem* elem, bool extendSelection)
{
	if(obj && elem){
		Selector& sel = obj->selector();

		if(extendSelection)
		{
		//	extend the selection
			if(!sel.is_selected(elem))
				sel.select(elem);
			else
				sel.deselect(elem);
		}
		else{
			sel.clear();
			sel.select(elem);
		}

		obj->selection_changed();
	}
}

void MainWindow::view3dMousePressed(QMouseEvent *event)
{
	using namespace ug;

	bool extendSelection = ((QApplication::keyboardModifiers()
							& Qt::ShiftModifier)
							== Qt::ShiftModifier);

	bool selectSubset = ((QApplication::keyboardModifiers()
							& Qt::ControlModifier)
							== Qt::ControlModifier);

	vector3 from, to;
	LGObject* obj = getActiveObject();

	bool pointOnGeom = m_pView->get_ray_to_geometry(from, to, event->x(), event->y());

	if(event->button() == Qt::RightButton){
		if(selectSubset)
			m_curSelectionMode = 0;
		else
			m_curSelectionMode = m_selectionMode;

		switch(m_curSelectionMode){
		case 0:{// click select
			if(!obj)
				return;

			switch(m_selectionElement){
				case 0:// vertices
					{
						Vertex* v = m_scene->get_clicked_vertex(obj, from, to);
						if(v){
							int si = obj->subset_handler().get_subset_index(v);
							if(selectSubset && (si != -1)){
								if(!extendSelection)	obj->selector().clear();
								SelectSubsetElements<Vertex>(obj->selector(),
																 obj->subset_handler(), si);
								obj->selection_changed();
							}
							else
								selectElement(obj, v, extendSelection);

							if(si == -1)
								m_sceneInspector->setActiveObject(m_sceneInspector->getActiveObjectIndex());
							else
								m_sceneInspector->setActiveSubset(m_sceneInspector->getActiveObjectIndex(), si);
						}
					}break;
				case 1://edges
					{
						Edge* e = m_scene->get_clicked_edge(obj, from, to, pointOnGeom);
						if(e){
							int si = obj->subset_handler().get_subset_index(e);
							if(selectSubset && (si != -1)){
								if(!extendSelection)	obj->selector().clear();
								SelectSubsetElements<Edge>(obj->selector(),
															   obj->subset_handler(), si);
								obj->selection_changed();
							}
							else
								selectElement(obj, e, extendSelection);

							if(si == -1)
								m_sceneInspector->setActiveObject(m_sceneInspector->getActiveObjectIndex());
							else
								m_sceneInspector->setActiveSubset(m_sceneInspector->getActiveObjectIndex(), si);
						}
					}break;
				case 2://faces
					{
						Face* f = m_scene->get_clicked_face(obj, from, to);
						if(f){
							int si = obj->subset_handler().get_subset_index(f);
							if(selectSubset && (si != -1)){
								if(!extendSelection)	obj->selector().clear();
								SelectSubsetElements<Face>(obj->selector(),
														   obj->subset_handler(), si);
								obj->selection_changed();
							}
							else
								selectElement(obj, f, extendSelection);

							if(si == -1)
								m_sceneInspector->setActiveObject(m_sceneInspector->getActiveObjectIndex());
							else
								m_sceneInspector->setActiveSubset(m_sceneInspector->getActiveObjectIndex(), si);
						}
					}break;
				case 3://volumes
					{
						Volume* v = m_scene->get_clicked_volume(obj, from, to);
						if(v){
							int si = obj->subset_handler().get_subset_index(v);
							if(selectSubset && (si != -1)){
								if(!extendSelection)	obj->selector().clear();
								SelectSubsetElements<Volume>(obj->selector(),
																 obj->subset_handler(), si);
								obj->selection_changed();
							}
							else
								selectElement(obj, v, extendSelection);

							if(si == -1)
								m_sceneInspector->setActiveObject(m_sceneInspector->getActiveObjectIndex());
							else
								m_sceneInspector->setActiveSubset(m_sceneInspector->getActiveObjectIndex(), si);
						}
					}break;
				}// end of element-switch
			}break;

		case 1:
		case 2:{// box-select
				m_mouseDownPos = QPoint(event->x(), event->y());
				m_pView->drawSelectionRect(true, event->x(), event->y(),
										   event->x(), event->y());
			}break;

		}//	end of mode-switch
	}
}

void MainWindow::view3dMouseMoved(QMouseEvent *event)
{
	switch(m_mouseMoveAction){
		case MMA_DEFAULT:
			if(m_curSelectionMode >= 1 && (event->buttons() & Qt::RightButton))
			{
			//	draw the selection rect.
				m_pView->drawSelectionRect(true, m_mouseDownPos.x(), m_mouseDownPos.y(),
									   event->x(), event->y());
				m_pView->update();
			}
			break;
	}
}

void MainWindow::view3dMouseReleased(QMouseEvent *event)
{
//	if box select is active and the right button was released,
//	then we have to select all elements in the box.
	if(m_curSelectionMode >= 1 && event->button() == Qt::RightButton)
	{
		m_pView->drawSelectionRect(false);

		LGObject* obj = getActiveObject();
		if(!obj){
			m_pView->update();
			return;
		}

		Selector& sel = obj->selector();

		bool extendSelection = ((QApplication::keyboardModifiers()
								& Qt::ShiftModifier)
								== Qt::ShiftModifier);

		if(!extendSelection)
			sel.clear();

		vector3 from, to;
		m_pView->get_ray_to_geometry(from, to, event->x(), event->y());

		float xMin = min(m_mouseDownPos.x(), event->x());
		float xMax = max(m_mouseDownPos.x(), event->x());
		float yMin = min(m_mouseDownPos.y(), event->y());
		float yMax = max(m_mouseDownPos.y(), event->y());

		switch(m_selectionElement){
			case 0:// vertices
			{
				vector<Vertex*> vrts;
				m_scene->get_vertices_in_rect(vrts, obj, xMin, yMin, xMax, yMax);
				for(size_t i = 0; i < vrts.size(); ++i)
					sel.select(vrts[i]);
			}break;

			case 1://edges
			{
				vector<Edge*> edges;
				if(m_curSelectionMode == 1)
					m_scene->get_edges_in_rect_cut(edges, obj, xMin, yMin, xMax, yMax);
				else
					m_scene->get_edges_in_rect(edges, obj, xMin, yMin, xMax, yMax);

				for(size_t i = 0; i < edges.size(); ++i)
					sel.select(edges[i]);
			}break;

			case 2://faces
			{
				vector<Face*> faces;
				if(m_curSelectionMode == 1)
					m_scene->get_faces_in_rect_cut(faces, obj, xMin, yMin, xMax, yMax);
				else
					m_scene->get_faces_in_rect(faces, obj, xMin, yMin, xMax, yMax);

				for(size_t i = 0; i < faces.size(); ++i)
					sel.select(faces[i]);
			}break;

			case 3://volumes
			{
				vector<Volume*> vols;
				if(m_curSelectionMode == 1)
					m_scene->get_volumes_in_rect_cut(vols, obj, xMin, yMin, xMax, yMax);
				else
					m_scene->get_volumes_in_rect(vols, obj, xMin, yMin, xMax, yMax);

				for(size_t i = 0; i < vols.size(); ++i)
					sel.select(vols[i]);
			}break;
		}// end of element-switch

		obj->selection_changed();
	}
}

void MainWindow::view3dKeyReleased(QKeyEvent *event)
{

}

void MainWindow::
insertVertexAtScreenCoord(number x, number y)
{
	if(LGObject* o = app::getActiveObject()){
	//	place a new vertex
		vector3 from, to;
		if(!m_pView->get_ray_to_geometry(from, to, x, y)){
			// UG_LOG("dbg - from: " << from << endl);
			// UG_LOG("dbg - to: " << to << endl);

			vector3 dir;
			VecSubtract(dir, to, from);
			vector3 p = *m_pView->camera().get_to();
			vector3 n = *m_pView->camera().get_dir();
			// UG_LOG("dbg - p: " << p << endl);
			// UG_LOG("dbg - n: " << n << endl);
			VecNormalize(n, n);
			vector3 newTo;
			number t;
			if(!RayPlaneIntersection(newTo, t, from, dir, p, n)){
				UG_LOG("ERROR: Can't determine target position of new vertex!\n");
				return;
			}
			to = newTo;
		}

		int si = app::getActiveSubsetIndex();
		if(si < 0) si = 0;

	//	check if a vertex is close
		const number snapDistSq = sq(GetOptions().drawPath.snapDistance);
		Vertex* vrt = NULL;
		Grid& g = o->grid();

		LGObject::position_accessor_t& aaPos = o->position_accessor();

		bool geometryChanged = false;
		for(VertexIterator ivrt = g.vertices_begin();
			ivrt != g.vertices_end(); ++ivrt)
		{
			if(VecDistanceSq(aaPos[*ivrt], to) <= snapDistSq){
				vrt = *ivrt;
				break;
			}
		}

		if(!vrt){
			vrt = o->create_vertex(to);
			o->subset_handler().assign_subset(vrt, si);
			geometryChanged = true;
		}

		Selector& sel = o->selector();
		for(VertexIterator ivrt = sel.begin<Vertex>();
			ivrt != sel.end<Vertex>(); ++ivrt)
		{
			if(*ivrt != vrt){
				Edge* e = *o->grid().create<RegularEdge>(EdgeDescriptor(*ivrt, vrt));
				o->subset_handler().assign_subset(e, si);
				geometryChanged = true;
			}
		}
		sel.clear();
		sel.select(vrt);
		if(geometryChanged)
			o->geometry_changed();
		else
			o->selection_changed();
		// UG_LOG("created vertex at " << to << "\n");
	}
}
