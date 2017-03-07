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

#include "app.h"
#include "standard_tools.h"
#include "tooltips.h"

//class ToolSliderTest : public ITool
//{
//	public:
//		void execute(LGObject* obj, QWidget* w){
//			ToolWidget* dlg = dynamic_cast<ToolWidget*>(w);
//			if(dlg){
//				UG_LOG("value: " << dlg->to_double(0) << std::endl);
//			}
//		}
//
//		const char* get_name()	{return "Slider Test";}
//		const char* get_tooltip()	{return TOOLTIP_SLIDER_TEST;}
//		const char* get_group()		{return "Camera";}
//
//		ToolWidget* get_dialog(QWidget* parent)
//		{
//			ToolWidget *dlg = new ToolWidget(get_name(), parent, this,
//									IDB_APPLY | IDB_OK | IDB_CLOSE);
//
//			dlg->addSlider(tr("x:"),-11.11, 13.13, -1.2345);
//			return dlg;
//		}
//};

class ToolCenterObject : public ITool
{
	public:
		void execute(LGObject* obj, QWidget* widget){
			ug::Sphere3 s = obj->get_bounding_sphere();

			app::getMainWindow()->getView3D()->fly_to(
								cam::vector3(s.get_center().x(),
											s.get_center().y(),
											s.get_center().z()),
								s.get_radius() * 4.f + 0.001);
		}

		const char* get_name()		{return "Center Object";}
		const char* get_tooltip()	{return TOOLTIP_CENTER_OBJECT;}
		const char* get_group()		{return "Camera";}
};

class ToolCenterSelection : public ITool
{
	public:
		void execute(LGObject* obj, QWidget* widget){
			ug::Selector& sel = obj->selector();
			ug::Grid::VertexAttachmentAccessor<ug::APosition> aaPos(obj->grid(), ug::aPosition);

			View3D* view = app::getMainWindow()->getView3D();
			cam::SCameraState oldCam = view->camera().get_camera_state();

			ug::vector3 center;

		//	calculate and focus the center
			if(ug::CalculateCenter(center, sel, aaPos)){
				view->fly_to(cam::vector3(center.x(), center.y(),center.z()),
							 oldCam.fDistance);
			}
		}

		const char* get_name()		{return "Center Selection";}
		const char* get_tooltip()	{return TOOLTIP_CENTER_SELECTION;}
		const char* get_group()		{return "Camera";}
};

class ToolTopView : public ITool
{
	public:
		void execute(LGObject* obj, QWidget* widget){
			View3D* view = app::getMainWindow()->getView3D();
			cam::SCameraState oldCam = view->camera().get_camera_state();
		//	construct a new state
			cam::SCameraState newCam;
			newCam.fDistance = oldCam.fDistance;
			newCam.vTo = oldCam.vTo;
			newCam.vFrom = newCam.vTo;
			newCam.vFrom.z() += newCam.fDistance;
			newCam.quatOrientation.set_values(0, 0, -1, 0);

			view->camera().set_camera_state(newCam);
			view->update();
		}

		const char* get_name()		{return "Top View";}
		const char* get_tooltip()	{return TOOLTIP_TOP_VIEW;}
		const char* get_group()		{return "Camera";}
};
/*
class ToolFrontView : public ITool
{
	public:
		void execute(LGObject* obj, QWidget* widget){
			View3D* view = app::getMainWindow()->getView3D();
			cam::SCameraState oldCam = view->camera().get_camera_state();
		//	construct a new state
			cam::SCameraState newCam;
			newCam.fDistance = oldCam.fDistance;
			newCam.vTo = oldCam.vTo;
			newCam.vFrom = newCam.vTo;
			newCam.vFrom.y() -= newCam.fDistance;
			newCam.quatOrientation.set_values(0, 1, 0, 0);

			view->camera().set_camera_state(newCam);
			view->update();
		}

		const char* get_name()		{return "Front View";}
		const char* get_tooltip()	{return TOOLTIP_FRONT_VIEW;}
		const char* get_group()		{return "Camera";}
};

class ToolSideView : public ITool
{
	public:
		void execute(LGObject* obj, QWidget* widget){
			View3D* view = app::getMainWindow()->getView3D();
			cam::SCameraState oldCam = view->camera().get_camera_state();
		//	construct a new state
			cam::SCameraState newCam;
			newCam.fDistance = oldCam.fDistance;
			newCam.vTo = oldCam.vTo;
			newCam.vFrom = newCam.vTo;
			newCam.vFrom.x() -= newCam.fDistance;
			newCam.quatOrientation.set_values(1, 0, 0, 0);

			view->camera().set_camera_state(newCam);
			view->update();
		}

		const char* get_name()		{return "Side View";}
		const char* get_tooltip()	{return TOOLTIP_SIDE_VIEW;}
		const char* get_group()		{return "Camera";}
};
*/

class ToolHideSelectedElements : public ITool
{
	public:
		void execute(LGObject* obj, QWidget*){
			using namespace ug;
			LGScene* scene = app::getActiveScene();
			ug::Selector& sel = obj->selector();
			scene->hide_elements(obj, sel.begin<Vertex>(), sel.end<Vertex>());
			scene->hide_elements(obj, sel.begin<Edge>(), sel.end<Edge>());
			scene->hide_elements(obj, sel.begin<Face>(), sel.end<Face>());
			scene->hide_elements(obj, sel.begin<Volume>(), sel.end<Volume>());
			obj->visuals_changed();
		}

		const char* get_name()		{return "Hide Selected Elements";}
		const char* get_tooltip()	{return TOOLTIP_HIDE_SELECTED_ELEMENTS;}
		const char* get_group()		{return "Camera";}
};


class ToolUnhideElements : public ITool
{
	public:
		void execute(LGObject* obj, QWidget*){
			LGScene* scene = app::getActiveScene();
			scene->unhide_elements(obj);
			obj->visuals_changed();
		}

		const char* get_name()		{return "Unhide Elements";}
		const char* get_tooltip()	{return TOOLTIP_UNHIDE_ELEMENTS;}
		const char* get_group()		{return "Camera";}
};


void RegisterCameraTools(ToolManager* toolMgr)
{
	toolMgr->register_tool(new ToolCenterObject);
	toolMgr->register_tool(new ToolCenterSelection);
	toolMgr->register_tool(new ToolTopView);
	//toolMgr->register_tool(new ToolFrontView);
	//toolMgr->register_tool(new ToolSideView);
	toolMgr->register_tool(new ToolHideSelectedElements);
	toolMgr->register_tool(new ToolUnhideElements);
}
