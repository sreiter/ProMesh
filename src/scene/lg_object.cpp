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

#include <cstring>
#include <string>
#include "lg_object.h"
#include "lib_grid/file_io/file_io.h"
#include "lib_grid/file_io/file_io_art.h"
#include "lib_grid/file_io/file_io_dump.h"
#include "lib_grid/file_io/file_io_lgb.h"
#include "lib_grid/file_io/file_io_lgm.h"
#include "lib_grid/file_io/file_io_msh.h"
#include "lib_grid/file_io/file_io_ncdf.h"
#include "lib_grid/file_io/file_io_ng.h"
#include "lib_grid/file_io/file_io_obj.h"
#include "lib_grid/file_io/file_io_stl.h"
#include "lib_grid/file_io/file_io_tetgen.h"
#include "lib_grid/file_io/file_io_tikz.h"
#include "lib_grid/file_io/file_io_txt.h"
#include "lib_grid/file_io/file_io_ug.h"
#include "lib_grid/file_io/file_io_ugx.h"
#include "lib_grid/file_io/file_io_vtu.h"

using namespace std;
using namespace ug;

LGObject* CreateLGObjectFromFile(const char* filename)
{
	LGObject* pObj = new LGObject;
	if(LoadLGObjectFromFile(pObj, filename))
		return pObj;

//	if the load failed we'll erase the object and return NULL.
	delete pObj;
	return NULL;
}

LGObject* CreateEmptyLGObject(const char* name)
{
    LGObject* obj = new LGObject;
    obj->set_name(name);
    // Grid& grid = obj->grid();
    // grid.enable_options(GRIDOPT_STANDARD_INTERCONNECTION | FACEOPT_STORE_ASSOCIATED_VOLUMES);
    return obj;
}

bool LoadLGObjectFromFile(LGObject* pObjOut, const char* filename)
{
	LOG("loading " << filename << " ...\n");

	Grid& grid = pObjOut->grid();
	SubsetHandler& sh = pObjOut->subset_handler();
	pObjOut->m_fileName = filename;

	// grid.enable_options(GRIDOPT_STANDARD_INTERCONNECTION | FACEOPT_STORE_ASSOCIATED_VOLUMES);

//	extract the suffix
	const char* pSuffix = strrchr(filename, '.');
	if(!pSuffix)
		return false;

	bool bLoadSuccessful = false;
	bool bSetDefaultSubsetColors = false;
	if(strcmp(pSuffix, ".ugx") == 0)
	{
	//	load from ugx
		GridReaderUGX ugxReader;
		if(!ugxReader.parse_file(filename)){
			UG_LOG("ERROR in LoadGridFromUGX: File not found: " << filename << std::endl);
			bLoadSuccessful = false;
		}
		else{
			if(ugxReader.num_grids() < 1){
				UG_LOG("ERROR in LoadGridFromUGX: File contains no grid.\n");
				bLoadSuccessful = false;
			}
			else{

				ugxReader.grid(grid, 0, aPosition);

				if(ugxReader.num_subset_handlers(0) > 0)
					ugxReader.subset_handler(sh, 0, 0);

				if(ugxReader.num_subset_handlers(0) > 1)
					ugxReader.subset_handler(pObjOut->crease_handler(), 1, 0);

				if(ugxReader.num_selectors(0) > 0)
					ugxReader.selector(pObjOut->selector(), 0, 0);

				bLoadSuccessful = true;
			}
		}
	}
	else if(strcmp(pSuffix, ".lgb") == 0)
	{
	//	load to lgb. We want to load the marks too.
		ISubsetHandler* ppSH[2];
		ppSH[0] = &pObjOut->subset_handler();
		ppSH[1] = &pObjOut->crease_handler();
		bLoadSuccessful = LoadGridFromLGB(grid, filename, ppSH, 2);
	}
	else{
		bLoadSuccessful = LoadGridFromFile(grid, sh, filename, aPosition);
		bSetDefaultSubsetColors = true;
	}

	if(bLoadSuccessful)
	{
	//	assign the name
		std::string name = filename;
		size_t slashPos = name.find_last_of('/');
		if(slashPos == std::string::npos)
			slashPos = name.find_last_of('\\');
		if(slashPos == std::string::npos)
			slashPos = 0;

		size_t pointPos = name.find_last_of('.');
		if(pointPos == std::string::npos)
			pointPos = name.size() - 1;
		pObjOut->set_name(name.substr(slashPos + 1, pointPos - slashPos - 1).c_str());

	//	initialize the subset-colors
		if(bSetDefaultSubsetColors)
			AssignSubsetColors(pObjOut->subset_handler());

		pObjOut->init_subsets();

		pObjOut->geometry_changed();
	}

	if(bLoadSuccessful){
		LOG("loading done\n");
		LOG("  num vertices:\t" << grid.num_vertices() << endl);
		LOG("  num edges: \t" << grid.num_edges() << endl);
		LOG("  num faces: \t" << grid.num_faces() << endl);
		LOG("  num volumes:\t" << grid.num_volumes() << endl);
		LOG(endl);
	}
	else{
		LOG("loading failed\n");
	}
	return bLoadSuccessful;
}

bool ReloadLGObject(LGObject* obj)
{
	obj->grid().clear_geometry();
	obj->subset_handler().clear();
	if(!LoadLGObjectFromFile(obj, obj->m_fileName.c_str())){
		UG_LOG("Reload Failed!" << std::endl);
		return false;
	}
	return true;
}

bool SaveLGObjectToFile(LGObject* pObj, const char* filename)
{
//	extract the suffix
	if(pObj){
		const char* pSuffix = strrchr(filename, '.');
		if(!pSuffix)
			return false;

		if(strcmp(pSuffix, ".ugx") == 0){
			GridWriterUGX ugxWriter;
			ugxWriter.add_grid(pObj->grid(), "defGrid", aPosition);
			ugxWriter.add_subset_handler(pObj->subset_handler(), "defSH", 0);
			ugxWriter.add_subset_handler(pObj->crease_handler(), "markSH", 0);
			ugxWriter.add_selector(pObj->selector(), "defSel", 0);
			return ugxWriter.write_to_file(filename);
		}
		else if(strcmp(pSuffix, ".lgb") == 0)
		{
		//	save to lgb. We want to save the marks too.
			ISubsetHandler* ppSH[2];
			ppSH[0] = &pObj->subset_handler();
			ppSH[1] = &pObj->crease_handler();
			return SaveGridToLGB(pObj->grid(), filename, ppSH, 2);
		}
		else
			return SaveGridToFile(pObj->grid(), pObj->subset_handler(), filename);
	}
	return false;
}

////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////
//	implementation of LGObject methods
LGObject::LGObject()
{
	init();
}

LGObject::LGObject(const char* name)
{
	init();
	m_name = name;
}

LGObject::~LGObject()
{
//TODO: release the display list.
}

void LGObject::init()
{
	m_selectionChangedSinceLastUndoPoint = false;

	m_shFacesForVolRendering.set_supported_elements(SHE_FACE);
	m_shFacesForVolRendering.assign_grid(m_grid);

	m_name = "default name";
	m_bVisible = true;
	set_color(QColor(Qt::white));
	m_elementMode = LGEM_VOLUME;
	m_numInitializedSubsets = 0;

//	set the default subset-info
	SubsetInfo defSI;
	defSI.name = "subset";
//	flags a subset color as uninitialized
	defSI.color = vector4(1., 1., 1., -1.);
	defSI.materialIndex = 0;
	defSI.subsetState = LGSS_VISIBLE | LGSS_INITIALIZED;
	m_subsetHandler.set_default_subset_info(defSI);

	m_undoHistory = UndoHistoryProvider::inst().create_undo_history();
	m_undoHistory.set_suffix(".ugx");

	m_transformType = TT_NONE;
}

void LGObject::visuals_changed()
{
//	set colors of new subsets
	for(int i = m_numInitializedSubsets; i < m_subsetHandler.num_subsets(); ++i)
	{
		SubsetInfo& si = m_subsetHandler.subset_info(i);
	//	check whether the color is still uninitialized.
		if(si.color.w() < 0){
			vector3 col = GetColorFromStandardPalette(i);
			si.color.x() = col.x();
			si.color.y() = col.y();
			si.color.z() = col.z();
			si.color.w() = 1.f;
		}
	}
	m_numInitializedSubsets = m_subsetHandler.num_subsets();

//	add an entry to the history
//	we have some situations in which we don't want to store undos.
//	especially if we're currently transforming.
	if(m_transformType == TT_NONE){
		create_undo_point();
	}

	ISceneObject::visuals_changed();
}

void LGObject::marks_changed()
{
	visuals_changed();
}

void LGObject::selection_changed()
{
	m_selectionChangedSinceLastUndoPoint = true;
	ISceneObject::selection_changed();
}

void LGObject::geometry_changed()
{
	CalculateFaceNormals(m_grid, m_grid.faces_begin(), m_grid.faces_end(), aPosition, aNormal);
	update_bounding_shapes();

//	call base implementation
	ISceneObject::geometry_changed();
}

void LGObject::add_indicator_point(float x, float y, float z,
								   float r, float g, float b, float a)
{
	m_indicatorPoints.push_back(IndicatorPoint(x, y, z, r, g, b, a));
}

void LGObject::clear_indicator_points()
{
	m_indicatorPoints.clear();
}

bool LGObject::get_indicator_point(size_t index, float& x, float& y, float& z,
								   float& r, float& g, float& b, float& a)
{
	if(index >= m_indicatorPoints.size())
		return false;

	IndicatorPoint& ip = m_indicatorPoints[index];
	x = ip.x; y = ip.y; z = ip.z;
	r = ip.r; g = ip.g; b = ip.b; a = ip.a;

	return true;
}

size_t LGObject::num_indicator_points()
{
	return m_indicatorPoints.size();
}

bool LGObject::load_ugx(const char* filename)
{
	m_subsetHandler.clear();
	m_creaseHandler.clear();
	m_selector.clear();
	m_grid.clear_geometry();

	GridReaderUGX ugxReader;
	if(!ugxReader.parse_file(filename)){
		UG_LOG("ERROR in LGObject::load_ugx: File not found: " << filename << std::endl);
		return false;
	}
	else{
		if(ugxReader.num_grids() < 1){
			UG_LOG("ERROR in LGObject::load_ugx: File contains no grid.\n");
			return false;
		}
		else{
			ugxReader.grid(m_grid, 0, aPosition);

			if(ugxReader.num_subset_handlers(0) > 0)
				ugxReader.subset_handler(m_subsetHandler, 0, 0);

			if(ugxReader.num_subset_handlers(0) > 1)
				ugxReader.subset_handler(m_creaseHandler, 1, 0);

			if(ugxReader.num_selectors(0) > 0)
				ugxReader.selector(m_selector, 0, 0);
			return true;
		}
	}
	return false;
}

void LGObject::create_undo_point()
{
	m_selectionChangedSinceLastUndoPoint = false;
	const char* filename = m_undoHistory.create_history_entry();
	SaveLGObjectToFile(this, filename);
}

bool LGObject::undo()
{
	if(m_selectionChangedSinceLastUndoPoint)
		create_undo_point();

	if(!m_undoHistory.can_undo())
		return false;

	const char* filename = m_undoHistory.undo();

//	ISubsetHandler* ppSH[2];
//	ppSH[0] = &subset_handler();
//	ppSH[1] = &crease_handler();
//	bool bLoadSuccessful = LoadGridFromLGB(m_grid, filename, ppSH, 2);
	bool bLoadSuccessful = load_ugx(filename);

	CalculateFaceNormals(m_grid, m_grid.faces_begin(), m_grid.faces_end(), aPosition, aNormal);
	update_bounding_shapes();

	emit sig_geometry_changed();
	emit sig_visuals_changed();

	return bLoadSuccessful;
}

bool LGObject::redo()
{
	if(!m_undoHistory.can_redo())
		return false;

	m_selectionChangedSinceLastUndoPoint = false;

	const char* filename = m_undoHistory.redo();

//	ISubsetHandler* ppSH[2];
//	ppSH[0] = &subset_handler();
//	ppSH[1] = &crease_handler();
//	bool bLoadSuccessful = LoadGridFromLGB(m_grid, filename, ppSH, 2);
	bool bLoadSuccessful = load_ugx(filename);

	CalculateFaceNormals(m_grid, m_grid.faces_begin(), m_grid.faces_end(), aPosition, aNormal);
	update_bounding_shapes();

	emit sig_geometry_changed();
	emit sig_visuals_changed();

	return bLoadSuccessful;
}


void LGObject::set_num_display_lists(int num)
{
//	glGenLists creates returns an index to a list.
//	those indices don't have to be freed again.
//TODO: improve this by better reusing old lists if
//		multiple resizes have been performed.

	int numOldLists = num_display_lists();
	if(num > numOldLists)
	{
		for(int i = 0; i < num; ++i)
			m_displayLists.push_back(glGenLists(1));
	}
	else if(num < numOldLists)
	{
		for(int i = num; i < numOldLists; ++i)
			glDeleteLists(get_display_list(i), 1);
		m_displayLists.resize(num);
	}

	m_displayModes.resize(num, LGRM_DOUBLE_PASS_SHADED);
}

void LGObject::update_bounding_shapes()
{
//	calculate mesh center and radius
	Grid::VertexAttachmentAccessor<APosition> aaPos(m_grid, aPosition);
	CalculateBoundingBox(m_boundBoxMin, m_boundBoxMax, m_grid.vertices_begin(), m_grid.vertices_end(), aaPos);
	m_boundSphere.set_radius(VecDistance(m_boundBoxMin, m_boundBoxMax) / 2.f);
	vector3 center;
	VecAdd(center, m_boundBoxMin, m_boundBoxMax);
	VecScale(center, center, 0.5f);
	m_boundSphere.set_center(center);
}

////////////////////////////////////////////////////////////////////////
//	subset state handling
uint LGObject::get_subset_state(int index) const
{
	return m_subsetHandler.subset_info(index).subsetState;
}

void LGObject::set_subset_state(int index, uint state)
{
	m_subsetHandler.subset_info(index).subsetState = state;
}

void LGObject::enable_subset_state(int index, uint state)
{
	m_subsetHandler.subset_info(index).subsetState |= state;
}

void LGObject::disable_subset_state(int index, uint state)
{
	m_subsetHandler.subset_info(index).subsetState &= (!state);
}

bool LGObject::subset_state_is_enabled(int index, uint state) const
{
	return (get_subset_state(index) & state) == state;
}

////////////////////////////////////////////////////////////////////////
//	subset visibility
void LGObject::set_subset_visibility(int index, bool visible)
{
	if(visible)
		enable_subset_state(index, LGSS_VISIBLE);
	else
		disable_subset_state(index, LGSS_VISIBLE);
}

bool LGObject::subset_is_visible(int index)
{
	if(index >= m_subsetHandler.num_subsets())
		return false;
	if(index < 0)
		return false;
	return subset_state_is_enabled(index, LGSS_VISIBLE);
}

////////////////////////////////////////////////////////////////////////
//	subset colors
QColor LGObject::get_subset_color(int index) const
{
	const vector4& col = m_subsetHandler.subset_info(index).color;
	QColor qcol;
	qcol.setRgbF(col.x(), col.y(), col.z());
	return qcol;
}

void LGObject::set_subset_color(int index, const QColor& color)
{
	vector4& col = m_subsetHandler.subset_info(index).color;
	col.x() = color.redF();
	col.y() = color.greenF();
	col.z() = color.blueF();
}

bool LGObject::subset_is_initialized(int index) const
{
	return subset_state_is_enabled(index, LGSS_INITIALIZED);
}

void LGObject::init_subset(int index)
{
	if(!subset_is_initialized(index)){
		enable_subset_state(index, LGSS_INITIALIZED);
		enable_subset_state(index, LGSS_VISIBLE);
	}
}

void LGObject::init_subsets()
{
	for(int i = 0; i < num_subsets(); ++i)
		init_subset(i);
}

////////////////////////////////////////
//	transforms

void LGObject::init_transform()
{
//	collect all vertices which are involved in the transform
	m_transformVertices.clear();
	CollectVerticesTouchingSelection(m_transformVertices, m_selector);

//	copy all positions of those vertices to m_transformInitialPositions
	Grid::VertexAttachmentAccessor<APosition> aaPos(m_grid, aPosition);
	m_transformInitialPositions.resize(m_transformVertices.size());

	for(size_t i = 0; i < m_transformVertices.size(); ++i)
		m_transformInitialPositions[i] = aaPos[m_transformVertices[i]];

//	calculate the center of those vertices
	m_transformStart = CalculateBarycenter(m_transformVertices.begin(),
										   m_transformVertices.end(), aaPos);
	m_transformCur = m_transformStart;
}

void LGObject::begin_transform(TransformType tt)
{
//	if we currently are transforming, then first cancel the transform
	if(m_transformType != TT_NONE)
		end_transform(false);

//	set the transform type and initialize the transform
	m_transformType = tt;
	init_transform();
}

ug::vector3 LGObject::transform_center()
{
	return m_transformCur;
}

void LGObject::grab(const ug::vector3& offset)
{
	if(m_transformType != TT_GRAB)
		return;

	assert(m_transformVertices.size() == m_transformInitialPositions.size());

//	Move the vertices according to the offset
	Grid::VertexAttachmentAccessor<APosition> aaPos(m_grid, aPosition);
	for(size_t i = 0; i < m_transformVertices.size(); ++i)
		VecAdd(aaPos[m_transformVertices[i]], m_transformInitialPositions[i], offset);

	VecAdd(m_transformCur, m_transformStart, offset);

//	the geometry has changed. We thus have to update them
	geometry_changed();
}

void LGObject::scale(const ug::vector3& scaleFacs)
{
	if(m_transformType != TT_SCALE)
		return;

	assert(m_transformVertices.size() == m_transformInitialPositions.size());

//	Move the vertices according to the scaleFac
	Grid::VertexAttachmentAccessor<APosition> aaPos(m_grid, aPosition);
	vector3 d;
	for(size_t i = 0; i < m_transformVertices.size(); ++i){
		VecSubtract(d, m_transformInitialPositions[i], m_transformCur);
		d.x() *= scaleFacs.x();
		d.y() *= scaleFacs.y();
		d.z() *= scaleFacs.z();
		VecAdd(aaPos[m_transformVertices[i]], m_transformCur, d);
	}

//	the geometry has changed. We thus have to update them
	geometry_changed();
}

void LGObject::end_transform(bool bApply)
{
	if((m_transformType != TT_NONE) && (!bApply)){
	//	UNDO TRANSFORM
		assert(m_transformVertices.size() == m_transformInitialPositions.size());

	//	We have to reset the vertices to their original positions
		Grid::VertexAttachmentAccessor<APosition> aaPos(m_grid, aPosition);
		for(size_t i = 0; i < m_transformVertices.size(); ++i)
			aaPos[m_transformVertices[i]] = m_transformInitialPositions[i];
	}

	m_transformType = TT_NONE;
//	we call geometry_changed again, to generate an undo-entry
//	(since transform type no is set to TT_NONE)
	geometry_changed();
}


void LGObject::buffer_current_vertex_coordinates()
{
	Grid& grid = this->grid();
	position_accessor_t aaPos = position_accessor();

	m_vertexCoordinateBuffer.clear();
	m_vertexCoordinateBuffer.reserve(grid.num<Vertex>());

	for(VertexIterator ivrt = grid.begin<Vertex>();
		ivrt != grid.end<Vertex>(); ++ivrt)
	{
		m_vertexCoordinateBuffer.push_back(aaPos[*ivrt]);
	}
}


void LGObject::restore_vertex_coordinates_from_buffer()
{
	Grid& grid = this->grid();
	position_accessor_t aaPos = position_accessor();

	const size_t buflen = m_vertexCoordinateBuffer.size();
	size_t ibuf = 0;

	for(VertexIterator ivrt = grid.begin<Vertex>();
		(ivrt != grid.end<Vertex>()) && (ibuf < buflen); ++ivrt, ++ibuf)
	{
		aaPos[*ivrt] = m_vertexCoordinateBuffer[ibuf];
	}

	geometry_changed();
}

