// created by Sebastian Reiter
// s.b.reiter@googlemail.com
// 19.06.2012 (m,d,y)

#ifndef __H__UG__lg_scene_impl__
#define __H__UG__lg_scene_impl__

#include "lg_scene.h"

template <class TIterator>
void LGScene::
hide_elements(LGObject* obj, TIterator elemsBegin, TIterator elemsEnd)
{
	using namespace ug;
	typedef typename PtrToValueType<typename TIterator::value_type>::base_type TElem;
	Grid::AttachmentAccessor<TElem, ABool> aaHidden(obj->grid(), m_aHidden);

	for(TIterator iter = elemsBegin; iter != elemsEnd; ++iter){
		aaHidden[*iter] = true;
	}
}

template <class TElem>
void LGScene::
unhide_elements(LGObject* obj)
{
	using namespace ug;
	Grid::AttachmentAccessor<TElem, ABool> aaHidden(obj->grid(), m_aHidden);
	SetAttachmentValues(aaHidden, obj->grid().begin<TElem>(),
						obj->grid().end<TElem>(), false);
}

#endif
