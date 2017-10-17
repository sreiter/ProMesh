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
#include "app.h"
#include "standard_tools.h"

using namespace std;
using namespace ug;

/*
//TMP: test a lua-call
	lua_State* L = script::GetLuaState();

	luabind::call_function<void>(L, "mark_crease_elements", &pObj->grid(),
							&pObj->crease_handler(), 50.f);

	lua_getglobal(L, "mark_crease_elements");
	lua_pushlightuserdata(L, &pObj->grid());
	lua_pushlightuserdata(L, &pObj->crease_handler());
	lua_pushnumber(L, 50);
	if(lua_pcall(L, 3, 0, 0) != 0){
		cout << "ERROR: calling of lua function failed: " << lua_tostring(L, -1) << endl;
		lua_pop(L, 1);
	}
*/




void RegisterStandardTools(ToolManager* toolMgr)
{

	toolMgr->set_group_icon("Camera", ":images/tool_camera.png");
	toolMgr->set_group_icon("Grid Generation", ":images/tool_geometry_generation.png");
	toolMgr->set_group_icon("Coordinate Transform", ":images/tool_transform.png");
	toolMgr->set_group_icon("Selection", ":images/tool_selection.png");
	toolMgr->set_group_icon("Subsets", ":images/tool_subsets.png");
	toolMgr->set_group_icon("Remeshing", ":images/tool_remeshing.png");
	toolMgr->set_group_icon("Info", ":images/tool_info.png");
	toolMgr->set_group_icon("Scripts", ":images/tool_scripts.png");

//	camera
	RegisterCameraTools(toolMgr);
	PreRegisterGridGenerationTools(toolMgr);
	RegisterInfoTools(toolMgr);

	RegisterTetgenTools();
	RegisterRegistryTools(toolMgr);
	
	RegisterCoordinateTransformTools(toolMgr);
	RegisterFracToLayerTools(toolMgr);
	RegisterScriptTools(toolMgr);

// //	remeshing | topology
// 	RegisterTopologyTools(toolMgr);

	PostRegisterGridGenerationTools(toolMgr);
}
