/*
 * Copyright (c) 2016:  G-CSC, Goethe University Frankfurt
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

#include <QScrollArea>
#include "property_widget.h"

PropertyWidget::
PropertyWidget(QWidget* parent) :
	QFrame(parent),
	m_content(NULL)
{
	m_scrollArea = new QScrollArea(this);
	//m_scrollArea->setBackgroundRole(QPalette::Dark);

	QVBoxLayout* layout = new QVBoxLayout(this);
	layout->addWidget(m_scrollArea);
	layout->setSpacing(0);
	layout->setContentsMargins(0, 0, 0, 0);
	this->setLayout(layout);

//	This widget will contain the widgetContainer and a spacer
	m_spacerWidget = new QWidget(m_scrollArea);
	m_spacerLayout = new QVBoxLayout(m_spacerWidget);
	m_spacerLayout->setSpacing(0);
	m_spacerLayout->setContentsMargins(0, 0, 0, 0);
	m_spacerWidget->setLayout(m_spacerLayout);

	m_scrollArea->setWidget(m_spacerWidget);
	m_scrollArea->setWidgetResizable(true);
}

PropertyWidget::
~PropertyWidget()
{

}
