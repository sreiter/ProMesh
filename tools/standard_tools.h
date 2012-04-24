//	created by Sebastian Reiter
//	s.b.reiter@googlemail.com
//	y10 m05 d04

#ifndef STANDARD_TOOLS_H
#define STANDARD_TOOLS_H

#include "tool_manager.h"

void RegisterStandardTools(ToolManager* toolMgr);

void RegisterCameraTools(ToolManager* toolMgr);
void RegisterObjectTools(ToolManager* toolMgr);
void RegisterSelectionTools(ToolManager* toolMgr);
void RegisterMarkTools(ToolManager* toolMgr);
void RegisterRemeshingTools(ToolManager* toolMgr);
void RegisterGridGenerationTools(ToolManager* toolMgr);
void RegisterCoordinateTransformTools(ToolManager* toolMgr);
void RegisterInfoTools(ToolManager* toolMgr);
void RegisterTopologyTools(ToolManager* toolMgr);
void RegisterSubsetTools(ToolManager* toolMgr);
void RegisterFracToLayerTools(ToolManager* toolMgr);
void RegisterRefinementTools(ToolManager* toolMgr);

#endif // STANDARD_TOOLS_H
