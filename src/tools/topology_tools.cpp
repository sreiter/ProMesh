//	created by Sebastian Reiter
//	s.b.reiter@googlemail.com
//	y10 m05 d10

#include <list>
#include <vector>
#include <cassert>
#include <map>
#include <stack>
#include <algorithm>
#include <sstream>
#include "app.h"
#include "standard_tools.h"

using namespace std;

namespace ug
{

template <class vector_t>
bool RayRayProjection(number& t1Out, number& t2Out,
						const vector_t& from1, const vector_t& dir1,
						const vector_t& from2, const vector_t& dir2)
{
	vector_t ab;
	VecSubtract(ab, from2, from1);
	number l11 = VecDot(dir1, dir1);
	number l12 = -VecDot(dir1, dir2);
	number l22 = VecDot(dir2, dir2);
	number ra = VecDot(dir1, ab);
	number rb = -VecDot(dir2, ab);

	number smallSq = SMALL * SMALL;
//	l11 and l22 are always >= 0
	if((l11 < smallSq) || (l22 < smallSq))
		return false;

	number tmp = l11 * l22 - l12 * l12;
	if(fabs(tmp) < SMALL)
		return false;

	t2Out = (l11*rb - l12*ra) / tmp;
	t1Out = (ra - l12*t2Out) / l11;
	return true;
}

template <class vector_t>
bool LineLineProjection(number& t1Out, number& t2Out,
						  const vector_t& a1, const vector_t& a2,
						  const vector_t& b1, const vector_t& b2)
{
	vector_t dirA, dirB;
	VecSubtract(dirA, a2, a1);
	VecSubtract(dirB, b2, b1);

	if(RayRayProjection(t1Out, t2Out, a1, dirA, b1, dirB)){
		if((t1Out >= 0) && (t1Out <= 1.) && (t2Out >= 0) && (t2Out <= 1.))
			return true;
		return false;
	}
	return false;
}

template <class TAAPosVRT>
VertexBase* ResolveVertexEdgeIntersection(Grid& grid, VertexBase* v,
										   EdgeBase* e, TAAPosVRT& aaPos,
										   number snapThreshold)
{
	typedef typename TAAPosVRT::ValueType vector_t;

	number snapThresholdSq = snapThreshold * snapThreshold;

//	make sure that the vertex is not an endpoint of e
	if(EdgeContains(e, v))
		return NULL;

//	we have to make sure that v and e are not connected by a face.
//	This could lead to infinite recursion
/*
	vector<Face*> faces;
	CollectFaces(faces, grid, e);
	for(size_t i = 0; i < faces.size(); ++i){
		if(FaceContains(faces[i], v))
			return NULL;
	}
*/
//	project the vertex to the line defined by the edge
	vector_t p;
	number t = DropAPerpendicular(p, aaPos[v], aaPos[e->vertex(0)],
								  aaPos[e->vertex(1)]);

	if((t >= 0) && (t <= 1.)){
		if(VecDistanceSq(p, aaPos[v]) < snapThresholdSq){
		//	to make sure that no double edges may occur, we'll use MergeVertices
			Vertex* nVrt = SplitEdge<Vertex>(grid, grid, e);
			aaPos[v] = p;
			MergeVertices(grid, v, nVrt);
			return v;
/*
		//	insert the vertex into the edge
			CreateEdgeSplitGeometry(grid, grid, e, v);
			grid.erase(e);
			return v;
*/
		}
	}
	return NULL;
}

/**
 * No support for volumes in the current version.
 * \todo Instead of manually refining the face, an external function SplitFace
 *		 should be used, which can take care of volumes, too.
 */
template <class TAAPosVRT>
bool ResolveVertexFaceIntersection(Grid& grid, VertexBase* v,
								   Face* f, TAAPosVRT& aaPos,
								   number snapThreshold)
{
	typedef typename TAAPosVRT::ValueType vector_t;

	number snapThresholdSq = snapThreshold * snapThreshold;

//	make sure that the vertex is not a corner of f
	if(FaceContains(f, v))
		return false;

//	calculate the normal
	vector_t n;
	CalculateNormal(n, f, aaPos);

//	project the vertex to the plane defined by the face
	vector_t p;
	ProjectPointToPlane(p, aaPos[v], aaPos[f->vertex(0)], n);

//	check whether the distance is fine
	if(VecDistanceSq(p, aaPos[v]) < snapThresholdSq){
		bool refined = false;
		vector<Face*> newFaces;
		VertexBase* newFaceVrt = NULL;
		VertexBase* nVrt = NULL;
		vector_t pi;
	//	now we have to check whether the projection lies in the face
		if(f->num_vertices() == 3){
			if(RayTriangleIntersection(pi, aaPos[f->vertex(0)], aaPos[f->vertex(1)],
										aaPos[f->vertex(2)], p, n))
			{
			//	ok we have to insert the vertex
			//	we'll create a temporary new vertex, which will then be merged with v.
			//	This is important, since we can avoid double elements this way.
				nVrt = *grid.create<Vertex>();
				VertexBase* newEdgeVrts[3] = {NULL, NULL, NULL};
				refined = f->refine(newFaces, &newFaceVrt, newEdgeVrts, nVrt);
			}
		}
		else if(f->num_vertices() == 4){
			bool success = false;
			if(RayTriangleIntersection(pi, aaPos[f->vertex(0)], aaPos[f->vertex(1)],
										aaPos[f->vertex(2)], p, n))
			{
				success = true;
			}
			else if(RayTriangleIntersection(pi, aaPos[f->vertex(0)], aaPos[f->vertex(2)],
											aaPos[f->vertex(3)], p, n))
			{
				success = true;
			}

			if(success){
			//	ok we have to insert the vertex
			//	we'll create a temporary new vertex, which will then be merged with v.
			//	This is important, since we can avoid double elements this way.
				nVrt = *grid.create<Vertex>();
				VertexBase* newEdgeVrts[4] = {NULL, NULL, NULL, NULL};
				refined = f->refine(newFaces, &newFaceVrt, newEdgeVrts, nVrt);
			}
		}

		if(refined){
		//	adjust position
			aaPos[v] = pi;
		//	register the new faces and erase the old one
			for(size_t i = 0; i < newFaces.size(); ++i)
				grid.register_element(newFaces[i], f);
			grid.erase(f);

		//	to make sure that no double edges may occur, we'll use MergeVertices
			MergeVertices(grid, v, nVrt);

			return true;
		}
	}

	return false;
}

/**
 * This method does not resolve intersections between close, parallel edges or
 * between degenerate edges. You can treat such cases with
 * ReolveVertexEdgeIntersection.
 */
template <class TAAPosVRT>
VertexBase* ResolveEdgeEdgeIntersection(Grid& grid, EdgeBase* e1, EdgeBase* e2,
										TAAPosVRT& aaPos, number snapThreshold)
{
	typedef typename TAAPosVRT::ValueType vector_t;

//	check whether one edge contains a vertex of another edge
	if(EdgeContains(e1, e2->vertex(0)) || EdgeContains(e1, e2->vertex(1)))
		return NULL;

	number snapThresholdSq = snapThreshold * snapThreshold;

	number t1, t2;
	if(LineLineProjection(t1, t2, aaPos[e1->vertex(0)], aaPos[e1->vertex(1)],
						  aaPos[e2->vertex(0)], aaPos[e2->vertex(1)]))
	{
	//	calculate the positions
		vector_t v1, v2;
		VecScaleAdd(v1, (1. - t1), aaPos[e1->vertex(0)], t1, aaPos[e1->vertex(1)]);
		VecScaleAdd(v2, (1. - t2), aaPos[e2->vertex(0)], t2, aaPos[e2->vertex(1)]);

	//	check whether the points are close to each other
		if(VecDistanceSq(v1, v2) < snapThresholdSq){
		//	calculate center
			vector_t p;
			VecScaleAdd(p, 0.5, v1, 0.5, v2);

		//	to make sure that no double edges may occur, we'll use MergeVertices
			Vertex* nVrt1 = SplitEdge<Vertex>(grid, grid, e1);
			Vertex* nVrt2 = SplitEdge<Vertex>(grid, grid, e2);
			aaPos[nVrt1] = p;
			MergeVertices(grid, nVrt1, nVrt2);

			return nVrt1;
		/*
		//	create a new vertex and split both edges using it
			Vertex* vrt = *grid.create<Vertex>();
			aaPos[vrt] = p;
			CreateEdgeSplitGeometry(grid, grid, e1, vrt);
			CreateEdgeSplitGeometry(grid, grid, e2, vrt);
			grid.erase(e1);
			grid.erase(e2);
		*/
		}
		else{
		/*
			LOG("distance check failed at: " << v1 << ", " << v2 << endl);
			UG_LOG("edges with vertices: " << aaPos[e1->vertex(0)] << aaPos[e1->vertex(1)] << endl);
			UG_LOG("                     " << aaPos[e2->vertex(0)] << aaPos[e2->vertex(1)]);
		*/
		}
	}
	return NULL;
}

/**
 * No support for volumes in the current version.
 * \todo Instead of manually refining the face, an external function SplitFace
 *		 should be used, which can take care of volume, too.
 */
template <class TAAPosVRT>
bool ResolveEdgeFaceIntersection(Grid& grid, EdgeBase* e, Face* f,
								 TAAPosVRT& aaPos, number snapThreshold)
{
	typedef typename TAAPosVRT::ValueType vector_t;

//	check whether one edge contains a vertex of another edge
	if(FaceContains(f, e->vertex(0)) || FaceContains(f, e->vertex(1)))
		return false;

	number snapThresholdSq = snapThreshold * snapThreshold;

	vector_t dir;
	VecSubtract(dir, aaPos[e->vertex(1)], aaPos[e->vertex(0)]);

	vector_t p;
	number t1, t2, s;
	bool refined = false;
	vector<Face*> newFaces;
	VertexBase* newFaceVrt = NULL;
	VertexBase* vrt = NULL;
	if(f->num_vertices() == 3){
		if(RayTriangleIntersection(p, t1, t2, s, aaPos[f->vertex(0)], aaPos[f->vertex(1)],
									aaPos[f->vertex(2)], aaPos[e->vertex(0)], dir))
		{
			if((s >= 0) && (s <= 1.)){
			//	split the face
				vrt = *grid.create<Vertex>();
				VertexBase* newEdgeVrts[3] = {NULL, NULL, NULL};
				refined = f->refine(newFaces, &newFaceVrt, newEdgeVrts, vrt);
			}
		}
	}
	else if(f->num_vertices() == 4){
		bool intersecting = false;
		if(RayTriangleIntersection(p, t1, t2, s, aaPos[f->vertex(0)], aaPos[f->vertex(1)],
									aaPos[f->vertex(2)], aaPos[e->vertex(0)], dir))
		{
			intersecting = true;
		}
		else if(RayTriangleIntersection(p, t1, t2, s, aaPos[f->vertex(0)], aaPos[f->vertex(2)],
										aaPos[f->vertex(3)], aaPos[e->vertex(0)], dir))
		{
			intersecting = true;
		}

		if(intersecting && (s >= 0) && (s <= 1.))
		{
		//	split the face
			vrt = *grid.create<Vertex>();
			VertexBase* newEdgeVrts[4] = {NULL, NULL, NULL, NULL};
			refined = f->refine(newFaces, &newFaceVrt, newEdgeVrts, vrt);
		}
	}

	if(refined && vrt){
	//	create a new vertex and adjust position
		aaPos[vrt] = p;

	//	register the new faces and erase the old one
		for(size_t i = 0; i < newFaces.size(); ++i)
			grid.register_element(newFaces[i], f);
		grid.erase(f);

	//	to make sure that no double edges may occur, we'll use MergeVertices
	//	and SplitEdge
		Vertex* nVrt = SplitEdge<Vertex>(grid, grid, e);
		MergeVertices(grid, vrt, nVrt);

/*
	//	split the edge with the new vertex and erase the old one
		CreateEdgeSplitGeometry(grid, grid, e, vrt);
		grid.erase(e);
*/

		return true;
	}

	return false;
}

/**
 *	Projects vertices in elems onto close edges in elems.
 *	Though this method can be used to remove degenerated triangles,
 *	it is not guaranteed, that no degenerated triangles will remain
 *	(indeed, new degenerated triangles may be introduced).
 */
template <class TAAPosVRT>
bool ProjectVerticesToCloseEdges(Grid& grid,
								 GeometricObjectCollection elems,
								 TAAPosVRT& aaPos,
								 number snapThreshold)
{
//	perform vertex/edge intersections
//	iterate over all vertices
	for(VertexBaseIterator vrtIter = elems.begin<VertexBase>();
		vrtIter != elems.end<VertexBase>();)
	{
		VertexBase* vrt = *vrtIter;
		++vrtIter;

	//	check against all edges
		for(EdgeBaseIterator eIter = elems.begin<EdgeBase>();
			eIter != elems.end<EdgeBase>();)
		{
			EdgeBase* e = *eIter;
			++eIter;

		//	try to insert the vertex into the edge
			ResolveVertexEdgeIntersection(grid, vrt, e, aaPos, snapThreshold);
		}
	}

	return true;
}

/**
 *	Projects vertices in elems onto close faces in elems.
 */
template <class TAAPosVRT>
bool ProjectVerticesToCloseFaces(Grid& grid,
								 GeometricObjectCollection elems,
								 TAAPosVRT& aaPos,
								 number snapThreshold)
{
//	perform vertex/face intersections
//	iterate over all vertices
	for(VertexBaseIterator vrtIter = elems.vertices_begin();
		vrtIter != elems.vertices_end();)
	{
		VertexBase* vrt = *vrtIter;
		++vrtIter;

	//	check against all faces
		for(FaceIterator fIter = elems.faces_begin(); fIter != elems.faces_end();)
		{
			Face* f = *fIter;
			++fIter;

		//	try to insert the vertex into the face
			ResolveVertexFaceIntersection(grid, vrt, f, aaPos, snapThreshold);
		}
	}
	return true;
}

/**THIS METHOD USES Grid::mark.
 * Intersects all edges in elems which are closer to each other
 * than snapThreshold.*/
template <class TAAPosVRT>
bool IntersectCloseEdges(Grid& grid,
						 GeometricObjectCollection elems,
						 TAAPosVRT& aaPos,
						 number snapThreshold)
{
//	we'll first mark all elements in elems to make sure that
//	only edges which were initially marked are intersected.
	grid.begin_marking();
	for(EdgeBaseIterator iter = elems.begin<EdgeBase>();
		iter != elems.end<EdgeBase>(); ++iter)
	{
		grid.mark(*iter);
	}

//	perform edge/edge and edge/face intersections
	for(EdgeBaseIterator mainIter = elems.begin<EdgeBase>();
		mainIter != elems.end<EdgeBase>();)
	{
		EdgeBase* e = *mainIter;
		++mainIter;

	//	if e is not marked, we can exit right away, since all succeeding
	//	edges won't be marked, too.
		if(!grid.is_marked(e))
			break;

	//	check all other edges up to e.
		for(EdgeBaseIterator iter = elems.begin<EdgeBase>(); *iter != e;)
		{
			EdgeBase* e2 = *iter;
			++iter;

		//	if an intersection occured, we have to move on to the next edge in the queue,
		//	since the old edge no longer exists.
			if(ResolveEdgeEdgeIntersection(grid, e, e2, aaPos, snapThreshold)){
				break;
			}
		}
	}
	grid.end_marking();
	return true;
}


///	returns the index of the first vertex closer to p than snapThreshold.
/**	returns -1 if nothing was found.*/
template <class TAAPosVRT>
static int FindCloseVertexInArray(vector<VertexBase*>& array,
								const typename TAAPosVRT::ValueType& p,
								TAAPosVRT& aaPos, number snapThreshold)
{
	number snapThrSq = snapThreshold * snapThreshold;
//	iterate over the array and check whether a vertex close to vrt already exists.
	for(size_t i = 0; i < array.size(); ++i){
		if(VecDistanceSq(aaPos[array[i]], p) < snapThrSq){
		//	we got one. return the index
			return (int)i;
		}
	}
	return -1;
}

////////////////////////////////////////////////////////////////////////
/**	This method uses Grid::mark
 */
bool ResolveGridIntersections(Grid& grid, TriangleIterator trisBegin,
							  TriangleIterator trisEnd, number snapThreshold)
{
//todo: add octree
	Grid::VertexAttachmentAccessor<APosition> aaPos(grid, aPosition);

//	we use a selector to select elements that shall be merged and
//	triangles that are to be processed and deleted.
	Selector sel(grid);
	sel.enable_autoselection(false);

//	we first select all associated vertices and perform a merge on them
	sel.select(trisBegin, trisEnd);
	SelectAssociatedVertices(sel, trisBegin, trisEnd);
	SelectAssociatedEdges(sel, trisBegin, trisEnd);
	RemoveDoubles<3>(grid, sel.vertices_begin(), sel.vertices_end(),
					 aPosition, snapThreshold);

////////////////////////////////
//	PERFORM AND RESOLVE TRIANGLE - TRIANGLE INTERSECTIONS

//	clear edges and vertices from the selector. faces have to stay, since we will
//	operate on them now.
	sel.clear<VertexBase>();
	sel.clear<EdgeBase>();

//	enable selection inheritance, since we want new elements to be
//	selected in this selector
	sel.enable_selection_inheritance(true);

//	we need some attachments in order to store new vertices and edges for
//	each face.
	typedef Attachment<vector<VertexBase*> >		AVrtVec;
	typedef Attachment<vector<pair<int, int> > >	AEdgeDescVec;
	AVrtVec aVrtVec;
	AEdgeDescVec aEdgeDescVec;
	grid.attach_to_faces(aVrtVec);
	grid.attach_to_faces(aEdgeDescVec);
	Grid::FaceAttachmentAccessor<AVrtVec> aaVrtVec(grid, aVrtVec);
	Grid::FaceAttachmentAccessor<AEdgeDescVec> aaEdgeDescVec(grid, aEdgeDescVec);

//	iterate over all triangles and perform intersecion with other triangles
	for(TriangleIterator triIter1 = sel.begin<Triangle>();
		triIter1 != sel.end<Triangle>(); ++triIter1)
	{
		Triangle* t1 = *triIter1;

	//	iterate over the rest of the triangles
		TriangleIterator triIter2 = triIter1;
		for(++triIter2; triIter2 != sel.end<Triangle>(); ++triIter2)
		{
			Triangle* t2 = *triIter2;

		//	we have to make sure, that t1 and t2 do not share an edge (two vertices)
			size_t numShared = NumSharedVertices(grid, t1, t2);
			if(numShared > 1)
				continue;

		//	perform normal comparision to avoid intersection of flat neighbours
			vector3 n1, n2;
			CalculateNormal(n1, t1, aaPos);
			CalculateNormal(n2, t2, aaPos);
			number d = VecDot(n1, n2);
			if(fabs(d) > 1. - SMALL)
				continue;

			vector3 ip[2];
			if(TriangleTriangleIntersection(aaPos[t1->vertex(0)], aaPos[t1->vertex(1)],
											aaPos[t1->vertex(2)], aaPos[t2->vertex(0)],
											aaPos[t2->vertex(1)], aaPos[t2->vertex(2)],
											&ip[0], &ip[1]) == 1)
			{
			//	add an edge between the two points
			//	to avoid insertion of double points, we first check whether the point
			//	already exists in the triangle. Do this for both triangles.
				Triangle* t[2]; t[0] = t1; t[1] = t2;

			//	prepare both triangles.
			//todo: think about performance optimizations.
			//	insertion of corner points could be avoided by bloating the code a little.
			//	this could increase performance.
				for(size_t i_tri = 0; i_tri < 2; ++i_tri){
				//	If it is encountered for the first time,
				//	we'll add its corner-vertices to its list of vertices.
					vector<VertexBase*>& vrts = aaVrtVec[t[i_tri]];
					if(vrts.empty()){
						for(size_t i = 0; i < t[i_tri]->num_vertices(); ++i)
							vrts.push_back(t[i_tri]->vertex(i));
					}
				}

				//	now check whether the vertex already exists
				int inds1[2];
				int inds2[2];
				for(size_t i = 0; i < 2; ++i){
					int tind1 = FindCloseVertexInArray(aaVrtVec[t[0]], ip[i],
													   aaPos, snapThreshold);
					int tind2 = FindCloseVertexInArray(aaVrtVec[t[1]], ip[i],
													   aaPos, snapThreshold);

					if(tind1 == -1){
						if(tind2 == -1){
						//	we have to create a new vertex
							VertexBase* vrt = *grid.create<Vertex>();
							aaPos[vrt] = ip[i];
							tind1 = (int)aaVrtVec[t[0]].size();
							tind2 = (int)aaVrtVec[t[1]].size();
							aaVrtVec[t[0]].push_back(vrt);
							aaVrtVec[t[1]].push_back(vrt);
						}
						else{
						//	the vertex already exists in t[1]
							tind1 = (int)aaVrtVec[t[0]].size();
							aaVrtVec[t[0]].push_back((aaVrtVec[t[1]])[tind2]);
						}
					}
					else if(tind2 == -1){
					//	the vertex already exists in t[0]
						tind2 = (int)aaVrtVec[t[1]].size();
						aaVrtVec[t[1]].push_back((aaVrtVec[t[0]])[tind1]);
					}

				//	ind1 now contains the index into the vertex array of t[0], at
				//	which a vertex with position ip[i] lies.
					inds1[i] = tind1;
					inds2[i] = tind2;
				}

			//	we found the indices of both endpoints and can now add an edge
			//	connecting both to the edgeDesc arrays of t[0] and t[1].
				if(inds1[0] != inds1[1])
					aaEdgeDescVec[t[0]].push_back(make_pair(inds1[0], inds1[1]));
				if(inds2[0] != inds2[1])
					aaEdgeDescVec[t[1]].push_back(make_pair(inds2[0], inds2[1]));
			}
		}
	}

//	all intersections have been resolved. Iterate over the triangles again and
//	create the new elements.
//	triangles that shall be deleted are pushed to vDelTris
	vector<Triangle*> vDelTris;
//	here we collect all vertices on which a merge has to be performed at the end
//	of the algorithm (vertices created through edge-edge intersections inside a triangle)
	vector<VertexBase*> cutVertices;
	Grid tgrid(GRIDOPT_STANDARD_INTERCONNECTION);
	AInt aInt;
	AVertexBase aVrt;
	tgrid.attach_to_vertices(aPosition);
	tgrid.attach_to_vertices(aInt);
	tgrid.attach_to_vertices_dv(aVrt, NULL);
	Grid::VertexAttachmentAccessor<APosition> taaPos(tgrid, aPosition);
	Grid::VertexAttachmentAccessor<AVertexBase> aaVrt(tgrid, aVrt);

//	holds vertices of tgrid, so that they are accessible by index.
	vector<VertexBase*> tgridVrts;

	for(TriangleIterator triIter = sel.begin<Triangle>();
		triIter != sel.end<Triangle>(); ++triIter)
	{
		Triangle* t = *triIter;

	//	we only proceed if there are intersecion-edges at all
		if(!aaEdgeDescVec[t].empty()){
			tgrid.clear_geometry();
			tgridVrts.clear();

		//	copy vertices associated with t1 to tgrid
			vector<VertexBase*>& vrts = aaVrtVec[t];
			for(size_t i = 0; i < vrts.size(); ++i){
				VertexBase* vrt = *tgrid.create<Vertex>();
				aaVrt[vrt] = vrts[i];
				taaPos[vrt] = aaPos[vrts[i]];
				tgridVrts.push_back(vrt);
			}

		//	now create the edges. vertices are found by indexing tgridVrts
			vector<pair<int, int> >& edgeDescs = aaEdgeDescVec[t];

		//	tri edges
			tgrid.create<Edge>(EdgeDescriptor(tgridVrts[0], tgridVrts[1]));
			tgrid.create<Edge>(EdgeDescriptor(tgridVrts[1], tgridVrts[2]));
			tgrid.create<Edge>(EdgeDescriptor(tgridVrts[2], tgridVrts[0]));

		//	new edges
			for(size_t i = 0; i < edgeDescs.size(); ++i){
				tgrid.create<Edge>(EdgeDescriptor(tgridVrts[edgeDescs[i].first],
												  tgridVrts[edgeDescs[i].second]));
			}

		//	we now have to resolve intersections between the edges
		//	first we'll try to snap vertices to edges
			ProjectVerticesToCloseEdges(tgrid, tgrid.get_geometric_objects(),
										taaPos, SMALL);

		//	now resolve edge/edge intersections
			IntersectCloseEdges(tgrid, tgrid.get_geometric_objects(),
								taaPos, SMALL);

		//	make sure that all vertices have an associated aaVrt
			for(VertexBaseIterator viter = tgrid.vertices_begin();
				viter != tgrid.vertices_end(); ++viter)
			{
				if(!aaVrt[*viter]){
				//	since the vertex does not have an associated vertex in grid,
				//	it is clear that it has been created through an edge-edge cut.
				//	Associates of such vertices have to be merged later on.
					aaVrt[*viter] = *grid.create<Vertex>();
					aaPos[aaVrt[*viter]] = taaPos[*viter];
					cutVertices.push_back(aaVrt[*viter]);
				}
			}

		//	ok. Everything is prepared. We can now triangulate the grid.
			if(TriangleFill_SweepLine(tgrid, tgrid.edges_begin(), tgrid.edges_end(),
										aPosition, aInt))
			{
			//	mark the triangle for deletion
				vDelTris.push_back(*triIter);

			//	add the triangles to the grid.
				for(TriangleIterator titer = tgrid.begin<Triangle>();
					titer != tgrid.end<Triangle>(); ++titer)
				{
					Triangle* ntri = *titer;

					grid.create<Triangle>(TriangleDescriptor(aaVrt[ntri->vertex(0)],
															aaVrt[ntri->vertex(1)],
															aaVrt[ntri->vertex(2)]),
										 *triIter);
				}
			}
			else{/*
				static int fileCounter = 1;
				string filenamePrefix = "/Users/sreiter/Desktop/failed_sweeplines/failed_sweepline_";
				stringstream ss2d, ss3d;
				ss2d << filenamePrefix << "2d_" << fileCounter << ".lgb";
				ss3d << filenamePrefix << "3d_" << fileCounter << ".lgb";
				++fileCounter;
				UG_LOG("TriangleFill_SweepLine failed!\n");
				SaveGridToFile(tgrid, ss3d.str().c_str(), aPosition);
			//	perform transformation to 2d and save that too.
				std::vector<vector3> vrts;
				for(VertexBaseIterator iter = tgrid.vertices_begin();
					iter != tgrid.vertices_end(); ++iter)
				{
					vrts.push_back(taaPos[*iter]);
				}
				std::vector<vector2> vrts2d(vrts.size());
				TransformPointSetTo2D(&vrts2d.front(), &vrts.front(),
									  vrts.size());

				size_t counter = 0;
				for(VertexBaseIterator iter = tgrid.vertices_begin();
					iter != tgrid.vertices_end(); ++iter, ++counter)
				{
					taaPos[*iter] = vector3(vrts2d[counter].x, vrts2d[counter].y, 0);
				}

				SaveGridToFile(tgrid, ss2d.str().c_str(), aPosition);
				*/
			}
		}
	}

//	detach attachments (tgrid is deleted anyways)
	grid.detach_from_faces(aVrtVec);
	grid.detach_from_faces(aEdgeDescVec);

////////////////////////////////
//	GRID POSTPROCESS
//	before we merge vertices in cutVertices, we'll select all faces
//	in order to make sure that only valid faces will be deleted.
	sel.clear();
	sel.select(vDelTris.begin(), vDelTris.end());		
	sel.select(cutVertices.begin(), cutVertices.end());

//	perform the merge (this has to be done on a selector.
//	  the current version of RemoveDoubles is a little restrictive
//	  in this regard.)
	if(!sel.empty<VertexBase>()){
		RemoveDoubles<3>(grid, sel.vertices_begin(), sel.vertices_end(),
						 aPosition, snapThreshold);
	}

	sel.clear<VertexBase>();
	sel.clear<EdgeBase>();

//	finally delete all refined triangles and associated unused edges and vertices	
	SelectInnerSelectionEdges(sel);
	SelectInnerSelectionVertices(sel);

	grid.erase(sel.begin<Face>(), sel.end<Face>());
	grid.erase(sel.begin<EdgeBase>(), sel.end<EdgeBase>());
	grid.erase(sel.begin<VertexBase>(), sel.end<VertexBase>());

	return true;
}

}//	end of namespace


class ToolResolveEdgeIntersections : public ITool
{
	public:
		void execute(LGObject* obj, QWidget* widget){
			using namespace ug;
			ToolWidget* dlg = dynamic_cast<ToolWidget*>(widget);

			number snapThreshold = ug::SMALL;

			if(dlg){
				snapThreshold = dlg->to_double(0);
			}

			ug::Grid& grid = obj->get_grid();
			ug::Selector& sel = obj->get_selector();
			Grid::VertexAttachmentAccessor<APosition> aaPos(grid, aPosition);

		//	the grid may now contain some degenerated triangles. We'll try to
		//	remove most of them by projecting vertices onto close edges
			SelectAssociatedVertices(sel, sel.begin<EdgeBase>(), sel.end<EdgeBase>());


			ProjectVerticesToCloseEdges(grid, sel.get_geometric_objects(),
										aaPos, snapThreshold);

			IntersectCloseEdges(grid, sel.get_geometric_objects(),
								aaPos, snapThreshold);
		//	remove doubles now
			RemoveDoubles<3>(grid, sel.begin<VertexBase>(), sel.end<VertexBase>(),
							 aPosition, snapThreshold);

			obj->geometry_changed();
		}

		const char* get_name()		{return "Resolve Edge Intersections";}
		const char* get_tooltip()	{return "Makes sure that all edge intersections are represented by a vertex.";}
		const char* get_group()		{return "Remeshing | Resolve Intersections";}

		ToolWidget* get_dialog(QWidget* parent){
			ToolWidget *dlg = new ToolWidget(get_name(), parent, this,
											IDB_APPLY | IDB_OK | IDB_CANCEL);
			dlg->addSpinBox(tr("snap threshold:"), 0, 1.e10, 0, 1, 9);

			return dlg;
		}
};

class ToolResolveTriangleIntersections : public ITool
{
	public:
		void execute(LGObject* obj, QWidget* widget){
			using namespace ug;
			ToolWidget* dlg = dynamic_cast<ToolWidget*>(widget);

			number snapThreshold = ug::SMALL;

			if(dlg){
				snapThreshold = dlg->to_double(0);
			}

			ug::Grid& grid = obj->get_grid();
			ug::Selector& sel = obj->get_selector();
			Grid::VertexAttachmentAccessor<APosition> aaPos(grid, aPosition);

		//	first we'll resolve triangle-triangle intersections
			ug::ResolveGridIntersections(grid, sel.begin<ug::Triangle>(),
										 sel.end<ug::Triangle>(), snapThreshold);

		//	the grid may now contain some degenerated triangles. We'll try to
		//	remove most of them by projecting vertices onto close edges
			SelectAssociatedVertices(sel, sel.begin<Triangle>(), sel.end<Triangle>());
			SelectAssociatedEdges(sel, sel.begin<Triangle>(), sel.end<Triangle>());
			ProjectVerticesToCloseEdges(grid, sel.get_geometric_objects(),
										aaPos, snapThreshold);

		//	remove doubles now
			RemoveDoubles<3>(grid, sel.begin<VertexBase>(), sel.end<VertexBase>(),
							 aPosition, snapThreshold);

			obj->geometry_changed();
		}

		const char* get_name()		{return "Resolve Triangle Intersections";}
		const char* get_tooltip()	{return "Makes sure that all triangle intersections are represented by an edge and vertices.";}
		const char* get_group()		{return "Remeshing | Resolve Intersections";}

		ToolWidget* get_dialog(QWidget* parent){
			ToolWidget *dlg = new ToolWidget(get_name(), parent, this,
											IDB_APPLY | IDB_OK | IDB_CANCEL);
			dlg->addSpinBox(tr("snap threshold:"), 0, 1.e10, 0, 1, 9);

			return dlg;
		}
};

class ToolProjectVerticesToCloseEdges : public ITool
{
	public:
		void execute(LGObject* obj, QWidget* widget){
			using namespace ug;
			ToolWidget* dlg = dynamic_cast<ToolWidget*>(widget);

			number snapThreshold = ug::SMALL;

			if(dlg){
				snapThreshold = dlg->to_double(0);
			}

			ug::Grid& grid = obj->get_grid();
			ug::Selector& sel = obj->get_selector();
			Grid::VertexAttachmentAccessor<APosition> aaPos(grid, aPosition);

			ProjectVerticesToCloseEdges(grid, sel.get_geometric_objects(),
										aaPos, snapThreshold);

		//	remove doubles now
			RemoveDoubles<3>(grid, sel.begin<VertexBase>(), sel.end<VertexBase>(),
							 aPosition, snapThreshold);

			obj->geometry_changed();
		}

		const char* get_name()		{return "Project Vertices To Close Edges";}
		const char* get_tooltip()	{return "Projects selected vertices to selected close edges.";}
		const char* get_group()		{return "Remeshing | Resolve Intersections";}

		ToolWidget* get_dialog(QWidget* parent){
			ToolWidget *dlg = new ToolWidget(get_name(), parent, this,
											IDB_APPLY | IDB_OK | IDB_CANCEL);
			dlg->addSpinBox(tr("snap threshold:"), 0, 1.e10, 0, 1, 9);

			return dlg;
		}
};

class ToolProjectVerticesToCloseFaces : public ITool
{
	public:
		void execute(LGObject* obj, QWidget* widget){
			using namespace ug;
			ToolWidget* dlg = dynamic_cast<ToolWidget*>(widget);

			number snapThreshold = ug::SMALL;

			if(dlg){
				snapThreshold = dlg->to_double(0);
			}

			ug::Grid& grid = obj->get_grid();
			ug::Selector& sel = obj->get_selector();
			Grid::VertexAttachmentAccessor<APosition> aaPos(grid, aPosition);

			ProjectVerticesToCloseFaces(grid, sel.get_geometric_objects(),
										aaPos, snapThreshold);

		//	remove doubles now
			RemoveDoubles<3>(grid, sel.begin<VertexBase>(), sel.end<VertexBase>(),
							 aPosition, snapThreshold);

			obj->geometry_changed();
		}

		const char* get_name()		{return "Project Vertices To Close Faces";}
		const char* get_tooltip()	{return "Projects selected vertices to selected close faces.";}
		const char* get_group()		{return "Remeshing | Resolve Intersections";}

		ToolWidget* get_dialog(QWidget* parent){
			ToolWidget *dlg = new ToolWidget(get_name(), parent, this,
											IDB_APPLY | IDB_OK | IDB_CANCEL);
			dlg->addSpinBox(tr("snap threshold:"), 0, 1.e10, 0, 1, 9);

			return dlg;
		}
};

class ToolIntersectCloseEdges : public ITool
{
	public:
		void execute(LGObject* obj, QWidget* widget){
			using namespace ug;
			ToolWidget* dlg = dynamic_cast<ToolWidget*>(widget);

			number snapThreshold = ug::SMALL;

			if(dlg){
				snapThreshold = dlg->to_double(0);
			}

			ug::Grid& grid = obj->get_grid();
			ug::Selector& sel = obj->get_selector();
			Grid::VertexAttachmentAccessor<APosition> aaPos(grid, aPosition);

			IntersectCloseEdges(grid, sel.get_geometric_objects(),
								aaPos, snapThreshold);

			obj->geometry_changed();
		}

		const char* get_name()		{return "Intersect Close Edges";}
		const char* get_tooltip()	{return "Performs intersections between selected close edges.";}
		const char* get_group()		{return "Remeshing | Resolve Intersections";}

		ToolWidget* get_dialog(QWidget* parent){
			ToolWidget *dlg = new ToolWidget(get_name(), parent, this,
											IDB_APPLY | IDB_OK | IDB_CANCEL);
			dlg->addSpinBox(tr("snap threshold:"), 0, 1.e10, 0, 1, 9);

			return dlg;
		}
};


class ToolEraseSelectedElements : public ITool
{
	public:
		void execute(LGObject* obj, QWidget* widget){
			using namespace ug;
			ToolWidget* dlg = dynamic_cast<ToolWidget*>(widget);
			bool eraseUnusedVrts = true;
			bool eraseUnusedEdges = true;
			bool eraseUnusedFaces = true;

			if(dlg){
				eraseUnusedVrts = dlg->to_bool(0);
				eraseUnusedEdges = dlg->to_bool(1);
				eraseUnusedFaces = dlg->to_bool(2);
			}

		//	adjust selection
			ug::Selector& sel = obj->get_selector();
			SelectAssociatedEdges(sel, sel.begin<VertexBase>(), sel.end<VertexBase>());
			SelectAssociatedFaces(sel, sel.begin<EdgeBase>(), sel.end<EdgeBase>());
			SelectAssociatedVolumes(sel, sel.begin<Face>(), sel.end<Face>());

			if(eraseUnusedFaces)
				ug::SelectInnerSelectionFaces(sel);

			if(eraseUnusedEdges)
				ug::SelectInnerSelectionEdges(sel);

			if(eraseUnusedVrts)
				ug::SelectInnerSelectionVertices(sel);

		//	erase selected elements
			ug::Grid& grid = obj->get_grid();
			grid.erase(sel.begin<ug::Volume>(), sel.end<ug::Volume>());
			grid.erase(sel.begin<ug::Face>(), sel.end<ug::Face>());
			grid.erase(sel.begin<ug::EdgeBase>(), sel.end<ug::EdgeBase>());
			grid.erase(sel.begin<ug::VertexBase>(), sel.end<ug::VertexBase>());

			obj->geometry_changed();
		}

		const char* get_name()		{return "Erase Selected Elements";}
		const char* get_tooltip()	{return "Erases selected elements and associated unreferenced geometry.";}
		const char* get_group()		{return "Remeshing";}

		ToolWidget* get_dialog(QWidget* parent){
			ToolWidget *dlg = new ToolWidget(get_name(), parent, this,
											IDB_APPLY | IDB_OK | IDB_CLOSE);
			dlg->addCheckBox(tr("erase associated unused vertices"), true);
			dlg->addCheckBox(tr("erase associated unused edges"), true);
			dlg->addCheckBox(tr("erase associated unused faces"), true);
			return dlg;
		}
};

class ToolRemoveDoubles : public ITool
{
	public:
		void execute(LGObject* obj, QWidget* widget){
			ToolWidget* dlg = dynamic_cast<ToolWidget*>(widget);
			double threshold = 0.0001;
			if(dlg)
				threshold = dlg->to_double(0);

			ug::Grid& grid = obj->get_grid();
			ug::Selector& sel = obj->get_selector();

			if(sel.num<ug::VertexBase>() == 0){
				UG_LOG("INFO in Remove Doubles: no vertices are selected. Aborting.\n");
			}
			else{
				size_t numVrts = grid.num<ug::VertexBase>();
                                ug::RemoveDoubles<3>(grid,
								  sel.begin<ug::VertexBase>(),
								  sel.end<ug::VertexBase>(),
								  ug::aPosition, threshold);
				UG_LOG("removed " << numVrts - grid.num<ug::VertexBase>() << " vertices with threshold " << threshold << ".\n");
				obj->geometry_changed();
			}
		}

		const char* get_name()		{return "Remove Double Vertices";}
		const char* get_tooltip()	{return "Removes selected vertices that are close to each other";}
		const char* get_group()		{return "Remeshing | Remove Doubles";}

		ToolWidget* get_dialog(QWidget* parent){
			ToolWidget *dlg = new ToolWidget(get_name(), parent, this,
											IDB_APPLY | IDB_OK | IDB_CANCEL);
			dlg->addSpinBox(tr("threshold:"), 0, 1000000., 0.0001, 0.00001, 9);
			return dlg;
		}
};

class ToolRemoveDoubleEdges : public ITool
{
	public:
		void execute(LGObject* obj, QWidget*){
			using namespace ug;
			Grid& grid = obj->get_grid();
			Selector& sel = obj->get_selector();

			if(sel.num<EdgeBase>() == 0){
				UG_LOG("INFO in Remove Double Edges: no edges are selected. Aborting.\n");
			}
			else{
				size_t numEdges= grid.num<EdgeBase>();
				RemoveDoubleEdges(grid, sel.begin<EdgeBase>(), sel.end<EdgeBase>());
				UG_LOG("removed " << numEdges - grid.num<EdgeBase>() << " edges.\n");
				obj->geometry_changed();
			}
		}

		const char* get_name()		{return "Remove Double Edges";}
		const char* get_tooltip()	{return "Removes selected doubles of selected edges.";}
		const char* get_group()		{return "Remeshing | Remove Doubles";}
};

class ToolMergeAtFirst : public ITool
{
	public:
		void execute(LGObject* obj, QWidget*){
			using namespace ug;

			ug::Grid& grid = obj->get_grid();
			ug::Selector& sel = obj->get_selector();

			if(sel.num<ug::VertexBase>() == 0){
				UG_LOG("INFO in 'Merge at First': no vertices are selected. Aborting.\n");
			}
			else{
				Grid::VertexAttachmentAccessor<APosition> aaPos(grid, aPosition);
				SelectAssociatedGeometricObjects(sel);

				vector3 first = aaPos[*sel.vertices_begin()];
				VertexBase* vrt = MergeMultipleVertices(grid, sel.vertices_begin(), sel.vertices_end());
				if(vrt)
					aaPos[vrt] = first;

				obj->geometry_changed();
			}
		}

		const char* get_name()		{return "Merge at First";}
		const char* get_tooltip()	{return "Merges all selected objects into a single vertex at the first selected vertex.";}
		const char* get_group()		{return "Remeshing | Merge Vertices";}
};

class ToolMergeAtCenter : public ITool
{
	public:
		void execute(LGObject* obj, QWidget*){
			using namespace ug;

			ug::Grid& grid = obj->get_grid();
			ug::Selector& sel = obj->get_selector();

			if(sel.num<ug::VertexBase>() == 0){
				UG_LOG("INFO in 'Merge at Center': no vertices are selected. Aborting.\n");
			}
			else{
				Grid::VertexAttachmentAccessor<APosition> aaPos(grid, aPosition);
				SelectAssociatedGeometricObjects(sel);

				vector3 center;
				CalculateCenter(center, sel, aaPos);
				VertexBase* vrt = MergeMultipleVertices(grid, sel.vertices_begin(), sel.vertices_end());
				if(vrt)
					aaPos[vrt] = center;

				obj->geometry_changed();
			}
		}

		const char* get_name()		{return "Merge at Center";}
		const char* get_tooltip()	{return "Merges all selected objects into a single vertex at the center of the selection.";}
		const char* get_group()		{return "Remeshing | Merge Vertices";}
};

class ToolMergeAtLast : public ITool
{
	public:
		void execute(LGObject* obj, QWidget*){
			using namespace ug;

			ug::Grid& grid = obj->get_grid();
			ug::Selector& sel = obj->get_selector();

			if(sel.num<ug::VertexBase>() == 0){
				UG_LOG("INFO in 'Merge at Last': no vertices are selected. Aborting.\n");
			}
			else{
				Grid::VertexAttachmentAccessor<APosition> aaPos(grid, aPosition);
				SelectAssociatedGeometricObjects(sel);

			//	todo: This iteration shouldn't be necessary!
				VertexBaseIterator vrtIter = sel.begin<VertexBase>();
				VertexBase* lastVrt = *vrtIter;
				for(; vrtIter != sel.end<VertexBase>(); ++vrtIter)
					lastVrt = *vrtIter;

				vector3 last = aaPos[lastVrt];
				VertexBase* vrt = MergeMultipleVertices(grid, sel.vertices_begin(), sel.vertices_end());
				if(vrt)
					aaPos[vrt] = last;

				obj->geometry_changed();
			}
		}

		const char* get_name()		{return "Merge at Last";}
		const char* get_tooltip()	{return "Merges all selected objects into a single vertex at the last selected vertex.";}
		const char* get_group()		{return "Remeshing | Merge Vertices";}
};


class ToolCollapseEdge : public ITool
{
	public:
		void execute(LGObject* obj, QWidget*){
			ug::Grid& grid = obj->get_grid();
			ug::Selector& sel = obj->get_selector();
			vector<ug::Face*> vFaces;
			vector<ug::EdgeBase*> vEdges;
			while(sel.num<ug::EdgeBase>() > 0){
				ug::EdgeBase* e = *sel.begin<ug::EdgeBase>();
			//	to make sure that all selected edges are collapsed,
			//	we have to check the adjacent triangles
				ug::CollectFaces(vFaces, grid, e);
				for(size_t i = 0; i < vFaces.size(); ++i){
					ug::Face* f = vFaces[i];
					if(f->num_edges() == 3){
						ug::CollectEdges(vEdges, grid, f);
						int counter = 0;
						for(size_t j = 0; j < 3; ++j){
							if(sel.is_selected(vEdges[j]))
								++counter;
						}
					//	if two edges are selected, we have
					//	to mark the unselected edge, too (since we
					//	don't know which will be removed).
						if(counter == 2){
							for(size_t j = 0; j < 3; ++j)
								sel.select(vEdges[j]);
						}
					}
				}

			//	calculate the center
				ug::Grid::VertexAttachmentAccessor<ug::APosition> aaPos(grid, ug::aPosition);
				ug::VecAdd(aaPos[e->vertex(0)], aaPos[e->vertex(0)], aaPos[e->vertex(1)]);
				ug::VecScale(aaPos[e->vertex(0)], aaPos[e->vertex(0)], 0.5);

			//	perform collapse
				ug::CollapseEdge(grid, e, e->vertex(0));
			}
			obj->geometry_changed();
		}

		const char* get_name()		{return "Collapse Edge";}
		const char* get_tooltip()	{return "Collapses the edge and removes adjacent triangles.";}
		const char* get_group()		{return "Remeshing | Edge Operations";}
};

class ToolSplitEdge : public ITool
{
	public:
		void execute(LGObject* obj, QWidget*){
		//	collect all edges that shall be splitted in a vector
		//	since new edges will be automatically selected again.
			ug::Grid& grid = obj->get_grid();
			ug::Selector& sel = obj->get_selector();
			vector<ug::EdgeBase*> vEdges;
			for(ug::EdgeBaseIterator iter = sel.begin<ug::EdgeBase>();
				iter != sel.end<ug::EdgeBase>(); ++iter)
			{
				vEdges.push_back(*iter);
			}

			ug::Grid::VertexAttachmentAccessor<ug::APosition> aaPos(grid, ug::aPosition);

		//	iterate through all edges in the vector and split them
			for(size_t i = 0; i < vEdges.size(); ++i){
				ug::vector3 center = ug::CalculateCenter(vEdges[i], aaPos);
				ug::Vertex* vrt = ug::SplitEdge<ug::Vertex>(grid, vEdges[i]);
				aaPos[vrt] = center;
			}

		//	done
			obj->geometry_changed();
		}

		const char* get_name()		{return "Split Edge";}
		const char* get_tooltip()	{return "Splits the edge and inserts new triangles.";}
		const char* get_group()		{return "Remeshing | Edge Operations";}
};

class ToolSwapEdge : public ITool
{
	public:
		void execute(LGObject* obj, QWidget*){
		//	collect all edges that shall be swapped in a vector
		//	since new edges will be automatically selected again.
			ug::Grid& grid = obj->get_grid();
			ug::Selector& sel = obj->get_selector();
			vector<ug::EdgeBase*> vEdges;
			for(ug::EdgeBaseIterator iter = sel.begin<ug::EdgeBase>();
				iter != sel.end<ug::EdgeBase>(); ++iter)
			{
				vEdges.push_back(*iter);
			}

		//	iterate through all edges in the vector and swap them
		//	if they are adjacent to two triangles
			ug::Face* faces[2];
			for(size_t i = 0; i < vEdges.size(); ++i){
				int numFaces = ug::GetAssociatedFaces(faces, grid, vEdges[i], 2);
				if(numFaces == 2){
					if(faces[0]->num_vertices() == 3 && faces[1]->num_vertices() == 3){
						ug::SwapEdge(grid, vEdges[i]);
					}
				}
			}

		//	done
			obj->geometry_changed();
		}

		const char* get_name()		{return "Swap Edge";}
		const char* get_tooltip()	{return "Swaps selected edges that are adjacent to exactly two triangles.";}
		const char* get_group()		{return "Remeshing | Edge Operations";}
};

class ToolPlaneCut : public ITool
{
	public:
		void execute(LGObject* obj, QWidget* widget){
			ToolWidget* dlg = dynamic_cast<ToolWidget*>(widget);
			ug::vector3 p(0, 0, 0);
			ug::vector3 n(0, 0, 1);
			//double threshold = 0.0001;

			if(dlg){
				p.x = dlg->to_double(0);
				p.y = dlg->to_double(1);
				p.z = dlg->to_double(2);
				n.x = dlg->to_double(3);
				n.y = dlg->to_double(4);
				n.z = dlg->to_double(5);
				//threshold = dlg->to_double(6);
			}

			ug::Selector& sel = obj->get_selector();

			if(ug::VecLengthSq(n) < ug::SMALL){
				UG_LOG("INFO in Plane Cut: Bad normal. Aborting.\n");
			}
			else{
				ug::CutEdgesWithPlane(sel, p, n);
				obj->geometry_changed();
			}
		}

		const char* get_name()		{return "Plane Cut";}
		const char* get_tooltip()	{return "Cuts selected edges along the given plane.";}
		const char* get_group()		{return "Remeshing";}

		ToolWidget* get_dialog(QWidget* parent){
			ToolWidget *dlg = new ToolWidget(get_name(), parent, this,
											IDB_APPLY | IDB_OK | IDB_CANCEL);
			dlg->addSpinBox(tr("plane-x:"), -1.e10, 1.e10, 0, 1, 9);
			dlg->addSpinBox(tr("plane-y:"), -1.e10, 1.e10, 0, 1, 9);
			dlg->addSpinBox(tr("plane-z:"), -1.e10, 1.e10, 0, 1, 9);
			dlg->addSpinBox(tr("plane-normal-x:"), -1, 1, 0, 0.1, 6);
			dlg->addSpinBox(tr("plane-normal-y:"), -1, 1, 0, 0.1, 6);
			dlg->addSpinBox(tr("plane-normal-z:"), -1, 1, 0, 0.1, 6);
			//dlg->addSpinBox(tr("threshold:"), 0, 1, 0.0001, 0.0001, 6);
			return dlg;
		}
};

class ToolAdjustEdgeOrientation : public ITool
{
	public:
		void execute(LGObject* obj, QWidget*){
			ug::Grid& grid = obj->get_grid();
			ug::Selector& sel = obj->get_selector();

			ug::AdjustEdgeOrientationToFaceOrientation(grid, sel.begin<ug::EdgeBase>(),
														sel.end<ug::EdgeBase>());

		//	done
			obj->geometry_changed();
		}

		const char* get_name()		{return "Adjust Edge Orientation";}
		const char* get_tooltip()	{return "Adjusts the orientation of boundary edges to associated faces.";}
		const char* get_group()		{return "Remeshing | Orientation";}
};

class ToolFixFaceOrientation : public ITool
{
	public:
		void execute(LGObject* obj, QWidget*){
			ug::Grid& grid = obj->get_grid();
			ug::Selector& sel = obj->get_selector();

			ug::FixFaceOrientation(grid, sel.begin<ug::Face>(),
									sel.end<ug::Face>());

		//	done
			obj->geometry_changed();
		}

		const char* get_name()		{return "Fix Face Orientation";}
		const char* get_tooltip()	{return "Tries to change orientation of selected faces so that all neighbouring faces point into the same direction.";}
		const char* get_group()		{return "Remeshing | Orientation";}
};

class ToolFixVolumeOrientation : public ITool
{
	public:
		void execute(LGObject* obj, QWidget*){
			ug::Grid& grid = obj->get_grid();
			ug::Selector& sel = obj->get_selector();
			ug::Grid::VertexAttachmentAccessor<ug::APosition> aaPos(grid, ug::aPosition);

			int numFlips = ug::FixOrientation(grid, sel.begin<ug::Volume>(), sel.end<ug::Volume>(), aaPos);

			UG_LOG("orientation of " << numFlips << " volumes flipped.\n");

		//	done
			obj->geometry_changed();
		}

		const char* get_name()		{return "Fix Volume Orientation";}
		const char* get_tooltip()	{return "Changes orientation of selected volumes, so that they are oriented correctly.";}
		const char* get_group()		{return "Remeshing | Orientation";}
};

class ToolInvertOrientation : public ITool
{
	public:
		void execute(LGObject* obj, QWidget*){
			ug::Grid& grid = obj->get_grid();
			ug::Selector& sel = obj->get_selector();

			ug::InvertOrientation(grid, sel.begin<ug::Face>(), sel.end<ug::Face>());

		//	done
			obj->geometry_changed();
		}

		const char* get_name()		{return "Invert Orientation";}
		const char* get_tooltip()	{return "Inverts the orientation of all selected faces.";}
		const char* get_group()		{return "Remeshing | Orientation";}
};


void RegisterTopologyTools(ToolManager* toolMgr)
{
	toolMgr->register_tool(new ToolEraseSelectedElements, Qt::Key_Delete);
	toolMgr->register_tool(new ToolMergeAtFirst);
	toolMgr->register_tool(new ToolMergeAtCenter);
	toolMgr->register_tool(new ToolMergeAtLast);
	toolMgr->register_tool(new ToolRemoveDoubles);
	toolMgr->register_tool(new ToolRemoveDoubleEdges);
	toolMgr->register_tool(new ToolCollapseEdge, Qt::Key_C, SMK_ALT);
	toolMgr->register_tool(new ToolSplitEdge);
	toolMgr->register_tool(new ToolSwapEdge, Qt::Key_W, SMK_ALT);
	toolMgr->register_tool(new ToolPlaneCut);
	toolMgr->register_tool(new ToolInvertOrientation);
	toolMgr->register_tool(new ToolAdjustEdgeOrientation);
	toolMgr->register_tool(new ToolFixFaceOrientation);
	toolMgr->register_tool(new ToolFixVolumeOrientation);
	toolMgr->register_tool(new ToolProjectVerticesToCloseEdges);
	toolMgr->register_tool(new ToolProjectVerticesToCloseFaces);
	toolMgr->register_tool(new ToolIntersectCloseEdges);
	toolMgr->register_tool(new ToolResolveEdgeIntersections);
	toolMgr->register_tool(new ToolResolveTriangleIntersections);
}
