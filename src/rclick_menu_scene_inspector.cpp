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

#include <QtWidgets>
#include "rclick_menu_scene_inspector.h"
#include "app.h"
#include "tools/tool_dialog.h"
#include "tools/subset_tools.h"

using namespace std;
using namespace ug;

RClickMenu_SceneInspector::
RClickMenu_SceneInspector(SceneInspector * sceneInspector) :
			QWidget(sceneInspector), m_sceneInspector(sceneInspector)
{
	m_menu = new QMenu(this);

//	populate the menu
	m_actAssignSubset = new QAction(tr("Assign To Subset"), this);
	connect(m_actAssignSubset, SIGNAL(triggered()), this, SLOT(assignSubset()));
	m_menu->addAction(m_actAssignSubset);

	m_actAssignNewSubset = new QAction(tr("Assign To New Subset"), this);
	connect(m_actAssignNewSubset, SIGNAL(triggered()), this, SLOT(assignNewSubset()));
	m_menu->addAction(m_actAssignNewSubset);

	m_actRename = new QAction(tr("Rename"), this);
	connect(m_actRename, SIGNAL(triggered()), this, SLOT(rename()));
	m_menu->addAction(m_actRename);
	
	m_actShowAllSubsets = new QAction(tr("Show All Subsets"), this);
	connect(m_actShowAllSubsets, SIGNAL(triggered()), this, SLOT(showAllSubsets()));
	m_menu->addAction(m_actShowAllSubsets);

	m_actHideAllSubsets = new QAction(tr("Hide All Subsets"), this);
	connect(m_actHideAllSubsets, SIGNAL(triggered()), this, SLOT(hideAllSubsets()));
	m_menu->addAction(m_actHideAllSubsets);

	m_actToggleAllSubsetVisibilities = new QAction(tr("Toggle All Subset Visibilities"), this);
	connect(m_actToggleAllSubsetVisibilities, SIGNAL(triggered()), this, SLOT(toggleAllSubsetVisibilities()));
	m_menu->addAction(m_actToggleAllSubsetVisibilities);

	m_actPrintSubsetContents = new QAction(tr("Print Subset Contents"), this);
	connect(m_actPrintSubsetContents, SIGNAL(triggered()), this, SLOT(printSubsetContents()));
	m_menu->addAction(m_actPrintSubsetContents);

	m_actReload = new QAction(tr("Reload"), this);
	connect(m_actReload, SIGNAL(triggered()), this, SLOT(reload()));
	m_menu->addAction(m_actReload);

//	m_actHideOtherSubsets = new QAction(tr("Hide Other Subsets"), this);
}

void RClickMenu_SceneInspector::
exec(const QPoint& p){
	m_menu->exec(p);
}

void RClickMenu_SceneInspector::assignSubset()
{
	LGObject* obj = app::getActiveObject();
	if(obj){
		int si = m_sceneInspector->getActiveSubsetIndex();
		if(si != -1){
			obj->write_selection_to_action_log();
			obj->log_action (QString("AssignSubset (mesh, %1, true, true, true, true)\n").
								arg(si));
			promesh::AssignSubset(obj, si, true, true, true, true);
			obj->geometry_changed();
		}
	}
}

void RClickMenu_SceneInspector::assignNewSubset()
{
	LGObject* obj = app::getActiveObject();
	if(obj){
		int si = obj->subset_handler().num_subsets();
		obj->write_selection_to_action_log();
		obj->log_action (QString("AssignSubset (mesh, %1, true, true, true, true)\n").
								arg(si));
		promesh::AssignSubset(obj, si, true, true, true, true);

		obj->geometry_changed();

		int activeObjectIndex = m_sceneInspector->getActiveObjectIndex();
		m_sceneInspector->setActiveSubset(activeObjectIndex, si);
		rename();
	}
}

void RClickMenu_SceneInspector::rename()
{
//	create a tool dialog with a single text-input box.
	this->close();
	LGObject* obj = app::getActiveObject();
	if(obj){
		/*ToolWidget* widget = new ToolWidget("rename", this,
										  NULL, IDB_APPLY);

		QDialog* dlg = new QDialog(NULL);
		QVBoxLayout* layout = new QVBoxLayout(dlg);
		dlg->setLayout(layout);
		layout->addWidget(widget);

		string curName = obj->name();
		int si = m_sceneInspector->getActiveSubsetIndex();
		if(si != -1)
			curName = obj->get_subset_name(si);
		
		widget->addTextBox("name:", curName.c_str());
		*/
		string curName = obj->name();
		int si = m_sceneInspector->getActiveSubsetIndex();
		if(si != -1)
			curName = obj->get_subset_name(si);

		QDialog* dlg = new QDialog(this);
		dlg->setWindowTitle(tr("rename"));
		QVBoxLayout* layout = new QVBoxLayout(dlg);
		dlg->setLayout(layout);

		QLineEdit* text = new QLineEdit(dlg);
		layout->addWidget(text);
		text->setText(QString::fromUtf8(curName.c_str()));
		text->selectAll();

		QHBoxLayout* hlayout = new QHBoxLayout();
		layout->addLayout(hlayout);

		hlayout->addStretch();

		QPushButton* btnCancel = new QPushButton(dlg);
		btnCancel->setText(tr("Cancel"));
		hlayout->addWidget(btnCancel);
		connect(btnCancel, SIGNAL(clicked()), dlg, SLOT(reject()));

		QPushButton* btnOk = new QPushButton(dlg);
		btnOk->setText(tr("Ok"));
		hlayout->addWidget(btnOk);
		btnOk->setDefault(true);
		connect(btnOk, SIGNAL(clicked()), dlg, SLOT(accept()));

		if(dlg->exec()){
			curName = text->text().toLocal8Bit().constData();
			if(si != -1){
				obj->set_subset_name(si, curName.c_str());
				obj->log_action (QString("SetSubsetName (mesh, %1, \"%2\")\n").
									arg(si).arg(curName.c_str()));
			}
			else
				obj->set_name(curName.c_str());
			obj->set_save_required(true);
			m_sceneInspector->refreshView();
		}
		delete dlg;
	}
}

void RClickMenu_SceneInspector::
showAllSubsets(){
	ISceneObject* obj = m_sceneInspector->getActiveObject();
	if(obj){
		for(int i = 0; i < obj->num_subsets(); ++i){
			obj->set_subset_visibility(i, true);
		}
		obj->visuals_changed();
		m_sceneInspector->refreshView();
	}
}

void RClickMenu_SceneInspector::
printSubsetContents()
{
	LGObject* obj = dynamic_cast<LGObject*>(m_sceneInspector->getActiveObject());
	int si = m_sceneInspector->getActiveSubsetIndex();
	if(obj && (si != -1)){
		PrintElementNumbers(obj->subset_handler().get_grid_objects_in_subset(si));
	}
}

void RClickMenu_SceneInspector::
hideAllSubsets(){
	ISceneObject* obj = m_sceneInspector->getActiveObject();
	if(obj){
		for(int i = 0; i < obj->num_subsets(); ++i){
			obj->set_subset_visibility(i, false);
		}
		obj->visuals_changed();
		m_sceneInspector->refreshView();
	}
}

void RClickMenu_SceneInspector::
toggleAllSubsetVisibilities(){
	ISceneObject* obj = m_sceneInspector->getActiveObject();
	if(obj){
		for(int i = 0; i < obj->num_subsets(); ++i){
			obj->set_subset_visibility(i, !obj->subset_is_visible(i));
		}
		obj->visuals_changed();
		m_sceneInspector->refreshView();
	}
}

void RClickMenu_SceneInspector::
reload(){
	this->close();
	LGObject* obj = app::getActiveObject();
	if(obj){
		ReloadLGObject(obj);
	}
}