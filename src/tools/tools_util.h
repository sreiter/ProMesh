#ifndef TOOLS_UTIL_H
#define TOOLS_UTIL_H

#include <QStringList>
#include "tool_coordinates.h"

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
	dlg->addComboBox(caption, entries, defInd);
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
