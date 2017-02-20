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

#ifndef RCLICK_MENU_SCENE_INSPECTOR_H
#define RCLICK_MENU_SCENE_INSPECTOR_H

#include <QMenu>
#include "scene_inspector.h"

class RClickMenu_SceneInspector : public QWidget
{
	Q_OBJECT

	public:
		RClickMenu_SceneInspector(SceneInspector * sceneInspector);

		void exec(const QPoint& p);

		QMenu*	getMenu ()	{return m_menu;}

	protected slots:
		void assignSubset();
		void assignNewSubset();
		void rename();
		void showAllSubsets();
		void hideAllSubsets();
		void toggleAllSubsetVisibilities();
		void printSubsetContents();
		void reload();

	private:
		QMenu*			m_menu;
		SceneInspector*	m_sceneInspector;

		QAction*	m_actAssignSubset;
		QAction*	m_actAssignNewSubset;
		QAction*	m_actRename;
		QAction*	m_actShowAllSubsets;
		QAction*	m_actHideAllSubsets;
		QAction*	m_actPrintSubsetContents;
		QAction*	m_actToggleAllSubsetVisibilities;
		QAction*	m_actReload;
		//QAction*	m_actHideOtherSubsets;
};

#endif // RCLICK_MENU_SCENE_INSPECTOR_H
