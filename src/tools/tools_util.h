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

#ifndef TOOLS_UTIL_H
#define TOOLS_UTIL_H

#include <QStringList>
#include "widgets/coordinates_widget.h"

class ToolWidget;

enum ToolDialogUtilElements
{
	TDUE_NONE = 0,
	TDUE_VERTEX = 1,
	TDUE_EDGE = 1 << 1,
	TDUE_FACE = 1 << 2,
	TDUE_VOLUME = 1 << 3,
	TDUE_ALL = TDUE_VERTEX | TDUE_EDGE | TDUE_FACE | TDUE_VOLUME
};

inline void ToolDialog_AddElementChooser(ToolWidget* dlg,
									const char* caption, size_t defInd)
{
	QStringList entries;
	entries.push_back("vertices");
	entries.push_back("edges");
	entries.push_back("faces");
	entries.push_back("volumes");
	if(defInd > 3)
		defInd = 3;
	dlg->addComboBox(caption, entries, (int)defInd);
}

/**
 * Pass an or combination of element enumerated in
 * ToolDialogUtilElements to enabledElems.*/
inline void ToolDialog_AddElementCheckBoxes(ToolWidget* dlg,
									 unsigned int enabledElems)
{
	dlg->addCheckBox("vertices", enabledElems & TDUE_VERTEX);
	dlg->addCheckBox("edges", enabledElems & TDUE_EDGE);
	dlg->addCheckBox("faces", enabledElems & TDUE_FACE);
	dlg->addCheckBox("volumes", enabledElems & TDUE_VOLUME);
}

#endif // TOOLS_UTIL_H
