// created by Sebastian Reiter
// s.b.reiter@gmail.com

#include "truncated_double_spin_box.h"

TruncatedDoubleSpinBox::
TruncatedDoubleSpinBox(QWidget* parent) :
	QDoubleSpinBox(parent)
{}

QString TruncatedDoubleSpinBox::
textFromValue(double value) const
{
	QString s = QDoubleSpinBox::textFromValue(value);
//	check if a decimal point is contained. If this is the
//	case, remove all zeros at the end of the string
	//chop, truncate, resize left

	int pointInd = s.indexOf('.');
	if(pointInd == -1)
		return s;

	int len = s.length();
	while((len > 0) && (s[len - 1] == '0'))
		--len;

	if(pointInd == len - 1)
		--len;
	
	return s.left(len);
}
