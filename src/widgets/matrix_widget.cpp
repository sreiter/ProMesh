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

#include <QGridLayout>
#include "matrix_widget.h"

MatrixWidget::MatrixWidget(int numRows, int numCols, QWidget* parent) :
	QWidget(parent),
	m_numRows(numRows),
	m_numCols(numCols)
{
//	create a grid layout
	QGridLayout* grid = new QGridLayout(this);
	grid->setSpacing(0);
	grid->setContentsMargins(0, 0, 0, 0);

//	create the spin boxes
	for(int col = 0; col < numCols; ++col){
		for(int row = 0; row < numRows; ++row){
			TruncatedDoubleSpinBox* box = new TruncatedDoubleSpinBox(this);
			box->setAlignment(Qt::AlignCenter);
			box->setDecimals(9);
			box->setRange(-1.e+12, 1.e+12);
			box->setLocale(QLocale(tr("C")));
			box->setSingleStep(0.1);

			if(col == row)
				box->setValue(1.0);
			else
				box->setValue(0);

			grid->addWidget(box, row, col, Qt::AlignCenter);
			m_spinBoxes.push_back(box);
			connect(box, SIGNAL(valueChanged(double)), this, SIGNAL(valueChanged()));
		}
	}
}

MatrixWidget::~MatrixWidget()
{
}

double MatrixWidget::value(int row, int col) const
{
	TruncatedDoubleSpinBox* box = get_spin_box(row, col);
	if(box)
		return box->value();
	return 0;
}

void MatrixWidget::set_value(int row, int col, double value)
{
	TruncatedDoubleSpinBox* box = get_spin_box(row, col);
	if(box)
		box->setValue(value);
}

TruncatedDoubleSpinBox* MatrixWidget::get_spin_box(int row, int col) const
{
	if(row >= 0 && row < m_numRows && col >= 0 && col < m_numCols){
		return m_spinBoxes[col * m_numRows + row];
	}
	return NULL;
}
