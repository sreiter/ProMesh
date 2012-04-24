// created by Sebastian Reiter
// s.b.reiter@googlemail.com
// 18.04.2012 (m,d,y)

#ifndef __H__UG__icon_tab_widget__
#define __H__UG__icon_tab_widget__

#include <QWidget>

class QToolBar;
class QStackedWidget;
class QSignalMapper;


class IconTabWidget : public QWidget
{
	Q_OBJECT

	public:
		IconTabWidget(QWidget* parent);
		virtual ~IconTabWidget();

		void addPage(QWidget* page, const QIcon& icon, const QString& tooltip);
		int count();
		QWidget* widget(int pageIndex);

	private:
		QToolBar*		m_toolBar;
		QStackedWidget*	m_stackedWidget;
		QSignalMapper*	m_signalMapper;
};

#endif
