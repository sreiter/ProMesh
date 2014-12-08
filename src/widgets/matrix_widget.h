//	created by Sebastian Reiter
//	s.b.reiter@googlemail.com
//	y11 m08 d02

#ifndef MATRIXWIDGET_H_
#define MATRIXWIDGET_H_

#include <vector>
#include <QWidget>
#include "truncated_double_spin_box.h"

class MatrixWidget : public QWidget
{
	Q_OBJECT

	public:
		MatrixWidget(int numRows, int numCols, QWidget* parent);
		virtual ~MatrixWidget();

		double value(int row, int col) const;
		void set_value(int row, int col, double value);

	protected:
		TruncatedDoubleSpinBox* get_spin_box(int row, int col) const;

	private:
		int	m_numRows;
		int m_numCols;

		std::vector<TruncatedDoubleSpinBox*>	m_spinBoxes;
};

#endif /* MATRIXWIDGET_H_ */
