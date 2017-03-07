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

#ifndef MATRIXWIDGET_H_
#define MATRIXWIDGET_H_

#include <vector>
#include <QWidget>
#include "truncated_double_spin_box.h"

class MatrixWidget : public QWidget
{
	Q_OBJECT

	public:
	/**	If specified, colLabels must have 'numRows' entries.*/
		MatrixWidget(	int numRows,
						int numCols,
						QWidget* parent,
						const char** colLabels = NULL);

		virtual ~MatrixWidget();

		double value(int row, int col) const;
		void set_value(int row, int col, double value);

	signals:
		void valueChanged();

	protected slots:
		void valueChanged(double);
		void textEdited(const QString& newText);
		
	protected:
		TruncatedDoubleSpinBox* get_spin_box(int row, int col) const;

	private:
		int	m_numRows;
		int m_numCols;

		std::vector<TruncatedDoubleSpinBox*>	m_spinBoxes;
		QLineEdit*								m_lineEdit;
		bool									m_bRefreshingCoords;
};


class LineEdit_ClearBeforeDrop : public QLineEdit
{
	Q_OBJECT
	public:
		LineEdit_ClearBeforeDrop (QWidget* parent);
		virtual ~LineEdit_ClearBeforeDrop ()	{};
	protected:
		virtual void dropEvent (QDropEvent* de);
};


#endif /* MATRIXWIDGET_H_ */
