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

#ifndef __H__SCENE_TEMPLATE__
#define __H__SCENE_TEMPLATE__

#include <vector>
#include "scene_interface.h"

////////////////////////////////////////////////////////////////////////
///	Provides a basic implementation of the scene interface.
/**
 * Implements object handling and slots.
 * Template parameter TObject has to derive from ISceneObject.
 */

template <class TObject>
class TScene : public IScene
{
	public:
		TScene();
		virtual ~TScene();

	//	new methods
		virtual int add_object(TObject* obj, bool autoDelete = true);
		virtual TObject* get_object(int index);
		//virtual TObject* get_object_by_name(const char* name);

	//	derived from IScene
		virtual bool erase_object(int index);
		virtual bool remove_object(int index);
		virtual int num_objects() const;
		virtual ISceneObject* get_scene_object(int index);
		//virtual ISceneObject* get_scene_object_by_name(const char* name);
		virtual int get_object_index(ISceneObject* pObj);

		virtual void update_visuals(int objIndex);
		virtual void update_visuals(ISceneObject* pObj);
		virtual void object_changed(int objIndex);
		virtual void object_changed(ISceneObject* pObj);

	protected:
		inline bool index_is_valid(int index) {return index >= 0 && index < (int)m_vObjects.size();}

	protected:
		struct ObjectInfo
		{
			ObjectInfo()	{}
			ObjectInfo(bool autoDelete) : m_autoDelete(autoDelete)	{}
			bool	m_autoDelete;
		};

		typedef std::vector<TObject*>	ObjectVec;
		typedef std::vector<ObjectInfo>	InfoVec;

	protected:
		ObjectVec	m_vObjects;
		InfoVec		m_vInfos;
};


////////////////////////////////////////////////
//	include implementation
#include "scene_template_impl.hpp"

#endif
