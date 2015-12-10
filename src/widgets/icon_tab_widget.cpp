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
 
#include "icon_tab_widget.h"
#include <QVBoxLayout>
#include <QToolBar>
#include <QToolButton>
#include <QStackedWidget>
#include <QSignalMapper>


IconTabWidget::IconTabWidget(QWidget* parent) : QWidget(parent)
{
	m_toolBar = new QToolBar(this);
	m_toolBar->setIconSize(QSize(24, 24));

	m_stackedWidget = new QStackedWidget(this);

	QVBoxLayout* layout = new QVBoxLayout();
	layout->addWidget(m_toolBar);
	layout->addWidget(m_stackedWidget);
	layout->setSpacing(0);
	layout->setContentsMargins(0, 0, 0, 0);
	this->setLayout(layout);

//	the signal mapper is used to connect the tool-buttons with the stacked widgets
	m_signalMapper = new QSignalMapper(this);
	connect(m_signalMapper, SIGNAL(mapped(int)), m_stackedWidget, SLOT(setCurrentIndex(int)));
}

IconTabWidget::~IconTabWidget()
{

}

void IconTabWidget::addPage(QWidget* page, const QIcon& icon, const QString& tooltip)
{
	QToolButton* toolBtn = new QToolButton(m_toolBar);
	toolBtn->setIcon(icon);
	toolBtn->setCheckable(true);
	toolBtn->setAutoExclusive(true);
	toolBtn->setToolTip(tooltip);

	m_signalMapper->setMapping(toolBtn, m_stackedWidget->count());
	connect(toolBtn, SIGNAL(clicked()), m_signalMapper, SLOT(map()));

	m_toolBar->addWidget(toolBtn);
	m_stackedWidget->addWidget(page);

//	if this is the first page, we'll select it
	if(m_stackedWidget->count() == 1){
		toolBtn->setChecked(true);
		m_stackedWidget->setCurrentIndex(0);
	}
}

int IconTabWidget::count()
{
	return m_stackedWidget->count();
}

QWidget* IconTabWidget::widget(int pageIndex)
{
	return m_stackedWidget->widget(pageIndex);
}
