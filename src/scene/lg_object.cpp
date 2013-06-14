//	created by Sebastian Reiter
//	s.b.reiter@googlemail.com
//	y09 m10 d01

#include <cstring>
#include <string>
#include "lg_object.h"

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
    Grid& grid = obj->get_grid();
    grid.enable_options(GRIDOPT_STANDARD_INTERCONNECTION | FACEOPT_STORE_ASSOCIATED_VOLUMES);
    return obj;
}

bool LoadLGObjectFromFile(LGObject* pObjOut, const char* filename)
{
	LOG("loading " << filename << " ... ");

	Grid& grid = pObjOut->get_grid();
	SubsetHandler& sh = pObjOut->get_subset_handler();

	grid.enable_options(GRIDOPT_STANDARD_INTERCONNECTION | FACEOPT_STORE_ASSOCIATED_VOLUMES);

//	extract the suffix
	const char* pSuffix = strrchr(filename, '.');
	if(!pSuffix)
		return false;

	bool bLoadSuccessful = false;
	bool bSetDefaultSubsetColors = true;
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
					ugxReader.subset_handler(pObjOut->get_crease_handler(), 1, 0);

				bLoadSuccessful = true;
			}
		}
	}
	else if(strcmp(pSuffix, ".txt") == 0)
	{
	//	load from txt
		bLoadSuccessful = LoadGridFromTXT(grid, filename);

	//	since there are no subsets in txt's we'll assign them manually
		sh.assign_subset(grid.faces_begin(), grid.faces_end(), 0);
	}
	else if(strcmp(pSuffix, ".obj") == 0)
	{
	//	load from obj
		bLoadSuccessful = LoadGridFromOBJ(grid, filename, aPosition,
											NULL, &sh);
	}
	else if(strcmp(pSuffix, ".stl") == 0)
	{
	//	load from obj
		bLoadSuccessful = LoadGridFromSTL(grid, filename, &sh, aPosition, aNormal);
	}
	else if(strcmp(pSuffix, ".lgb") == 0)
	{
	//	load to lgb. We want to load the marks too.
		ISubsetHandler* ppSH[2];
		ppSH[0] = &pObjOut->get_subset_handler();
		ppSH[1] = &pObjOut->get_crease_handler();
		bLoadSuccessful = LoadGridFromLGB(grid, filename, ppSH, 2);
		bSetDefaultSubsetColors = false;
	}
	else if(strcmp(pSuffix, ".ele") == 0)
	{
		AInt aVolAttrib;
		grid.attach_to_volumes(aVolAttrib);
		Grid::VolumeAttachmentAccessor<AInt> aaVolAttribs(grid, aVolAttrib);

	//	import from tetgen
		string nodeFile = string(filename).substr(0, strlen(filename) - 4);
		string faceFile = nodeFile;
		nodeFile.append(".node");
		faceFile.append(".face");
		bLoadSuccessful = ImportGridFromTETGEN(grid, nodeFile.c_str(), faceFile.c_str(),
											filename, aPosition,
											NULL, NULL, NULL, &aVolAttrib);

	//	we have to assign all volumes to subsets
		for(VolumeIterator iter = grid.volumes_begin(); iter != grid.volumes_end(); ++iter)
		{
			if(aaVolAttribs[*iter] != -1)
				sh.assign_subset(*iter, aaVolAttribs[*iter]);
		}

		if(sh.num_subsets() == 0)
		{
		//	assign all volumes to subset 1.
			sh.assign_subset(grid.volumes_begin(), grid.volumes_end(), 0);
		}

		grid.detach_from_volumes(aVolAttrib);
	}
	else if(strcmp(pSuffix, ".lgm") == 0)
	{
		bLoadSuccessful = ImportGridFromLGM(grid, filename,
											aPosition, &sh);
	}
	else if(strcmp(pSuffix, ".ng") == 0)
	{
		bLoadSuccessful = ImportGridFromNG(grid, filename,
											aPosition, &sh);
	}
	else{
		bLoadSuccessful = LoadGridFromFile(grid, sh, filename, aPosition);
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

		pObjOut->geometry_changed();

	//	assign all faces that are volume-boundaries to a subset.
/*
		if(grid.num_volumes() > 0)
		{
			int subsetIndex = sh.num_subsets();
			for(FaceIterator iter = grid.faces_begin(); iter != grid.faces_end(); ++iter)
			{
				Face* f = *iter;
				if(sh.get_subset_index(f) == -1)
				{
					if(IsVolumeBoundaryFace(grid, f))
						sh.assign_subset(f, subsetIndex);
				}
			}
			sh.subset_info(subsetIndex).name = "boundary";
		}
*/
/*
	//	initialise subsets
		pObjOut->init_subsets();

	//	initialize the subset-colors
		if(bSetDefaultSubsetColors)
		{
			for(int i = 0; i < pObjOut->num_subsets(); ++i)
				pObjOut->set_subset_color(i, QColor(255, 255, 255, 255));
		}

	//	check subset names
		for(int i = 0; i < sh.num_subsets(); ++i)
		{
			if(sh.subset_info(i).name.size() == 0)
			{
				sh.subset_info(i).name = "subset";
			}
		}
*/
	}

	if(bLoadSuccessful){
		LOG("done\n");
		LOG("  num vertices:\t" << grid.num_vertices() << endl);
		LOG("  num edges: \t" << grid.num_edges() << endl);
		LOG("  num faces: \t" << grid.num_faces() << endl);
		LOG("  num volumes:\t" << grid.num_volumes() << endl);
		LOG(endl);
	}
	else{
		LOG("failed\n");
	}
	return bLoadSuccessful;
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
			ugxWriter.add_grid(pObj->get_grid(), "defGrid", aPosition);
			ugxWriter.add_subset_handler(pObj->get_subset_handler(), "defSH", 0);
			ugxWriter.add_subset_handler(pObj->get_crease_handler(), "markSH", 0);
			return ugxWriter.write_to_file(filename);
		}
		else if(strcmp(pSuffix, ".lgb") == 0)
		{
		//	save to lgb. We want to save the marks too.
			ISubsetHandler* ppSH[2];
			ppSH[0] = &pObj->get_subset_handler();
			ppSH[1] = &pObj->get_crease_handler();
			return SaveGridToLGB(pObj->get_grid(), filename, ppSH, 2);
		}
		else
			return SaveGridToFile(pObj->get_grid(), pObj->get_subset_handler(), filename);
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
	m_undoHistory.set_suffix(".lgb");

	m_transformType = TT_NONE;
}

void LGObject::visuals_changed()
{
//	add an entry to the history
//	we have some situations in which we don't want to store undos.
//	especially if we're currently transforming.
	if(m_transformType == TT_NONE){
		const char* filename = m_undoHistory.create_history_entry();
		SaveLGObjectToFile(this, filename);
	}

//	set colors of new subsets
	for(int i = m_numInitializedSubsets; i < m_subsetHandler.num_subsets(); ++i)
	{
		SubsetInfo& si = m_subsetHandler.subset_info(i);
	//	check whether the color is still uninitialized.
		if(si.color.w < 0){
			vector3 col = GetColorFromStandardPalette(i);
			si.color.x = col.x;
			si.color.y = col.y;
			si.color.z = col.z;
			si.color.w = 1.f;
		}
	}
	m_numInitializedSubsets = m_subsetHandler.num_subsets();

	ISceneObject::visuals_changed();
}

void LGObject::marks_changed()
{
	visuals_changed();
}

void LGObject::selection_changed()
{
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


bool LGObject::undo()
{
	if(!m_undoHistory.can_undo())
		return false;

	const char* filename = m_undoHistory.undo();
	m_subsetHandler.clear();
	m_creaseHandler.clear();
	m_grid.clear_geometry();

	ISubsetHandler* ppSH[2];
	ppSH[0] = &get_subset_handler();
	ppSH[1] = &get_crease_handler();
	bool bLoadSuccessful = LoadGridFromLGB(m_grid, filename, ppSH, 2);

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

	const char* filename = m_undoHistory.redo();
	m_subsetHandler.clear();
	m_creaseHandler.clear();
	m_grid.clear_geometry();

	ISubsetHandler* ppSH[2];
	ppSH[0] = &get_subset_handler();
	ppSH[1] = &get_crease_handler();
	bool bLoadSuccessful = LoadGridFromLGB(m_grid, filename, ppSH, 2);

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
	qcol.setRgbF(col.x, col.y, col.z);
	return qcol;
}

void LGObject::set_subset_color(int index, const QColor& color)
{
	vector4& col = m_subsetHandler.subset_info(index).color;
	col.x = color.redF();
	col.y = color.greenF();
	col.z = color.blueF();
}

bool LGObject::subset_is_initialized(int index) const
{
	return subset_state_is_enabled(index, LGSS_INITIALIZED);
}

void LGObject::init_subset(int index)
{
	enable_subset_state(index, LGSS_INITIALIZED);
	enable_subset_state(index, LGSS_VISIBLE);
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
		d.x *= scaleFacs.x;
		d.y *= scaleFacs.y;
		d.z *= scaleFacs.z;
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


