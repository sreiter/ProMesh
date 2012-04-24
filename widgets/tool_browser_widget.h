// created by Sebastian Reiter
// s.b.reiter@googlemail.com
// 18.04.2012 (m,d,y)

#ifndef __H__UG__tool_browser_widget__
#define __H__UG__tool_browser_widget__

#include <QWidget>

class IconTabWidget;
class WidgetList;

class ToolBrowserWidget : public QWidget
{
	Q_OBJECT

	public:
		ToolBrowserWidget(QWidget* parent);
		virtual ~ToolBrowserWidget();

		void addPage(const QIcon& icon, const QString& tooltip);

		void addWidget(int pageIndex, QWidget* widget,
					   Qt::Alignment alignment = Qt::AlignLeft);

		int count();

	protected:
		WidgetList* getWidgetList(int pageIndex);

	private:
		IconTabWidget*	m_iconTab;

};

#endif
