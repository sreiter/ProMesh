// created by Sebastian Reiter
// s.b.reiter@googlemail.com
// 18.04.2012 (m,d,y)

#ifndef __H__UG__widget_container__
#define __H__UG__widget_container__

#include <QFrame>

class QLayout;
class QFrame;


class WidgetContainer : public QFrame
{
	Q_OBJECT

	public:
		WidgetContainer(QWidget* parent);
		virtual ~WidgetContainer();

		void addWidget(QWidget* widget, Qt::Alignment alignment = Qt::AlignLeft);

	private:
		QLayout*		m_layout;
};

#endif
