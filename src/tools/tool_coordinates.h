//	created by Sebastian Reiter
//	s.b.reiter@googlemail.com
//	y2011 m07 d06

#ifndef TOOL_COORDINATES_H
#define TOOL_COORDINATES_H

#include <QFrame>
#include <QString>
#include <sstream>
#include <iomanip>
#include "app.h"
#include "standard_tools.h"
#include "common/error.h"


class TruncatedDoubleSpinBox;
class QLineEdit;

class CoordinatesWidget: public QFrame
{
	Q_OBJECT;

	public:
		CoordinatesWidget(const QString& name, QWidget* parent,
						  ITool* tool, bool applyOnChange = false,
						  bool showApplyButton = true);

		void set_coords(double x, double y, double z);

		double x() const;
		double y() const;
		double z() const;

	protected slots:
		void valueChanged(double);

		void textEdited(const QString& newText);
		
		void apply();
		
	protected:
		ITool*					m_tool;
		TruncatedDoubleSpinBox*	m_x;
		TruncatedDoubleSpinBox*	m_y;
		TruncatedDoubleSpinBox*	m_z;
		QLineEdit*		m_lineEdit;
		bool			m_applyOnChange;
		bool			m_bRefreshingCoords;
};

#endif // TOOL_COORDINATES_H
