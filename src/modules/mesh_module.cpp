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

#include <QApplication>
#include <QDesktopServices>
#include <QDockWidget>
#include <QFileDialog>
#include <QKeyEvent>

#include "app.h"

#include "clip_plane_widget.h"
#include "mesh_module.h"
#include "rclick_menu_scene_inspector.h"
#include "scene_inspector.h"
#include "scene/lg_scene.h"
#include "tools/standard_tools.h"
#include "tools/tool_manager.h"
#include "widgets/projector_widget.h"
#include "widgets/property_widget.h"
#include "widgets/tool_browser_widget.h"
#include "widgets/widget_list.h"

using namespace std;
using namespace ug;

MeshModule::
MeshModule ()
{}

MeshModule::
MeshModule (QWidget* parent) :
	IModule(parent),
	m_sceneInspector(NULL),
	m_sceneInspectorMenu(NULL)
{}

MeshModule::
~MeshModule ()
{}

void MeshModule::
activate(SceneInspector* sceneInspector, LGScene* scene)
{
	if(m_sceneInspector == sceneInspector && m_scene == scene)
		return;
	
	if(m_sceneInspector || m_scene){
		deactivate();
		if(m_sceneInspectorMenu)
			delete m_sceneInspectorMenu;
	}

	m_sceneInspector = sceneInspector;

	if(m_dockWidgets.empty()){
	//	create the projector dock
		QDockWidget* pProjectorDock = new QDockWidget(tr("Projectors"), parentWidget());
		pProjectorDock->setFeatures(QDockWidget::NoDockWidgetFeatures);
		pProjectorDock->setObjectName(tr("projector_dock"));
		WidgetList* projectorList = new WidgetList(pProjectorDock);
		m_projectorWidget = new ProjectorWidget(projectorList);
		projectorList->addWidget(m_projectorWidget);
		pProjectorDock->setWidget(projectorList);
		m_dockWidgets.push_back(make_pair(Qt::RightDockWidgetArea, pProjectorDock));

	//	create the clip-plane widget
		QDockWidget* pClipPlaneDock= new QDockWidget(tr("Clip Planes"), parentWidget());
		pClipPlaneDock->setFeatures(QDockWidget::NoDockWidgetFeatures);
		pClipPlaneDock->setObjectName(tr("clip_plane_widget_dock"));
		m_clipPlaneWidget = new ClipPlaneWidget(pClipPlaneDock);
		pClipPlaneDock->setWidget(m_clipPlaneWidget);
		m_dockWidgets.push_back(make_pair(Qt::RightDockWidgetArea, pClipPlaneDock));

		m_toolManager = new ToolManager(parentWidget());
		try{
			RegisterStandardTools(m_toolManager);
		}
		catch(UGError& err){
			UG_LOG("ERROR: ")
			for(size_t i = 0; i < err.num_msg(); ++i){
				if(i > 0){
					UG_LOG("       ");
				}
				UG_LOG(err.get_msg(i) << endl);
			}
			UG_LOG("------------------------------------------------------------------------------------------\n")
		}


	//	create the tool browser
		QDockWidget* toolBrowserDock = new QDockWidget(tr("Tool Browser"), parentWidget());
		toolBrowserDock->setFeatures(QDockWidget::NoDockWidgetFeatures);
		toolBrowserDock->setObjectName(tr("tool_browser_dock"));

		m_toolBrowser = new ToolBrowser(parentWidget());
		m_toolBrowser->refresh(m_toolManager);
		m_toolBrowser->setObjectName(tr("tool_browser"));
		toolBrowserDock->setWidget(m_toolBrowser);
		m_dockWidgets.push_back(make_pair(Qt::LeftDockWidgetArea, toolBrowserDock));

	//	script menu
		QAction* actNewScript = new QAction(tr("New Script"), parentWidget());
		actNewScript->setStatusTip("Creates a new script and opens it for editing");
		connect(actNewScript, SIGNAL(triggered()), this, SLOT(newScript()));

		QAction* actEditScript = new QAction(tr("Edit Script"), parentWidget());
		actEditScript->setStatusTip("Opens a script for editing");
		connect(actEditScript, SIGNAL(triggered()), this, SLOT(editScript()));

		QAction* actBrowseUserScripts = new QAction(tr("Browse User Scripts"), parentWidget());
		actBrowseUserScripts->setStatusTip("Opens the path at which user scripts are located.");
		connect(actBrowseUserScripts, SIGNAL(triggered()), this, SLOT(browseUserScripts()));
		
		QAction* actRefreshToolDialogs = new QAction(tr("Refresh Tool Dialogs"), parentWidget());
		actRefreshToolDialogs->setShortcut(tr("Ctrl+T"));
		actRefreshToolDialogs->setStatusTip("Refreshes contents of tht tool-dialogs.");
		connect(actRefreshToolDialogs, SIGNAL(triggered()), this, SLOT(refreshToolDialogsClicked()));

		QMenu* sceneMenu = new QMenu("&Scripts", parentWidget());
		sceneMenu->addAction(actNewScript);
		sceneMenu->addAction(actEditScript);
		sceneMenu->addSeparator();
		sceneMenu->addAction(actBrowseUserScripts);
		sceneMenu->addSeparator();
		sceneMenu->addAction(actRefreshToolDialogs);

		m_menus.push_back(sceneMenu);
	}

	if(m_sceneInspector){
		if(!m_sceneInspectorMenu){
			m_sceneInspectorMenu = new RClickMenu_SceneInspector(m_sceneInspector);
			m_sceneInspectorMenu->setVisible(false);
		}

		connect(m_sceneInspector, SIGNAL(subsetChanged(ISceneObject*, int)),
				m_projectorWidget, SLOT(setActiveSubset(ISceneObject*, int)));
	}

	if(m_scene != scene){
		connect(scene, SIGNAL(object_to_be_removed(ISceneObject*)),
				m_projectorWidget, SLOT(objectToBeRemoved(ISceneObject*)));
		m_clipPlaneWidget->setScene(scene);
		m_scene = scene;
	}
}

void MeshModule::
deactivate()
{
	if(m_sceneInspector){
		m_sceneInspector->disconnect(m_projectorWidget);
		m_sceneInspector = NULL;
	}

	if(m_scene){
		m_scene->disconnect(m_projectorWidget);
		m_scene = NULL;
	}
}


MeshModule::dock_list_t
MeshModule::
getDockWidgets()
{
	return m_dockWidgets;
}

std::vector<QToolBar*>
MeshModule::
getToolBars()
{
	return std::vector<QToolBar*>();
}

QMenu*
MeshModule::
getSceneInspectorMenu()
{
	return m_sceneInspectorMenu->getMenu();
}


std::vector<QMenu*>
MeshModule::
getMenus()
{
	return m_menus;
}


void MeshModule::
keyPressEvent(QKeyEvent* event)
{
	Qt::KeyboardModifiers qtMods = QApplication::keyboardModifiers();
	uint mods = 0;

	if(qtMods.testFlag(Qt::ShiftModifier))
		mods |= SMK_SHIFT;
	if(qtMods.testFlag(Qt::ControlModifier))
		mods |= SMK_CTRL;
	if(qtMods.testFlag(Qt::AltModifier))
		mods |= SMK_ALT;

	m_toolManager->execute_shortcut(event->key(), mods);
}


void MeshModule::refreshToolDialogsClicked()
{
	RefreshScriptTools(m_toolManager);
	m_toolBrowser->refresh(m_toolManager);
}

void MeshModule::browseUserScripts()
{
	QDir scriptDir = app::UserScriptDir();
	QString path = QDir::toNativeSeparators(app::UserScriptDir().path());
	QDesktopServices::openUrl(QUrl("file:///" + path));
}

void MeshModule::newScript()
{
	QString fileName = QFileDialog::getSaveFileName(
									parentWidget(),
									tr("Script Name"),
									QDir::toNativeSeparators(app::UserScriptDir().path()),
									tr("script files (*.lua)"));
	if(!fileName.endsWith(".lua"))
		fileName.append(".lua");

	if(!fileName.isEmpty()){
		if(QFile::exists(fileName))
			QFile::remove(fileName);
		QFile::copy(":/resources/default-script.lua", fileName);
		QFile::setPermissions(fileName, QFileDevice::ReadOwner | QFileDevice::WriteOwner |
										QFileDevice::ReadUser | QFileDevice::WriteUser |
										QFileDevice::ReadGroup);

		QDesktopServices::openUrl(QUrl("file:///" + fileName));
	}
}

void MeshModule::editScript()
{
	QString fileName = QFileDialog::getOpenFileName(
									parentWidget(),
									tr("Script Name"),
									QDir::toNativeSeparators(app::UserScriptDir().path()),
									tr("script files (*.lua)"));

	if(!fileName.isEmpty()){
		QDesktopServices::openUrl(QUrl("file:///" + fileName));
	}
}
