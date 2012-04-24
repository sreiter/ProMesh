// created by Sebastian Reiter
// s.b.reiter@googlemail.com
// 18.04.2012 (m,d,y)

#include <QFrame>
#include <QVBoxLayout>
#include "widget_container.h"


WidgetContainer::WidgetContainer(QWidget* parent) : QFrame(parent)
{
	m_layout = new QVBoxLayout(this);
	m_layout->setSpacing(0);
	m_layout->setContentsMargins(0, 0, 0, 0);
	this->setLayout(m_layout);
	this->setLineWidth(1);
	//this->setContentsMargins(1, 1, 1, 0);
	this->setFrameStyle(QFrame::Box | QFrame::Plain);
}

WidgetContainer::~WidgetContainer()
{

}

void WidgetContainer::addWidget(QWidget* widget, Qt::Alignment alignment)
{
	QFrame* frame = new QFrame(this);
	QVBoxLayout* layout = new QVBoxLayout(frame);
	frame->setLayout(layout);
	layout->setContentsMargins(10, 2, 4, 2);

	//frame->setBackgroundRole(QPalette::Shadow);
	frame->setLineWidth(1);
	frame->setFrameStyle(QFrame::Box | QFrame::Plain);
	frame->setContentsMargins(0, 0, 0, 1);

	widget->setParent(frame);
	layout->addWidget(widget);
	layout->setAlignment(widget, alignment);
	m_layout->addWidget(frame);
}
