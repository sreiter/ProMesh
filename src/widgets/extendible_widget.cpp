// created by Sebastian Reiter
// s.b.reiter@googlemail.com
// 19.04.2012 (m,d,y)
 
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QToolButton>
#include <QLabel>
#include "extendible_widget.h"


ExtendibleWidget::ExtendibleWidget(QWidget* parent) :
	QWidget(parent),
	m_widget(NULL)
{
	m_vLayout = new QVBoxLayout(this);
	QHBoxLayout* hLayout = new QHBoxLayout();

	m_vLayout->setSpacing(0);
	m_vLayout->setContentsMargins(0, 0, 0, 0);
	hLayout->setSpacing(10);
	hLayout->setContentsMargins(0, 0, 0, 0);

	this->setLayout(m_vLayout);
	m_vLayout->addLayout(hLayout);

	m_toolButton = new QToolButton(this);
	m_toolButton->setCheckable(true);
	m_toolButton->setArrowType(Qt::RightArrow);
	hLayout->addWidget(m_toolButton);

	ExtendibleWidgetHeader* header = new ExtendibleWidgetHeader(this);
	connect(header, SIGNAL(double_clicked()), this, SLOT(toggle()));
	m_header = header;
	hLayout->addWidget(m_header);

	connect(m_toolButton, SIGNAL(toggled(bool)), this, SLOT(setChecked(bool)));
}

ExtendibleWidget::~ExtendibleWidget()
{

}


void ExtendibleWidget::setText(const QString& text)
{
	m_header->setText(text);
}

void ExtendibleWidget::setInfoText(const QString& toolTip)
{
	m_header->setToolTip(toolTip);
	this->setToolTip(toolTip);
}

void ExtendibleWidget::setWidget(QWidget* widget)
{
	if(m_widget){
		m_vLayout->removeWidget(m_widget);
	}
	m_widget = widget;
	m_vLayout->addWidget(m_widget);
	if(m_toolButton->isChecked())
		m_widget->show();
	else
		m_widget->hide();
}

void ExtendibleWidget::setChecked(bool checked)
{
	if(m_toolButton->isChecked() != checked){
		m_toolButton->setChecked(checked);
		return;
	}

	if(checked){
		m_toolButton->setArrowType(Qt::DownArrow);
		if(m_widget)
			m_widget->show();
	}
	else{
		m_toolButton->setArrowType(Qt::RightArrow);
		if(m_widget)
			m_widget->hide();
	}
}

void ExtendibleWidget::toggle()
{
	setChecked(!m_toolButton->isChecked());
}
