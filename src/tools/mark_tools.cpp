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
#include "tools/selection_tools.h"

using namespace ug;

class ToolClearMarks : public ITool
{
	public:
		void execute(LGObject* obj, QWidget*){
			obj->crease_handler().clear();
			obj->marks_changed();
		}

		const char* get_name()		{return "Clear Marks";}
		const char* get_tooltip()	{return TOOLTIP_CLEAR_MARKS;}
		const char* get_group()		{return "Selection | Marks";}
};

class ToolMarkCreaseEdges : public ITool
{
	public:
		void execute(LGObject* obj, QWidget* widget){
			ToolWidget* dlg = dynamic_cast<ToolWidget*>(widget);
			double minAngle = 25.;
			bool clearMarks = true;
			if(dlg){
				minAngle = dlg->to_double(0);
				clearMarks = (dlg->to_int(1) != 0);
			}

			if(clearMarks)
				obj->crease_handler().clear();

			ug::MarkCreaseEdges(obj->grid(),
							obj->crease_handler(),
							obj->grid().begin<Edge>(),
							obj->grid().end<Edge>(),
							REM_CREASE, minAngle);
			ug::MarkFixedCreaseVertices(obj->grid(),
									obj->crease_handler(),
									ug::REM_CREASE, ug::REM_FIXED);
			obj->marks_changed();
		}

		const char* get_name()		{return "Mark Crease Edges";}
		const char* get_tooltip()	{return TOOLTIP_MARK_CREASE_EDGES;}
		const char* get_group()		{return "Selection | Marks";}

		ToolWidget* get_dialog(QWidget* parent){
			ToolWidget *dlg = new ToolWidget(get_name(), parent, this,
									IDB_APPLY | IDB_OK | IDB_CLOSE);
			dlg->addSpinBox(tr("min crease angle:"), 0, 180, 25, 1, 0);
			dlg->addCheckBox(tr("clear marks:"), true);
			return dlg;
		}
};

class ToolMarkSelection : public ITool
{
	public:
		void execute(LGObject* obj, QWidget*){
			ug::promesh::MarkSelection(obj);
			obj->marks_changed();
		}

		const char* get_name()		{return "Mark Selection";}
		const char* get_tooltip()	{return TOOLTIP_MARK_SELECTION;}
		const char* get_group()		{return "Selection | Marks";}
};

class ToolUnmarkSelection : public ITool
{
	public:
		void execute(LGObject* obj, QWidget*)
		{
			ug::promesh::UnmarkSelection(obj);

			obj->marks_changed();
		}

		const char* get_name()		{return "Unmark Selection";}
		const char* get_tooltip()	{return TOOLTIP_UNMARK_SELECTION;}
		const char* get_group()		{return "Selection | Marks";}
};


void RegisterMarkTools(ToolManager* toolMgr)
{
	toolMgr->register_tool(new ToolClearMarks);
	toolMgr->register_tool(new ToolMarkSelection, Qt::Key_M);
	toolMgr->register_tool(new ToolUnmarkSelection);
	toolMgr->register_tool(new ToolMarkCreaseEdges);
}
