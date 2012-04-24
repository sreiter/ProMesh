//	created by Sebastian Reiter
//	s.b.reiter@googlemail.com
//	y10 m02 d08

#ifndef APP_H
#define APP_H

#include "main_window.h"
#include "scene/lg_object.h"
#include "common/log.h"

namespace app
{

inline MainWindow* getMainWindow()
{
	static MainWindow* mainWindow = new MainWindow;
	return mainWindow;
}

inline LGObject* getActiveObject()
{
	return getMainWindow()->getActiveObject();
}

inline LGScene* getActiveScene()
{
	return getMainWindow()->get_scene();
}

inline LGObject* createEmptyLGObject(const char* name)
{
    return getMainWindow()->create_empty_object(name);
}

void PerformClickSelection(float x, float y, bool extendSelection = false);

}//	end of namespace

#endif // APP_H
