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

#include <QtWidgets>
#include <vector>
#include <string>
#include "app.h"
#include "tool_manager.h"
#include "common/util/string_util.h"
#include "widgets/tool_browser_widget.h"
#include "widgets/widget_container.h"
#include "widgets/extendible_widget.h"

using namespace std;
using namespace ug;

enum ToolTypes{
	TT_TOOL = 0,
	TT_FUNCTION = 1
};

enum UserRolls{
	UR_INDEX = Qt::UserRole,
	UR_TYPE
};

ToolManager::ToolManager(QWidget* parent) :
	QObject(parent)
{
	m_parentWidget = parent;
}

ToolManager::~ToolManager()
{
	for(size_t i = 0; i < m_registeredTools.size(); ++i)
		delete m_registeredTools[i].m_tool;
}

void ToolManager::register_tool(ITool* tool, int shortcutKey,
								uint shortcutModifierKeys)
{
	m_registeredTools.push_back(ToolEntry(tool, shortcutKey, shortcutModifierKeys));
}

void ToolManager::remove_tool(ITool* tool)
{
	for(size_t i = 0; i < m_registeredTools.size(); ++i){
		if(m_registeredTools[i].m_tool == tool){
			m_registeredTools.erase(m_registeredTools.begin() + i);
		}
	}
}

void ToolManager::set_group_icon(const std::string& grpName, const char* iconName)
{
	m_groupIconMap[grpName] = QIcon(iconName);
	add_known_group(grpName);
}

QIcon ToolManager::group_icon(const std::string& grpName) const
{
	GroupIconMap::const_iterator i = m_groupIconMap.find(grpName);
	if(i != m_groupIconMap.end()){
		return i->second;
	}
	return m_defaultIcon;
}

void ToolManager::add_known_group(const std::string& grpName)
{
	for(size_t i = 0; i < m_knownGroups.size(); ++i){
		if(m_knownGroups[i] == grpName)
			return;
	}
	m_knownGroups.push_back(grpName);
}

void ToolManager::launchTool(int toolID)
{
	PROFILE_FUNC();
	if(toolID >= 0 && toolID < (int)m_registeredTools.size()){
		ITool* tool = m_registeredTools[toolID].m_tool;

		QWidget* widget = tool->get_dialog(m_parentWidget);

		if(widget){
			QDialog* dlg = new QDialog(m_parentWidget);
			dlg->setWindowTitle(tool->get_name());
			QVBoxLayout* layout = new QVBoxLayout(dlg);
			dlg->setLayout(layout);
			layout->addWidget(widget);
			dlg->show();
		}
		else{
			LGObject* obj = app::getActiveObject();
			if(!obj){
			//todo: create the appropriate object for the current module
				obj = app::createEmptyObject("new mesh", SOT_LG);
			}

			if(obj || tool->accepts_null_object_ptr()){
				try{
					obj->create_undo_point_if_selection_changed();
					tool->execute(obj, NULL);
				}
				catch(UGError& err){
					UG_LOG("Execution of tool " << tool->get_name()
						   << " failed with the following message:\n");
					for(size_t i = 0; i < err.num_msg(); ++i)
						UG_LOG(" " << err.get_file(i) << ": " << err.get_line(i)
							   << ": " << err.get_msg(i) << "\n");
				}
			}
		}
	}
}

void ToolManager::execute_shortcut(int key, uint modifiers)
{
//	iterate through all tools and check, whether the shortcut matches
	if(!key)
		return;

	for(size_t i = 0; i < m_registeredTools.size(); ++i){
		if((m_registeredTools[i].m_shortcutKey == key)
		   && (m_registeredTools[i].m_shortcutModifiers == modifiers))
		{
			launchTool((int)i);
			break;
		}
	}
}
