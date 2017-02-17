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

#ifndef __H__PROMESH_projector_widget
#define __H__PROMESH_projector_widget

#include <QFrame>

class QLayout;
class QVBoxLayout;
class QComboBox;
class PropertyWidget;
class ISceneObject;
class LGObject;
class LGScene;

namespace ug {
	class RefinementProjector;
}


class ProjectorWidget : public QFrame
{
	Q_OBJECT

	public:
		ProjectorWidget (QWidget* parent);
		virtual ~ProjectorWidget ();

	public slots:
		void setActiveSubset(ISceneObject* obj, int subsetIndex);
		void objectToBeRemoved(ISceneObject* pObj);

	private slots:
		// void object_added(ISceneObject* pObj);
		void projectorTypeChanged(const QString &text);
		void valueChanged();

	protected:
		virtual void changeEvent(QEvent* evt);

	private:
		void update_content(ug::RefinementProjector* proj);

		// PropertyWidget*	getPropertyWidget(const char* projectorName);

		typedef std::map<std::string, PropertyWidget*>	PropertyWidgetMap;

		QVBoxLayout* 		m_vlayout;
		QComboBox*			m_typeBox;
		QWidget*			m_curContent;
		PropertyWidgetMap	m_propertyWidgetMap; ///< cached property widgets
		LGScene*			m_scene;
		LGObject*			m_activeObject;
		int					m_activeSubsetIndex;

};

#endif	//__H__UG_projector_widget
