// created by Sebastian Reiter
// s.b.reiter@googlemail.com
// 18.04.2012 (m,d,y)

#ifndef __H__UG__widget_list__
#define __H__UG__widget_list__

#include <QWidget>

class QLayout;
class QScrollArea;
class WidgetContainer;


class WidgetList : public QWidget
{
	Q_OBJECT

	public:
		WidgetList(QWidget* parent);
		virtual ~WidgetList();

	///	adds a widget to the internal widget container
		void addWidget(QWidget* widget, Qt::Alignment alignment = Qt::AlignLeft);

	///	returns the internal widget container
		WidgetContainer* widgetContainer()	{return m_widgetContainer;}

	private:
		QScrollArea*		m_scrollArea;
		WidgetContainer*	m_widgetContainer;
};

#endif
