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
#include "common/util/archivar.h"
#include "common/util/factory.h"

// #include "lib_grid/boost_class_serialization_exports.h"
#include "lib_grid/refinement/projectors/projectors.h"

static ug::Factory<ug::RefinementProjector, ug::ProjectorTypes>	projFactory;

ProjectorWidget::
ProjectorWidget (QWidget* parent, LGScene* scene) :
	QFrame(parent),
	m_curContent(NULL),
	m_scene(scene),
	m_activeObject(NULL),
	m_activeSubsetIndex(-1)
{
	m_vlayout = new QVBoxLayout(this);

	QHBoxLayout* hbox = new QHBoxLayout();
	m_vlayout->addLayout(hbox);

	hbox->addWidget(new QLabel(tr("type:")));
	m_typeBox = new QComboBox(this);
	hbox->addWidget(m_typeBox);
	hbox->addStretch(2);

	m_typeBox->addItem(QString("none"));
	for(size_t iproj = 0; iproj < projFactory.num_classes(); ++iproj){
		m_typeBox->addItem(QString(projFactory.class_name(iproj).c_str()));
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
	if(m_activeObject && m_activeSubsetIndex >= 0){
		setEnabled(true);
		ug::SPRefinementProjector proj =
				m_activeObject->projection_handler().projector(subsetIndex);

		if(proj.valid()){
			QString itemText = QString(projFactory.class_name(*proj).c_str());
			int newIndex = m_typeBox->findText(itemText);
			if(newIndex != m_typeBox->currentIndex())
				m_typeBox->setCurrentIndex(newIndex);
			else
				update_content(proj.get());
		}
		else if(m_typeBox->currentIndex() != 0){
			m_typeBox->setCurrentIndex(0);
		}
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
	if(!m_activeObject || m_activeSubsetIndex < 0)
		return;

	std::string projName = text.toStdString();
	if(projName.compare("none") == 0){
		m_activeObject->projection_handler()
			.set_projector(m_activeSubsetIndex, ug::SPRefinementProjector());
		update_content(NULL);
	}
	else{
	//	if the current projector for the active subset has a different name, create a new one
		ug::SPRefinementProjector curProj =
				m_activeObject->projection_handler().projector(m_activeSubsetIndex);
		if(curProj.invalid() || projFactory.class_name(*curProj) != projName){
			ug::SPRefinementProjector proj = projFactory.create(projName);
			proj->set_geometry(m_activeObject->geometry());
			m_activeObject->projection_handler().set_projector(m_activeSubsetIndex, proj);
			// UG_LOG("created projector: " << typeid(*proj).name() << std::endl);
			update_content(proj.get());
		}
		else
			update_content(curProj.get());
	}
}

void ProjectorWidget::
update_content(ug::RefinementProjector* proj)
{
	if(!proj){
		if(m_curContent){
			delete m_curContent;
			m_curContent = NULL;
		}
		return;
	}

	static ug::Archivar <tooldlg_oarchive,
						 ug::RefinementProjector,
						 ug::ProjectorTypes>
				archivar;

	tooldlg_oarchive oa(this);

	archivar.archive(oa, *proj, "properties");

	QWidget* newContent = oa.widget();

	if(m_curContent)
		delete m_curContent;
	m_curContent = newContent;
	m_vlayout->addWidget(m_curContent);

//	connect change-signals of all tool-widgets to contentChanged
	QList<ToolWidget*> toolWidgets = newContent->findChildren<ToolWidget*>();
	for(QList<ToolWidget*>::iterator iter = toolWidgets.begin();
		iter!= toolWidgets.end(); ++iter)
	{
		ToolWidget* tw = *iter;
		connect(tw, SIGNAL(valueChanged(int)), this, SLOT(valueChanged()));
	}
}

void ProjectorWidget::
valueChanged ()
{
	if(m_curContent && m_activeObject){
		static ug::Archivar <tooldlg_iarchive,
							 ug::RefinementProjector,
							 ug::ProjectorTypes>
				archivar;

		ug::SPRefinementProjector proj =
				m_activeObject->projection_handler().projector(m_activeSubsetIndex);

		tooldlg_iarchive ia(m_curContent);
		archivar.archive(ia, *proj);
	}
}