/*
 * Copyright (c) 2016:  G-CSC, Goethe University Frankfurt
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

#ifndef __H__PROMESH_property_widget_impl
#define __H__PROMESH_property_widget_impl

#include <QVBoxLayout>
#include "tooldlg_oarchive.h"
#include "tooldlg_iarchive.h"
#include "common/boost_serialization_routines.h"

template <typename T>
void PropertyWidget::
populate (const T& t, const char* name)
{
	tooldlg_oarchive oa(m_spacerWidget);
	oa << ug::make_nvp(name, t);
	QWidget* newContent = oa.widget();

	if(m_content) {
		m_spacerLayout->replaceWidget(m_content, newContent);
	}
	else{
		m_spacerLayout->addWidget(newContent);
		m_spacerLayout->addStretch(1);
	}
	m_content = newContent;
}

template <typename T>
void PropertyWidget::
populate (const T* t, const char* name)
{
	populate(*t, name);
}

template <typename T>
void PropertyWidget::
retrieve_values (T& t)
{
	tooldlg_iarchive ia(m_content);
	ia >> t;
}

#endif	//__H__UG_property_widget_impl
