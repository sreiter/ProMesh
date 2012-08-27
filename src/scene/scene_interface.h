//	created by Sebastian Reiter
//	s.b.reiter@googlemail.com
//	y09 m09 d29

#ifndef __H__SCENE_INTERFACE__
#define __H__SCENE_INTERFACE__

#include <QObject>
#include <QColor>
#include "../view3d/renderer3d_interface.h"

////////////////////////////////////////////////////////////////////////
//	predeclarations
class ISceneObject;

////////////////////////////////////////////////////////////////////////
//	IScene
///	Abstract interface for a scene-class.
/**
 * A scene is supposed to organize objects and to draw them on using openGL
 * on a call to draw().
 * The scene specializes IRenderer3D.
 */

class IScene : public QObject, public IRenderer3D
{
	Q_OBJECT

	public:
		IScene()	{}
		virtual ~IScene()	{}

	//	derived methods from IRenderer3D
	//	...

	//	virtual methods of the scene interface
		virtual bool erase_object(int index) = 0;
		virtual bool remove_object(int index) = 0;
		virtual int num_objects() const = 0;
		virtual ISceneObject* get_scene_object(int index) = 0;
		//virtual ISceneObject* get_scene_object_by_name(const char* name) = 0;
		virtual int get_object_index(ISceneObject* pObj) = 0;

		virtual void update_visuals() = 0;
		virtual void update_visuals(int objIndex) = 0;
		virtual void update_visuals(ISceneObject* pObj) = 0;
		virtual void object_changed(int objIndex) = 0;
		virtual void object_changed(ISceneObject* pObj) = 0;

	public slots:
		virtual void visibility_changed(ISceneObject* pObj) = 0;
		virtual void color_changed(ISceneObject* pObj) = 0;

	signals:
	///	informs about new objects
	/**	has to be triggered by derivates of IScene,
	  * whenever an object has been added to the scene.*/
		void object_added(ISceneObject* pObj);

	///	informs about the removal of an object from the scene
	/**	this is also triggered whenever an object is about to be erased.*/
		void object_to_be_removed(ISceneObject* pObj);

	///	informs about a scheduled erase
		void object_to_be_erased(ISceneObject* pObj);

	///	called when an object was removed from the scene
		void object_removed();

	///	informs that the scenes visuals have been updated
		void visuals_updated();

	///	infoforms that the properties of an object were changed.
		void object_properties_changed(ISceneObject* pObj);

	//	slots:
	//	object added(pObj)
	//	object removed(pObj)
	//	object erased(pObj)
	//	object changed(pObj)
	//	visuals updated(pObj)
	//	scene updated()
};

////////////////////////////////////////////////////////////////////////
//	ISceneObject
class ISceneObject : public QObject
{
	Q_OBJECT

	public:
		virtual ~ISceneObject()	{}

	//	objects
		virtual const char* name() = 0;
		virtual void set_name(const char* name) = 0;
		virtual void set_visibility(bool visible) = 0;
		virtual bool is_visible() = 0;

		virtual QColor get_color() const = 0;
		virtual void set_color(const QColor& color) = 0;

	//	subsets
		virtual int num_subsets() = 0;
		virtual const char* get_subset_name(int index) const = 0;
		virtual bool subset_is_visible(int index) = 0;
		virtual QColor get_subset_color(int index) const = 0;

		virtual void set_subset_name(int index, const char* name) = 0;
		virtual void set_subset_visibility(int index, bool visible) = 0;
		virtual void set_subset_color(int index, const QColor& color) = 0;

	//	geometry
	///	triggers sig_geometry_changed and calls visuals_changed().
		virtual void geometry_changed();

	///	triggers sig_visuals_changed.
		virtual void visuals_changed();

	///	triggers sig_properties_changed
		virtual void properties_changed();

	///	returns the values of the i-th indicator point.
	/** an indicator point is a point with a color independent of the grid.
	 * You should only use the returned values, if the method returns true.*/
		virtual bool get_indicator_point(size_t index, float& x, float& y, float& z,
										 float& r, float& g, float& b, float& a)
			{return false;}

	///	the number of indicator points
		virtual size_t num_indicator_points()	{return 0;}

	signals:
	///	triggered when the objects geometry has changed
		void sig_geometry_changed();
		void sig_visuals_changed();

	///	informs that invisible properties like the name changed
		void sig_properties_changed();

/*
		virtual void set_draw_mode(unsigned int drawMode) = 0;
		virtual void get_draw_mode(unsigned int drawMode) = 0;

	//	subsets

		virtual void set_subset_name(const char* name) = 0;

		virtual void set_subset_color_solid(int subsetIndex,
											float a, float r,
											float g, float b) = 0;
		virtual void get_subset_color_solid(int subsetIndex,
											float& aOut, float& rOut,
											float& gOut, float& bOut) = 0;
		virtual void set_subset_color_wire(int subsetIndex,
											float a, float r,
											float g, float b) = 0;
		virtual void get_subset_color_wire(int subsetIndex,
											float& aOut, float& rOut,
											float& gOut, float& bOut) = 0;
*/
};

#endif // __H__SCENE_INTERFACE__
