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

#ifndef CLIP_PLANE_WIDGET_H
#define CLIP_PLANE_WIDGET_H

#include <QWidget>

////////////////////////////////////////////////////////////////////////
//	predeclarations
class LGScene;
class QSlider;
class QCheckBox;

////////////////////////////////////////////////////////////////////////
//	ClipPlaneWidget
///	controls clip-planes
class ClipPlaneWidget : public QWidget
{
	Q_OBJECT

	public:
		ClipPlaneWidget(QWidget* parent);
		virtual ~ClipPlaneWidget();

		void setScene(LGScene* scene);

	protected slots:
		void updateClipPlanes();
		void valueChanged(int newValue);
		void stateChanged(int newState);

	protected:
		void setClipPlane(int index, float ia);///<	does not call update_scene()

	protected:
		LGScene*	m_scene;
		QSlider*	m_slider[3];
		QCheckBox*	m_checkBox[3];
};



#endif // CLIP_PLANE_WIDGET_H
