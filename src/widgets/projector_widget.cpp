/*
 * Copyright (c) 2016:  G-CSC, Goethe University Frankfurt
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

#include <QListWidget>
#include <QComboBox>
#include <QEvent>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <typeinfo>
#include "projector_widget.h"
#include "promesh_plugin.h"
#include "../scene/lg_scene.h"

#include "tooldlg_oarchive.h"
#include "tooldlg_iarchive.h"
#include "common/boost_serialization_routines.h"
#include "lib_grid/boost_class_serialization_exports.h"


#include "lib_grid/algorithms/refinement/projectors.h"

#include <boost/static_assert.hpp>
#include <boost/mpl/for_each.hpp>
#include <boost/type_traits/is_base_of.hpp>


ProjectorWidget::
ProjectorWidget (QWidget* parent, LGScene* scene) :
	QFrame(parent),
	m_curContent(NULL),
	m_scene(scene),
	m_activeObject(NULL),
	m_activeSubsetIndex(-1)
{
	m_vlayout = new QVBoxLayout(this);

	auto hbox = new QHBoxLayout();
	m_vlayout->addLayout(hbox);

	hbox->addWidget(new QLabel(tr("type:")));
	m_typeBox = new QComboBox(this);
	hbox->addWidget(m_typeBox);
	hbox->addStretch(2);

	auto reg = ug::promesh::GetProMeshRegistry();

	for(size_t iproj = 0; iproj < reg.num_projectors(); ++iproj){
		m_typeBox->addItem(QString(reg.projector_name(iproj).c_str()));
	}

	setEnabled(false);

	connect(m_scene, SIGNAL(object_to_be_removed(ISceneObject*)),
			this, SLOT(objectToBeRemoved(ISceneObject*)));

	connect(m_typeBox, SIGNAL(currentIndexChanged(const QString&)),
			this, SLOT(projectorTypeChanged(const QString&)));
}

ProjectorWidget::
~ProjectorWidget ()
{
}

void ProjectorWidget::
objectToBeRemoved(ISceneObject* pObj)
{
	if(pObj == static_cast<ISceneObject*>(m_activeObject))
		setActiveSubset(NULL, -1);
}

void ProjectorWidget::
changeEvent(QEvent* evt)
{
	QFrame::changeEvent(evt);
	if(evt->type() == QEvent::EnabledChange){
		if(isEnabled()){
			m_typeBox->setVisible(true);
		}
		else{
			m_typeBox->setVisible(false);
			if(m_curContent)
				delete m_curContent;
			m_curContent = NULL;
		}
	}
}


void ProjectorWidget::
setActiveSubset(ISceneObject* obj, int subsetIndex)
{
	if (m_activeObject == dynamic_cast<LGObject*>(obj) &&
		m_activeSubsetIndex == subsetIndex)
	{
		return;
	}

	m_activeObject = dynamic_cast<LGObject*>(obj);
	m_activeSubsetIndex = subsetIndex;
	if(m_activeObject){
		std::cout << "setActiveSubset executed.\n";
		setEnabled(true);
		ug::SPRefinementProjector proj =
				m_activeObject->projection_handler().projector(subsetIndex);

		auto reg = ug::promesh::GetProMeshRegistry();
		QString itemText = QString(reg.projector_name(*proj).c_str());
		int newIndex = m_typeBox->findText(itemText);
		if(newIndex != m_typeBox->currentIndex())
			m_typeBox->setCurrentIndex(newIndex);
		else
			update_content(proj.get());
	}
	else{
		setEnabled(false);
		if(m_curContent)
			delete m_curContent;
		m_curContent = NULL;
	}
}

void ProjectorWidget::
projectorTypeChanged(const QString &text)
{
	if(!m_activeObject)
		return;

	auto reg = ug::promesh::GetProMeshRegistry();
	std::string projName = text.toStdString();
//	if the current projector for the active subset has a different name, create a new one
	ug::SPRefinementProjector curProj =
			m_activeObject->projection_handler().projector(m_activeSubsetIndex);
	if(reg.projector_name(*curProj) != projName){
		ug::SPRefinementProjector proj = reg.projector_info(projName).factory();
		proj->set_geometry(m_activeObject->geometry());
		m_activeObject->projection_handler().set_projector(m_activeSubsetIndex, proj);
		// UG_LOG("created projector: " << typeid(*proj).name() << std::endl);
		update_content(proj.get());
	}
	else
		update_content(curProj.get());
}

void ProjectorWidget::
update_content(ug::RefinementProjector* proj)
{
	tooldlg_oarchive oa(this);
	oa << ug::make_nvp("projector", proj);
	QWidget* newContent = oa.widget();

	if(m_curContent)
		delete m_curContent;
	m_curContent = newContent;
	m_vlayout->addWidget(m_curContent);

//	connect change-signals of all tool-widgets to contentChanged
	QList<ToolWidget*> toolWidgets = newContent->findChildren<ToolWidget*>();
	for(auto tw : toolWidgets) {
		connect(tw, SIGNAL(valueChanged(int)), this, SLOT(valueChanged()));
	}
}


namespace detail{
	template <typename TArchive, typename TBaseClass, typename TDerivedClass>
	void CallArchiveOnDerivedClass (TArchive& ar, TBaseClass& base)
	{
		BOOST_STATIC_ASSERT((boost::is_base_of<TBaseClass, TDerivedClass>::value));
		TDerivedClass& derived = dynamic_cast<TDerivedClass&>(base);
		ar & derived;
	}

	template <typename TRegistry>
	struct RegisterTypesFunctor
	{
		RegisterTypesFunctor(TRegistry* reg) : m_reg(reg) {}

	///	inserts classes into the set of known classes
	/**	This function allows for the usage of boost::mpl::for_each*/
		template <typename T> void operator()(T)
		{
			m_reg->template add<T>();
		}

		private:
		TRegistry* m_reg;
	};
}

template <class TArchive, class TBase, class TSeq>
struct CallArchiveOnMostDerivedClass
{
	CallArchiveOnMostDerivedClass()
	{
		std::cout << "calling for_each on " << typeid(TSeq).name() << std::endl;
		detail::RegisterTypesFunctor<CallArchiveOnMostDerivedClass> func(this);
		boost::mpl::for_each<TSeq>(func);
	}

	template <class T>
	void call(TArchive& ar, T& t)
	{
		std::string name(typeid(t).name());
		std::cout << "calling archive on class '" << name  << "'\n";
		typename callback_map_t::iterator i = m_callbackMap.find(name);
		if(i != m_callbackMap.end())
			i->second(ar, t);
		else
			std::cout << "WARNING: invalid derived class used in serializeation.\n" << std::endl;
	}

	template <class TDerived>
	void add()
	{
		std::string name(typeid(TDerived).name());
		std::cout << "registering class '" << name << "'\n";
		m_callbackMap[name] =
				&detail::CallArchiveOnDerivedClass<TArchive, TBase, TDerived>;
	}

	private:
	typedef void (*callback_t)(TArchive&, TBase&);
	typedef std::map<std::string, callback_t>	callback_map_t;
	callback_map_t	m_callbackMap;
};

void ProjectorWidget::
valueChanged ()
{
	UG_LOG("DEBUG: Value changed!\n");
	if(m_curContent && m_activeObject){
		static CallArchiveOnMostDerivedClass <tooldlg_iarchive,
											  ug::RefinementProjector,
											  ug::ProjectorTypes>
				serializer;

		ug::SPRefinementProjector proj =
				m_activeObject->projection_handler().projector(m_activeSubsetIndex);

		tooldlg_iarchive ia(m_curContent);
		serializer.call(ia, *proj);
	}
}