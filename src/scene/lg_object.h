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

#ifndef __H__LG_OBJECT__
#define __H__LG_OBJECT__

#include <QtOpenGL>
#include <QObject>
#include "scene_interface.h"
#include "lg_include.h"
#include "mesh.h"
#include "undo.h"

////////////////////////////////////////////////////////////////////////
//	predeclarations
class LGObject;

////////////////////////////////////////////////////////////////////////
//	constants
///	constants to store in the subset-states.
enum LGSubsetState
{
///	marks a subset as initialized
	LGSS_INITIALIZED = ug::SS_USER_STATE << 1,
///	marks a subset as visible
	LGSS_VISIBLE = ug::SS_USER_STATE << 2
};

///	used to set the element-type that is displayed.
enum LGElementMode
{
	LGEM_NONE = 0,
	LGEM_VERTEX = 1,
	LGEM_EDGE = 1<<1 | LGEM_VERTEX,
	LGEM_FACE = 1<<2 | LGEM_EDGE,
	LGEM_VOLUME = 1<<3 | LGEM_FACE
};

enum LGRenderMode
{
	LGRM_DOUBLE_PASS_SHADED,
	LGRM_SINGLE_PASS_COLOR,
	LGRM_DOUBLE_PASS_COLOR,
	LGRM_SINGLE_PASS_NO_LIGHT
};

enum TransformType{
	TT_NONE,
	TT_GRAB,
	TT_ROTATE,
	TT_SCALE
};

////////////////////////////////////////////////////////////////////////
//	global constants
extern const char* LG_SUPPORTED_FILE_FORMATS_OPEN;
extern const char* LG_SUPPORTED_FILE_FORMATS_SAVE;

////////////////////////////////////////////////////////////////////////
//	methods
LGObject* CreateLGObjectFromFile(const char* filename);
LGObject* CreateEmptyLGObject(const char* name);
bool LoadLGObjectFromFile(LGObject* pObjOut, const char* filename, bool performLoadPostprocessing = true);
void PerformLoadPostprocessing(LGObject* obj);
bool SaveLGObjectToFile(LGObject* pObj, const char* filename);
bool ReloadLGObject(LGObject* obj);

////////////////////////////////////////////////////////////////////////
//	LGObject
///	holds a grid, a subset-handler and the render-object.
class LGObject : public ISceneObject, public ug::promesh::Mesh
{
	Q_OBJECT

	public:
		LGObject();
		LGObject(const char* name);

		virtual ~LGObject();

	//	from ISceneObject
		virtual const char* name()				{return m_name.c_str();}
		virtual void set_name(const char* name)	{m_name = name; properties_changed();}
		virtual void set_visibility(bool visible)	{m_bVisible = visible;}
		virtual bool is_visible()					{return m_bVisible;}
		virtual QColor get_color() const	{return m_color;}
		virtual void set_color(const QColor& color)	{m_color = color;}

		virtual int num_subsets()				{return m_subsetHandler.num_subsets();}
		virtual const char* get_subset_name(int index) const	{return m_subsetHandler.subset_info(index).name.c_str();}
		virtual bool subset_is_visible(int index);
		virtual QColor get_subset_color(int index) const;

		virtual void set_subset_name(int index, const char* name)	{m_subsetHandler.subset_info(index).name = name; properties_changed();}
		virtual void set_subset_visibility(int index, bool visible);
		virtual void set_subset_color(int index, const QColor& color);

		virtual void geometry_changed();
	///	creates an undo entry, if no transform is currently performed.
		virtual void visuals_changed(bool createUndoPoint = true);
		void marks_changed();
		void selection_changed();

	///	an indicator point is a point with a color independent of the grid.
	/**	You can use indicator points to highlight errors or problematic regions. */
		void add_indicator_point(float x, float y, float z,
								 float r, float g, float b, float a);

	///	clears all indicator points
		void clear_indicator_points();

	///	returns the values of the i-th indicator point.
	/**	You should only use the returned values, if the method returns true.*/
		virtual bool get_indicator_point(size_t index, float& x, float& y, float& z,
										 float& r, float& g, float& b, float& a);

	///	the number of indicator points
		virtual size_t num_indicator_points();

	//	subset handling
		bool subset_is_initialized(int index) const;
		void init_subset(int index);
		void init_subsets();

		uint get_subset_state(int index) const;
		void set_subset_state(int index, uint state);
		void enable_subset_state(int index, uint state);
		void disable_subset_state(int index, uint state);
		bool subset_state_is_enabled(int index, uint state) const;

	//	rendering
		void set_num_display_lists(int num);
		inline int num_display_lists()				{return (int)m_displayLists.size();}
		inline GLuint get_display_list(int index)	{return m_displayLists[index];}
		inline void set_display_list_mode(int index, LGRenderMode mode)	{m_displayModes[index] = mode;}
		inline int get_display_list_mode(int index)						{return m_displayModes[index];}

	///	set the type of elements that shall be rendered.
		inline void set_element_mode(uint mode)		{m_elementMode = mode;}
	///	get the type of the elements that shall be rendered.
		inline uint get_element_mode()				{return m_elementMode;}
		bool volume_rendering_enabled()				{return (m_grid.num_volumes() > 0) && ((m_elementMode & LGEM_VOLUME) == LGEM_VOLUME);}
		bool face_rendering_enabled()				{return (m_grid.num_faces() > 0) && ((m_elementMode & LGEM_FACE) == LGEM_FACE);}
		bool edge_rendering_enabled()				{return (m_grid.num_edges() > 0) && ((m_elementMode & LGEM_EDGE) == LGEM_EDGE);}
		bool vertex_rendering_enabled()				{return (m_grid.num_vertices() > 0) && ((m_elementMode & LGEM_VERTEX) == LGEM_VERTEX);}

	//	geometry info
		void update_bounding_shapes();
		inline ug::Sphere3& get_bounding_sphere()	{return m_boundSphere;}
		inline void get_bounding_box(ug::vector3& vMinOut, ug::vector3& vMaxOut)
			{vMinOut = m_boundBoxMin; vMaxOut = m_boundBoxMax;}

	//	undo / redo
		bool undo();
		bool redo();

	///	call this method to create an undo point if the selection changed since the last point
		void create_undo_point_if_selection_changed();

	///	creates an undo-point
	/** \note This method is automatically invoked from 'visuals_changed(true)'*/
		void create_undo_point();

	////////////////////////////////////////////////////////////////////////////
	//	TRANSFORMS
	///	Begins a new transform as indicated in the specified transform-type.
	/**	If another transform is currently active, it will be canceled.*/
		void begin_transform(TransformType tt);
	/// returns the center of the current transform
	/**	If not called between begin_transform and end_transform, the return
	 * value is undefined.*/
		ug::vector3 transform_center();

	//	grabbing
	///	specifies the offset from begin_grab, to where the objects shall be moved.
	/**	Only has effect if called between begin_grab and end_grab.*/
		void grab(const ug::vector3& offset);

	//	scaling
	///	specifies the scale-facs from begin_scale. Defines the amount to scale along each exis.
	/**	Only has effect if called between begin_grab and end_grab.*/
		void scale(const ug::vector3& scaleFacs);

	///	ends the transform and either applies or reverts the changes made.
		void end_transform(bool bApply);

	///	stores current vertex coordinates in a coordinate buffer
	/**	You may use 'restore_vertex_coordinates_from_buffer' to restore
	 * vertex coordinates from buffered coordinates.*/
		void buffer_current_vertex_coordinates();

	///	assigns buffered coordinates to the mesh-vertices
	/** Vertex coordinates can be buffered by a call to
	 * 'buffer_current_vertex_coordinates'.
	 *
	 * \note that buffered coordinates and mesh-vertices are associated by
	 * their index in the respective sequence. If the topology of a mesh
	 * was changed since the last call to 'buffer_current_vertex_coordinates',
	 * restoring vertex coordinates with this method may lead to unexpected results.*/
		void restore_vertex_coordinates_from_buffer();
		
	///	returns true if something was changed since the last save
		bool save_required() const					{return m_saveRequired;}

	///	call this method to change the saveRequired flag.
	/**	Note: This method will automatically be called whenever the geometry
	 *		  or the selection changed*/
		void set_save_required(bool saveRequired)	{m_saveRequired = saveRequired;}

	///	The action log lists all operations which were performed on the object.
		const QString& action_log() const	{return m_actionLog;}

	///	Adds text to the action log and emits 'actionLogChanged(str)'.
		void log_action(const QString& str);

	///	Writes a command to the action log which selects all currently selected elements
		void write_selection_to_action_log();

	///	clears the action log and emits 'actionLogCleared()'.
		void clear_action_log();

	signals:
		void actionLogChanged(const QString& newContent);
		void actionLogCleared();

	protected:
		void init();

	///	collects the vertices which will be affected and calculates the center.
	/**	Uses Grid::mark()*/
		void init_transform();

	///	loads a file from ugx without emitting signals
		bool load_ugx(const char* filename);

	protected:
		typedef std::vector<GLuint>	DisplayListVec;
		typedef std::vector<int>	DisplayModeVec;

	public:
	//protected:
		std::string			m_fileName;
		std::string			m_name;

		ug::SubsetHandler	m_shFacesForVolRendering;

		ug::vector3			m_boundBoxMin;
		ug::vector3			m_boundBoxMax;
		ug::Sphere3			m_boundSphere;

		DisplayListVec		m_displayLists;
		DisplayModeVec		m_displayModes;

	//	the type of the elements that shall be rendered.
		uint				m_elementMode;

		bool				m_bVisible;
		QColor				m_color;

		UndoHistory			m_undoHistory;

		int					m_numInitializedSubsets;
		bool				m_selectionChangedSinceLastUndoPoint;

		bool				m_saveRequired;
		
	//	currently used by LGScene to update the selection visuals only.
		int					m_selectionDisplayListIndex;
		
		QString				m_actionLog;

	private:
	//	transform
		TransformType		m_transformType;
		ug::vector3			m_transformStart;	// center where transform started
		ug::vector3			m_transformCur;		// center of the current selection
		ug::vector3			m_transformCurScales;
		std::vector<ug::Vertex*>	m_transformVertices;
		std::vector<ug::vector3>	m_transformInitialPositions;
		std::vector<ug::vector3>	m_vertexCoordinateBuffer;///< used in calls to 'buffer_current_vertex_coordinates' and 'restore_vertex_coordinates_from_buffer'

	protected:
		struct IndicatorPoint{
			IndicatorPoint()	{}
			IndicatorPoint(float nx, float ny, float nz, float nr,
						   float ng, float nb, float na) :
				x(nx), y(ny), z(nz), r(nr), g(ng), b(nb), a(na)	{}

			float x, y, z;
			float r, g, b, a;
		};

		std::vector<IndicatorPoint>	m_indicatorPoints;
};

#endif // __H__LG_OBJECT__
