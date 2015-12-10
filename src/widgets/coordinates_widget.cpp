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

#include <QtWidgets>
#include "truncated_double_spin_box.h"
#include "coordinates_widget.h"

CoordinatesWidget::
CoordinatesWidget(const QString& name, QWidget* parent,
				  ITool* tool, bool applyOnChange,
				  bool showApplyButton) :
	QFrame(parent),
	m_applyOnChange(applyOnChange),
	m_bRefreshingCoords(false)
{
	m_tool = tool;
	setFrameStyle(QFrame::StyledPanel | QFrame::Sunken);

	QVBoxLayout* vLayout = new QVBoxLayout(this);
	vLayout->setSpacing(2);

	QFormLayout* formLayout = new QFormLayout();
	formLayout->setSpacing(5);
	formLayout->setHorizontalSpacing(10);
	formLayout->setVerticalSpacing(8);
	vLayout->addLayout(formLayout);

	vLayout->addSpacing(15);

//	input boxes
	m_x = new TruncatedDoubleSpinBox(this);
	m_x->setLocale(QLocale(tr("C")));
	m_x->setValue(0);
	m_x->setDecimals(9);
	m_x->setRange(-1e+9, 1e+9);
	m_x->setSingleStep(1.);
	connect(m_x, SIGNAL(valueChanged(double)), this, SLOT(valueChanged(double)));
	formLayout->addRow(tr("x:"), m_x);

	m_y = new TruncatedDoubleSpinBox(this);
	m_y->setLocale(QLocale(tr("C")));
	m_y->setValue(0);
	m_y->setDecimals(9);
	m_y->setRange(-1e+9, 1e+9);
	m_y->setSingleStep(1.);
	connect(m_y, SIGNAL(valueChanged(double)), this, SLOT(valueChanged(double)));
	formLayout->addRow(tr("y:"), m_y);

	m_z = new TruncatedDoubleSpinBox(this);
	m_z->setLocale(QLocale(tr("C")));
	m_z->setValue(0);
	m_z->setDecimals(9);
	m_z->setRange(-1e+9, 1e+9);
	m_z->setSingleStep(1.);
	connect(m_z, SIGNAL(valueChanged(double)), this, SLOT(valueChanged(double)));
	formLayout->addRow(tr("z:"), m_z);

	m_lineEdit = new QLineEdit(this);
	m_lineEdit->setText(tr("0 0 0"));
	connect(m_lineEdit, SIGNAL(textEdited(const QString&)),
			this, SLOT(textEdited(const QString&)));
	formLayout->addRow(tr("text input:"), m_lineEdit);

//	create apply, ok and cancel buttons
	if(showApplyButton){
		QPushButton* btn = new QPushButton(tr("Apply"), this);
		vLayout->addWidget(btn, 0, Qt::AlignLeft);
		connect(btn, SIGNAL(clicked()), this, SLOT(apply()));
	}
}

void CoordinatesWidget::
set_coords(double x, double y, double z)
{
//todo store default coords for cancel
	m_bRefreshingCoords = true;
	m_x->setValue(x);
	m_y->setValue(y);
	m_z->setValue(z);
	std::stringstream ss;
	ss << x << " " << y << " " << z;
	m_lineEdit->setText(ss.str().c_str());
	m_bRefreshingCoords = false;
}

double CoordinatesWidget::x() const	{return m_x->value();}
double CoordinatesWidget::y() const	{return m_y->value();}
double CoordinatesWidget::z() const	{return m_z->value();}

void CoordinatesWidget::
valueChanged(double)
{
//	if we're not refreshing the value from the text box,
//	we'll have to update text box
	if(m_bRefreshingCoords)
		return;

	m_bRefreshingCoords = true;

	std::stringstream ss;
	ss << m_x->value() << " " << m_y->value() << " " << m_z->value();

	m_lineEdit->setText(ss.str().c_str());

	if(m_applyOnChange)
		apply();

	m_bRefreshingCoords = false;
}

void CoordinatesWidget::
textEdited(const QString& newText)
{
//	only refresh coordinates if we're not already doing it.
	if(m_bRefreshingCoords)
		return;

	m_bRefreshingCoords = true;

//	parse the coordinates
	std::stringstream ss(m_lineEdit->text().toStdString());
	double val;
	int coordCounter = 0;
	while(!ss.eof()){
	//	we'll ignore ' ', ',', '(', ')' '/'
		int nextChar = ss.peek();
		if(nextChar == ',' || nextChar == '('
		  || nextChar == ')' || nextChar == '/'
		  || nextChar == ' ')
		{
			ss.ignore(1);
			continue;
		}

		ss >> val;
		if(ss.fail())
			break;
		switch(coordCounter){
			case 0:	m_x->setValue(val); break;
			case 1:	m_y->setValue(val); break;
			case 2:	m_z->setValue(val); break;
		}
		++coordCounter;
	}

	if(m_applyOnChange)
		apply();

	m_bRefreshingCoords = false;
}

void CoordinatesWidget::
apply()
{
	LGObject* obj = app::getActiveObject();
	if(m_tool && obj){
		try{
			m_tool->execute(obj, this);
		}
		catch(ug::UGError error){
			UG_LOG("Execution of tool " << m_tool->get_name() << " failed with the following message:\n");
			UG_LOG("  " << error.get_msg() << std::endl);
		}
	}
}
