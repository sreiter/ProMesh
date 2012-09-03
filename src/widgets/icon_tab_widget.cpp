// created by Sebastian Reiter
// s.b.reiter@googlemail.com
// 18.04.2012 (m,d,y)
 
#include "icon_tab_widget.h"
#include <QVBoxLayout>
#include <QToolBar>
#include <QToolButton>
#include <QStackedWidget>
#include <QSignalMapper>


IconTabWidget::IconTabWidget(QWidget* parent) : QWidget(parent)
{
	m_toolBar = new QToolBar(this);
	m_toolBar->setIconSize(QSize(24, 24));

	m_stackedWidget = new QStackedWidget(this);

	QVBoxLayout* layout = new QVBoxLayout();
	layout->addWidget(m_toolBar);
	layout->addWidget(m_stackedWidget);
	layout->setSpacing(0);
	layout->setContentsMargins(0, 0, 0, 0);
	this->setLayout(layout);

//	the signal mapper is used to connect the tool-buttons with the stacked widgets
	m_signalMapper = new QSignalMapper(this);
	connect(m_signalMapper, SIGNAL(mapped(int)), m_stackedWidget, SLOT(setCurrentIndex(int)));
}

IconTabWidget::~IconTabWidget()
{

}

void IconTabWidget::addPage(QWidget* page, const QIcon& icon, const QString& tooltip)
{
	QToolButton* toolBtn = new QToolButton(m_toolBar);
	toolBtn->setIcon(icon);
	toolBtn->setCheckable(true);
	toolBtn->setAutoExclusive(true);
	toolBtn->setToolTip(tooltip);

	m_signalMapper->setMapping(toolBtn, m_stackedWidget->count());
	connect(toolBtn, SIGNAL(clicked()), m_signalMapper, SLOT(map()));

	m_toolBar->addWidget(toolBtn);
	m_stackedWidget->addWidget(page);

//	if this is the first page, we'll select it
	if(m_stackedWidget->count() == 1){
		toolBtn->setChecked(true);
		m_stackedWidget->setCurrentIndex(0);
	}
}

int IconTabWidget::count()
{
	return m_stackedWidget->count();
}

QWidget* IconTabWidget::widget(int pageIndex)
{
	return m_stackedWidget->widget(pageIndex);
}
