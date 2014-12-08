//	created by Sebastian Reiter
//	s.b.reiter@googlemail.com
//	y11 m08 d02

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
