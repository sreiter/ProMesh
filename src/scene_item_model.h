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

#ifndef SCENE_ITEM_MODEL_H
#define SCENE_ITEM_MODEL_H

#include <QAbstractItemModel>
#include <QIcon>
#include <vector>
#include "scene/scene_interface.h"

////////////////////////////////////////////////////////////////////////
//	predeclarations
struct SceneItemInfo;

////////////////////////////////////////////////////////////////////////
//	typedefs
typedef std::vector<SceneItemInfo*> SceneItemInfoVec;

////////////////////////////////////////////////////////////////////////
//	constants
enum SceneItemType
{
	SIT_UNKNOWN = 0,
	SIT_OBJECT = 1,
	SIT_SUBSET = 2
};

enum SceneItemDataRole
{
	SIDR_VISIBLE = Qt::UserRole + 1,	//bool value
	SIDR_COLOR_SOLID,
	SIDR_COLOR_WIRE
};

////////////////////////////////////////////////////////////////////////
//	SceneItemInfo
///	holds information about the hierarchical representation of the item_model.
struct SceneItemInfo
{
	unsigned int type;
	SceneItemInfo* parent;
	SceneItemInfoVec children;

	ISceneObject* obj;///<	the associated scene-object (valid for subsets too).
	int index;///<	used as subset-index if type == SIT_SUBSET
};


////////////////////////////////////////////////////////////////////////
//	SceneItemModel
///	mediator between the scene and its gui-representation in a tree-view.
class SceneItemModel : public QAbstractItemModel
{
	Q_OBJECT

	public:
		SceneItemModel();
		virtual ~SceneItemModel();

		void setScene(IScene* scene);

		virtual Qt::ItemFlags flags ( const QModelIndex & index ) const;
		virtual QVariant headerData ( int section,
									  Qt::Orientation orientation,
									  int role = Qt::DisplayRole ) const;
		virtual int rowCount ( const QModelIndex & parent = QModelIndex() ) const;
		virtual int columnCount ( const QModelIndex & parent = QModelIndex() ) const;
		virtual QVariant data ( const QModelIndex & index,
								int role = Qt::DisplayRole ) const;
		virtual bool setData ( const QModelIndex & index,
							   const QVariant & value,
							   int role = Qt::EditRole );
		virtual QModelIndex index ( int row, int column,
									const QModelIndex & parent = QModelIndex() ) const;
		virtual QModelIndex parent ( const QModelIndex & index ) const;

		ISceneObject* objectFromIndex(const QModelIndex& index) const;
		QModelIndex parentObjectIndexFromIndex(const QModelIndex& index) const;

		void refreshSubsets();

	protected slots:
		void newObject(ISceneObject* obj);
		void removeObject(ISceneObject* obj);

	protected:
		SceneItemInfo* itemInfoFromIndex(const QModelIndex& index) const;
		QModelIndex indexFromItemInfo(SceneItemInfo* itemInfo,
									  int column) const;
		void updateItemInfo(SceneItemInfo* itemInfo);
		void eraseItemInfo(int index);

	protected:
		IScene* m_scene;
		SceneItemInfoVec	m_itemInfos;

	//	Icons
		QIcon	m_iconVisible;
		QIcon	m_iconInvisible;
		QIcon	m_iconColor;
};


#endif // SCENE_ITEM_MODEL_H
