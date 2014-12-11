//	created by Sebastian Reiter
//	s.b.reiter@googlemail.com
//	y10 m05 d04

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

//	camera
	RegisterCameraTools(toolMgr);

//	grid generation
	RegisterGridGenerationTools(toolMgr);

//	coordinate transform
	RegisterCoordinateTransformTools(toolMgr);

//	selection
	RegisterSelectionTools(toolMgr);

//	selection | marks
	RegisterMarkTools(toolMgr);

//	subsets
	RegisterSubsetTools(toolMgr);

//	remeshing | refinement
	RegisterRefinementTools(toolMgr);

//	remeshing | topology
	RegisterTopologyTools(toolMgr);

//	remeshing | optimization
	RegisterRemeshingTools(toolMgr);

//	remeshing | layers
	RegisterFracToLayerTools(toolMgr);

//	info
	RegisterInfoTools(toolMgr);

//	registry
	RegsiterRegistryTools(toolMgr);

//	scripts
	RegisterScriptTools(toolMgr);
}
