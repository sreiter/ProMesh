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
 * GNU Lesser General Publi^c License for more details.
 */

#ifndef __H__PROMESH_scripting
#define __H__PROMESH_scripting

#include <string>
#include <vector>
#include "common/util/variant.h"
#include "../../plugins/LuaShell/lua_shell.h"

typedef SmartPtr<ug::luashell::LuaShell> SPLuaShell;

SPLuaShell GetDefaultLuaShell ();

void SetScriptDefaultVariables (SPLuaShell luaShell, const char* scriptContent);


struct ScriptParameter{
	std::string varName;
	std::string argName;
	std::string typeName;
	std::string options;

	ug::Variant	val;
	ug::Variant min;
	ug::Variant max;
	ug::Variant step;
	ug::Variant digits;
};

struct ScriptDeclarations{
	std::string					name;
	std::vector<ScriptParameter>	inputs;
};


void ParseScriptDeclarations (ScriptDeclarations& declsOut,
                              const char* scriptContent);


namespace ug{ namespace promesh{ class Mesh; }}

void ExecuteScript (const char* scriptContent, ug::promesh::Mesh* mesh);
void ExecuteScriptFromFile (const char* filename, ug::promesh::Mesh* mesh);


#endif	//__H__PROMESH_scripting
