// created by Sebastian Reiter
// s.b.reiter@googlemail.com
// 18.04.2012 (m,d,y)
 
#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>

#include "icon_tab_widget.h"
#include "tool_browser_widget.h"
#include "widget_container.h"
#include "widget_list.h"
#include "extendible_widget.h"


ToolBrowserWidget::ToolBrowserWidget(QWidget* parent) : QWidget(parent)
{
	m_iconTab = new IconTabWidget(this);

	QVBoxLayout* layout = new QVBoxLayout(this);
	layout->addWidget(m_iconTab);
	layout->setSpacing(0);
	layout->setContentsMargins(0, 0, 0, 0);
	this->setLayout(layout);
}

ToolBrowserWidget::~ToolBrowserWidget()
{

}

void ToolBrowserWidget::addPage(const QIcon& icon, const QString& tooltip)
{
	WidgetList* widgetList = new WidgetList(this);
/*
	widgetList->addWidget(new QLabel(tooltip, widgetList));
	widgetList->addWidget(new QPushButton(tr("Hello"), widgetList));
	widgetList->addWidget(new QPushButton(tr("Hello You"), widgetList));

	ExtendibleWidget* extWidget = new ExtendibleWidget(widgetList);
	WidgetContainer* container = new WidgetContainer(extWidget);
	//widgetList->addWidget(container);
	container->addWidget(new QPushButton(tr("Hello Are You"), container));
	container->addWidget(new QPushButton(tr("Hello Them"), container));
	container->addWidget(new QPushButton(tr("Hello Them All Once More And Again and Again"), container));

	extWidget->setWidget(container);
	widgetList->addWidget(extWidget);
	//container->addWidget(extWidget);
*/
	m_iconTab->addPage(widgetList, icon, tooltip);
}

void ToolBrowserWidget::addWidget(int pageIndex, QWidget* widget,
								  Qt::Alignment alignment)
{
	getWidgetList(pageIndex)->addWidget(widget, alignment);
}

int ToolBrowserWidget::count()
{
	return m_iconTab->count();
}

WidgetList* ToolBrowserWidget::getWidgetList(int pageIndex)
{
	if(pageIndex >= 0 && pageIndex < m_iconTab->count()){
		return qobject_cast<WidgetList*>(m_iconTab->widget(pageIndex));
	}
	return NULL;
}
