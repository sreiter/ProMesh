/*
 * Copyright (c) 2017:  G-CSC, Goethe University Frankfurt
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

#ifndef __H__PROMESH_script_tools
#define __H__PROMESH_script_tools

#include "../scripting.h"
#include "tool_manager.h"


class ScriptTool : public ITool
{
	public:
		ScriptTool(std::string path, std::string group, SPLuaShell luaShell);

		virtual void execute(LGObject* obj, QWidget* widget);

		virtual const char* get_name();

		virtual const char* get_tooltip();
		virtual const char* get_group();
		virtual const char* get_shortcut();

		virtual bool accepts_null_object_ptr();

		virtual QWidget* get_dialog(QWidget* parent);

		virtual bool dialog_changed(QWidget* dlg);

		virtual void refresh_dialog(QWidget* dialog);

		void parse_script_decls(bool force);

		std::string path();
		SPLuaShell lua_shell();

	private:
		ScriptDeclarations	m_scriptDecls;
		std::string m_scriptName;
		std::string m_scriptPath;
		std::string m_group;
		QByteArray	m_scriptContent;
		SPLuaShell	m_luaShell;
};


#endif	//__H__PROMESH_script_tools
