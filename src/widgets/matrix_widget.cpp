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
#include <QLabel>
#include <QLineEdit>
#include <sstream>
#include "matrix_widget.h"


LineEdit_ClearBeforeDrop::
LineEdit_ClearBeforeDrop (QWidget* parent) :
	QLineEdit (parent)
{}

void LineEdit_ClearBeforeDrop::
dropEvent (QDropEvent* de)
{
	setText("");
	QLineEdit::dropEvent(de);
}



MatrixWidget::
MatrixWidget(
		int numRows,
		int numCols,
		QWidget* parent,
		const char** labels,
		bool stretchValues) :
	QWidget(parent),
	m_numRows(numRows),
	m_numCols(numCols),
	m_lineEdit(NULL),
	m_bRefreshingCoords(false)
{
//	create a grid layout
	m_grid = new QGridLayout(this);
	m_grid->setSpacing(0);
	m_grid->setContentsMargins(0, 0, 0, 0);

	const bool useLabels = labels || numCols == 1;
	int colOffset = useLabels ? 1 : 0;
	Qt::Alignment spinBoxAlignment = useLabels ? Qt::AlignLeft : Qt::AlignCenter;

	if(useLabels && stretchValues)
		m_grid->setColumnStretch(1, 1);

//	create the spin boxes
	for(int col = 0; col < numCols; ++col){
		for(int row = 0; row < numRows; ++row){
			if(labels && col == 0){
				QLabel* lbl = new QLabel(QString(labels[row]).append(" "), this);
				m_grid->addWidget(lbl, row, 0, Qt::AlignRight);
			}
			TruncatedDoubleSpinBox* box = new TruncatedDoubleSpinBox(this);
			box->setDecimals(9);
			box->setRange(-1.e+12, 1.e+12);
			box->setLocale(QLocale(tr("C")));
			box->setSingleStep(0.1);
			if(numCols == 1){
				connect(box, SIGNAL(valueChanged(double)), this, SLOT(valueChanged(double)));
			}
			else
				box->setAlignment(Qt::AlignCenter);

			if(numCols > 1 && col == row)
				box->setValue(1.0);
			else
				box->setValue(0);


			m_grid->addWidget(box, row, col + colOffset, spinBoxAlignment);

			m_spinBoxes.push_back(box);
		}
	}

	if(numCols == 1){
		QString txt = "0";
		for(int i = 1; i < numRows; ++i)
			txt.append(" 0");

		m_lineEdit = new LineEdit_ClearBeforeDrop(this);
		m_lineEdit->setText(txt);
		if(!stretchValues && !m_spinBoxes.empty())
			m_lineEdit->setFixedWidth(m_spinBoxes.front()->width());
		else{
			m_lineEdit->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
		}

		connect(m_lineEdit, SIGNAL(textEdited(const QString&)),
				this, SLOT(textEdited(const QString&)));

		m_grid->addWidget(new QLabel(tr("text "), this), numRows, 0, Qt::AlignRight);
		m_grid->addWidget(m_lineEdit, numRows, 1, Qt::AlignLeft);
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
	if(m_bRefreshingCoords)
		return;

	m_bRefreshingCoords = true;
	TruncatedDoubleSpinBox* box = get_spin_box(row, col);
	if(box){
		box->setValue(value);
		update_text();
	}
	m_bRefreshingCoords = false;
}

TruncatedDoubleSpinBox* MatrixWidget::get_spin_box(int row, int col) const
{
	if(row >= 0 && row < m_numRows && col >= 0 && col < m_numCols){
		return m_spinBoxes[col * m_numRows + row];
	}
	return NULL;
}

void MatrixWidget::
valueChanged(double)
{
//	if we're not refreshing the value from the text box,
//	we'll have to update text box
	if(m_bRefreshingCoords)
		return;

	m_bRefreshingCoords = true;

	update_text();

	emit valueChanged();
	m_bRefreshingCoords = false;
}

void MatrixWidget::
textEdited(const QString& newText)
{
//	only refresh coordinates if we're not already doing it.
	if(m_bRefreshingCoords)
		return;

	m_bRefreshingCoords = true;

//	parse the coordinates
	std::stringstream ss(m_lineEdit->text().toStdString());
	double val;
	size_t coordCounter = 0;
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

		if(coordCounter < m_spinBoxes.size())
			m_spinBoxes[coordCounter]->setValue(val);

		++coordCounter;
	}

	emit valueChanged();
	m_bRefreshingCoords = false;
}

void MatrixWidget::
update_text()
{
	std::stringstream ss;
	for(size_t i = 0; i < m_spinBoxes.size(); ++i){
		ss << m_spinBoxes[i]->cleanText().toStdString();
		if(i + 1 < m_spinBoxes.size())
			ss << " ";
	}

	m_lineEdit->setText(ss.str().c_str());
}