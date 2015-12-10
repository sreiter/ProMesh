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

#ifndef __H__UG__tool_browser_widget__
#define __H__UG__tool_browser_widget__

#include <map>
#include <string>
#include <QWidget>
#include "tools/tool_manager.h"

class ExtendibleWidget;
class IconTabWidget;
class QSignalMapper;
class QVBoxLayout;
class WidgetContainer;
class WidgetList;

class ToolBrowser : public QWidget
{
	Q_OBJECT

	public:
		ToolBrowser(QWidget* parent);
		virtual ~ToolBrowser();

		void refresh(ToolManager* toolMgr);

	private slots:
		void executeTool(int toolID);

	protected:
		WidgetContainer* group_container(const std::string& groupName);
		size_t tool_index(const std::string& toolName);
		void deleteEmptyChildGroups(QWidget* w);

	private:
		struct ToolEntry{
			ToolEntry() : parentContainer(NULL), widget(NULL),
						  extendibleWidget(NULL), tool(NULL), revision(0) {}
			WidgetContainer*	parentContainer;
			QWidget*			widget;
			ExtendibleWidget*	extendibleWidget;///< only used if the widget is embedded in an extendibleWidget
			ITool*				tool;
			size_t				revision;
		};

		ToolManager*					m_toolMgr;
		IconTabWidget*					m_iconTab;
		QVBoxLayout*					m_layout;
		QSignalMapper*					m_signalMapper;
		std::map<std::string, WidgetContainer*>	m_groupContainers;
		std::map<std::string, ToolEntry>	m_toolMap;///< key: groupName + toolName
		size_t								m_revision;///<	the revision is used to identify unused tool-entries
};

#endif
