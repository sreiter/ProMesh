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
#include "tools/subset_tools.h"
#include "tooltips.h"

using namespace std;
using namespace ug;

class ToolAssignSubset : public ITool
{
	public:
		void execute(LGObject* obj, QWidget* widget){
			ToolWidget* dlg = dynamic_cast<ToolWidget*>(widget);

			QString subsetName = "";
			int newIndex = 0;
			bool vertices = true;
			bool edges = true;
			bool faces = true;
			bool volumes = true;

			if(dlg){
				subsetName = dlg->to_string(0);
				newIndex = dlg->to_int(1);
				vertices = dlg->to_bool(2);
				edges = dlg->to_bool(3);
				faces = dlg->to_bool(4);
				volumes = dlg->to_bool(5);
			}

			promesh::AssignSubset(obj, newIndex, vertices, edges, faces, volumes);
			if((newIndex >= 0) && (!subsetName.isEmpty()))
				promesh::SetSubsetName(obj, newIndex, subsetName.toLocal8Bit().constData());

			dlg->setString(0, QString(""));

			obj->geometry_changed();
		}

		const char* get_name()		{return "Assign Subset";}
		const char* get_tooltip()	{return TOOLTIP_ASSIGN_SUBSET;}
		const char* get_group()		{return "Subsets";}

		ToolWidget* get_dialog(QWidget* parent){
			ToolWidget *dlg = new ToolWidget(get_name(), parent, this,
											IDB_APPLY | IDB_OK | IDB_CLOSE);
			dlg->addTextBox(tr("new subset name:"), "");
			dlg->addSpinBox(tr("new subset index:"), -1, 1e+9, 0., 1., 0);
			dlg->addCheckBox(tr("assign vertices"), true);
			dlg->addCheckBox(tr("assign edges"), true);
			dlg->addCheckBox(tr("assign faces"), true);
			dlg->addCheckBox(tr("assign volumes"), true);
			return dlg;
		}
};

class ToolAssignNewSubset : public ITool
{
	public:
		void execute(LGObject* obj, QWidget* widget){
			ToolWidget* dlg = dynamic_cast<ToolWidget*>(widget);

			QString subsetName = "";
			int newIndex = obj->subset_handler().num_subsets();
			bool vertices = true;
			bool edges = true;
			bool faces = true;
			bool volumes = true;

			if(dlg){
				subsetName = dlg->to_string(0);
				vertices = dlg->to_bool(1);
				edges = dlg->to_bool(2);
				faces = dlg->to_bool(3);
				volumes = dlg->to_bool(4);
			}

			promesh::AssignSubset(obj, newIndex, vertices, edges, faces, volumes);
			if((newIndex >= 0) && (!subsetName.isEmpty()))
				promesh::SetSubsetName(obj, newIndex, subsetName.toLocal8Bit().constData());

			dlg->setString(0, QString(""));

			obj->geometry_changed();
		}

		const char* get_name()		{return "Assign New Subset";}
		const char* get_tooltip()	{return TOOLTIP_ASSIGN_SUBSET" The new index is automatically chosen as #subsets";}
		const char* get_group()		{return "Subsets";}

		ToolWidget* get_dialog(QWidget* parent){
			ToolWidget *dlg = new ToolWidget(get_name(), parent, this,
											IDB_APPLY | IDB_OK | IDB_CLOSE);
			dlg->addTextBox(tr("new subset name:"), "");
			dlg->addCheckBox(tr("assign vertices"), true);
			dlg->addCheckBox(tr("assign edges"), true);
			dlg->addCheckBox(tr("assign faces"), true);
			dlg->addCheckBox(tr("assign volumes"), true);
			return dlg;
		}
};

class ToolAssignSubsetColors : public ITool
{
	public:
		void execute(LGObject* obj, QWidget*){
			promesh::AssignSubsetColors(obj);
			obj->visuals_changed();
		}

		const char* get_name()		{return "Assign Subset Colors";}
		const char* get_tooltip()	{return TOOLTIP_ASSIGN_SUBSET_COLORS;}
		const char* get_group()		{return "Subsets";}
};


class ToolSeparateFacesByEdgeSubsets : public ITool
{
	public:
		void execute(LGObject* obj, QWidget* widget){
			promesh::SeparateFacesByEdgeSubsets(obj);
			obj->geometry_changed();
		}

		const char* get_name()	{return "Separate Faces By Edge Subsets";}
		const char* get_tooltip()	{return TOOLTIP_SEPARATE_FACES_BY_EDGE_SUBSETS;}
		const char* get_group()		{return "Subsets | Separate";}
};

class ToolSeparateFacesBySelectedEdges : public ITool
{
	public:
		void execute(LGObject* obj, QWidget*){
			promesh::SeparateFacesBySelectedEdges(obj);
			obj->geometry_changed();
		}

		const char* get_name()		{return "Separate Faces By Selected Edges";}
		const char* get_tooltip()	{return TOOLTIP_SEPARATE_FACES_BY_SELECTED_EDGES;}
		const char* get_group()		{return "Subsets | Separate";}
};

class ToolSeparateVolumesByFaceSubsets : public ITool
{
	public:
		void execute(LGObject* obj, QWidget*){
			promesh::SeparateVolumesByFaceSubsets(obj);
			obj->geometry_changed();
		}

		const char* get_name()	{return "Separate Volumes By Face Subsets";}
		const char* get_tooltip()	{return TOOLTIP_SEPARATE_VOLUMES_BY_FACE_SUBSETS;}
		const char* get_group()		{return "Subsets | Separate";}
};

class ToolSeparateVolumesBySelectedFaces : public ITool
{
	public:
		void execute(LGObject* obj, QWidget*){
			promesh::SeparateVolumesBySelectedFaces(obj);
			obj->geometry_changed();
		}

		const char* get_name()	{return "Separate Volumes By Selected Faces";}
		const char* get_tooltip()	{return TOOLTIP_SEPARATE_VOLUMES_BY_SELECTED_FACES;}
		const char* get_group()		{return "Subsets | Separate";}
};

class ToolSeparateIrregularManifoldSubsets : public ITool
{
	public:
		void execute(LGObject* obj, QWidget*){
			promesh::SeparateIrregularManifoldSubsets(obj);
			obj->geometry_changed();
		}

		const char* get_name()	{return "Separate Irregular Manifold Faces";}
		const char* get_tooltip()	{return TOOLTIP_SEPARATE_IRREGULAR_MANIFOLD_SUBSETS;}
		const char* get_group()		{return "Subsets | Separate";}
};

class ToolMoveSubset : public ITool
{
	public:
		void execute(LGObject* obj, QWidget* widget){
			ToolWidget* dlg = dynamic_cast<ToolWidget*>(widget);
			int oldIndex = 0;
			int newIndex = 0;

			if(dlg){
				oldIndex = dlg->to_int(0);
				newIndex = dlg->to_int(1);
			}

			promesh::MoveSubset(obj, oldIndex, newIndex);

			obj->geometry_changed();
		}

		const char* get_name()		{return "Move Subset";}
		const char* get_tooltip()	{return TOOLTIP_MOVE_SUBSET;}
		const char* get_group()		{return "Subsets";}

		ToolWidget* get_dialog(QWidget* parent){
			ToolWidget *dlg = new ToolWidget(get_name(), parent, this,
											IDB_APPLY | IDB_OK | IDB_CLOSE);
			dlg->addSpinBox(tr("old subset index:"), 0, 1e+9, 0., 1., 0);
			dlg->addSpinBox(tr("new subset index:"), 0, 1e+9, 0., 1., 0);
			return dlg;
		}
};

class ToolSwapSubsets : public ITool
{
	public:
		void execute(LGObject* obj, QWidget* widget){
			ToolWidget* dlg = dynamic_cast<ToolWidget*>(widget);
			int oldIndex = 0;
			int newIndex = 0;

			if(dlg){
				oldIndex = dlg->to_int(0);
				newIndex = dlg->to_int(1);
			}

			promesh::SwapSubsets(obj, oldIndex, newIndex);

			obj->geometry_changed();
		}

		const char* get_name()		{return "Swap Subsets";}
		const char* get_tooltip()	{return TOOLTIP_SWAP_SUBSETS;}
		const char* get_group()		{return "Subsets";}

		ToolWidget* get_dialog(QWidget* parent){
			ToolWidget *dlg = new ToolWidget(get_name(), parent, this,
											IDB_APPLY | IDB_OK | IDB_CLOSE);
			dlg->addSpinBox(tr("subset index 1:"), 0, 1e+9, 0., 1., 0);
			dlg->addSpinBox(tr("subset index 2:"), 0, 1e+9, 0., 1., 0);
			return dlg;
		}
};

class ToolJoinSubsets : public ITool
{
	public:
		void execute(LGObject* obj, QWidget* widget){
			ToolWidget* dlg = dynamic_cast<ToolWidget*>(widget);
			int target = 0;
			int s1 = 0;
			int s2 = 0;
			bool eraseUnused = true;

			if(dlg){
				target = dlg->to_int(0);
				s1 = dlg->to_int(1);
				s2 = dlg->to_int(2);
				eraseUnused = dlg->to_bool(3);
			}

			promesh::JoinSubsets(obj, target, s1, s2, eraseUnused);

			obj->geometry_changed();
		}

		const char* get_name()		{return "Join Subsets";}
		const char* get_tooltip()	{return TOOLTIP_JOIN_SUBSETS;}
		const char* get_group()		{return "Subsets";}

		ToolWidget* get_dialog(QWidget* parent){
			ToolWidget *dlg = new ToolWidget(get_name(), parent, this,
											IDB_APPLY | IDB_OK | IDB_CLOSE);
			dlg->addSpinBox(tr("target subset"), 0, 1e+9, 0, 1, 0);
			dlg->addSpinBox(tr("subset 1:"), 0, 1e+9, 0., 1., 0);
			dlg->addSpinBox(tr("subset 2:"), 0, 1e+9, 0., 1., 0);
			dlg->addCheckBox(tr("remove old unused subsets"), true);
			return dlg;
		}
};

class ToolEraseSubset : public ITool
{
	public:
		void execute(LGObject* obj, QWidget* widget){
			ToolWidget* dlg = dynamic_cast<ToolWidget*>(widget);
			bool eraseGeometry = true;
			int index = 0;

			if(dlg){
				eraseGeometry = dlg->to_bool(0);
				index = dlg->to_int(1);
			}
			promesh::EraseSubset(obj, index, eraseGeometry);
			obj->geometry_changed();
		}

		const char* get_name()		{return "Erase Subset";}
		const char* get_tooltip()	{return TOOLTIP_ERASE_SUBSET;}
		const char* get_group()		{return "Subsets";}

		ToolWidget* get_dialog(QWidget* parent){
			ToolWidget *dlg = new ToolWidget(get_name(), parent, this,
											IDB_APPLY | IDB_OK | IDB_CLOSE);
			dlg->addCheckBox(tr("erase geometry"), true);
			dlg->addSpinBox(tr("subset index:"), 0, 1e+9, 0., 1., 0);
			return dlg;
		}
};

class ToolEraseEmptySubsets : public ITool
{
	public:
		void execute(LGObject* obj, QWidget*){
			promesh::EraseEmptySubsets(obj);
			obj->geometry_changed();
		}

		const char* get_name()		{return "Erase Empty Subsets";}
		const char* get_tooltip()	{return TOOLTIP_ERASE_EMPTY_SUBSETS;}
		const char* get_group()		{return "Subsets";}
};

class ToolAdjustSubsetsForUG3 : public ITool
{
	public:
		void execute(LGObject* obj, QWidget* widget){
			ToolWidget* dlg = dynamic_cast<ToolWidget*>(widget);
			bool keepIntfSubs = false;
			if(dlg){
				keepIntfSubs = dlg->to_bool(0);
			}
			promesh::AdjustSubsetsForUG3(obj, keepIntfSubs);
			obj->geometry_changed();
		}

		const char* get_name()		{return "Adjust Subsets For UG3";}
		const char* get_tooltip()	{return TOOLTIP_ADJUST_SUBSETS_FOR_UG3;}
		const char* get_group()		{return "Subsets";}

		ToolWidget* get_dialog(QWidget* parent){
			ToolWidget *dlg = new ToolWidget(get_name(), parent, this,
											IDB_APPLY | IDB_OK | IDB_CLOSE);
			dlg->addCheckBox(tr("keep existing interface subsets"), false);
			return dlg;
		}
};

class ToolAdjustSubsetsForUG4 : public ITool
{
	public:
		void execute(LGObject* obj, QWidget* widget){
			ToolWidget* dlg = dynamic_cast<ToolWidget*>(widget);
			bool preserveExistingSubsets = false;
			if(dlg){
				preserveExistingSubsets = dlg->to_bool(0);
			}
			promesh::AdjustSubsetsForUG4(obj, preserveExistingSubsets);
			obj->geometry_changed();
		}

		const char* get_name()		{return "Adjust Subsets For UG4";}
		const char* get_tooltip()	{return TOOLTIP_ADJUST_SUBSETS_FOR_UG4;}
		const char* get_group()		{return "Subsets";}

		ToolWidget* get_dialog(QWidget* parent){
			ToolWidget *dlg = new ToolWidget(get_name(), parent, this,
											IDB_APPLY | IDB_OK | IDB_CLOSE);
			dlg->addCheckBox(tr("preserve existing subsets"), false);
			return dlg;
		}
};

class ToolSeparateFaceSubsetsByNormal : public ITool
{
	public:
		void execute(LGObject* obj, QWidget*){
			promesh::SeparateFaceSubsetsByNormal(obj);
			obj->geometry_changed();
		}

		const char* get_name()		{return "Separate Face Subsets By Normal";}
		const char* get_tooltip()	{return TOOLTIP_SEPARATE_FACE_SUBSETS_BY_NORMAL;}
		const char* get_group()		{return "Subsets | Separate";}
};

class ToolSeparateFaceSubsetByNormal : public ITool
{
	public:
		void execute(LGObject* obj, QWidget* widget){
			ToolWidget* dlg = dynamic_cast<ToolWidget*>(widget);
			int si = 0;
			if(dlg)
				si = dlg->to_int(0);
			promesh::SeparateFaceSubsetByNormal(obj, si);
			obj->geometry_changed();
		}

		const char* get_name()		{return "Separate Face Subset By Normal";}
		const char* get_tooltip()	{return TOOLTIP_SEPARATE_FACE_SUBSET_BY_NORMAL;}
		const char* get_group()		{return "Subsets | Separate";}

		ToolWidget* get_dialog(QWidget* parent){
			ToolWidget *dlg = new ToolWidget(get_name(), parent, this,
											IDB_APPLY | IDB_OK | IDB_CLOSE);
			dlg->addSpinBox(tr("subset index"), 0, 1.e9, 0, 1, 0);
			return dlg;
		}
};

class ToolAssignSubsetsByQuality : public ITool
{
	public:
		void execute(LGObject* obj, QWidget* widget){
			using namespace ug;
			ToolWidget* dlg = dynamic_cast<ToolWidget*>(widget);
			int numSecs = 5;

			if(dlg){
				numSecs = dlg->to_int(0);
			}
			promesh::AssignSubsetsByQuality(obj, numSecs);
			obj->geometry_changed();
		}

		const char* get_name()	{return "Assign Subsets by Quality";}
		const char* get_tooltip()	{return TOOLTIP_ASSIGN_SUBSETS_BY_QUALITY;}
		const char* get_group()		{return "Subsets";}

		ToolWidget* get_dialog(QWidget* parent){
			ToolWidget *dlg = new ToolWidget(get_name(), parent, this,
											IDB_APPLY | IDB_OK | IDB_CLOSE);
			dlg->addSpinBox(tr("num sections"), 1, 1e+9, 5, 1., 0);
			return dlg;
		}
};

class ToolSeparateDegeneratedBoundaryFaceSubsets : public ITool
{
	public:
		void execute(LGObject* obj, QWidget* widget){
			using namespace ug;
			ToolWidget* dlg = dynamic_cast<ToolWidget*>(widget);
			number angle = 60;
			if(dlg)
				angle = (number)dlg->to_double(0);

			promesh::SeparateDegeneratedBoundaryFaceSubsets(obj, angle);

			obj->geometry_changed();
		}

		const char* get_name()		{return "Separate Degenerated Boundary Face Subsets";}
		const char* get_tooltip()	{return TOOLTIP_SEPARATE_DEGENERATED_BOUNDARY_FACE_SUBSETS;}
		const char* get_group()		{return "Subsets | Separate";}

		ToolWidget* get_dialog(QWidget* parent){
			ToolWidget *dlg = new ToolWidget(get_name(), parent, this,
											IDB_APPLY | IDB_OK | IDB_CLOSE);
			dlg->addSpinBox(tr("max angle"), -1.e9, 1.e9, 60, 1, 9);
			return dlg;
		}
};

class ToolCopySubsetIndicesToSides : public ITool
{
	public:
		void execute(LGObject* obj, QWidget* widget){
			ToolWidget* dlg = dynamic_cast<ToolWidget*>(widget);
			bool selectionOnly = true;
			bool toUnassignedOnly = false;
			if(dlg){
				selectionOnly = dlg->to_bool(0);
				toUnassignedOnly = dlg->to_bool(1);
			}
			promesh::CopySubsetIndicesToSides(obj, selectionOnly, toUnassignedOnly);
			obj->geometry_changed();
		}

		const char* get_name()		{return "Copy Subset Indices To Sides";}
		const char* get_tooltip()	{return TOOLTIP_COPY_SUBSET_INDICES_TO_SIDES;}
		const char* get_group()		{return "Subsets";}

		ToolWidget* get_dialog(QWidget* parent){
			ToolWidget *dlg = new ToolWidget(get_name(), parent, this,
											IDB_APPLY | IDB_OK | IDB_CLOSE);
			dlg->addCheckBox(tr("selection only"), true);
			dlg->addCheckBox(tr("copy to unassigned sides only"), false);
			return dlg;
		}
};

class ToolAssignSubsetsByElementType : public ITool
{
	public:
		void execute(LGObject* obj, QWidget*){
			promesh::AssignSubsetsByElementType(obj);
			obj->geometry_changed();
		}

		const char* get_name()	{return "Assign Subsets By Element Type";}
		const char* get_tooltip()	{return TOOLTIP_ASSIGN_SUBSETS_BY_ELEMENT_TYPE;}
		const char* get_group()		{return "Subsets";}
};


void RegisterSubsetTools(ToolManager* toolMgr)
{
	toolMgr->register_tool(new ToolAssignSubset, Qt::Key_S, SMK_ALT);
	toolMgr->register_tool(new ToolAssignNewSubset);
	toolMgr->register_tool(new ToolMoveSubset);
	toolMgr->register_tool(new ToolSwapSubsets);
	toolMgr->register_tool(new ToolJoinSubsets);
	toolMgr->register_tool(new ToolEraseSubset);
	toolMgr->register_tool(new ToolAdjustSubsetsForUG3);
	// toolMgr->register_tool(new ToolAdjustSubsetsForUG4);
	toolMgr->register_tool(new ToolAssignSubsetsByQuality);
	toolMgr->register_tool(new ToolSeparateFacesByEdgeSubsets);
	toolMgr->register_tool(new ToolSeparateFacesBySelectedEdges);
	toolMgr->register_tool(new ToolSeparateVolumesByFaceSubsets);
	toolMgr->register_tool(new ToolSeparateVolumesBySelectedFaces);
	toolMgr->register_tool(new ToolSeparateFaceSubsetByNormal);
	toolMgr->register_tool(new ToolSeparateFaceSubsetsByNormal);
	toolMgr->register_tool(new ToolSeparateIrregularManifoldSubsets);
	toolMgr->register_tool(new ToolSeparateDegeneratedBoundaryFaceSubsets);
	toolMgr->register_tool(new ToolCopySubsetIndicesToSides);
	toolMgr->register_tool(new ToolAssignSubsetColors);
	toolMgr->register_tool(new ToolAssignSubsetsByElementType);
	toolMgr->register_tool(new ToolEraseEmptySubsets);
}
