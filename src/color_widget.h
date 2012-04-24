//	created by Sebastian Reiter
//	s.b.reiter@googlemail.com
//	y09 m11 d04

#ifndef COLOR_WIDGET_H
#define COLOR_WIDGET_H

#include <QWidget>
#include <QColor>

class ColorWidget : public QWidget
{
	Q_OBJECT

	public:
		ColorWidget(QWidget* parent);

	public slots:
		void setColor(const QColor& color);

	signals:
		void colorChanged(const QColor& color);

	protected:
		void paintEvent(QPaintEvent *event);
		void mouseReleaseEvent(QMouseEvent* event);

	protected:
		QColor	m_color;
};

#endif // COLOR_WIDGET_H
