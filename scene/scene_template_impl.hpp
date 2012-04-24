//	created by Sebastian Reiter
//	s.b.reiter@googlemail.com
//	y09 m10 d01

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
			emit object_to_be_erased(obj);
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
		emit object_to_be_removed(obj);
		m_vObjects.erase(m_vObjects.begin() + index);
		m_vInfos.erase(m_vInfos.begin() + index);
		emit visuals_updated();
		return true;
	}

	return false;
}

template <class TObject>
int TScene<TObject>::
num_objects() const
{
	return m_vObjects.size();
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
			return i;
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
