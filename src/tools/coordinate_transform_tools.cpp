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
		entries.push_back(tr("selection center"));
		entries.push_back(tr("object pivot"));
		dlg->addComboBox(tr("pivot:"), entries, 0);

		entries.clear();
		entries.push_back(tr("selection"));
		entries.push_back(tr("object"));
		dlg->addComboBox(tr("target:"), entries, 0);

		dlg->addMatrix(tr("transform:"), 4, 4);
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
	toolMgr->register_tool(new ToolStoreVertexCoordinates);
	toolMgr->register_tool(new ToolRestoreVertexCoordinates);

	toolMgr->register_tool(new ToolTransform);
	toolMgr->register_tool(new ToolApplyHeightfield);
}
