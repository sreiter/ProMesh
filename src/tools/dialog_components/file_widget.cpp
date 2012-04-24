// created by Sebastian Reiter
// s.b.reiter@googlemail.com
// 07.03.2012 (m,d,y)

#include <QHBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QFileDialog>
#include <iostream>
#include "file_widget.h"
#include "app.h"

FileWidget::FileWidget(FileWidgetType fwt, QString filter, QWidget* parent) :
	QWidget(parent),
	m_type(fwt),
	m_filter(filter)
{
//	create a grid layout
	QHBoxLayout* layout = new QHBoxLayout(this);

//	create a text-box and a browse button
	m_lbl = new QLabel(tr("-- no file selected --"), this);
	layout->addWidget(m_lbl);

	QPushButton* btn = new QPushButton(tr("Browse ..."), this);
	layout->addWidget(btn);
	connect(btn, SIGNAL(clicked()), this, SLOT(browse()));
}

FileWidget::~FileWidget()
{

}

const QString FileWidget::filename() const
{
	if(m_filenames.empty()){
		return QString("");
	}
	return m_filenames.front();
}

const QStringList& FileWidget::filenames() const
{
	return m_filenames;
}

void FileWidget::browse()
{
	QString path = app::getMainWindow()->settings().value("FileWidget-path", ".").toString();
	m_filenames.clear();
	switch(m_type){
		case FWT_OPEN:{
			QString fileName = QFileDialog::getOpenFileName(
								this,
								tr("Open"),
								path,
								m_filter);
			m_filenames.push_back(fileName);
		}break;

		case FWT_OPEN_SEVERAL:{
			m_filenames = QFileDialog::getOpenFileNames(
								this,
								tr("Open"),
								path,
								m_filter);
		}break;

		case FWT_SAVE:{

		}break;
	}

	QString str = filename();
	if(!str.isEmpty()){
		app::getMainWindow()->settings().setValue("FileWidget-path", QFileInfo(str).absolutePath());
		m_lbl->setText(str);
	}
}
