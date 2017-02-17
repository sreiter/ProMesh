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

#ifndef __H__PROMESH_module_interface
#define __H__PROMESH_module_interface

#include <vector>
#include <QMainWindow>

class LGScene;
class SceneInspector;
class QMenu;


class IModule : public QObject {
	Q_OBJECT

public:
	typedef std::vector<std::pair<Qt::DockWidgetArea, QDockWidget*> >
			dock_list_t;

	IModule ();
	IModule (QWidget* parent);
	virtual ~IModule ();

	/// this method is called before any virtual method of the module is called.
	/** Expecially none of the 'get...' methods is called before 'activate'.
	 * Activate should be used to establish connections, e.g., to the sceneInspector
	 * and to create widgets which were not already created in the constructor.
	 * Those widgets will afterwards be queried through the 'get...' methods.
	 * \sa deactivate
	 */
	virtual
	void activate(SceneInspector* sceneInspector, LGScene* scene);

	///	no 'get...' method is called after 'deactivate' until the next call to 'activate'
	/** One should disconnect all connections to widgets outside of the module,
	 * e.g., the sceneInspector. No need to disconnect from menus which belong
	 * to the module.*/
	virtual
	void deactivate();

	virtual
	dock_list_t
	getDockWidgets();

	virtual
	std::vector<QToolBar*>
	getToolBars();

	virtual
	QMenu* getSceneInspectorMenu();

	virtual
	std::vector<QMenu*> getMenus();


	QWidget* parentWidget()				{return m_parent;}
	const QWidget* parentWidget() const	{return m_parent;}

	virtual void keyPressEvent(QKeyEvent* event)	{};

private:
	QWidget*	m_parent;
};

#endif	//__H__UG_module_interface
