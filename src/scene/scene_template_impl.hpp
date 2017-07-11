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

#ifndef __H__SCENE_TEMPLATE_IMPL__
#define __H__SCENE_TEMPLATE_IMPL__

#include <vector>
#include "scene_template.h"

template <class TObject>
TScene<TObject>::
TScene() : IScene()
{
}

template <class TObject>
TScene<TObject>::
~TScene()
{
	for(int i = 0; i < num_objects(); ++i)
	{
		if(m_vInfos[i].m_autoDelete)
			delete m_vObjects[i];
	}
}

////////////////////////////////////////////////////////////////////////
//	new methods
template <class TObject>
int TScene<TObject>::
add_object(TObject* obj, bool autoDelete)
{
//	make sure, that the object is not already contained.
	int oldInd = get_object_index(obj);
	assert(oldInd == -1 && "object is already contained in the scene!");

	if(oldInd == -1)
	{
		int newInd = num_objects();
		m_vObjects.push_back(obj);
		m_vInfos.push_back(ObjectInfo(autoDelete));
		emit IScene::object_added(obj);
		return newInd;
	}
	return -1;
}

template <class TObject>
TObject* TScene<TObject>::
get_object(int index)
{
	if(index_is_valid(index))
		return m_vObjects[index];
	return NULL;
}

////////////////////////////////////////////////////////////////////////
//	derived from IScene
template <class TObject>
bool TScene<TObject>::
erase_object(int index)
{
	if(index_is_valid(index))
	{
		TObject* obj = get_object(index);
		bool retVal = remove_object(index);

		if(m_vInfos[index].m_autoDelete)
		{
			emit IScene::object_to_be_erased(obj);
			delete obj;
		}

		return retVal;
	}

	return false;
}

template <class TObject>
bool TScene<TObject>::
remove_object(int index)
{
	if(index_is_valid(index))
	{
		ISceneObject* obj = get_object(index);
		emit IScene::object_to_be_removed(obj);
		m_vObjects.erase(m_vObjects.begin() + index);
		m_vInfos.erase(m_vInfos.begin() + index);
		emit IScene::visuals_updated();
		emit IScene::object_removed();
		return true;
	}

	return false;
}

template <class TObject>
int TScene<TObject>::
num_objects() const
{
	return (int)m_vObjects.size();
}

template <class TObject>
ISceneObject* TScene<TObject>::
get_scene_object(int index)
{
	if(index_is_valid(index))
		return m_vObjects[index];
	return NULL;
}

template <class TObject>
int TScene<TObject>::
get_object_index(ISceneObject* pObj)
{
	for(size_t i = 0; i < m_vObjects.size(); ++i)
	{
		if(m_vObjects[i] == pObj)
			return (int)i;
	}
	return -1;
}

template <class TObject>
void TScene<TObject>::
update_visuals(int objIndex)
{
//TODO: emit signal
}

template <class TObject>
void TScene<TObject>::
update_visuals(ISceneObject* pObj)
{
//TODO: emit signal
}

template <class TObject>
void TScene<TObject>::
object_changed(int objIndex)
{
//TODO: emit signal
}

template <class TObject>
void TScene<TObject>::
object_changed(ISceneObject* pObj)
{
//TODO: emit signal
}

#endif
