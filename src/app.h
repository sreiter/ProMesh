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

#ifndef APP_H
#define APP_H

#include <QDir>
#include "main_window.h"
#include "scene/lg_object.h"
#include "common/log.h"


namespace app
{

// class CameraDesc{
// 	public:
// 		static CameraDesc&
// 		inst()	{static CameraDesc desc;
// 			 	 return desc;}

// 		ug::vector3	viewScale;

// 	private:
// 		CameraDesc() :
// 			viewScale(1, 1, 1)
// 			{}
// };



// inline CameraDesc& getCameraDesc()
// {
// 	return CameraDesc::inst();
// }

inline MainWindow* getMainWindow()
{
	static MainWindow* mainWindow = new MainWindow;
	return mainWindow;
}

inline LGObject* getActiveObject()
{
	return getMainWindow()->getActiveObject();
}

inline int getActiveSubsetIndex()
{
	return getMainWindow()->getSceneInspector()->getActiveSubsetIndex();
}

inline LGScene* getActiveScene()
{
	return getMainWindow()->get_scene();
}

inline LGObject* createEmptyObject(const char* name, SceneObjectType sot)
{
    return getMainWindow()->create_empty_object(name, sot);
}


/// returns the path in which the application resides
QDir AppDir();

///	returns the path in which user-data is placed (e.g. $HOME/.promesh)
QDir UserDataDir();

///	returns the path in which user-scripts are placed (e.g. $HOME/.promesh/scripts)
QDir UserScriptDir();

///	returns the user specified paths in which custom user scripts are placed
std::vector<QDir> CustomUserScriptDirs();

///	returns the path in which temporary data may be placed (e.g. $HOME/.promesh/tmp)
QDir UserTmpDir();

///	returns the path in which the help may be placed (e.g. $HOME/.promesh/help)
QDir UserHelpDir();

/// returns the system-temporary path in which ProMesh temporary files may be placed
QDir ProMeshTmpDir();

/// returns a unique temporary file name placed in ProMeshTmpDir
QString TmpFileName(const QString& prefix, const QString& suffix);

/// returns a unique temporary file name placed in the given directory
QString TmpFileName(const QDir& dir, const QString& prefix, const QString& suffix);

void PerformClickSelection(float x, float y, bool extendSelection = false);

///	returns the version of promesh as a string
QString GetVersionString();
}//	end of namespace

#endif // APP_H
