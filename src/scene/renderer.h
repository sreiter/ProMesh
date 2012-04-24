// created by Sebastian Reiter
// s.b.reiter@googlemail.com
// y09 m07 d17

////////////////////////////////////////////////////////////////////////
//	The Renderer-Singleton defined in this file contains
//	several algorithms, that create a display-list from a grid.
////////////////////////////////////////////////////////////////////////

#ifndef __H__PM__RENDER_METHODS__
#define __H__PM__RENDER_METHODS__

#include "gl.h"
#include "glu.h"
#include "lib_grid/lib_grid.h"
#include "plane_sphere.h"

////////////////////////////////////////////////////////////////////////
//	Renderer
///	renders geometry with the specified settings.
/**
 * The renderer is a singleton class. You can specify options at any
 * time. During rendering the currently active options are used.
 * You may for example set a clip-plane, etc.
 * The renderer features different methods for rendering.
 * If you want to implement a new redering-style, you should do this
 * in this class.
 */
class Renderer
{
	public:
		static Renderer& inst()
		{
			static Renderer renderer;
			return renderer;
		}

		void enable_clip_plane(bool bEnable);
		void set_clip_plane(const lgmath::vector3& p, const lgmath::vector3& n);
		inline void set_clip_plane(const lgmath::Plane& plane)	{m_clipPlane = plane;}
		inline lgmath::Plane get_clip_plane()	{return m_clipPlane;}
		inline bool clip_plane_is_enabled()		{return m_bClipPlaneEnabled;}

	/// Renders the geometry by assigning different colors to each subset.
		void standard_render(GLuint displayList, libGrid::Grid& grid,
							libGrid::SubsetHandler& sh,
							libGrid::APosition& aPos, libGrid::ANormal& aNorm,
							libGrid::ASphere& aBoundingSphere,
							bool bWire = false, int colorBaseIndex = 0);
		void standard_render(GLuint displayList, libGrid::Grid& grid,
							libGrid::SubsetHandler& sh,
							libGrid::APosition& aPos, libGrid::ANormal& aNorm,
							libGrid::ASphere& aBoundingSphere,
							bool bWire = false, int colorBaseIndex = 0);

	protected:
		Renderer();

	protected:
	//	clip planes
		lgmath::Plane			m_clipPlane;
		bool					m_bClipPlaneEnabled;
};

////////////////////////////////////////////////////////////////////////
///	You may use this method to retrieve the renderer
inline Renderer& GetRenderer()	{return Renderer::inst();}


#endif
