//	created by Sebastian Reiter
//	s.b.reiter@googlemail.com
//	y09 m10 d01

#ifndef __H__LG_SCENE__
#define __H__LG_SCENE__

#include <string>
#include "lg_include.h"
#include "lg_object.h"
#include "../view3d/renderer3d_interface.h"
#include "scene_template.h"

//TODO:	remove this restriction
const int MAX_NUM_CLIP_PLANES = 3;

class LGScene : public TScene<LGObject>
{
	Q_OBJECT

	typedef TScene<LGObject> BaseClass;

	public:
		LGScene();
		virtual ~LGScene()	{}

	///	adds obj to the scene and updates its visuals.
		virtual int add_object(LGObject* obj, bool autoDelete = true);

	///	updates all visuals of the scene
		virtual void update_visuals();

	///	updates the visuals of the object at the given index.
		virtual void update_visuals(int objIndex);

	///	updates the visuals of pObj.
		virtual void update_visuals(LGObject* pObj);

		ug::VertexBase* get_clicked_vertex(LGObject* pObj,
									const ug::vector3& from,
									const ug::vector3& to);

		ug::EdgeBase* get_clicked_edge(LGObject* pObj,
									const ug::vector3& from,
									const ug::vector3& to,
									bool closestToTo = false);

		ug::Face* get_clicked_face(LGObject* pObj,
									const ug::vector3& from,
									const ug::vector3& to);

		ug::Volume* get_clicked_volume(LGObject* pObj,
										const ug::vector3& from,
										const ug::vector3& to);

	/**	given a rect in screen coordinates, this methods finds all
	 *	vertices which lie in that rect and writes them to vrtsOut.
	 * \return number of vertices in the rect.*/
		size_t get_vertices_in_rect(std::vector<ug::VertexBase*>& vrtsOut,
									LGObject* obj,
									float xMin, float yMin, float xMax, float yMax);

	/**	given a rect in screen coordinates, this methods finds all
	 *	edges which lie completly in that rect and writes them to edgesOut.
	 * \return number of edges in the rect.*/
		size_t get_edges_in_rect(std::vector<ug::EdgeBase*>& edgesOut,
								 LGObject* obj,
								 float xMin, float yMin, float xMax, float yMax);

	/**	given a rect in screen coordinates, this methods finds all
	 *	faces which lie completly in that rect and writes them to facesOut.
	 * \return number of faces in the rect.*/
		size_t get_faces_in_rect(std::vector<ug::Face*>& facesOut,
							    LGObject* obj,
							    float xMin, float yMin, float xMax, float yMax);

	/**	This algorithm uses Grid::mark.
	 *
	 *  given a rect in screen coordinates, this methods finds all
	 *	volumes which lie completly in that rect and writes them to volsOut.
	 * \return number of volumes in the rect.*/
		size_t get_volumes_in_rect(std::vector<ug::Volume*>& volsOut,
								   LGObject* obj,
								   float xMin, float yMin, float xMax, float yMax);

	/**	given a rect in screen coordinates, this methods finds all
	 *	edges which intersect that rect and writes them to edgesOut.
	 * \return number of edges in the rect.*/
		size_t get_edges_in_rect_cut(std::vector<ug::EdgeBase*>& edgesOut,
								 LGObject* obj,
								 float xMin, float yMin, float xMax, float yMax);

	/**	given a rect in screen coordinates, this methods finds all
	 *	faces which intersect that rect and writes them to facesOut.
	 * \return number of faces in the rect.*/
		size_t get_faces_in_rect_cut(std::vector<ug::Face*>& facesOut,
							    LGObject* obj,
							    float xMin, float yMin, float xMax, float yMax);

	/**	This algorithm uses Grid::mark.
	 *
	 *  given a rect in screen coordinates, this methods finds all
	 *	volumes which intersect that rect and writes them to volsOut.
	 * \return number of volumes in the rect.*/
		size_t get_volumes_in_rect_cut(std::vector<ug::Volume*>& volsOut,
								   LGObject* obj,
								   float xMin, float yMin, float xMax, float yMax);

	//	derived from IRenderer3D
	///	this method is called when the renderer shall draw its content
		virtual void draw();

	///	use this method to set the front draw mode of the renderer
		virtual void set_draw_mode_front(unsigned int drawMode);

	///	use this method to set the back draw mode of the renderer
		virtual void set_draw_mode_back(unsigned int drawMode);

	///	the camara transform. mat is an array of 16 floats.
		virtual void set_transform(float* mat);

	///	the perspective transform
		virtual void set_perspective(float fovy, int viewWidth, int viewHeight,
									float zNear, float zFar);

	///	returns the distance of the near and far clipping plane.
		virtual void get_clip_distance_estimate(float& nearOut, float& farOut,
												float fromX, float fromY, float fromZ,
												float toX, float toY, float toZ);

	//	derived from TScene
		virtual void update_visuals(ISceneObject* pObj);

	//	geometry
	///	returns the bounding box of the scene
		void get_bounding_box(ug::vector3& vMinOut, ug::vector3& vMaxOut);
		ug::Sphere get_bounding_sphere();

	//	clipping planes
		inline int numClipPlanes()					{return MAX_NUM_CLIP_PLANES;}
		void enableClipPlane(int index, bool enable);
		inline bool clipPlaneIsEnabled(int index)	{return m_clipPlaneEnabled[index];}
		void setClipPlane(int index, const ug::Plane& plane);

	//	hide / unhide parts of the geometry
	/**	Note that this method doesn't invoke obj->visuals_changed. The caller is
	 * responsible to do so.*/
		template <class TIterator>
		void hide_elements(LGObject* obj, TIterator elemsBegin, TIterator elemsEnd);

	/**	Note that this method doesn't invoke obj->visuals_changed. The caller is
	 * responsible to do so.*/
		template <class TElem>
		void unhide_elements(LGObject* obj);

	/**	Note that this method doesn't invoke obj->visuals_changed. The caller is
	 * responsible to do so.*/
		void unhide_elements(LGObject* obj);

	signals:
		void geometry_changed();

	public slots:
		virtual void visibility_changed(ISceneObject* pObj);
		virtual void color_changed(ISceneObject* pObj);

	protected slots:
		void object_geometry_changed();
		void object_visuals_changed();

	protected:
		void calculate_bounding_spheres(LGObject* pObj);

		void render_skeleton(LGObject* pObj);

		void render_creases(LGObject* pObj, int displayListIndex);

		void render_selection(LGObject* pObj, int displayListIndex);

		void render_points(LGObject* pObj, const ug::vector4& color,
						  ug::VertexBaseIterator vrtsBegin,
						  ug::VertexBaseIterator vrtsEnd,
						  ug::Grid::VertexAttachmentAccessor<ug::APosition>& aaPos);

		void render_point_subsets(LGObject* pObj, int baseDisplayListIndex = 0);

		void render_edges(LGObject* pObj, const ug::vector4& color,
						  ug::EdgeBaseIterator edgesBegin,
						  ug::EdgeBaseIterator edgesEnd,
						  ug::Grid::VertexAttachmentAccessor<ug::APosition>& aaPos);

		void render_edge_subsets(LGObject* pObj, int baseDisplayListIndex = 0);

		void render_triangles(LGObject* pObj, const ug::vector4& color,
						  ug::FaceIterator trisBegin,
						  ug::FaceIterator trisEnd,
						  ug::Grid::VertexAttachmentAccessor<ug::APosition>& aaPos,
						  ug::Grid::FaceAttachmentAccessor<ug::ANormal>& aaNorm);

		void render_quadrilaterals(LGObject* pObj,
						  const ug::vector4& color,
						  ug::FaceIterator quadsBegin,
						  ug::FaceIterator quadsEnd,
						  ug::Grid::VertexAttachmentAccessor<ug::APosition>& aaPos,
						  ug::Grid::FaceAttachmentAccessor<ug::ANormal>& aaNorm);

		void rerender_volumes(LGObject* pObj,
						  const ug::vector4& color,
						  ug::VolumeIterator volsBegin,
						  ug::VolumeIterator volsEnd,
						  ug::Grid::VertexAttachmentAccessor<ug::APosition>& aaPos,
						  ug::Grid::FaceAttachmentAccessor<ug::ANormal>& aaNorm);

		void render_faces(LGObject* pObj, ug::Grid& grid,
						  ug::SubsetHandler& sh, bool renderAll = false);
		void render_volumes(LGObject* pObj);
		void render_faces_without_clip_plane(LGObject* pObj);
		void render_faces_with_clip_plane(LGObject* pObj);

		bool clip_face(ug::Face* f, const ug::Sphere& boundingSphere,
						ug::Grid::VertexAttachmentAccessor<ug::APosition>& aaPos);
		bool clip_volume(ug::Volume* v, const ug::Sphere& boundingSphere,
						ug::Grid::VertexAttachmentAccessor<ug::APosition>& aaPos);

		ug::RelativePositionIndicator clip_sphere(const ug::Sphere& sphere);
		ug::RelativePositionIndicator clip_point(const ug::vector3& point);

	protected:
		typedef ug::Attachment<char> AChar;

	protected:
		unsigned int m_drawModeFront;
		unsigned int m_drawModeBack;
		int m_viewWidth;
		int m_viewHeight;
		float m_matTransform[16];
		float m_fovy;
		float m_aspectRatio;
		float m_zNear;
		float m_zFar;

	//	attachments
		ug::ASphere		m_aSphere;
		ug::AInt		m_aInt;
		ug::ABool		m_aRendered;
		ug::ABool		m_aHidden;

	//	clip planes
		ug::Plane	m_clipPlanes[MAX_NUM_CLIP_PLANES];
		bool		m_clipPlaneEnabled[MAX_NUM_CLIP_PLANES];
};


////////////////////////////////////////
//	include implementation
#include "lg_scene_impl.hpp"

#endif // __H__LG_SCENE__
