/*
 * Copyright (c) 2017:  G-CSC, Goethe University Frankfurt
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

#include "module_interface.h"

IModule::
IModule () :
	QObject (),
	m_parent (NULL)
{}

IModule::
IModule (QWidget* parent) :
	QObject (parent),
	m_parent (parent)
{}

IModule::
~IModule ()
{}


void IModule::
activate (SceneInspector* sceneInspector, LGScene* scene)
{
}

void IModule::
deactivate ()
{
}


IModule::dock_list_t
IModule::
getDockWidgets()
{
	return std::vector<std::pair<Qt::DockWidgetArea, QDockWidget*> > ();
}

std::vector<QToolBar*>
IModule::
getToolBars()
{
	return std::vector<QToolBar*> ();
}

std::vector<QMenu*>
IModule::
getMenus()
{
	return std::vector<QMenu*>();
}

QMenu*
IModule::
getSceneInspectorMenu()
{
	return NULL;
}
