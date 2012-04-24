// created by Sebastian Reiter
// s.b.reiter@googlemail.com
// y09 m07 d17

#include <cassert>
#include "lg_include.h"
#include "wx_app.h"
#include "renderer.h"

using namespace lgmath;
using namespace libGrid;

Renderer::Renderer()
{
//	set up clip plane
	set_clip_plane(vector3(0, 0, 0), vector3(0, 1, 0));
	enable_clip_plane(false);
}

void Renderer::enable_clip_plane(bool bEnable)
{
	m_bClipPlaneEnabled = bEnable;
}

void Renderer::set_clip_plane(const lgmath::vector3& p, const lgmath::vector3& n)
{
	m_clipPlane = Plane(p, n);
}

void Renderer::standard_render(GLuint displayList, libGrid::Grid& grid,
							libGrid::SubsetHandler& sh,
							libGrid::APosition& aPos, libGrid::ANormal& aNorm,
							libGrid::ASphere& aBoundingSphere,
							bool bWire, int colorBaseIndex)
{
	assert(grid.has_vertex_attachment(aPosition) && "missing vertex attachment: aPosition.");
	assert(grid.has_face_attachment(aNormal) && "missing face attachment: aNormal.");
	assert(grid.has_face_attachment(aBoundingSphere) && "no bounding-spheres attached to faces of grid.");
	assert(grid.has_volume_attachment(aBoundingSphere) && "no bounding-spheres attached to volumes of grid.");

//	fill the vertex list from the grid
	Grid::VertexAttachmentAccessor<APosition> aaPos(grid, aPosition);
	Grid::FaceAttachmentAccessor<ANormal> aaNorm(grid, aNormal);
	Grid::FaceAttachmentAccessor<ASphere> aaSphereFACE(grid, aBoundingSphere);
	Grid::VolumeAttachmentAccessor<ASphere> aaSphereVOL(grid, aBoundingSphere);

//	create a display list
	glNewList(displayList, GL_COMPILE);

	if(bWire)
	{
		glDepthMask(false);
		glEnable(GL_LINE_SMOOTH);
		glLineWidth(0.5f);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glEnable(GL_BLEND);
		glPolygonMode (GL_FRONT, GL_LINE);
		glDisable(GL_LIGHTING);
		GLfloat lineColor[4] = {0.1f, 0.1f, 0.1f, 1.f};
		glMaterialfv( GL_FRONT_AND_BACK, GL_AMBIENT, lineColor);
	}
	else
	{
		glPolygonMode (GL_FRONT, GL_FILL);
		glEnable(GL_LIGHTING);
		glPolygonOffset(1.f, 1.f);
		glEnable(GL_POLYGON_OFFSET_FILL);
		glDepthMask(true);
		glBlendFunc(GL_ONE, GL_ZERO);
		glDisable(GL_LINE_SMOOTH);
		glDisable(GL_BLEND);
	}

	if(!m_bClipPlaneEnabled)
	{
	//	ignore volumes. simply draw all faces which are in subsets.
	//	draw triangles
		for(uint i = 0; i < sh.num_subsets(); ++i)
		{
			const vector4& col = app::get_surface_color_rgba(i + colorBaseIndex);
			GLfloat faceColor[4] = {col.x, col.y, col.z, col.w};
			glMaterialfv( GL_FRONT_AND_BACK, GL_AMBIENT, faceColor);

		//	draw triangles
			glBegin(GL_TRIANGLES);

			for(TriangleIterator iter = sh.begin<Triangle>(i);
				iter != sh.end<Triangle>(i); ++iter)
			{
				Triangle* tri = *iter;

				vector3& n = aaNorm[tri];
				glNormal3f(n.x, n.y, n.z);
				glColor3f(0.0, 0.0, 0.0);

				for(int i = 0; i < 3; ++i)
				{
					vector3& v = aaPos[tri->vertex(i)];
					glVertex3f(v.x, v.y, v.z);
				}

			//	draw both sides
				glNormal3f(-n.x, -n.y, -n.z);

				for(int i = 2; i >= 0; --i)
				{
					vector3& v = aaPos[tri->vertex(i)];
					glVertex3f(v.x, v.y, v.z);
				}
			}

			glEnd();

		//	draw quads
			glBegin(GL_QUADS);

			for(QuadrilateralIterator iter = sh.begin<Quadrilateral>(i);
				iter != sh.end<Quadrilateral>(i); ++iter)
			{
				Quadrilateral* q = *iter;

				vector3& n = aaNorm[q];
				glNormal3f(n.x, n.y, n.z);
				glColor3f(0.0, 0.0, 0.0);

				for(int i = 0; i < 4; ++i)
				{
					vector3& v = aaPos[q->vertex(i)];
					glVertex3f(v.x, v.y, v.z);
				}

			//	draw both sides
				glNormal3f(-n.x, -n.y, -n.z);

				for(int i = 3; i >= 0; --i)
				{
					vector3& v = aaPos[q->vertex(i)];
					glVertex3f(v.x, v.y, v.z);
				}

			}

			glEnd();
		}
	}
	else
	{
//	if a clip-plane is enabled, we'll also have to draw all faces that are in a subset.
//	This time however we have to check whether they are visible or not.
//	Bounding-Sphere-Accessors
	//	draw triangles

		for(uint sInd = 0; sInd < sh.num_subsets(); ++sInd)
		{
			const vector4& col = app::get_surface_color_rgba(sInd + colorBaseIndex);
			GLfloat faceColor[4] = {col.x, col.y, col.z, col.w};
			glMaterialfv( GL_FRONT_AND_BACK, GL_AMBIENT, faceColor);
			glBegin(GL_TRIANGLES);
			for(TriangleIterator iter = sh.begin<Triangle>(sInd); iter != sh.end<Triangle>(sInd); ++iter)
			{
				Triangle* tri = *iter;

				if(!ClipFace(tri, aaSphereFACE[tri], m_clipPlane, aaPos))
				{
					vector3& n = aaNorm[tri];
					glNormal3f(n.x, n.y, n.z);
					glColor3f(0.0, 0.0, 0.0);

					for(int i = 0; i < 3; ++i)
					{
						vector3& v = aaPos[tri->vertex(i)];
						glVertex3f(v.x, v.y, v.z);
					}

				//	draw both sides
					glNormal3f(-n.x, -n.y, -n.z);
					glColor3f(0.0, 0.0, 0.0);

					for(int i = 2; i >= 0; --i)
					{
						vector3& v = aaPos[tri->vertex(i)];
						glVertex3f(v.x, v.y, v.z);
					}
				}
			}
			glEnd();
		}


	//	draw quads
		for(uint sInd = 0; sInd < sh.num_subsets(); ++sInd)
		{
			const vector4& col = app::get_surface_color_rgba(sInd + colorBaseIndex);
			GLfloat faceColor[4] = {col.x, col.y, col.z, col.w};
			glMaterialfv( GL_FRONT_AND_BACK, GL_AMBIENT, faceColor);
			glBegin(GL_QUADS);
			for(QuadrilateralIterator iter = sh.begin<Quadrilateral>(sInd); iter != sh.end<Quadrilateral>(sInd); ++iter)
			{
				Quadrilateral* q = *iter;

				if(!ClipFace(q, aaSphereFACE[q], m_clipPlane, aaPos))
				{
					vector3& n = aaNorm[q];
					glNormal3f(n.x, n.y, n.z);
					glColor3f(0.0, 0.0, 0.0);

					for(int i = 0; i < 4; ++i)
					{
						vector3& v = aaPos[q->vertex(i)];
						glVertex3f(v.x, v.y, v.z);
					}

				//	draw both sides
					glNormal3f(-n.x, -n.y, -n.z);
					glColor3f(0.0, 0.0, 0.0);

					for(int i = 3; i >= 0; --i)
					{
						vector3& v = aaPos[q->vertex(i)];
						glVertex3f(v.x, v.y, v.z);
					}
				}
			}
			glEnd();
		}


	//	now we have to draw all faces of volumes that intersect the clip-plane.
		for(uint sInd = 0; sInd < sh.num_subsets(); ++sInd)
		{
			const vector4& col = app::get_volume_color_rgba(sInd + colorBaseIndex);
			GLfloat faceColor[4] = {col.x, col.y, col.z, col.w};
			glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, faceColor);
			for(VolumeIterator vIter = sh.begin<Volume>(sInd); vIter != sh.end<Volume>(sInd); ++vIter)
			{
				Volume* v = *vIter;
				bool bDraw = false;
			//	associated faces are only drawn, if the volume cuts the clip-plane.
				if(PlaneSphereTest(m_clipPlane, aaSphereVOL[v]) == RPI_CUT)
				{
				//	check if there are more than two vertices on the inner
				//	and at least on vertex on the outer side.
					uint numVrts = v->num_vertices();
					bool bGotOuter = false;
					int numInner = 0;
					for(uint i = 0; i < numVrts; ++i)
					{
						RelativePositionIndicator rpi = PlanePointTest(m_clipPlane, aaPos[v->vertex(i)]);
						if(rpi == RPI_OUTSIDE)
							bGotOuter = true;
						else
							++numInner;
					}

					bDraw = bGotOuter && (numInner > 2);
				}

				if(bDraw)
				{
				//	iterate through associated faces.
					FaceIterator iterEnd = grid.associated_faces_end(v);
					for(FaceIterator iter = grid.associated_faces_begin(v); iter != iterEnd; ++iter)
					{
						Face* f = *iter;
						if(sh.get_subset_index(f) == -1)
						{
							if(!ClipFace(f, aaSphereFACE[f], m_clipPlane, aaPos))
							{
								int numVrts = (int)f->num_vertices();
								switch(numVrts)
								{
								case 3:	glBegin(GL_TRIANGLES);
									break;
								case 4: glBegin(GL_QUADS);
									break;
								default:
									LOG("weird face during visualization!!! bad number of vertices.\n");
									continue;
								}
								vector3& n = aaNorm[f];
								glNormal3f(n.x, n.y, n.z);
								glColor3f(0.0, 0.0, 0.0);

								for(int i = 0; i < numVrts; ++i)
								{
									vector3& v = aaPos[f->vertex(i)];
									glVertex3f(v.x, v.y, v.z);
								}

							//	draw both sides
								glNormal3f(-n.x, -n.y, -n.z);
								glColor3f(0.0, 0.0, 0.0);

								for(int i = numVrts-1; i >= 0; --i)
								{
									vector3& v = aaPos[f->vertex(i)];
									glVertex3f(v.x, v.y, v.z);
								}

								glEnd();
							}
						}
					}
				}
			}
		}
	}

	glEndList();
}
