// created by Sebastian Reiter
// s.b.reiter@gmail.com

#ifndef __H__truncated_double_spin_box
#define __H__truncated_double_spin_box

#include <QString>
#include <QDoubleSpinBox>

class TruncatedDoubleSpinBox : public QDoubleSpinBox
{
	Q_OBJECT

	public:
		TruncatedDoubleSpinBox(QWidget* parent);
		virtual QString textFromValue ( double value ) const;
};

#endif	//__H__truncated_double_spin_box
