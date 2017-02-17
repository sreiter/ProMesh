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

#include <iostream>
#include "scene_item_model.h"

using namespace std;

SceneItemModel::SceneItemModel() : QAbstractItemModel()
{
	m_scene = NULL;
	m_iconVisible.addFile(":images/visible_16.png");
	m_iconInvisible.addFile(":images/invisible_16.png");
	m_iconColor.addFile(":images/cube_solid.png");
}

SceneItemModel::~SceneItemModel()
{
}

void SceneItemModel::setScene(IScene* scene)
{
//	notify view
	emit layoutAboutToBeChanged();

//	update connections
	if(m_scene)
	{
		disconnect(m_scene, SIGNAL(object_added(ISceneObject*)),
				   this, SLOT(new_object(ISceneObject*)));
		disconnect(m_scene, SIGNAL(object_to_be_removed(ISceneObject*)),
					this, SLOT(removeObject(ISceneObject*)));
	}

	m_scene = scene;

	if(m_scene)
	{
		connect(m_scene, SIGNAL(object_added(ISceneObject*)),
				this, SLOT(newObject(ISceneObject*)));
		connect(m_scene, SIGNAL(object_to_be_removed(ISceneObject*)),
				this, SLOT(removeObject(ISceneObject*)));
	}

//	notify view
	emit layoutChanged();
}

void SceneItemModel::refreshSubsets()
{
	if(!m_scene)
		return;

//	first we'll clear the subset-entries of all objects
	for(size_t i = 0; i < m_itemInfos.size(); ++i){
	//	the model-index of the i-th scene object
		QModelIndex objModelIndex = index(i, 0, QModelIndex());
		SceneItemInfo* psii = m_itemInfos[i];
		ISceneObject* obj = psii->obj;

		if((int)psii->children.size() > obj->num_subsets()){
		//	notify that subsets are removed from the list
			beginRemoveRows(objModelIndex, obj->num_subsets(),
							(int)psii->children.size()-1);

		//	call update to adjust children
			updateItemInfo(psii);

		//	removal done. inform the model.
			endRemoveRows();
		}
		else if((int)psii->children.size() < obj->num_subsets()){
		//	notify base class
			beginInsertRows(objModelIndex, psii->children.size(),
							obj->num_subsets()-1);

		//	call update to adjust children
			updateItemInfo(psii);

		//	notify base class
			endInsertRows();
		}
		else{
		//	call update to adjust children
			updateItemInfo(psii);
		}
	}
}

Qt::ItemFlags SceneItemModel::flags ( const QModelIndex & index ) const
{
	int col = index.column();
	switch(col)
	{
		case 0: return Qt::ItemIsSelectable | Qt::ItemIsEnabled/* | Qt::ItemIsEditable*/;
		case 1: return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
		case 2: return Qt::ItemIsEnabled | Qt::ItemIsSelectable| Qt::ItemIsEditable;
	}
	return Qt::NoItemFlags;
}

QVariant SceneItemModel::headerData ( int section,
							Qt::Orientation orientation,
							int role) const
{
	switch(section)
	{
		case 0:
		{
			if(role == Qt::DisplayRole)
				return QString(tr("geometry name"));
		}break;
		case 1:
		{
			if(role == Qt::DecorationRole)
				return m_iconVisible;
		}break;
		case 2:
		{
			if(role == Qt::DecorationRole)
				return m_iconColor;
		}break;
	}

	return QVariant();
}

int SceneItemModel::rowCount ( const QModelIndex & parent) const
{
	if(m_scene)
	{
		SceneItemInfo* itemInfo = itemInfoFromIndex(parent);
		if(itemInfo)
			return itemInfo->children.size();
		else
			return m_itemInfos.size();
	}

	return 0;
}

int SceneItemModel::columnCount ( const QModelIndex & parent) const
{
	if(parent.isValid())
		return 3;
	return 3;
}

QVariant SceneItemModel::data ( const QModelIndex & index,
								int role) const
{

	int col = index.column();

	if(SceneItemInfo* itemInfo = itemInfoFromIndex(index))
	{
		switch(col)
		{
			case 0:
			{
				if(role == Qt::DisplayRole)
				{
					switch(itemInfo->type)
					{
						case SIT_OBJECT:
							return QString::fromUtf8(itemInfo->obj->name());
						case SIT_SUBSET:{
							QString name = QString::number(itemInfo->index);
							name.append(": ");
							name.append(QString::fromUtf8(itemInfo->obj->get_subset_name(itemInfo->index)));
							return name;
						}
					}
					return QString(tr("UNKNOWN NAME"));
				}
			}break;
			case 1:
			{
				if(role == SIDR_VISIBLE)
				{
					if(itemInfo->type == SIT_OBJECT)
						return QVariant(itemInfo->obj->is_visible());
					else if(itemInfo->type == SIT_SUBSET)
						return QVariant(
								itemInfo->obj->subset_is_visible(
										itemInfo->index));
					return QVariant(true);
				}
			}break;
			case 2:
			{
				if(role == SIDR_COLOR_SOLID)
				{
					if(itemInfo->type == SIT_OBJECT)
						return QVariant((uint)itemInfo->obj->get_color().rgb());
					else if(itemInfo->type == SIT_SUBSET)
						return QVariant(
								(uint)itemInfo->obj->get_subset_color(
										itemInfo->index).rgb());
					return QVariant((uint)Qt::red);
				}
			}break;
		}
	}

	return QVariant();
}

bool SceneItemModel::setData ( const QModelIndex & index,
								const QVariant & value,
								int role)
{
	if(SceneItemInfo* itemInfo = itemInfoFromIndex(index))
	{
		if(index.column() == 0 && role == Qt::EditRole)
		{
			if(value.toString().isEmpty())
				return false;

			if(itemInfo->type == SIT_OBJECT)
				itemInfo->obj->set_name(value.toString().toLatin1());
			else if(itemInfo->type == SIT_SUBSET)
				itemInfo->obj->set_subset_name(itemInfo->index,
									value.toString().toLatin1());
			else
				return false;

			emit dataChanged(index, index);
			return true;
		}

		if(index.column() == 1 && role == SIDR_VISIBLE)
		{
			{
				bool updateVisuals = true;
				if(itemInfo->type == SIT_OBJECT)
					itemInfo->obj->set_visibility(value.toBool());
				else if(itemInfo->type == SIT_SUBSET)
					itemInfo->obj->set_subset_visibility(
							itemInfo->index, value.toBool());
				else
					updateVisuals = false;

			//	the visuals have to be updated
				if(updateVisuals)
				{
					itemInfo->obj->visuals_changed(false);
					//m_scene->visibility_changed(itemInfo->obj);
				}

			}
			emit dataChanged(index, index);
			return true;
		}

		if(index.column() == 2 && role == SIDR_COLOR_SOLID)
		{
		//	extract color from QVariant
			bool ok = false;
			QRgb col = (QRgb)value.toUInt(&ok);
			if(ok)
			{
				bool updateVisuals = true;
				if(itemInfo->type == SIT_OBJECT)
					itemInfo->obj->set_color(QColor(col));
				else if(itemInfo->type == SIT_SUBSET)
					itemInfo->obj->set_subset_color(
									itemInfo->index, QColor(col));
				else
					updateVisuals = false;

			//	the visuals have to be updated
				if(updateVisuals)
				{
					m_scene->color_changed(itemInfo->obj);
				}

				emit dataChanged(index, index);
				return true;
			}
		}
	}

	return QAbstractItemModel::setData(index, value, role);
}

QModelIndex SceneItemModel::index ( int row, int column,
									const QModelIndex & parent) const
{
	if(row < 0 || column < 0)
		return QModelIndex();

	if(m_scene)
	{/*
		if(parent.isValid()){
			if(parent.column() == 1)
				return QModelIndex();
		}*/

		SceneItemInfo* parentInfo = itemInfoFromIndex(parent);
		if(!parentInfo)
		{
		//	since the parent is invalid, the index is a top-level index.
		//	ItemInfos for top-level objects are stored in m_itemInfos.
			if(row < (int)m_itemInfos.size())
				return createIndex(row, column, m_itemInfos[row]);
		}
		else
		{
		//	make sure, that the given row is ok
			if(row < (int)parentInfo->children.size())
			{
				return createIndex(row, column, parentInfo->children[row]);//	subsets are handled by indices.
			}
		}
	}
	return QModelIndex();
}

QModelIndex SceneItemModel::parent ( const QModelIndex & index ) const
{
	if(index.isValid())
	{
	//	return item-index of parent
		if(SceneItemInfo* itemInfo = itemInfoFromIndex(index))
		{
			if(itemInfo->parent)
				return indexFromItemInfo(itemInfo->parent, 0);
		}
	}
	return QModelIndex();
}

void SceneItemModel::newObject(ISceneObject* obj)
{
	if(!m_scene)
		return;

//	row-index of the new object:
	int rowIndex = m_scene->num_objects() - 1;

//	notify base class
	beginInsertRows(QModelIndex(), rowIndex, rowIndex);

//	create a new ItemInfo.
	SceneItemInfo* itemInfo = new SceneItemInfo;
	m_itemInfos.push_back(itemInfo);
	itemInfo->type = SIT_OBJECT;
	itemInfo->obj = obj;
	itemInfo->parent = NULL;

//	call update to populate children
	updateItemInfo(itemInfo);

//	notify base class
	endInsertRows();
}

void SceneItemModel::removeObject(ISceneObject* obj)
{
	if(!m_scene)
		return;

//	row index of obj
	int row = m_scene->get_object_index(obj);
	if(row < 0)
		return;

//	inform the model about item removal
	beginRemoveRows(QModelIndex(), row, row);

//	erase the item-info entry
	eraseItemInfo(row);

//	removal done. inform the model.
	endRemoveRows();
}

SceneItemInfo* SceneItemModel::itemInfoFromIndex(const QModelIndex& index) const
{
	if(index.isValid())
		return static_cast<SceneItemInfo*>(index.internalPointer());
	return NULL;
}

ISceneObject* SceneItemModel::objectFromIndex(const QModelIndex& index) const
{
	if(SceneItemInfo* itemInfo = itemInfoFromIndex(index))
		return itemInfo->obj;
	return NULL;
}

QModelIndex SceneItemModel::parentObjectIndexFromIndex(const QModelIndex& index) const
{
	QModelIndex tInd = index;
	while(tInd.isValid()){
		SceneItemInfo* info = itemInfoFromIndex(tInd);
		if(info->type == SIT_OBJECT)
			return tInd;
		tInd = tInd.parent();
	}
	return QModelIndex();
}

QModelIndex SceneItemModel::indexFromItemInfo(SceneItemInfo* itemInfo,
											  int column) const
{
	SceneItemInfo* parentInfo = itemInfo->parent;

	if(parentInfo)
	{
	//	get the index at which the item is stored by searching
	//	the parents children list
		for(size_t i = 0; i < parentInfo->children.size(); ++i)
		{
			if(parentInfo->children[i] == itemInfo)
				return createIndex(i, column, itemInfo);
		}
	}
	else
	{
	//	get the index at which the item is stored by searching
	//	the the classes m_itemInfo.
		for(size_t i = 0; i < m_itemInfos.size(); ++i)
		{
			if(m_itemInfos[i] == itemInfo)
				return createIndex(i, column, itemInfo);
		}
	}

	return QModelIndex();
}

void SceneItemModel::updateItemInfo(SceneItemInfo* itemInfo)
{
	switch(itemInfo->type)
	{
		case SIT_OBJECT:
		{
		//	clear children
			if(itemInfo->obj->num_subsets() < (int)itemInfo->children.size())
			{
				for(size_t i = itemInfo->obj->num_subsets();
					i < itemInfo->children.size(); ++i){
					delete itemInfo->children[i];
				}
				itemInfo->children.resize(itemInfo->obj->num_subsets());
			}

		//	add children
			for(int i = (int)itemInfo->children.size();
				i < itemInfo->obj->num_subsets(); ++i)
			{
				SceneItemInfo* newInfo = new SceneItemInfo;
				newInfo->obj = itemInfo->obj;
				newInfo->type = SIT_SUBSET;
				newInfo->index = i;
				newInfo->parent = itemInfo;
				itemInfo->children.push_back(newInfo);
			}
		}break;
	}
}

void SceneItemModel::eraseItemInfo(int index)
{
//	remove the entry from m_itemInfos
	SceneItemInfo* psi = *(m_itemInfos.begin() + index);
	for(size_t i = 0; i < psi->children.size(); ++i)
		delete psi->children[i];

	delete *(m_itemInfos.begin() + index);

	m_itemInfos.erase(m_itemInfos.begin() + index);
}
