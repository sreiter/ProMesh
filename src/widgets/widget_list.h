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
