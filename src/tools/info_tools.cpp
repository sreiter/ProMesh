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
#include "tetgen.h"
#include "tooltips.h"

using namespace std;
using namespace ug;

class ToolPrintSelectionCenter : public ITool
{
	public:
		void execute(LGObject* obj, QWidget*){

			ug::Grid& grid = obj->grid();
			ug::Selector& sel = obj->selector();
			Grid::VertexAttachmentAccessor<APosition> aaPos(grid, aPosition);

			vector3 center;

		//	calculate and print the center
			if(CalculateCenter(center, sel, aaPos)){
				UG_LOG("selection center: " << center << endl);
			}
			else{
				UG_LOG("selection center: (-,-,-)\n");
			}
		}

		const char* get_name()		{return "Print Selection Center";}
		const char* get_tooltip()	{return TOOLTIP_PRINT_SELECTION_CENTER;}
		const char* get_group()		{return "Info";}
};

class ToolPrintSelectionDirection : public ITool
{
	public:
		void execute(LGObject* obj, QWidget*){

			ug::Selector& sel = obj->selector();
			LGObject::position_accessor_t aaPos = obj->position_accessor();

			bool gotSome = false;
			if(sel.num<Vertex>() >= 2){
				VertexIterator i0 = sel.begin<Vertex>();
				VertexIterator i1 = sel.begin<Vertex>();
				++i1;
				while(i1 != sel.end<Vertex>()){
					vector3 d;
					VecSubtract(d, aaPos[*i1], aaPos[*i0]);
					UG_LOG("  direction between vertices " << aaPos[*i0] << " and "
							<< aaPos[*i1] << ": " << d << endl);
					++i0;
					++i1;
				}
				gotSome = true;
			}
			
			if(sel.num<Edge>() >= 1){
				for(EdgeIterator eiter = sel.begin<Edge>();
					eiter != sel.end<Edge>(); ++eiter)
				{
					Edge* e = *eiter;
					vector3 d;
					VecSubtract(d, aaPos[e->vertex(1)], aaPos[e->vertex(0)]);
					UG_LOG("  direction of edge " << aaPos[e->vertex(0)] << " - "
							<< aaPos[e->vertex(1)] << ": " << d << endl);
				}
				gotSome = true;
			}

			if(!gotSome){
				UG_LOG("ERROR in 'Print Selection Direction':\n"
					   "At least 2 vertices or one edge have to be selected!\n");
				return;
			}
		}

		const char* get_name()		{return "Print Selection Direction";}
		const char* get_tooltip()	{return "Prints the direction of subsequently selected vertices.";}
		const char* get_group()		{return "Info";}
};

class ToolPrintGeometryInfo : public ITool
{
	public:
		void execute(LGObject* obj, QWidget*){

			ug::Grid& grid = obj->grid();
			vector3 vMin, vMax;
			vector3 vDim;
			obj->get_bounding_box(vMin, vMax);
			VecSubtract(vDim, vMax, vMin);

			UG_LOG("Geometry Info:\n");
			UG_LOG("  pivot:\t\t" << obj->pivot() << endl);
			UG_LOG("  bounding box:\t" << vMin << ", " << vMax << endl);
			UG_LOG("  dimensions:\t" << vDim << endl);

			UG_LOG("  vertices:\t" << grid.num<Vertex>() << endl);
			UG_LOG("  edges:\t" << grid.num<Edge>() << endl);
			UG_LOG("  faces:\t" << grid.num<Face>() << endl);
			UG_LOG("  volumes:\t " << grid.num<Volume>() << endl);

			UG_LOG(endl);
		}

		const char* get_name()		{return "Print Geometry Info";}
		const char* get_tooltip()	{return TOOLTIP_PRINT_GEOMETRY_INFO;}
		const char* get_group()		{return "Info";}
};

class ToolPrintFaceQuality : public ITool
{
	public:
		void execute(LGObject* obj, QWidget*){
			using namespace ug;
			ug::Grid& grid = obj->grid();
			ug::Selector& sel = obj->selector();
			Grid::VertexAttachmentAccessor<APosition> aaPos(grid, aPosition);

			UG_LOG("face qualities:\n");
			for(FaceIterator iter = sel.begin<Face>(); iter != sel.end<Face>(); ++iter){
				UG_LOG("  " << FaceQuality(*iter, aaPos) << endl);
			}
			UG_LOG(endl);
		}

		const char* get_name()		{return "Print Face Quality";}
		const char* get_tooltip()	{return TOOLTIP_PRINT_FACE_QUALITY;}
		const char* get_group()		{return "Info";}
};

class ToolPrintSelectionInfo : public ITool
{
	public:
		void execute(LGObject* obj, QWidget*){
			using namespace ug;
			ug::Grid& grid = obj->grid();
			ug::Selector& sel = obj->selector();
			UG_LOG("Selection Info:\n");
			PrintElementNumbers(sel.get_grid_objects());

		//	count the number of selected boundary faces
			if(grid.num_volumes() > 0 && sel.num<Face>() > 0){
				int numBndFaces = 0;
				for(FaceIterator iter = sel.faces_begin(); iter != sel.faces_end(); ++iter){
					if(IsVolumeBoundaryFace(grid, *iter))
						++numBndFaces;
				}
				UG_LOG("  selected boundary faces: " << numBndFaces << endl);
			}
			UG_LOG(endl);
		}

		const char* get_name()		{return "Print Selection Info";}
		const char* get_tooltip()	{return TOOLTIP_PRINT_SELECTION_INFO;}
		const char* get_group()		{return "Info";}
};


template <class TGeomObj>
static bool SubsetContainsSelected(SubsetHandler& sh, Selector& sel, int si)
{
	typedef typename geometry_traits<TGeomObj>::iterator GeomObjIter;
	for(GeomObjIter iter = sh.begin<TGeomObj>(si);
	iter != sh.end<TGeomObj>(si); ++iter)
	{
		if(sel.is_selected(*iter)){
			return true;
		}
	}

	return false;
}

class ToolPrintSelectionContainingSubsets : public ITool
{
	public:
		void execute(LGObject* obj, QWidget*){
			using namespace ug;
			ug::Selector& sel = obj->selector();
			ug::SubsetHandler& sh = obj->subset_handler();

			UG_LOG("Selection containing subsets:");

		//	check for each subset whether it contains selected elements
			for(int i = 0; i < sh.num_subsets(); ++i){
			//	check vertices
				bool gotOne = SubsetContainsSelected<Vertex>(sh, sel, i);
			//	check edges
				if(!gotOne)
					gotOne = SubsetContainsSelected<Edge>(sh, sel, i);
			//	check faces
				if(!gotOne)
					gotOne = SubsetContainsSelected<Face>(sh, sel, i);
			//	check volumes
				if(!gotOne)
					gotOne = SubsetContainsSelected<Volume>(sh, sel, i);

				if(gotOne){
					UG_LOG(" " << i);
				}
			}

			UG_LOG(endl);
		}

		const char* get_name()		{return "Print Selection Containing Subsets";}
		const char* get_tooltip()	{return TOOLTIP_PRINT_SELECTION_CONTAINING_SUBSETS;}
		const char* get_group()		{return "Info";}
};


class ToolPrintVertexDistance : public ITool
{
	public:
		void execute(LGObject* obj, QWidget*){
			using namespace ug;
			ug::Grid& grid = obj->grid();
			ug::Selector& sel = obj->selector();
			Grid::VertexAttachmentAccessor<APosition> aaPos(grid, aPosition);
			UG_LOG("Vertex Distance:");

			number max = 0;
			number min = obj->get_bounding_sphere().get_radius() * 3.;

			vector<Vertex*> vrts;
			CollectVerticesTouchingSelection(vrts, sel);

		//	if there are less than 2 vertices, both distances are set to 0
			if(vrts.size() < 2)
				min = max = 0;
			else{
			//	iterate over all selected vertices
				typedef vector<Vertex*>::iterator VrtIter;
				for(VrtIter baseIter = vrts.begin();
					baseIter != vrts.end(); ++baseIter)
				{
					vector3 basePos = aaPos[*baseIter];

				//	iteate over all vertices between baseVrt and sel.end
					VrtIter iter = baseIter;
					for(iter++; iter != vrts.end(); ++iter){
						number dist = VecDistance(basePos, aaPos[*iter]);
						if(dist > max)
							max = dist;
						if(dist < min)
							min = dist;
					}
				}
			}

			UG_LOG("    min = " << min << ",    max = " << max << "\n");
		}

		const char* get_name()		{return "Print Vertex Distance";}
		const char* get_tooltip()	{return TOOLTIP_PRINT_VERTEX_DISTANCE;}
		const char* get_group()		{return "Info";}
};


class ToolPrintLeastSquaresPlane: public ITool
{
	public:
		void execute(LGObject* obj, QWidget*){
			using namespace ug;
			ug::Grid& grid = obj->grid();
			ug::Selector& sel = obj->selector();
			Grid::VertexAttachmentAccessor<APosition> aaPos(grid, aPosition);

			std::vector<Vertex*> vrts;
			CollectVerticesTouchingSelection(vrts, sel);
			std::vector<vector3> points;
			points.reserve(vrts.size());

			for(size_t i = 0; i < vrts.size(); ++i){
				points.push_back(aaPos[vrts[i]]);
			}

			if(!points.empty()){
				vector3 center, normal;
				if(FindClosestPlane(center, normal, &points.front(), points.size())){
					UG_LOG("LeastSquaresPlane:\n");
					UG_LOG("  center: " << center << "\n");
					UG_LOG("  normal: " << normal<< "\n");
				}
				else{
					UG_LOG("Problem in PrintLeastSquaresPlane:\n");
					UG_LOG("Couldn't find least squares plane. Please alter your selection.\n");
				}
			}
			else{
				UG_LOG("Problem in PrintLeastSquaresPlane:\n");
				UG_LOG("At least 3 vertices, 2 edges or 1 face has to be selected!\n");
			}
		}

		const char* get_name()		{return "Print Least Squares Plane";}
		const char* get_tooltip()	{return TOOLTIP_PRINT_LEAST_SQUARES_PLANE;}
		const char* get_group()		{return "Info";}
};


void RegisterInfoTools(ToolManager* toolMgr)
{
	toolMgr->set_group_icon("Info", ":images/tool_info.png");

	toolMgr->register_tool(new ToolPrintSelectionCenter, Qt::Key_I, SMK_ALT);
	toolMgr->register_tool(new ToolPrintSelectionDirection);
	toolMgr->register_tool(new ToolPrintGeometryInfo, Qt::Key_I);
	toolMgr->register_tool(new ToolPrintFaceQuality);
	toolMgr->register_tool(new ToolPrintSelectionInfo);
	toolMgr->register_tool(new ToolPrintSelectionContainingSubsets);
	toolMgr->register_tool(new ToolPrintVertexDistance);
	toolMgr->register_tool(new ToolPrintLeastSquaresPlane);
}

