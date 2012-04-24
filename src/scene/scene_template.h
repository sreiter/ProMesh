//	created by Sebastian Reiter
//	s.b.reiter@googlemail.com
//	y09 m10 d01

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
