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

#include <QHBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QFileDialog>
#include <iostream>
#include "file_widget.h"
#include "app.h"

FileWidget::FileWidget(FileWidgetType fwt, QString filter, QWidget* parent) :
	QWidget(parent),
	m_type(fwt),
	m_filter(filter)
{
//	create a grid layout
	QHBoxLayout* layout = new QHBoxLayout(this);
	layout->setContentsMargins(0, 0, 0, 0);

//	create a text-box and a browse button
	m_lbl = new QLabel(tr("-- no file selected --"), this);
	layout->addWidget(m_lbl);

	QPushButton* btn = new QPushButton(tr("Browse ..."), this);
	layout->addWidget(btn);
	connect(btn, SIGNAL(clicked()), this, SLOT(browse()));
}

FileWidget::~FileWidget()
{

}

const QString FileWidget::filename() const
{
	if(m_filenames.empty()){
		return QString("");
	}
	return m_filenames.front();
}

const QStringList& FileWidget::filenames() const
{
	return m_filenames;
}

void FileWidget::browse()
{
	QString path = app::getMainWindow()->settings().value("FileWidget-path", ".").toString();
	m_filenames.clear();
	switch(m_type){
		case FWT_OPEN:{
			QString fileName = QFileDialog::getOpenFileName(
								this,
								tr("Open"),
								path,
								m_filter);
			m_filenames.push_back(fileName);
		}break;

		case FWT_OPEN_SEVERAL:{
			m_filenames = QFileDialog::getOpenFileNames(
								this,
								tr("Open"),
								path,
								m_filter);
		}break;

		case FWT_SAVE:{

		}break;
	}

	QString str = filename();
	if(!str.isEmpty()){
		app::getMainWindow()->settings().setValue("FileWidget-path", QFileInfo(str).absolutePath());
		m_lbl->setText(str);
	}
}
