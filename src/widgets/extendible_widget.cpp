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
 
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QToolButton>
#include <QLabel>
#include "extendible_widget.h"


ExtendibleWidget::ExtendibleWidget(QWidget* parent) :
	QWidget(parent),
	m_widget(NULL)
{
	QVBoxLayout* mainLayout = new QVBoxLayout(this);
	m_vLayout = new QVBoxLayout();
	QHBoxLayout* hLayout = new QHBoxLayout();

	mainLayout->setSpacing(0);
	mainLayout->setContentsMargins(0, 0, 10, 0);
	hLayout->setSpacing(10);
	hLayout->setContentsMargins(0, 0, 0, 0);
	m_vLayout->setContentsMargins(10, 0, 0, 0);

	this->setLayout(mainLayout);
	mainLayout->addLayout(hLayout);
	mainLayout->addLayout(m_vLayout);

	m_toolButton = new QToolButton(this);
	m_toolButton->setCheckable(true);
	m_toolButton->setArrowType(Qt::RightArrow);
	hLayout->addWidget(m_toolButton);

	ExtendibleWidgetHeader* header = new ExtendibleWidgetHeader(this);
	connect(header, SIGNAL(clicked()), this, SLOT(toggle()));
	connect(header, SIGNAL(double_clicked()), this, SLOT(toggle()));

	m_header = header;
	hLayout->addWidget(m_header);

	connect(m_toolButton, SIGNAL(toggled(bool)), this, SLOT(setChecked(bool)));
}

ExtendibleWidget::~ExtendibleWidget()
{

}


void ExtendibleWidget::setText(const QString& text)
{
	m_header->setText(text);
}

void ExtendibleWidget::setInfoText(const QString& toolTip)
{
	m_header->setToolTip(toolTip);
	this->setToolTip(toolTip);
}

void ExtendibleWidget::setWidget(QWidget* widget)
{
	if(m_widget){
		m_vLayout->removeWidget(m_widget);
	}
	m_widget = widget;
	m_vLayout->addWidget(m_widget);
	if(m_toolButton->isChecked())
		m_widget->show();
	else
		m_widget->hide();
}

void ExtendibleWidget::setChecked(bool checked)
{
	if(m_toolButton->isChecked() != checked){
		m_toolButton->setChecked(checked);
		return;
	}

	if(checked){
		m_toolButton->setArrowType(Qt::DownArrow);
		if(m_widget)
			m_widget->show();
	}
	else{
		m_toolButton->setArrowType(Qt::RightArrow);
		if(m_widget)
			m_widget->hide();
	}
}

void ExtendibleWidget::toggle()
{
	setChecked(!m_toolButton->isChecked());
}
