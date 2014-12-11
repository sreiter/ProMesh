//	created by Sebastian Reiter
//	s.b.reiter@googlemail.com
//	y09 m11 d04

#include <QtWidgets>
#include "color_widget.h"

ColorWidget::ColorWidget(QWidget* parent) : QWidget(parent)
{
	m_color = Qt::black;
}

void ColorWidget::setColor(const QColor& color)
{
	m_color = color;
	update();
	emit colorChanged(color);
}

void ColorWidget::paintEvent(QPaintEvent* event)
{
	QPainter painter(this);
	painter.setBrush(QBrush(m_color, Qt::SolidPattern));
	painter.drawRect(rect());
}

void ColorWidget::mouseReleaseEvent(QMouseEvent* event)
{
	QColorDialog* editor = new QColorDialog(m_color, this);
	connect(editor, SIGNAL(colorSelected(QColor)),
			this, SLOT(setColor(QColor)));
	editor->exec();
}
