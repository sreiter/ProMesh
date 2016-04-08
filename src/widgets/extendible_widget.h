/*
 * Copyright (c) 2008-2015:  G-CSC, Goethe University Frankfurt
 * Copyright (c) 2006-2008:  Steinbeis Forschungszentrum (STZ Ölbronn)
 * Copyright (c) 2006-2015:  Sebastian Reiter
 * Author: Sebastian Reiter
 *
 * This file is part of ProMesh.
 * 
 * ProMesh is free software: you can redistribute it and/or modify it under the
 * terms of the GNU Lesser General Public License version 3 (as published by the
 * Free Software Foundation) with the following additional attribution
 * requirements (according to LGPL/GPL v3 §7):
 * 
 * (1) The following notice must be displayed in the Appropriate Legal Notices
 * of covered and combined works: "Based on ProMesh (www.promesh3d.com)".
 * 
 * (2) The following bibliography is recommended for citation and must be
 * preserved in all covered files:
 * "Reiter, S. and Wittum, G. ProMesh -- a flexible interactive meshing software
 *   for unstructured hybrid grids in 1, 2, and 3 dimensions. In preparation."
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU Lesser General Public License for more details.
 */

#ifndef __H__UG__extendible_widget__
#define __H__UG__extendible_widget__

#include <QWidget>
#include <QLabel>

class QToolButton;
class QVBoxLayout;
class ExtendibleWidgetHeader;


class ExtendibleWidget : public QFrame
{
	Q_OBJECT

	public:
		ExtendibleWidget(QWidget* parent);
		virtual ~ExtendibleWidget();

		void setText(const QString& text);
		void setWidget(QWidget* widget);
		void setInfoText(const QString& text);

		QWidget* widget()	{return m_widget;}
		
	public slots:
		void setChecked(bool checked);
		void toggle();

	private:
		QVBoxLayout*			m_vLayout;
		QToolButton*			m_toolButton;
		QLabel*					m_header;
		QWidget*				m_widget;
};



class ExtendibleWidgetHeader : public QLabel
{
	Q_OBJECT

	public:
		ExtendibleWidgetHeader(QWidget* parent) : QLabel(parent)	{}
		virtual ~ExtendibleWidgetHeader()	{}

	signals:
		void clicked();
		void double_clicked();

	protected:
		virtual void mouseReleaseEvent(QMouseEvent* evt)
		{
			QLabel::mouseReleaseEvent(evt);
			emit(clicked());
		}

		virtual void mouseDoubleClickEvent(QMouseEvent* evt)
		{
			QLabel::mouseDoubleClickEvent(evt);
			emit(double_clicked());
		}
};


#endif
