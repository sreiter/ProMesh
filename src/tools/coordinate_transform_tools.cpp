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

#include <vector>
#include "app.h"
#include "standard_tools.h"
#include "tools_util.h"
#include "heightfields/interpolated_heightfield.h"
#include "tools/coordinate_transform_tools.h"
#include "tooltips.h"

using namespace ug;
using namespace std;


class ToolCoordinates : public ITool
{
public:
	void execute(LGObject* obj, QWidget* widget){
		ToolWidget* dlg = dynamic_cast<ToolWidget*>(widget);

		vector3 nc(0, 0, 0);
		if(dlg){
			nc.x() = dlg->to_double(0);
			nc.y() = dlg->to_double(1);
			nc.z() = dlg->to_double(2);
		}

		promesh::MoveSelectionTo(obj, nc);

		obj->geometry_changed();
	}

	const char* get_name()		{return "Coordinates";}
	const char* get_tooltip()	{return TOOLTIP_COORDINATES;}
	const char* get_group()		{return "Coordinate Transform";}

	ToolWidget* get_dialog(QWidget* parent)
	{
		ToolWidget *dlg = new ToolWidget(get_name(), parent, this,
								IDB_APPLY | IDB_OK | IDB_CLOSE);

		dlg->addSpinBox(tr("x:"), -1.e+9, 1.e+9, 0, 1, 9);
		dlg->addSpinBox(tr("y:"), -1.e+9, 1.e+9, 0, 1, 9);
		dlg->addSpinBox(tr("z:"), -1.e+9, 1.e+9, 0, 1, 9);

		connect(app::getActiveScene(), SIGNAL(geometry_changed()), dlg, SLOT(refreshContents()));
		connect(app::getActiveScene(), SIGNAL(selection_changed()), dlg, SLOT(refreshContents()));
		connect(app::getMainWindow(), SIGNAL(activeObjectChanged()), dlg, SLOT(refreshContents()));

		return dlg;
	}

	virtual void refresh_dialog(QWidget* dialog)
	{
		ToolWidget* dlg = dynamic_cast<ToolWidget*>(dialog);
		if(!dlg)
			return;

		LGObject* obj = app::getActiveObject();
		vector3 center(0, 0, 0);
		if(obj){
		//	calculate the center of the current selection
			Grid::VertexAttachmentAccessor<APosition> aaPos(obj->grid(), aPosition);
			CalculateCenter(center, obj->selector(), aaPos);
		}

		dlg->setNumber(0, center.x());
		dlg->setNumber(1, center.y());
		dlg->setNumber(2, center.z());
	}
};


class ToolMove : public ITool
{
public:
	void execute(LGObject* obj, QWidget* widget){
		ToolWidget* dlg = dynamic_cast<ToolWidget*>(widget);
		ug::vector3 vMove(0, 0, 0);
		if(dlg){
			vMove.x() = dlg->to_double(0);
			vMove.y() = dlg->to_double(1);
			vMove.z() = dlg->to_double(2);
		}

		promesh::Move(obj, vMove);

		obj->geometry_changed();
	}

	const char* get_name()		{return "Move";}
	const char* get_tooltip()	{return TOOLTIP_MOVE;}
	const char* get_group()		{return "Coordinate Transform";}

	ToolWidget* get_dialog(QWidget* parent){
		ToolWidget *dlg = new ToolWidget(get_name(), parent, this,
								IDB_APPLY | IDB_OK | IDB_CLOSE);
		dlg->addSpinBox(tr("x:"), -1.e+9, 1.e+9, 0, 1, 9);
		dlg->addSpinBox(tr("y:"), -1.e+9, 1.e+9, 0, 1, 9);
		dlg->addSpinBox(tr("z:"), -1.e+9, 1.e+9, 0, 1, 9);
		return dlg;
	}
};

class ToolMoveMeshTo : public ITool
{
public:
	void execute(LGObject* obj, QWidget* widget){
		ToolWidget* dlg = dynamic_cast<ToolWidget*>(widget);
		ug::vector3 vMove(0, 0, 0);
		if(dlg){
			vMove.x() = dlg->to_double(0);
			vMove.y() = dlg->to_double(1);
			vMove.z() = dlg->to_double(2);
		}

		promesh::MoveMeshTo(obj, vMove);

		obj->geometry_changed();
	}

	const char* get_name()		{return "Move Mesh To";}
	const char* get_tooltip()	{return TOOLTIP_MOVE_MESH_TO;}
	const char* get_group()		{return "Coordinate Transform";}

	ToolWidget* get_dialog(QWidget* parent){
		ToolWidget *dlg = new ToolWidget(get_name(), parent, this,
								IDB_APPLY | IDB_OK | IDB_CLOSE);
		dlg->addSpinBox(tr("x:"), -1.e+9, 1.e+9, 0, 1, 9);
		dlg->addSpinBox(tr("y:"), -1.e+9, 1.e+9, 0, 1, 9);
		dlg->addSpinBox(tr("z:"), -1.e+9, 1.e+9, 0, 1, 9);
		return dlg;
	}
};


class ToolNormalMove : public ITool
{
public:
	void execute(LGObject* obj, QWidget* widget){
		ToolWidget* dlg = dynamic_cast<ToolWidget*>(widget);

		number offset = 0.1;

		if(dlg){
			offset = dlg->to_double(0);
		}

		promesh::MoveAlongNormal(obj, offset);

		obj->geometry_changed();
	}

	const char* get_name()		{return "Normal Move";}
	const char* get_tooltip()	{return TOOLTIP_NORMAL_MOVE;}
	const char* get_group()		{return "Coordinate Transform";}

	ToolWidget* get_dialog(QWidget* parent){
		ToolWidget *dlg = new ToolWidget(get_name(), parent, this,
								IDB_APPLY | IDB_OK | IDB_CLOSE);

		dlg->addSpinBox(tr("offset:"), -1.e+9, 1.e+9, 0.1, 0.11, 9);
		return dlg;
	}
};

class ToolScale : public ITool
{
public:
	void execute(LGObject* obj, QWidget* widget){
	ToolWidget* dlg = dynamic_cast<ToolWidget*>(widget);
		ug::vector3 vScale(1, 1, 1);
		bool scaleAroundPivot = false;
		if(dlg){
			vScale.x() = dlg->to_double(0);
			vScale.y() = dlg->to_double(1);
			vScale.z() = dlg->to_double(2);
			scaleAroundPivot = dlg->to_bool(3);
		}

		if(scaleAroundPivot)
			promesh::ScaleAroundPivot(obj, vScale);
		else
			promesh::ScaleAroundCenter(obj, vScale);

		obj->geometry_changed();
	}

	const char* get_name()		{return "Scale";}
	const char* get_tooltip()	{return TOOLTIP_SCALE;}
	const char* get_group()		{return "Coordinate Transform";}

	ToolWidget* get_dialog(QWidget* parent){
		ToolWidget *dlg = new ToolWidget(get_name(), parent, this,
								IDB_APPLY | IDB_OK | IDB_CLOSE);
		dlg->addSpinBox(tr("x:"), -1.e+9, 1.e+9, 1., 0.1, 9);
		dlg->addSpinBox(tr("y:"), -1.e+9, 1.e+9, 1., 0.1, 9);
		dlg->addSpinBox(tr("z:"), -1.e+9, 1.e+9, 1., 0.1, 9);
		dlg->addCheckBox(tr("scale around pivot"), false);

		return dlg;
	}
};

class ToolRotate : public ITool
{
public:
	void execute(LGObject* obj, QWidget* widget){
	ToolWidget* dlg = dynamic_cast<ToolWidget*>(widget);
		ug::vector3 rot(0, 0, 0);
		bool rotateAroundPivot = false;

		if(dlg){
		//	convert to radiants on the fly
			rot.x() = dlg->to_double(0) * (PI / 180.);
			rot.y() = dlg->to_double(1) * (PI / 180.);
			rot.z() = dlg->to_double(2) * (PI / 180.);
			rotateAroundPivot = dlg->to_bool(3);
		}

		if(rotateAroundPivot)
			promesh::RotateAroundPivot(obj, rot);
		else
			promesh::RotateAroundPivot(obj, rot);

		obj->geometry_changed();
	}

	const char* get_name()		{return "Rotate";}
	const char* get_tooltip()	{return TOOLTIP_ROTATE;}
	const char* get_group()		{return "Coordinate Transform";}

	ToolWidget* get_dialog(QWidget* parent){
		ToolWidget *dlg = new ToolWidget(get_name(), parent, this,
								IDB_APPLY | IDB_OK | IDB_CLOSE);

		dlg->addSpinBox(tr("x:"), -1.e+9, 1.e+9, 0, 1, 9);
		dlg->addSpinBox(tr("y:"), -1.e+9, 1.e+9, 0, 1, 9);
		dlg->addSpinBox(tr("z:"), -1.e+9, 1.e+9, 0, 1, 9);

		dlg->addCheckBox(tr("rotate around pivot"), false);
		return dlg;
	}
};

class ToolTransform : public ITool
{
public:
	void execute(LGObject* obj, QWidget* widget){
	ToolWidget* dlg = dynamic_cast<ToolWidget*>(widget);
		bool local = true;
		bool applyToSelection = true;

		ug::matrix44 mat;
		MatDiagSet(mat, 1.);

		if(dlg){
			local = (dlg->to_int(0) == 0);
			applyToSelection = (dlg->to_int(1) == 0);
			mat = dlg->to_matrix44(2);
		}

		Grid& grid = obj->grid();
		Grid::VertexAttachmentAccessor<APosition> aaPos(grid, aPosition);
		Selector& sel = obj->selector();

		GridObjectCollection goc;
		vector3 center(0, 0, 0);
		if(applyToSelection){
			goc = sel.get_grid_objects();
			if(local)
				center = CalculateCenter(goc.begin<Vertex>(),
										goc.end<Vertex>(),
										aaPos);
		}
		else{
			goc = grid.get_grid_objects();
			if(local)
				center = obj->pivot();
		}

		for(ug::VertexIterator iter = goc.begin<Vertex>();
			iter != goc.end<Vertex>(); ++iter)
		{
			ug::vector3 v = aaPos[*iter];
			VecSubtract(v, v, center);

			vector4 v4(v.x(), v.y(), v.z(), 1.);
			ug::vector4 vtmp;
			MatVecMult(vtmp, mat, v4);
			v = vector3(vtmp.x(), vtmp.y(), vtmp.z());
			if(fabs(vtmp.w()) > SMALL)
				VecScale(v, v, 1. / vtmp.w());
			else{
				UG_LOG("Error during Transform: 4-th component in result is 0. Aborting\n");
				obj->geometry_changed();
				return;
			}
			VecAdd(aaPos[*iter], v, center);
		}

		obj->geometry_changed();
	}

	const char* get_name()		{return "Transform";}
	const char* get_tooltip()	{return TOOLTIP_TRANSFORM;}
	const char* get_group()		{return "Coordinate Transform";}

	ToolWidget* get_dialog(QWidget* parent){
		ToolWidget *dlg = new ToolWidget(get_name(), parent, this,
								IDB_APPLY | IDB_OK | IDB_CLOSE);
		QStringList entries;
		entries.push_back(tr("local"));
		entries.push_back(tr("global"));
		dlg->addComboBox(tr("pivot:"), entries, 0);

		entries.clear();
		entries.push_back(tr("selection"));
		entries.push_back(tr("object"));
		dlg->addComboBox(tr("target:"), entries, 0);

		dlg->addMatrix(tr("transform:"), 4, 4);
		return dlg;
	}
};

class ToolConeTransform : public ITool
{
public:
	void execute(LGObject* obj, QWidget* widget){
		ToolWidget* dlg = dynamic_cast<ToolWidget*>(widget);
		using namespace ug;

		vector3 base(0, 0, 0), axis(0, 0, 1);
		number scaleAtTip = 1.0;

		if(dlg){
			base = dlg->to_vector3(0);
			axis = dlg->to_vector3(1);
			scaleAtTip = dlg->to_double(2);
		}

		promesh::ConeTransform(obj, base, axis, scaleAtTip);

		obj->geometry_changed();
	}

	const char* get_name()		{return "Cone Transform";}
	const char* get_tooltip()	{return TOOLTIP_CONE_TRANSFORM;}
	const char* get_group()		{return "Coordinate Transform";}

	ToolWidget* get_dialog(QWidget* parent){
		ToolWidget *dlg = new ToolWidget(get_name(), parent, this,
								IDB_APPLY | IDB_OK | IDB_CLOSE);
		dlg->addVector(tr("base:"), 3);
		dlg->addVector(tr("axis:"), 3);
		dlg->addSpinBox(tr("scale at tip:"), 1.e-12, 1.e+12, 1., 0.1, 9);
		return dlg;
	}
};

class ToolLaplacianSmooth : public ITool
{
public:
	void execute(LGObject* obj, QWidget* widget){
	ToolWidget* dlg = dynamic_cast<ToolWidget*>(widget);
		double alpha = 0.25;
		int numIterations = 10;

		if(dlg){
			alpha = dlg->to_double(0);
			numIterations = dlg->to_int(1);
		}

		promesh::LaplacianSmooth(obj, alpha, numIterations);

		obj->geometry_changed();
	}

	const char* get_name()		{return "Laplacian Smooth";}
	const char* get_tooltip()	{return TOOLTIP_LAPLACIAN_SMOOTH;}
	const char* get_group()		{return "Coordinate Transform";}

	ToolWidget* get_dialog(QWidget* parent){
		ToolWidget *dlg = new ToolWidget(get_name(), parent, this,
								IDB_APPLY | IDB_OK | IDB_CLOSE);
		dlg->addSpinBox(tr("alpha:"), 0, 1., 0.25, 0.001, 5);
		dlg->addSpinBox(tr("iterations:"), 1, 10000000, 10, 1, 0);
		return dlg;
	}
};


class ToolTangentialSmooth : public ITool
{
public:
	void execute(LGObject* obj, QWidget* widget){
	ToolWidget* dlg = dynamic_cast<ToolWidget*>(widget);
		double alpha = 0.25;
		int numIterations = 10;

		if(dlg){
			alpha = dlg->to_double(0);
			numIterations = dlg->to_int(1);
		}

		promesh::TangentialSmooth(obj, alpha, numIterations);

		obj->geometry_changed();
	}

	const char* get_name()		{return "Tangential Smooth";}
	const char* get_tooltip()	{return TOOLTIP_TANGENTIAL_SMOOTH;}
	const char* get_group()		{return "Coordinate Transform";}

	ToolWidget* get_dialog(QWidget* parent){
		ToolWidget *dlg = new ToolWidget(get_name(), parent, this,
								IDB_APPLY | IDB_OK | IDB_CLOSE);
		dlg->addSpinBox(tr("alpha:"), 0, 1., 0.25, 0.001, 5);
		dlg->addSpinBox(tr("iterations:"), 1, 10000000, 10, 1, 0);
		return dlg;
	}
};


class ToolProjectToPlane: public ITool
{
	public:
		void execute(LGObject* obj, QWidget* widget){
			using namespace ug;
			ToolWidget* dlg = dynamic_cast<ToolWidget*>(widget);
			if(dlg){

				CoordinatesWidget* dlgPoint = dynamic_cast<CoordinatesWidget*>(dlg->to_widget(0));
				assert(dlgPoint);
				vector3 point;
				point.x() = dlgPoint->x();
				point.y() = dlgPoint->y();
				point.z() = dlgPoint->z();

				CoordinatesWidget* dlgNormal = dynamic_cast<CoordinatesWidget*>(dlg->to_widget(1));
				assert(dlgNormal);
				vector3 normal;
				normal.x() = dlgNormal->x();
				normal.y() = dlgNormal->y();
				normal.z() = dlgNormal->z();

				ProjectToPlane(obj, point, normal);

				obj->geometry_changed();
			}
		}

		const char* get_name()		{return "Project To Plane";}
		const char* get_tooltip()	{return TOOLTIP_RPOJECT_TO_PLANE;}
		const char* get_group()		{return "Coordinate Transform";}

		QWidget* get_dialog(QWidget* parent){
			ToolWidget *dlg = new ToolWidget(get_name(), parent, this,
								IDB_APPLY | IDB_OK | IDB_CLOSE);

			dlg->addWidget("center", new CoordinatesWidget("", dlg, this, false, false));
			dlg->addWidget("normal", new CoordinatesWidget("", dlg, this, false, false));
			return dlg;
		}
};


class ToolProjectToLimitPLoop : public ITool
{
public:
	void execute(LGObject* obj, QWidget*){
		promesh::ProjectToLimitPLoop(obj);
		obj->geometry_changed();
	}

	const char* get_name()		{return "Project to Limit PLoop";}
	const char* get_tooltip()	{return TOOLTIP_PROJECT_TO_LIMIT_PLOOP;}
	const char* get_group()		{return "Coordinate Transform | Loop Projection";}
};

class ToolProjectToLimitSmoothBoundary : public ITool
{
public:
	void execute(LGObject* obj, QWidget*){
		promesh::ProjectToLimitSmoothBoundary(obj);

		obj->geometry_changed();
	}

	const char* get_name()		{return "Project to Limit Smooth Boundary";}
	const char* get_tooltip()	{return TOOLTIP_PROJECT_TO_LIMIT_SMOOTH_BOUNDARY;}
	const char* get_group()		{return "Coordinate Transform | Loop Projection";}
};

class ToolSetPivot : public ITool
{
public:
	void execute(LGObject* obj, QWidget* widget){
		CoordinatesWidget* dlg = dynamic_cast<CoordinatesWidget*>(widget);
		ug::vector3 newPivot(0, 0, 0);

		if(dlg){
		//	convert to radiants on the fly
			newPivot.x() = dlg->x();
			newPivot.y() = dlg->y();
			newPivot.z() = dlg->z();
		}

		promesh::SetPivot(obj, newPivot);
	}

	const char* get_name()		{return "Set Pivot";}
	const char* get_tooltip()	{return TOOLTIP_SET_PIVOT;}
	const char* get_group()		{return "Coordinate Transform | Pivot";}

	QWidget* get_dialog(QWidget* parent){
		return new CoordinatesWidget(get_name(), parent, this, false);
	}
};

class ToolSetPivotToSelectionCenter : public ITool
{
public:
	void execute(LGObject* obj, QWidget*){
		promesh::SetPivotToSelectionCenter(obj);
	}

	const char* get_name()		{return "Set Pivot To Selection Center";}
	const char* get_tooltip()	{return TOOLTIP_SET_PIVOT_TO_SELECTION_CENTER;}
	const char* get_group()		{return "Coordinate Transform | Pivot";}
};

class ToolSetPivotToMeshCenter : public ITool
{
public:
	void execute(LGObject* obj, QWidget*){
		promesh::SetPivotToMeshCenter(obj);
	}

	const char* get_name()		{return "Set Pivot To Mesh Center";}
	const char* get_tooltip()	{return TOOLTIP_SET_PIVOT_TO_MESH_CENTER;}
	const char* get_group()		{return "Coordinate Transform | Pivot";}
};


class ToolFlattenBentQuadrilaterals : public ITool
{
public:
	void execute(LGObject* obj, QWidget* widget){
		using namespace ug;

		ToolWidget* dlg = dynamic_cast<ToolWidget*>(widget);

		number stepSize = 0.01;
		int numIterations = 100;

		if(dlg){
			stepSize = dlg->to_double(0);
			numIterations = dlg->to_int(1);
		}

		promesh::FlattenBentQuadrilaterals(obj, stepSize, numIterations);

		obj->geometry_changed();
	}

	const char* get_name()		{return "Flatten Bent Quads";}
	const char* get_tooltip()	{return TOOLTIP_FLATTEN_BENT_QUADRILATERALS;}
	const char* get_group()		{return "Coordinate Transform";}

	QWidget* get_dialog(QWidget* parent){
		ToolWidget *dlg = new ToolWidget(get_name(), parent, this,
								IDB_APPLY | IDB_OK | IDB_CLOSE);
		dlg->addSpinBox(tr("step size:"), 1.e-9, 1, 0.01, 0.01, 9);
		dlg->addSpinBox(tr("iterations:"), 1, 1.e6, 100, 10, 0);
		return dlg;
	}
};


class ToolApplyHeightfield : public ITool
{
public:
	void execute(LGObject* obj, QWidget* widget){
		using namespace ug;

		ToolWidget* dlg = dynamic_cast<ToolWidget*>(widget);

		QString filename;
		if(dlg){
			filename = dlg->to_string(0);
		}

		if(!filename.isEmpty()){
			InterpolatedHeightfield interpHf;
			IHeightfield* hf = &interpHf;

			LGObject* obj = app::getActiveObject();
			if(!obj)
				return;

			UG_LOG("Applying heightfield: " << filename.toStdString() << "\n");

			Grid& g = obj->grid();
			Grid::VertexAttachmentAccessor<APosition> aaPos(g, aPosition);
			vector3 min, max;

			CalculateBoundingBox(min, max, g.vertices_begin(), g.vertices_end(), aaPos);

			if(hf->initialize(filename.toStdString().c_str(), min.x(), min.y(),
							max.x(), max.y()))
			{
			//	iterate over all nodes and adjust height.
				for(Grid::traits<Vertex>::iterator iter = g.vertices_begin();
					iter != g.vertices_end(); ++iter)
				{
					vector3& v = aaPos[*iter];
					v.z() = hf->height(v.x(), v.y());
				}
			}

			obj->geometry_changed();
		}

	}

	const char* get_name()		{return "Apply Heightfield";}
	const char* get_tooltip()	{return TOOLTIP_APPLY_HEIGHT_FIELD;}
	const char* get_group()		{return "Coordinate Transform | Heightfields";}

	QWidget* get_dialog(QWidget* parent){
		ToolWidget *dlg = new ToolWidget(get_name(), parent, this,
								IDB_APPLY | IDB_OK | IDB_CLOSE);
		dlg->addFileBrowser(tr("heightfield:"), FWT_OPEN, "*.*");
		return dlg;
	}
};


class ToolStoreVertexCoordinates : public ITool
{
public:
	void execute(LGObject* obj, QWidget*){
		obj->buffer_current_vertex_coordinates();
	}

	const char* get_name()		{return "Store Vertex Coordinates";}
	const char* get_tooltip()	{return "Stores current vertex coordinates to the coordinate-buffer. Use 'Restore Vertex Coordinates' to restore them.";}
	const char* get_group()		{return "Coordinate Transform | Coordinate Buffer";}
};


class ToolRestoreVertexCoordinates : public ITool
{
public:
	void execute(LGObject* obj, QWidget*){
		obj->restore_vertex_coordinates_from_buffer();
	}

	const char* get_name()		{return "Restore Vertex Coordinates";}
	const char* get_tooltip()	{return "Restores vertex coordinates from the coordinate-buffer. Use 'Store Vertex Coordinates' to store them.";}
	const char* get_group()		{return "Coordinate Transform | Coordinate Buffer";}
};


void RegisterCoordinateTransformTools(ToolManager* toolMgr)
{
	toolMgr->set_group_icon("Coordinate Transform", ":images/tool_transform.png");

	toolMgr->register_tool(new ToolCoordinates);
	toolMgr->register_tool(new ToolStoreVertexCoordinates);
	toolMgr->register_tool(new ToolRestoreVertexCoordinates);

	toolMgr->register_tool(new ToolSetPivotToSelectionCenter);
	toolMgr->register_tool(new ToolSetPivotToMeshCenter);
	toolMgr->register_tool(new ToolSetPivot);

	toolMgr->register_tool(new ToolMove);
	toolMgr->register_tool(new ToolMoveMeshTo);
	toolMgr->register_tool(new ToolNormalMove);
	toolMgr->register_tool(new ToolScale);
	toolMgr->register_tool(new ToolRotate);
	toolMgr->register_tool(new ToolTransform);
	toolMgr->register_tool(new ToolConeTransform);
	toolMgr->register_tool(new ToolLaplacianSmooth);
	toolMgr->register_tool(new ToolTangentialSmooth);
	toolMgr->register_tool(new ToolProjectToPlane);
	toolMgr->register_tool(new ToolProjectToLimitPLoop);
	toolMgr->register_tool(new ToolProjectToLimitSmoothBoundary);
	toolMgr->register_tool(new ToolFlattenBentQuadrilaterals);
	toolMgr->register_tool(new ToolApplyHeightfield);
}

//the code below should be used as soon as registry tools are used directly.
// void RegisterCoordinateTransformTools(ToolManager* toolMgr)
// {
// 	toolMgr->set_group_icon("Coordinate Transform", ":images/tool_transform.png");

// 	toolMgr->register_tool(new ToolCoordinates);
// 	toolMgr->register_tool(new ToolStoreVertexCoordinates);
// 	toolMgr->register_tool(new ToolRestoreVertexCoordinates);

// 	toolMgr->register_tool(new ToolTransform);
// 	toolMgr->register_tool(new ToolProjectToPlane);
// 	toolMgr->register_tool(new ToolApplyHeightfield);
	
// 	// toolMgr->register_tool(new ToolSetPivotToSelectionCenter);
// 	// toolMgr->register_tool(new ToolSetPivotToMeshCenter);
// 	// toolMgr->register_tool(new ToolSetPivot);

// 	// toolMgr->register_tool(new ToolMove);
// 	// toolMgr->register_tool(new ToolMoveMeshTo);
// 	// toolMgr->register_tool(new ToolNormalMove);
// 	// toolMgr->register_tool(new ToolScale);
// 	// toolMgr->register_tool(new ToolRotate);

// 	// toolMgr->register_tool(new ToolConeTransform);
// 	// toolMgr->register_tool(new ToolLaplacianSmooth);
// 	// toolMgr->register_tool(new ToolTangentialSmooth);


// 	// toolMgr->register_tool(new ToolProjectToLimitPLoop);
// 	// toolMgr->register_tool(new ToolProjectToLimitSmoothBoundary);

// 	// toolMgr->register_tool(new ToolFlattenBentQuadrilaterals);

// }
