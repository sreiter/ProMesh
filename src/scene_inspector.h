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

#ifndef SCENE_INSPECTOR_H
#define SCENE_INSPECTOR_H

#include <QWidget>
#include <QTreeView>
#include "scene_item_model.h"

class QTreeView;
class IScene;

////////////////////////////////////////////////////////////////////////
//	SceneInspector
///	Displays information about objects in a scene in a TreeView
class SceneInspector : public QTreeView
{
	Q_OBJECT

	public:
		SceneInspector(QWidget* parent);
		virtual ~SceneInspector();

		void setScene(IScene* scene);

		inline const SceneItemModel* getItemModel()		{return m_model;}

		int getActiveObjectIndex();
		int getActiveSubsetIndex();
		ISceneObject* getActiveObject();
		void setActiveObject(int index);
		void setActiveSubset(int objIndex, int subsetIndex);

		void hideSubset(int objIndex, int subsetIndex);
		void showSubset(int objIndex, int subsetIndex);

	protected:
		virtual void mousePressEvent(QMouseEvent* event);
		virtual void mouseReleaseEvent(QMouseEvent* event);

	signals:
		void mouseClicked(QMouseEvent* event);

	public slots:
		void refreshView();

	protected:
		SceneItemModel*	m_model;
};

#endif // SCENE_INSPECTOR_H
