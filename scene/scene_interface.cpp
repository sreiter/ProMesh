//	created by Sebastian Reiter
//	s.b.reiter@googlemail.com
//	y10 m02 d08

#include "scene_interface.h"

void ISceneObject::geometry_changed()
{
	emit sig_geometry_changed();
	visuals_changed();
}

void ISceneObject::visuals_changed()
{
	emit sig_visuals_changed();
}