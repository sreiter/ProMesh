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

#include <vector>
#include <map>
#include <QString>
#include <QObject>
#include <QAction>
#include <QSignalMapper>
#include <QIcon>
#include "../scene/lg_object.h"
#include "tool_dialog.h"

#ifndef TOOL_MANAGER_H
#define TOOL_MANAGER_H

class QWidget;

enum ShortcutModifierKeys{
	SMK_NONE =	0,
	SMK_CTRL =	1,
	SMK_SHIFT =	1<<1,
	SMK_ALT =	1<<2
};

///	The base class of a tool
class ITool : public QObject
{
	Q_OBJECT

	public:
		virtual ~ITool()	{};

		virtual void execute(LGObject* obj,
							 QWidget* widget) = 0;

		virtual const char* get_name() = 0;

		virtual const char* get_tooltip()	{return "";}
		virtual const char* get_group()		{return "";}
		virtual const char* get_shortcut()	{return "";}

		virtual bool accepts_null_object_ptr()	{return false;}

		virtual QWidget* get_dialog(QWidget* parent)	{return NULL;}
	/**	Only dialogs which were created with the tools get_dialog method should
	 * be passed into this method.*/
		virtual bool dialog_changed(QWidget* dlg)		{return false;}
	///	refresh the given dialog.
	/**	Only dialogs which were created with the tools get_dialog method should
	 * be passed into this method.*/
		virtual void refresh_dialog(QWidget* dialog)	{}
};

///	launches tools and populates menus with associated actions.
/**
 * The tool-manager deletes all registered tools in its destructor.
 */
class ToolManager : public QObject
{
	Q_OBJECT

	public:
		typedef std::map<std::string, QIcon> GroupIconMap;

		ToolManager(QWidget* parent);
	///	calls delete on all registered tools.
		~ToolManager();

	///	registers a tool at the ToolManager.
	/**	Please note, that only tools which are registered before
	 *	a call to populateMenu will appear in the menu.
	 *	Only pass tools that have been created using new.
	 *	Memory managment of registered tools will be performed by
	 *	the ToolManager.
	 *
	 *	Pass a key constant enumerated in Qt::Key to the key parameter
	 *	Pass an or-combination of constants enumerated in ShortcutModifierKeys
	 *	to shortcutModifierKeys
	 *	*/
		void register_tool(ITool* tool, int key = 0,
						   uint shortcutModifierKeys = SMK_NONE);

		void remove_tool(ITool* tool);

		size_t num_tools() const			{return m_registeredTools.size();}
		
		ITool* tool(size_t index) const		{return m_registeredTools[index].m_tool;}


	///	specify the resource- or file-name for the group's icon
	/**	Note that icons are currently only loaded for top-level groups.
	 * Also note, that the icon has to be specified before a tool browser
	 * is created.
	 * \note: The group will be added automatically to the list of known groups
	 * \sa add_known_group.*/
		void set_group_icon(const std::string& grpName, const char* iconName);
		QIcon group_icon(const std::string& grpName) const;

	///	executes the specified shortcut
	/**	The key contains a lowercase character. Pass an or combination of
	 * constants enumerated in ShortcutModifierKeys as modifiers.*/
		void execute_shortcut(int key, uint modifiers);


	///	adds a group to the list of known groups.
	/**	Only top-level groups should be added to this list. The order in which
	 * groups are made known is the order in which they should be displayed later on.
	 * \sa set_group_icon*/
		void add_known_group(const std::string& grpName);
		size_t num_known_groups () const 				{return m_knownGroups.size();}
		const std::string& known_group (size_t i) const	{return m_knownGroups.at(i);}

	public slots:
		void launchTool(int toolID);

	private:
		QWidget*	m_parentWidget;
		
		struct ToolEntry{
			ToolEntry(ITool* tool, int key, uint modifiers) :
				m_tool(tool), m_shortcutKey(key),
				m_shortcutModifiers(modifiers) {}
			ITool*	m_tool;
			int		m_shortcutKey;
			uint	m_shortcutModifiers;
		};

		std::vector<ToolEntry>		m_registeredTools;
		std::vector<std::string>	m_knownGroups;
		GroupIconMap	m_groupIconMap;
		QIcon			m_defaultIcon;
};

#endif // TOOL_MANAGER_H
