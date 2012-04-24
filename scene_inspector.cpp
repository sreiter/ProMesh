
#include <iostream>
#include <QtGui>
#include "scene/scene_interface.h"
#include "scene_inspector.h"
#include "scene_item_model.h"
#include "delegates.h"

#include <modeltest.h>

using namespace std;

////////////////////////////////////////////////////////////////////////
SceneInspector::SceneInspector(QWidget* parent) : QTreeView(parent)
{
//	create objects and connect them
	//m_treeView = new QTreeView(this);

	m_model = new SceneItemModel;
//	if debugging of SceneItemModel is required, then uncomment the following line.
	//new ModelTest(m_model, this);

	this->setModel(m_model);
	this->setItemDelegateForColumn(1, new VisibilityDelegate(1, this));
	this->setItemDelegateForColumn(2, new ColorDelegate(2, SIDR_COLOR_SOLID, this));

	QHeaderView* header = this->header();
	header->setStretchLastSection(false);
	header->setResizeMode(0, QHeaderView::Stretch);
	header->setResizeMode(1, QHeaderView::Fixed);
	header->setResizeMode(2, QHeaderView::Fixed);
	this->resizeColumnToContents(1);
	this->resizeColumnToContents(2);

	setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

	//setExpandsOnDoubleClick(false);
	//setEditTriggers(QAbstractItemView::DoubleClicked);
/*
//	set up layout
	QVBoxLayout* layout = new QVBoxLayout;
	layout->addWidget(m_treeView);

	setLayout(layout);
*/
}

SceneInspector::~SceneInspector()
{
	delete m_model;
}

void SceneInspector::setScene(IScene* scene)
{
	m_model->setScene(scene);
	connect(scene, SIGNAL(visuals_updated()), this, SLOT(refreshView()));
}

void SceneInspector::mousePressEvent(QMouseEvent * event)
{
	QModelIndex index = indexAt(event->pos());
	if(index.isValid()){
		if(index.column() == 1)
		{
			bool visible = m_model->data(index, SIDR_VISIBLE).toBool();
			m_model->setData(index, QVariant(!visible), SIDR_VISIBLE);

			if(visible){
				return;
			}
		}
	}
	QTreeView::mousePressEvent(event);
}

void SceneInspector::mouseReleaseEvent(QMouseEvent* event)
{
	QTreeView::mouseReleaseEvent(event);
	emit(mouseClicked(event));
}

int SceneInspector::getActiveObjectIndex()
{
	QModelIndex index = currentIndex();
	if(index.isValid()){
		QModelIndex objIndex = m_model->parentObjectIndexFromIndex(index);
		if(objIndex.isValid())
			return objIndex.row();
	}
	return -1;
}

int SceneInspector::getActiveSubsetIndex()
{
	QModelIndex index = currentIndex();
	if(index.isValid()){
		if(index.parent().isValid()){
		//	the index is a subset index
			return index.row();
		}
	}
	return -1;
}

ISceneObject* SceneInspector::getActiveObject()
{
	QModelIndex index = currentIndex();
	if(index.isValid())
		return m_model->objectFromIndex(index);
	return NULL;
}

void SceneInspector::setActiveObject(int index)
{
	if(index >= 0)
		this->setCurrentIndex(m_model->index(index, 0));
}

void SceneInspector::setActiveSubset(int objIndex, int subsetIndex)
{
	this->setCurrentIndex(m_model->index(subsetIndex, 0, m_model->index(objIndex, 0)));
}

void SceneInspector::refreshView()
{
	m_model->refreshSubsets();
	dataChanged(QModelIndex(), QModelIndex());
}

void SceneInspector::hideSubset(int objIndex, int subsetIndex)
{
	QModelIndex objModelIndex = m_model->index(objIndex, 1, QModelIndex());
	QModelIndex index = m_model->index(subsetIndex, 1, objModelIndex);
	if(index.isValid())
		m_model->setData(index, QVariant(false), SIDR_VISIBLE);
}

void SceneInspector::showSubset(int objIndex, int subsetIndex)
{
	QModelIndex objModelIndex = m_model->index(objIndex, 1, QModelIndex());
	QModelIndex index = m_model->index(subsetIndex, 1, objModelIndex);
	if(index.isValid())
		m_model->setData(index, QVariant(true), SIDR_VISIBLE);
}
