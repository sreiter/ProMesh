// created by Sebastian Reiter
// s.b.reiter@googlemail.com
// 18.04.2012 (m,d,y)

#include <QVBoxLayout>
#include <QScrollArea>
#include <QPushButton>
#include <QResizeEvent>
#include <QFrame>
#include "widget_container.h"
#include "widget_list.h"


WidgetList::WidgetList(QWidget* parent) : QWidget(parent)
{
	m_scrollArea = new QScrollArea(this);
	//m_scrollArea->setBackgroundRole(QPalette::Dark);

	QVBoxLayout* layout = new QVBoxLayout(this);
	layout->addWidget(m_scrollArea);
	layout->setSpacing(0);
	layout->setContentsMargins(0, 0, 0, 0);
	this->setLayout(layout);

//	This widget will contain the widgetContainer and a spacer
	QWidget* spacerWidget = new QWidget(m_scrollArea);
	QVBoxLayout* spacerLayout = new QVBoxLayout(spacerWidget);
	spacerLayout->setSpacing(0);
	spacerLayout->setContentsMargins(0, 0, 0, 0);
	spacerWidget->setLayout(spacerLayout);

	m_widgetContainer = new WidgetContainer(spacerWidget);
	spacerLayout->addWidget(m_widgetContainer);

	spacerLayout->addStretch();

	m_scrollArea->setWidget(spacerWidget);
	m_scrollArea->setWidgetResizable(true);
}

WidgetList::~WidgetList()
{

}

void WidgetList::addWidget(QWidget* widget, Qt::Alignment alignment)
{
	m_widgetContainer->addWidget(widget, alignment);
}

