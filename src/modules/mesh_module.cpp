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
#include "widgets/matrix_widget.h"
#include "tools/coordinate_transform_tools.h"
#include "widgets/live_script_dialog.h"

using namespace std;
using namespace ug;

MeshModule::
MeshModule ()
{}

MeshModule::
MeshModule (QWidget* parent) :
	IModule(parent),
	m_sceneInspector(NULL),
	m_scene(NULL),
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
	//	projector dock
		QDockWidget* pProjectorDock = new QDockWidget(tr("Projectors"), parentWidget());
		pProjectorDock->setFeatures(QDockWidget::NoDockWidgetFeatures);
		pProjectorDock->setObjectName(tr("projector_dock"));
		WidgetList* projectorList = new WidgetList(pProjectorDock);
		m_projectorWidget = new ProjectorWidget(projectorList);
		projectorList->addWidget(m_projectorWidget);
		pProjectorDock->setWidget(projectorList);
		m_dockWidgets.push_back(make_pair(Qt::RightDockWidgetArea, pProjectorDock));


	//	clip-plane dock
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


	//	tool browser dock
		QDockWidget* toolBrowserDock = new QDockWidget(tr("Tool Browser"), parentWidget());
		toolBrowserDock->setFeatures(QDockWidget::NoDockWidgetFeatures);
		toolBrowserDock->setObjectName(tr("tool_browser_dock"));

		m_toolBrowser = new ToolBrowser(parentWidget());
		m_toolBrowser->refresh(m_toolManager);
		m_toolBrowser->setObjectName(tr("tool_browser"));
		toolBrowserDock->setWidget(m_toolBrowser);
		m_dockWidgets.push_back(make_pair(Qt::LeftDockWidgetArea, toolBrowserDock));


	//	coordinate dock
		QDockWidget* coordinatesDock = new QDockWidget(tr("Coordinates"), parentWidget());
		coordinatesDock->setFeatures(QDockWidget::NoDockWidgetFeatures);
		coordinatesDock->setObjectName(tr("coordinates_dock"));
		const char* coordLabels[] = {"x", "y", "z"};
		m_coordsWidget = new MatrixWidget(3, 1, coordinatesDock, coordLabels, true);
		connect(scene, SIGNAL(geometry_changed()), this, SLOT(refreshCoordinates()));
		connect(scene, SIGNAL(selection_changed()), this, SLOT(refreshCoordinates()));
		connect(m_sceneInspector, SIGNAL(objectChanged(ISceneObject*)), this, SLOT(refreshCoordinates()));
		connect(m_coordsWidget, SIGNAL(valueChanged()), this, SLOT(coordinatesChanged()));

		// m_coordsWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
		// QHBoxLayout* coordinatesLayout = new QHBoxLayout(coordinatesDock);
		// coordinatesLayout->addWidget(m_coordsWidget);
		m_coordsWidget->setFixedHeight(m_coordsWidget->sizeHint().height());
		coordinatesDock->setWidget(m_coordsWidget);
		m_dockWidgets.push_back(make_pair(Qt::LeftDockWidgetArea, coordinatesDock));

	//	script editor
		m_scriptEditor = new QScriptEditor(parentWidget());
		m_scriptEditor->setWindowFlags(Qt::Window |
		                               Qt::CustomizeWindowHint |
		                               Qt::WindowCloseButtonHint |
		                               Qt::WindowMaximizeButtonHint);

	//	script menu
		QAction* actScriptEditor = new QAction(tr("Script Editor"), parentWidget());
		actScriptEditor->setStatusTip("Opens the script editor");
		connect(actScriptEditor, SIGNAL(triggered()), this, SLOT(showScriptEditor()));

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
		sceneMenu->addAction(actScriptEditor);
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
		m_sceneInspector->disconnect(this);
		m_sceneInspector = NULL;
	}

	if(m_scene){
		m_scene->disconnect(m_projectorWidget);
		m_scene->disconnect(this);
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

void MeshModule::showScriptEditor()
{
	m_scriptEditor->show();
	m_scriptEditor->raise();
	m_scriptEditor->activateWindow();
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

void MeshModule::refreshCoordinates()
{
	LGObject* obj = app::getActiveObject();
	vector3 center(0, 0, 0);
	if(obj){
	//	calculate the center of the current selection
		Grid::VertexAttachmentAccessor<APosition> aaPos(obj->grid(), aPosition);
		CalculateCenter(center, obj->selector(), aaPos);
	}

	for(int i = 0; i < 3; ++i)
		m_coordsWidget->set_value(i, 0, center[i]);
}

void MeshModule::coordinatesChanged()
{
	LGObject* obj = app::getActiveObject();
	if(obj){
		vector3 c(0, 0, 0);
		for(int i = 0; i < 3; ++i)
			c[i] = m_coordsWidget->value(i, 0);
		promesh::MoveSelectionTo(obj, c);
		obj->geometry_changed();
	}
}