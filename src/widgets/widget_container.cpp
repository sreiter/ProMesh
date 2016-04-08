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

#include <QFrame>
#include <QVBoxLayout>
#include "widget_container.h"


WidgetContainer::WidgetContainer(QWidget* parent) : QFrame(parent)
{
	m_layout = new QVBoxLayout(this);
	m_layout->setSpacing(0);
	m_layout->setContentsMargins(0, 0, 0, 0);
	this->setLayout(m_layout);
	this->setLineWidth(1);
	// m_layout->addWidget(new WidgetContainerSeparator(this));
	//this->setContentsMargins(1, 1, 1, 0);
	//this->setFrameStyle(QFrame::Box | QFrame::Plain);
	// setFrameStyle(QFrame::StyledPanel | QFrame::Sunken);
	// setFrameStyle(QFrame::NoFrame | QFrame::Plain);
}

WidgetContainer::~WidgetContainer()
{

}

void WidgetContainer::addWidget(QWidget* widget, Qt::Alignment alignment)
{
/*	QFrame* frame = new QFrame(this);
	QVBoxLayout* layout = new QVBoxLayout(frame);
	frame->setLayout(layout);
	layout->setContentsMargins(10, 2, 4, 2);

	//frame->setBackgroundRole(QPalette::Shadow);
	frame->setLineWidth(1);
	frame->setFrameStyle(QFrame::Box | QFrame::Plain);
	frame->setContentsMargins(0, 0, 0, 1);

	widget->setParent(frame);
	layout->addWidget(widget);
	layout->setAlignment(widget, alignment);
	m_layout->addWidget(frame);*/
	widget->setParent(this);
	m_layout->addWidget(widget);
	m_layout->setAlignment(widget, alignment);
	// m_layout->addWidget(new WidgetContainerSeparator(this));
}



WidgetContainerSeparator::WidgetContainerSeparator(QWidget* parent) :
	QFrame(parent)
{
	setFrameStyle(QFrame::HLine | QFrame::Plain);
	setLineWidth(0);
}

WidgetContainerSeparator::~WidgetContainerSeparator()
{
}
