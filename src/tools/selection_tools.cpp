//	created by Sebastian Reiter
//	s.b.reiter@googlemail.com
//	y10 m05 d07

#include <stack>
#include <vector>
#include "app.h"
#include "standard_tools.h"
#include "tools_util.h"
#include "common/node_tree/node_tree.h"

class ToolSelectAssociatedManifoldFaces : public ITool
{
	public:
		void execute(LGObject* obj, QWidget*){
			using namespace ug;
			using namespace std;

			Grid& grid = obj->get_grid();
			Selector& sel = obj->get_selector();
		//	select associated faces of selected elements
			SelectAssociatedFaces(sel, sel.vertices_begin(), sel.vertices_end());
			SelectAssociatedFaces(sel, sel.edges_begin(), sel.edges_end());
			SelectAssociatedFaces(sel, sel.volumes_begin(), sel.volumes_end());

		//	push all selected faces to a stack
			stack<Face*> stk;
			for(FaceIterator iter = sel.faces_begin(); iter != sel.faces_end(); ++iter){
				stk.push(*iter);
			}

		//	while there are faces in the stack, get their associated edges.
		//	if those edges are adjacent to exactly two faces, then select the
		//	neighboured face and push it to the stack (if it was unselected previously)
			vector<EdgeBase*> edges;
			while(!stk.empty()){
				Face* f = stk.top();
				stk.pop();
				CollectEdges(edges, grid, f);

				for(size_t i = 0; i < edges.size(); ++i){
					Face* faces[2];
					if(GetAssociatedFaces(faces, grid, edges[i], 2) == 2){
						for(size_t j = 0; j < 2; ++j){
							if(!sel.is_selected(faces[j])){
								sel.select(faces[j]);
								stk.push(faces[j]);
							}
						}
					}
				}
			}

			obj->selection_changed();
		}

		const char* get_name()		{return "Associated Manifold Faces";}
		const char* get_tooltip()	{return "Selects faces associated with the selection, avoiding non-manifold configurations.";}
		const char* get_group()		{return "Selection | Faces";}
};

class ToolSelectNonManifoldEdges : public ITool
{
	public:
		void execute(LGObject* obj, QWidget*){
			using namespace ug;
			using namespace std;

			Grid& grid = obj->get_grid();
			Selector& sel = obj->get_selector();

		//	iterate over all edges and check how many adjacent faces each has.
		//	if there are more than 2, the edge will be selected
			for(EdgeBaseIterator iter = grid.edges_begin();
				iter != grid.edges_end(); ++iter)
			{
				if(NumAssociatedFaces(grid, *iter) != 2)
					sel.select(*iter);
			}

			obj->selection_changed();
		}

		const char* get_name()		{return "Non Manifold Edges";}
		const char* get_tooltip()	{return "Selects edges with more than 2 associated faces.";}
		const char* get_group()		{return "Selection | Edges";}
};

class ToolClearSelection : public ITool
{
	public:
		void execute(LGObject* obj, QWidget*){
			obj->get_selector().clear();
			obj->selection_changed();
		}

		const char* get_name()		{return "Clear Selection";}
		const char* get_tooltip()	{return "Clears the selection";}
		const char* get_group()		{return "Selection";}
};


class ToolSelectSmoothEdgePath : public ITool
{
	public:
		void execute(LGObject* obj, QWidget* widget){
			ToolWidget* dlg = dynamic_cast<ToolWidget*>(widget);
			if(dlg)
				SelectSmoothEdgePath(obj->get_selector(),
									dlg->to_double(0),
									dlg->to_bool(1));
			else
				SelectSmoothEdgePath(obj->get_selector(), 15., true);
			obj->selection_changed();
		}

		const char* get_name()		{return "Smooth Path";}
		const char* get_tooltip()	{return "Selects a smooth edge path.";}
		const char* get_group()		{return "Selection | Edges";}

		ToolWidget* get_dialog(QWidget* parent){
			ToolWidget *dlg = new ToolWidget(get_name(), parent, this,
											IDB_APPLY | IDB_OK | IDB_CLOSE);
			dlg->addSpinBox(tr("max deviation:"), 0, 180, 20, 1, 0);
			dlg->addCheckBox(tr("stop at selected vertices:"), true);
			return dlg;
		}
};

class ToolSelectBoundaryVertices : public ITool
{
	public:
		void execute(LGObject* obj, QWidget*){
			using namespace ug;
			Grid& grid = obj->get_grid();
			Selector& sel = obj->get_selector();

			size_t numSel = sel.num<ug::VertexBase>();

			ug::SelectBoundaryElements(sel, grid.begin<VertexBase>(), grid.end<VertexBase>());

			UG_LOG("Selected " << sel.num<ug::VertexBase>() - numSel << " boundary vertices.\n");
			obj->selection_changed();
		}

		const char* get_name()		{return "Boundary Vertices";}
		const char* get_tooltip()	{return "Selects vertices that lie on the boundary of the geometry";}
		const char* get_group()		{return "Selection | Vertices";}
};

class ToolSelectInnerVertices : public ITool
{
	public:
		void execute(LGObject* obj, QWidget*){
			using namespace ug;
			Grid& grid = obj->get_grid();
			Selector& sel = obj->get_selector();

			size_t numSel = sel.num<ug::VertexBase>();

			ug::SelectInnerElements(sel, grid.begin<VertexBase>(), grid.end<VertexBase>());

			UG_LOG("Selected " << sel.num<ug::VertexBase>() - numSel << " inner vertices.\n");
			obj->selection_changed();
		}

		const char* get_name()		{return "Inner Vertices";}
		const char* get_tooltip()	{return "Selects vertices that do not lie on the boundary of the geometry";}
		const char* get_group()		{return "Selection | Vertices";}
};

class ToolSelectBoundaryEdges : public ITool
{
	public:
		void execute(LGObject* obj, QWidget*){
			using namespace ug;
			Grid& grid = obj->get_grid();
			Selector& sel = obj->get_selector();

			size_t numSelEdges = sel.num<ug::EdgeBase>();

			ug::SelectBoundaryElements(sel, grid.begin<EdgeBase>(), grid.end<EdgeBase>());

			UG_LOG("Selected " << sel.num<ug::EdgeBase>() - numSelEdges << " boundary edges.\n");
			obj->selection_changed();
		}

		const char* get_name()		{return "Boundary Edges";}
		const char* get_tooltip()	{return "Selects edges that lie on the boundary of the geometry";}
		const char* get_group()		{return "Selection | Edges";}
};

class ToolSelectInnerEdges : public ITool
{
	public:
		void execute(LGObject* obj, QWidget*){
			using namespace ug;
			Grid& grid = obj->get_grid();
			Selector& sel = obj->get_selector();

			size_t numSelEdges = sel.num<ug::EdgeBase>();

			ug::SelectInnerElements(sel, grid.begin<EdgeBase>(), grid.end<EdgeBase>());

			UG_LOG("Selected " << sel.num<ug::EdgeBase>() - numSelEdges << " inner edges.\n");
			obj->selection_changed();
		}

		const char* get_name()		{return "Inner Edges";}
		const char* get_tooltip()	{return "Selects edges that do not lie on the boundary of the geometry";}
		const char* get_group()		{return "Selection | Edges";}
};

class ToolSelectBoundaryFaces : public ITool
{
	public:
		void execute(LGObject* obj, QWidget*){
			using namespace ug;
			Grid& grid = obj->get_grid();
			Selector& sel = obj->get_selector();

			size_t numSel = sel.num<ug::Face>();

			ug::SelectBoundaryElements(sel, grid.begin<Face>(), grid.end<Face>());

			UG_LOG("Selected " << sel.num<ug::Face>() - numSel << " boundary faces.\n");
			obj->selection_changed();
		}

		const char* get_name()		{return "Boundary Faces";}
		const char* get_tooltip()	{return "Selects faces that lie on the boundary of the geometry";}
		const char* get_group()		{return "Selection | Faces";}
};

class ToolSelectInnerFaces : public ITool
{
	public:
		void execute(LGObject* obj, QWidget*){
			using namespace ug;
			Grid& grid = obj->get_grid();
			Selector& sel = obj->get_selector();

			size_t numSel = sel.num<ug::Face>();

			ug::SelectInnerElements(sel, grid.begin<Face>(), grid.end<Face>());

			UG_LOG("Selected " << sel.num<ug::Face>() - numSel << " inner faces.\n");
			obj->selection_changed();
		}

		const char* get_name()		{return "Inner Faces";}
		const char* get_tooltip()	{return "Selects faces that do not lie on the boundary of the geometry";}
		const char* get_group()		{return "Selection | Faces";}
};


class ToolSelectShortEdges : public ITool
{
	public:
		void execute(LGObject* obj, QWidget* widget){
			ToolWidget* dlg = dynamic_cast<ToolWidget*>(widget);
			using namespace ug;

			number maxLength = 0.0001;
			if(dlg){
				maxLength = dlg->to_double(0);
			}

			number maxLengthSq = maxLength * maxLength;
			Grid& grid = obj->get_grid();
			Selector& sel = obj->get_selector();
			Grid::VertexAttachmentAccessor<APosition> aaPos(grid, aPosition);

			size_t numSelEdges = sel.num<EdgeBase>();
			for(EdgeBaseIterator iter = grid.begin<EdgeBase>();
				iter != grid.end<EdgeBase>(); ++iter)
			{
				EdgeBase* e = *iter;
				if(VecDistanceSq(aaPos[e->vertex(0)], aaPos[e->vertex(1)]) < maxLengthSq)
					sel.select(e);
			}

			UG_LOG(  sel.num<EdgeBase>() - numSelEdges << " short edges selected.\n");

			obj->selection_changed();
		}

		const char* get_name()		{return "Short Edges";}
		const char* get_tooltip()	{return "Selects edges that are shorter than a given threshold.";}
		const char* get_group()		{return "Selection | Edges";}

		ToolWidget* get_dialog(QWidget* parent){
			ToolWidget *dlg = new ToolWidget(get_name(), parent, this,
											IDB_APPLY | IDB_OK | IDB_CLOSE);
			dlg->addSpinBox(tr("max length:"), 0, 1000000000., 0.001, 0.0001, 9);
			return dlg;
		}
};

class ToolSelectLongEdges : public ITool
{
	public:
		void execute(LGObject* obj, QWidget* widget){
			ToolWidget* dlg = dynamic_cast<ToolWidget*>(widget);
			using namespace ug;

			number minLength = 1;
			if(dlg){
				minLength = dlg->to_double(0);
			}

			number minLengthSq = minLength * minLength;
			Grid& grid = obj->get_grid();
			Selector& sel = obj->get_selector();
			Grid::VertexAttachmentAccessor<APosition> aaPos(grid, aPosition);

			size_t numSelEdges = sel.num<EdgeBase>();
			for(EdgeBaseIterator iter = grid.begin<EdgeBase>();
				iter != grid.end<EdgeBase>(); ++iter)
			{
				EdgeBase* e = *iter;
				if(VecDistanceSq(aaPos[e->vertex(0)], aaPos[e->vertex(1)]) > minLengthSq)
					sel.select(e);
			}

			UG_LOG(  sel.num<EdgeBase>() - numSelEdges << " long edges selected.\n");

			obj->selection_changed();
		}

		const char* get_name()		{return "Long Edges";}
		const char* get_tooltip()	{return "Selects edges that are longer than a given threshold.";}
		const char* get_group()		{return "Selection | Edges";}

		ToolWidget* get_dialog(QWidget* parent){
			ToolWidget *dlg = new ToolWidget(get_name(), parent, this,
											IDB_APPLY | IDB_OK | IDB_CLOSE);
			dlg->addSpinBox(tr("min length:"), 0, 1.e12, 1, 0.1, 9);
			return dlg;
		}
};

class ToolSelectDegenerateFaces : public ITool
{
	public:
		void execute(LGObject* obj, QWidget* widget){
			ToolWidget* dlg = dynamic_cast<ToolWidget*>(widget);
			using namespace ug;

			number maxHeight = 0.0001;
			if(dlg){
				maxHeight = dlg->to_double(0);
			}

			number maxHeightSq = maxHeight * maxHeight;
			Grid& grid = obj->get_grid();
			Selector& sel = obj->get_selector();
			Grid::VertexAttachmentAccessor<APosition> aaPos(grid, aPosition);

			size_t numSelFaces = sel.num<Face>();
			for(FaceIterator iter = grid.begin<Face>();
				iter != grid.end<Face>(); ++iter)
			{
				Face* f = *iter;
			//	iterate over the edges and check which is the longest.
				number maxLenSq = -1;
				size_t bestInd = -1;
				EdgeDescriptor ed;
				for(size_t i = 0; i < f->num_edges(); ++i){
					f->edge_desc(i, ed);
					number lenSq = VecDistanceSq(aaPos[ed.vertex(0)], aaPos[ed.vertex(1)]);
					if(lenSq > maxLenSq){
						maxLenSq = lenSq;
						bestInd = i;
					}
				}

				if(maxLenSq < maxHeightSq)
					sel.select(f);
				else{
				//	project the other vertices to the line and check the height
				//todo: this is not enough for quadrilaterals
					vector3 p;
					vector3& v = aaPos[f->vertex((bestInd + 2) % f->num_vertices())];
					f->edge_desc(bestInd, ed);
					number t;
					if(DistancePointToLine(t, v, aaPos[ed.vertex(0)], aaPos[ed.vertex(1)]) < maxHeight)
						sel.select(f);
				}
			}

			UG_LOG(  sel.num<Face>() - numSelFaces << " degenerated faces selected.\n");

			obj->selection_changed();
		}

		const char* get_name()		{return "Degenerate Faces";}
		const char* get_tooltip()	{return "Selects faces that have a height shorter than a given threshold.";}
		const char* get_group()		{return "Selection | Faces";}

		ToolWidget* get_dialog(QWidget* parent){
			ToolWidget *dlg = new ToolWidget(get_name(), parent, this,
											IDB_APPLY | IDB_OK | IDB_CLOSE);
			dlg->addSpinBox(tr("max height:"), 0, 1000000000., 0.001, 0.0001, 9);
			return dlg;
		}
};

class ToolSelectLinkedFlatFaces : public ITool
{
	public:
		void execute(LGObject* obj, QWidget* widget){
			ToolWidget* dlg = dynamic_cast<ToolWidget*>(widget);
			using namespace ug;

			number maxDeviationAngle = 1;
			bool traverseFlipped = false;
			bool traverseDegeneratedFaces = false;
			bool stopAtSelectedEdges = false;

			if(dlg){
				maxDeviationAngle = dlg->to_double(0);
				traverseFlipped = dlg->to_bool(1);
				traverseDegeneratedFaces = dlg->to_bool(2);
				stopAtSelectedEdges = dlg->to_bool(3);
			}

			Selector& sel = obj->get_selector();
			if(traverseDegeneratedFaces)
				ug::SelectLinkedFlatAndDegeneratedFaces(sel, maxDeviationAngle,
														traverseFlipped,
														stopAtSelectedEdges);
			else
				ug::SelectLinkedFlatFaces(sel, maxDeviationAngle, traverseFlipped,
										  stopAtSelectedEdges);

			obj->selection_changed();
		}

		const char* get_name()		{return "Linked Flat Faces";}
		const char* get_tooltip()	{return "Selects linked faces of selected faces that have a similar normal.";}
		const char* get_group()		{return "Selection | Faces";}

		ToolWidget* get_dialog(QWidget* parent){
			ToolWidget *dlg = new ToolWidget(get_name(), parent, this,
											IDB_APPLY | IDB_OK | IDB_CLOSE);
			dlg->addSpinBox(tr("max deviation angle:"), 0, 180, 1, 1, 6);
			dlg->addCheckBox(tr("traverse flipped faces:"), false);
			dlg->addCheckBox(tr("traverse degenerated faces:"), false);
			dlg->addCheckBox(tr("stop at selected edges:"), false);

			return dlg;
		}
};

class ToolSelectIntersectingTriangles : public ITool
{
	public:
		void execute(LGObject* obj, QWidget*){
			using namespace ug;
			using namespace ug::node_tree;

			Grid& grid = obj->get_grid();
			Selector& sel = obj->get_selector();

		//	make sure that triangles are present.
			if(grid.num<Triangle>() == 0){
				UG_LOG("Given grid does not contain any triangles. Aborting 'Select Intersecting Triangles'.\n");
				return;
			}

			if(grid.num<Quadrilateral>() > 0){
				UG_LOG("WARNING: Quadrilateral intersections are ignored during 'Select Intersecting Triangles'.\n");
			}

		//	create an octree
		//	sort the triangles of grid into an octree to speed-up projection performance
			SPOctree octree;
			octree = CreateOctree(grid, grid.begin<Triangle>(),
										grid.end<Triangle>(),
										10, 30, false, aPosition);

			if(!octree.valid()){
				UG_LOG("  Octree creation failed in ToolSelectIntersectingTriangles. Aborting.\n");
				return;
			}

		//	access the position attachment of the grid
			Grid::VertexAttachmentAccessor<APosition> aaPos(grid, aPosition);

		//	we'll use a traverser to find the intersections.
			Traverser_IntersectFaces intersectionTraverser;

		//	for each face the face itself and direct neighbors shall be ignored
			std::vector<Face*> ignoreList;

			UG_LOG("intersections found:");
			bool gotOne = false;
			size_t triCount = 0;
		//	now iterate over all triangles of the grid and find intersections
			for(TriangleIterator iter = grid.begin<Triangle>();
				iter != grid.end<Triangle>(); ++iter, ++triCount)
			{
				Triangle* t = *iter;

			//	add neighbors and the face itself to the ignore list
				CollectNeighbors(ignoreList, t, grid, NHT_VERTEX_NEIGHBORS);
				intersectionTraverser.clear_ignore_list();
				for(size_t i = 0; i < ignoreList.size(); ++i){
					intersectionTraverser.ignore_element(ignoreList[i]);
				}
				intersectionTraverser.ignore_element(t);

				if(intersectionTraverser.intersect_tri(aaPos[t->vertex(0)], aaPos[t->vertex(1)],
														aaPos[t->vertex(2)], octree))
				{
				//	check the intersecting face:
					/*
					const std::vector<CollisionElementID>& faces =
						intersectionTraverser.get_intersected_element_ids();
					*/
				//	an intersection occurred. Log the index and select the triangle.
					gotOne = true;
					UG_LOG(" " << triCount);
					sel.select(t);
				}
			}

			if(!gotOne){
				UG_LOG(" none");
			}
			UG_LOG(std::endl);

			obj->selection_changed();
		}

		const char* get_name()		{return "Intersecting Triangles";}
		const char* get_tooltip()	{return "Selects intersecting triangles. Neighbors are ignored.";}
		const char* get_group()		{return "Selection | Faces";}
};

class ToolSelectAssociatedVertices : public ITool
{
	public:
		void execute(LGObject* obj, QWidget*){
			ug::Selector& sel = obj->get_selector();
			ug::SelectAssociatedVertices(sel,
										sel.begin<ug::EdgeBase>(),
										sel.end<ug::EdgeBase>());
			ug::SelectAssociatedVertices(sel,
										sel.begin<ug::Face>(),
										sel.end<ug::Face>());
			ug::SelectAssociatedVertices(sel,
										sel.begin<ug::Volume>(),
										sel.end<ug::Volume>());
			obj->selection_changed();
		}

		const char* get_name()		{return "Associated Vertices";}
		const char* get_tooltip()	{return "Selects vertices that belong to selected edges, faces and volumes.";}
		const char* get_group()		{return "Selection | Vertices";}
};

class ToolSelectAssociatedEdges : public ITool
{
	public:
		void execute(LGObject* obj, QWidget*){
			ug::Selector& sel = obj->get_selector();

			ug::SelectAssociatedEdges(sel,
										sel.begin<ug::Face>(),
										sel.end<ug::Face>());
			ug::SelectAssociatedEdges(sel,
										sel.begin<ug::Volume>(),
										sel.end<ug::Volume>());
			obj->selection_changed();
		}

		const char* get_name()		{return "Associated Edges";}
		const char* get_tooltip()	{return "Selects edges that belong to selected faces and volumes.";}
		const char* get_group()		{return "Selection | Edges";}
};

class ToolSelectAssociatedFaces : public ITool
{
	public:
		void execute(LGObject* obj, QWidget*){
			ug::Selector& sel = obj->get_selector();

			ug::SelectAssociatedFaces(sel,
										sel.begin<ug::Volume>(),
										sel.end<ug::Volume>());
			obj->selection_changed();
		}

		const char* get_name()		{return "Associated Faces";}
		const char* get_tooltip()	{return "Selects faces that belong to selected volumes.";}
		const char* get_group()		{return "Selection | Faces";}
};

class ToolSelectAll : public ITool
{
	public:
		void execute(LGObject* obj, QWidget*){
			ug::Grid& grid = obj->get_grid();
			ug::Selector& sel = obj->get_selector();
			sel.select(grid.vertices_begin(), grid.vertices_end());
			sel.select(grid.edges_begin(), grid.edges_end());
			sel.select(grid.faces_begin(), grid.faces_end());
			sel.select(grid.volumes_begin(), grid.volumes_end());
			obj->selection_changed();
		}

		const char* get_name()		{return "All";}
		const char* get_tooltip()	{return "Selects all vertices, edges ,faces and volumes of the current grid";}
		const char* get_group()		{return "Selection";}
};

class ToolSelectAllVertices : public ITool
{
	public:
		void execute(LGObject* obj, QWidget*){
			ug::Grid& grid = obj->get_grid();
			ug::Selector& sel = obj->get_selector();
			sel.select(grid.vertices_begin(), grid.vertices_end());
			obj->selection_changed();
		}

		const char* get_name()		{return "All Vertices";}
		const char* get_tooltip()	{return "Selects all vertices of the current grid";}
		const char* get_group()		{return "Selection | Vertices";}
};

class ToolDeselectAllVertices : public ITool
{
	public:
		void execute(LGObject* obj, QWidget*){
			ug::Selector& sel = obj->get_selector();
			sel.deselect(sel.vertices_begin(), sel.vertices_end());
			obj->selection_changed();
		}

		const char* get_name()		{return "Deselect All";}
		const char* get_tooltip()	{return "Deselects all vertices of the current grid";}
		const char* get_group()		{return "Selection | Vertices";}
};

class ToolSelectAllEdges : public ITool
{
	public:
		void execute(LGObject* obj, QWidget*){
			ug::Grid& grid = obj->get_grid();
			ug::Selector& sel = obj->get_selector();
			sel.select(grid.edges_begin(), grid.edges_end());
			obj->selection_changed();
		}

		const char* get_name()		{return "All Edges";}
		const char* get_tooltip()	{return "Selects all edges of the current grid";}
		const char* get_group()		{return "Selection | Edges";}
};

class ToolDeselectAllEdges : public ITool
{
	public:
		void execute(LGObject* obj, QWidget*){
			ug::Selector& sel = obj->get_selector();
			sel.deselect(sel.edges_begin(), sel.edges_end());
			obj->selection_changed();
		}

		const char* get_name()		{return "Deselect All";}
		const char* get_tooltip()	{return "Deselects all edges of the current grid";}
		const char* get_group()		{return "Selection | Edges";}
};

class ToolSelectAllFaces : public ITool
{
	public:
		void execute(LGObject* obj, QWidget*){
			ug::Grid& grid = obj->get_grid();
			ug::Selector& sel = obj->get_selector();
			sel.select(grid.faces_begin(), grid.faces_end());
			obj->selection_changed();
		}

		const char* get_name()		{return "All Faces";}
		const char* get_tooltip()	{return "Selects all faces of the current grid";}
		const char* get_group()		{return "Selection | Faces";}
};

class ToolDeselectAllFaces : public ITool
{
	public:
		void execute(LGObject* obj, QWidget*){
			ug::Selector& sel = obj->get_selector();
			sel.deselect(sel.faces_begin(), sel.faces_end());
			obj->selection_changed();
		}

		const char* get_name()		{return "Deselect All";}
		const char* get_tooltip()	{return "Deselects all faces of the current grid";}
		const char* get_group()		{return "Selection | Faces";}
};

class ToolSelectAllVolumes : public ITool
{
	public:
		void execute(LGObject* obj, QWidget*){
			ug::Grid& grid = obj->get_grid();
			ug::Selector& sel = obj->get_selector();
			sel.select(grid.volumes_begin(), grid.volumes_end());
			obj->selection_changed();
		}

		const char* get_name()		{return "All Volumes";}
		const char* get_tooltip()	{return "Selects all volumes of the current grid";}
		const char* get_group()		{return "Selection | Volumes";}
};

class ToolDeselectAllVolumes : public ITool
{
	public:
		void execute(LGObject* obj, QWidget*){
			ug::Selector& sel = obj->get_selector();
			sel.deselect(sel.volumes_begin(), sel.volumes_end());
			obj->selection_changed();
		}

		const char* get_name()		{return "Deselect All";}
		const char* get_tooltip()	{return "Deselects all volumes of the current grid";}
		const char* get_group()		{return "Selection | Volumes";}
};

class ToolSelectMarkedVertices : public ITool
{
	public:
		void execute(LGObject* obj, QWidget*){
			using namespace ug;
			obj->get_selector().select(
					obj->get_crease_handler().begin<VertexBase>(REM_FIXED),
					obj->get_crease_handler().end<VertexBase>(REM_FIXED));
			obj->marks_changed();
		}

		const char* get_name()		{return "Marked Vertices";}
		const char* get_tooltip()	{return "Selects vertices which are marked.";}
		const char* get_group()		{return "Selection | Vertices";}
};

class ToolSelectMarkedEdges : public ITool
{
	public:
		void execute(LGObject* obj, QWidget*){
			using namespace ug;
			obj->get_selector().select(
					obj->get_crease_handler().begin<EdgeBase>(REM_CREASE),
					obj->get_crease_handler().end<EdgeBase>(REM_CREASE));
			obj->marks_changed();
		}

		const char* get_name()		{return "Marked Edges";}
		const char* get_tooltip()	{return "Selects edges which are marked.";}
		const char* get_group()		{return "Selection | Edges";}
};

class ToolSelectUnorientableVolumes : public ITool
{
	public:
		void execute(LGObject* obj, QWidget*){
			using namespace ug;
			Grid& grid = obj->get_grid();
			Selector& sel = obj->get_selector();

			Grid::AttachmentAccessor<VertexBase, APosition> aaPos(grid, aPosition);

			int numUnorientable = 0;

		//	iterate over all volumes. Check whether the orientation can be determined.
			for(VolumeIterator iter = grid.volumes_begin();
				iter != grid.volumes_end(); ++iter)
			{
				Volume* v = *iter;
			//	get orientation of original volume
				bool bOriented = CheckOrientation(v, aaPos);
			//	flip the volume
				grid.flip_orientation(v);

			//	if orientations match, then the volume can not be oriented.
				if(bOriented == CheckOrientation(v, aaPos)){
					sel.select(v);
					++numUnorientable;
				}

			//	reflip orientation
				grid.flip_orientation(v);
			}

			UG_LOG("Unorientable volumes: " << numUnorientable << "\n");
			obj->selection_changed();
		}

		const char* get_name()		{return "Unorientable Volumes";}
		const char* get_tooltip()	{return "Selects all volumes whose orientation can not be determined";}
		const char* get_group()		{return "Selection | Volumes";}
};

class ToolExtendSelection : public ITool
{
	public:
		void execute(LGObject* obj, QWidget* widget){
			ToolWidget* dlg = dynamic_cast<ToolWidget*>(widget);
			using namespace ug;

			int neighborhoodSize = 1;
			if(dlg){
				neighborhoodSize = dlg->to_int(0);
			}

			Selector& sel = obj->get_selector();
			ExtendSelection(sel, (size_t)neighborhoodSize);

			obj->selection_changed();
		}

		const char* get_name()		{return "Extend Selection";}
		const char* get_tooltip()	{return "Selects neighbors of selected elements.";}
		const char* get_group()		{return "Selection";}

		ToolWidget* get_dialog(QWidget* parent){
			ToolWidget *dlg = new ToolWidget(get_name(), parent, this,
											IDB_APPLY | IDB_OK | IDB_CLOSE);
			dlg->addSpinBox(tr("neighborhood size:"), 0, 1.e9, 1, 1, 0);
			return dlg;
		}
};

class ToolSelectVertexByIndex : public ITool
{
	public:
		void execute(LGObject* obj, QWidget* widget){
			ToolWidget* dlg = dynamic_cast<ToolWidget*>(widget);
			if(dlg){
				ug::Grid& grid = obj->get_grid();
				int index = dlg->to_int(0);
				int counter = 0;

				ug::VertexBaseIterator iter = grid.begin<ug::VertexBase>();
				while(counter < index && iter != grid.end<ug::VertexBase>()){
					++counter;
					++iter;
				}

				if(counter == index){
					obj->get_selector().select(*iter);
					obj->selection_changed();
				}
			}
		}

		const char* get_name()		{return "Vertex By Index";}
		const char* get_tooltip()	{return "Selects a vertex given its index.";}
		const char* get_group()		{return "Selection | Vertices";}

		ToolWidget* get_dialog(QWidget* parent){
			ToolWidget *dlg = new ToolWidget(get_name(), parent, this,
											IDB_APPLY | IDB_OK | IDB_CLOSE);
			dlg->addSpinBox(tr("index:"), 0, 1000000000000., 0, 1, 0);
			return dlg;
		}
};

class ToolSelectEdgeByIndex : public ITool
{
	public:
		void execute(LGObject* obj, QWidget* widget){
			ToolWidget* dlg = dynamic_cast<ToolWidget*>(widget);
			if(dlg){
				ug::Grid& grid = obj->get_grid();
				int index = dlg->to_int(0);
				int counter = 0;

				ug::EdgeBaseIterator iter = grid.begin<ug::EdgeBase>();
				while(counter < index && iter != grid.end<ug::EdgeBase>()){
					++counter;
					++iter;
				}

				if(counter == index){
					obj->get_selector().select(*iter);
					obj->selection_changed();
				}
			}
		}

		const char* get_name()		{return "Edge By Index";}
		const char* get_tooltip()	{return "Selects a edge given its index.";}
		const char* get_group()		{return "Selection | Edges";}

		ToolWidget* get_dialog(QWidget* parent){
			ToolWidget *dlg = new ToolWidget(get_name(), parent, this,
											IDB_APPLY | IDB_OK | IDB_CLOSE);
			dlg->addSpinBox(tr("index:"), 0, 1000000000000., 0, 1, 0);
			return dlg;
		}
};

class ToolSelectFaceByIndex : public ITool
{
	public:
		void execute(LGObject* obj, QWidget* widget){
			ToolWidget* dlg = dynamic_cast<ToolWidget*>(widget);
			if(dlg){
				ug::Grid& grid = obj->get_grid();
				int index = dlg->to_int(0);
				int counter = 0;

				ug::FaceIterator iter = grid.begin<ug::Face>();
				while(counter < index && iter != grid.end<ug::Face>()){
					++counter;
					++iter;
				}

				if(counter == index){
					obj->get_selector().select(*iter);
					obj->selection_changed();
				}
			}
		}

		const char* get_name()		{return "Face By Index";}
		const char* get_tooltip()	{return "Selects a face given its index.";}
		const char* get_group()		{return "Selection | Faces";}

		ToolWidget* get_dialog(QWidget* parent){
			ToolWidget *dlg = new ToolWidget(get_name(), parent, this,
											IDB_APPLY | IDB_OK | IDB_CLOSE);
			dlg->addSpinBox(tr("index:"), 0, 1000000000000., 0, 1, 0);
			return dlg;
		}
};

class ToolSelectVolumeByIndex : public ITool
{
	public:
		void execute(LGObject* obj, QWidget* widget){
			ToolWidget* dlg = dynamic_cast<ToolWidget*>(widget);
			if(dlg){
				ug::Grid& grid = obj->get_grid();
				int index = dlg->to_int(0);
				int counter = 0;

				ug::VolumeIterator iter = grid.begin<ug::Volume>();
				while(counter < index && iter != grid.end<ug::Volume>()){
					++counter;
					++iter;
				}

				if(counter == index){
					obj->get_selector().select(*iter);
					obj->selection_changed();
				}
			}
		}

		const char* get_name()		{return "Volume By Index";}
		const char* get_tooltip()	{return "Selects a volume given its index.";}
		const char* get_group()		{return "Selection | Volumes";}

		ToolWidget* get_dialog(QWidget* parent){
			ToolWidget *dlg = new ToolWidget(get_name(), parent, this,
											IDB_APPLY | IDB_OK | IDB_CLOSE);
			dlg->addSpinBox(tr("index:"), 0, 1000000000000., 0, 1, 0);
			return dlg;
		}
};

class ToolSelectVertexByCoordinate : public ITool
{
	public:
		void execute(LGObject* obj, QWidget* widget){
			using namespace ug;
			CoordinatesWidget* dlg = dynamic_cast<CoordinatesWidget*>(widget);
			if(dlg){
				Grid& grid = obj->get_grid();
				Grid::VertexAttachmentAccessor<APosition> aaPos(grid, aPosition);

				vector3 coord;
				coord.x = dlg->x();
				coord.y = dlg->y();
				coord.z = dlg->z();

				VertexBase* vrt = FindClosestByCoordinate<VertexBase>(coord,
												 grid.vertices_begin(),
												 grid.vertices_end(),
												 aaPos);
				if(vrt){
				//	This message is important, since the user can see
				//	whether the correct vertex was chosen.
					UG_LOG("Selected vertex at: " << aaPos[vrt] << "\n");
					obj->get_selector().select(vrt);
					obj->selection_changed();
				}
			}
		}

		const char* get_name()		{return "Vertex By Coordinate";}
		const char* get_tooltip()	{return "Selects a vertex given a coordinate.";}
		const char* get_group()		{return "Selection | Vertices";}

		QWidget* get_dialog(QWidget* parent){
			return new CoordinatesWidget(get_name(), parent, this, false);
		}
};

class ToolSelectEdgeByCoordinate : public ITool
{
	public:
		void execute(LGObject* obj, QWidget* widget){
			using namespace ug;
			CoordinatesWidget* dlg = dynamic_cast<CoordinatesWidget*>(widget);
			if(dlg){
				Grid& grid = obj->get_grid();
				Grid::VertexAttachmentAccessor<APosition> aaPos(grid, aPosition);

				vector3 coord;
				coord.x = dlg->x();
				coord.y = dlg->y();
				coord.z = dlg->z();

				EdgeBase* e = FindClosestByCoordinate<EdgeBase>(coord,
												 grid.edges_begin(),
												 grid.edges_end(),
												 aaPos);
				if(e){
				//	This message is important, since the user can see
				//	whether the correct vertex was chosen.
					UG_LOG("Selected edge with center: " << CalculateCenter(e, aaPos) << "\n");
					obj->get_selector().select(e);
					obj->selection_changed();
				}
				else{
					UG_LOG("No matching edge found.\n");
				}
			}
		}

		const char* get_name()		{return "Edge By Coordinate";}
		const char* get_tooltip()	{return "Selects the edge whose center is closest to the specified coordinate.";}
		const char* get_group()		{return "Selection | Edges";}

		QWidget* get_dialog(QWidget* parent){
			return new CoordinatesWidget(get_name(), parent, this, false);
		}
};

class ToolSelectFaceByCoordinate : public ITool
{
	public:
		void execute(LGObject* obj, QWidget* widget){
			using namespace ug;
			CoordinatesWidget* dlg = dynamic_cast<CoordinatesWidget*>(widget);
			if(dlg){
				Grid& grid = obj->get_grid();
				Grid::VertexAttachmentAccessor<APosition> aaPos(grid, aPosition);

				vector3 coord;
				coord.x = dlg->x();
				coord.y = dlg->y();
				coord.z = dlg->z();

				Face* f = FindClosestByCoordinate<Face>(coord,
												 grid.faces_begin(),
												 grid.faces_end(),
												 aaPos);
				if(f){
				//	This message is important, since the user can see
				//	whether the correct vertex was chosen.
					UG_LOG("Selected face with center: " << CalculateCenter(f, aaPos) << "\n");
					obj->get_selector().select(f);
					obj->selection_changed();
				}
				else{
					UG_LOG("No matching face found.\n");
				}
			}
		}

		const char* get_name()		{return "Face By Coordinate";}
		const char* get_tooltip()	{return "Selects the face whose center is closest to the specified coordinate.";}
		const char* get_group()		{return "Selection | Faces";}

		QWidget* get_dialog(QWidget* parent){
			return new CoordinatesWidget(get_name(), parent, this, false);
		}
};

class ToolSelectVolumeByCoordinate : public ITool
{
	public:
		void execute(LGObject* obj, QWidget* widget){
			using namespace ug;
			CoordinatesWidget* dlg = dynamic_cast<CoordinatesWidget*>(widget);
			if(dlg){
				Grid& grid = obj->get_grid();
				Grid::VertexAttachmentAccessor<APosition> aaPos(grid, aPosition);

				vector3 coord;
				coord.x = dlg->x();
				coord.y = dlg->y();
				coord.z = dlg->z();

				Volume* v = FindClosestByCoordinate<Volume>(coord,
												 grid.volumes_begin(),
												 grid.volumes_end(),
												 aaPos);
				if(v){
				//	This message is important, since the user can see
				//	whether the correct vertex was chosen.
					UG_LOG("Selected volume with center: " << CalculateCenter(v, aaPos) << "\n");
					obj->get_selector().select(v);
					obj->selection_changed();
				}
				else{
					UG_LOG("No matching volume found.\n");
				}
			}
		}

		const char* get_name()		{return "Volume By Coordinate";}
		const char* get_tooltip()	{return "Selects the volume whose center is closest to the specified coordinate.";}
		const char* get_group()		{return "Selection | Volumes";}

		QWidget* get_dialog(QWidget* parent){
			return new CoordinatesWidget(get_name(), parent, this, false);
		}
};

template <class TElem>
static size_t SelectUnconnectedVertices(ug::Grid& grid, ug::Selector& sel)
{
	using namespace ug;
	std::vector<TElem*> elems;

	size_t numUnconnected = 0;
	for(VertexBaseIterator iter = grid.vertices_begin();
		iter != grid.vertices_end(); ++iter)
	{
		CollectAssociated(elems, grid, *iter);
		if(elems.size() == 0){
			sel.select(*iter);
			numUnconnected++;
		}
	}
	return numUnconnected;
}

class ToolSelectUnconnectedVertices : public ITool
{
	public:
		void execute(LGObject* obj, QWidget* widget){
			using namespace ug;
			ToolWidget* dlg = dynamic_cast<ToolWidget*>(widget);
			if(dlg){
				Grid& grid = obj->get_grid();
				Selector& sel = obj->get_selector();

				int choice = dlg->to_int(0);

				size_t numUnconnected = 0;
				switch(choice){
					case 0:	numUnconnected = SelectUnconnectedVertices<EdgeBase>(grid, sel); break;
					case 1:	numUnconnected = SelectUnconnectedVertices<Face>(grid, sel); break;
					case 2:	numUnconnected = SelectUnconnectedVertices<Volume>(grid, sel); break;
				}

				UG_LOG(numUnconnected << " unconnected vertices found.\n");
				if(numUnconnected > 0)
					obj->selection_changed();
			}
		}

		const char* get_name()		{return "Unconnected Vertices";}
		const char* get_tooltip()	{return "Selects vertices which are not connected to the given element type.";}
		const char* get_group()		{return "Selection | Vertices";}

		ToolWidget* get_dialog(QWidget* parent){
			ToolWidget *dlg = new ToolWidget(get_name(), parent, this,
											IDB_APPLY | IDB_OK | IDB_CLOSE);
			QStringList entries;
			entries.push_back("edges");
			entries.push_back("faces");
			entries.push_back("volumes");

			dlg->addComboBox("not connected to:", entries, 0);
			return dlg;
		}
};

class ToolSelectLastUnselectedFace : public ITool
{
	public:
		void execute(LGObject* obj, QWidget*){
			ug::Grid& grid = obj->get_grid();
			ug::Selector& sel = obj->get_selector();
			ug::Face* lastUnselected = NULL;

			for(ug::FaceIterator iter = grid.faces_begin();
				iter != grid.faces_end(); ++iter)
			{
				if(!sel.is_selected(*iter))
					lastUnselected = *iter;
			}

			if(lastUnselected)
				sel.select(lastUnselected);

			obj->selection_changed();
		}

		const char* get_name()		{return "Last Unselected Face";}
		const char* get_tooltip()	{return "Selects the last unselected face. Mainly used for debugging.";}
		const char* get_group()		{return "Selection | Faces";}
};

class ToolSelectSubset : public ITool
{
	public:
		void execute(LGObject* obj, QWidget* widget){
			ToolWidget* dlg = dynamic_cast<ToolWidget*>(widget);
			if(dlg){
				ug::SubsetHandler& sh = obj->get_subset_handler();
				ug::Selector& sel = obj->get_selector();

				bool selVrts = true;
				bool selEdges = true;
				bool selFaces = true;
				bool selVols = true;
				int si = 0;

				if(dlg){
					selVrts = dlg->to_bool(0);
					selEdges = dlg->to_bool(1);
					selFaces = dlg->to_bool(2);
					selVols = dlg->to_bool(3);
					si = dlg->to_int(4);
				}

				if(si >= 0){
					if(selVrts)
						sel.select(sh.begin<ug::VertexBase>(si), sh.end<ug::VertexBase>(si));
					if(selEdges)
						sel.select(sh.begin<ug::EdgeBase>(si), sh.end<ug::EdgeBase>(si));
					if(selFaces)
						sel.select(sh.begin<ug::Face>(si), sh.end<ug::Face>(si));
					if(selVols)
						sel.select(sh.begin<ug::Volume>(si), sh.end<ug::Volume>(si));
				}
				else{
					ug::Grid& grid = obj->get_grid();
				//	subset -1 has to be selected. Those are not directly accessible.
					if(selVrts){
						for(ug::VertexBaseIterator iter = grid.vertices_begin();
							iter != grid.vertices_end(); ++iter)
						{
							if(sh.get_subset_index(*iter) == -1)
								sel.select(*iter);
						}
					}
					if(selEdges){
						for(ug::EdgeBaseIterator iter = grid.edges_begin();
							iter != grid.edges_end(); ++iter)
						{
							if(sh.get_subset_index(*iter) == -1)
								sel.select(*iter);
						}
					}
					if(selFaces){
						for(ug::FaceIterator iter = grid.faces_begin();
							iter != grid.faces_end(); ++iter)
						{
							if(sh.get_subset_index(*iter) == -1)
								sel.select(*iter);
						}
					}
					if(selVols){
						for(ug::VolumeIterator iter = grid.volumes_begin();
							iter != grid.volumes_end(); ++iter)
						{
							if(sh.get_subset_index(*iter) == -1)
								sel.select(*iter);
						}
					}
				}

				obj->selection_changed();
			}
		}

		const char* get_name()		{return "Subset";}
		const char* get_tooltip()	{return "Selects all elements of a subset.";}
		const char* get_group()		{return "Selection";}

		ToolWidget* get_dialog(QWidget* parent){
			ToolWidget *dlg = new ToolWidget(get_name(), parent, this,
											IDB_APPLY | IDB_OK | IDB_CLOSE);
			ToolDialog_AddElementCheckBoxes(dlg, TDUE_ALL);
			dlg->addSpinBox(tr("subset index:"), -1, 1e9, 0, 1, 0);
			return dlg;
		}
};

class ToolSelectSubsetBoundary : public ITool
{
	public:
		void execute(LGObject* obj, QWidget* widget){
			using namespace ug;
			ToolWidget* dlg = dynamic_cast<ToolWidget*>(widget);
			if(dlg){
				ug::SubsetHandler& sh = obj->get_subset_handler();
				ug::Selector& sel = obj->get_selector();

				bool edgeBnds = true;
				bool faceBnds = true;
				bool volBnds = true;
				int si = 0;

				if(dlg){
					edgeBnds = dlg->to_bool(0);
					faceBnds = dlg->to_bool(1);
					volBnds = dlg->to_bool(2);
					si = dlg->to_int(3);
				}

				if(edgeBnds)
					SelectAreaBoundary(sel, sh.begin<EdgeBase>(si), sh.end<EdgeBase>(si));
				if(faceBnds)
					SelectAreaBoundary(sel, sh.begin<Face>(si), sh.end<Face>(si));
				if(volBnds)
					SelectAreaBoundary(sel, sh.begin<Volume>(si), sh.end<Volume>(si));

				obj->selection_changed();
			}
		}

		const char* get_name()		{return "Subset Boundary";}
		const char* get_tooltip()	{return "Selects the boundary of a subset.";}
		const char* get_group()		{return "Selection";}

		ToolWidget* get_dialog(QWidget* parent){
			ToolWidget *dlg = new ToolWidget(get_name(), parent, this,
											IDB_APPLY | IDB_OK | IDB_CLOSE);
			dlg->addCheckBox("bounds of edges", true);
			dlg->addCheckBox("bounds of faces", true);
			dlg->addCheckBox("bounds of volumes", true);
			dlg->addSpinBox(tr("subset index:"), -1, 1e9, 0, 1, 0);
			return dlg;
		}
};

template <class TGeomObj>
static void SelectUnassignedElements(ug::Grid& grid, ug::SubsetHandler& sh, ug::Selector& sel)
{
	typedef typename ug::geometry_traits<TGeomObj>::iterator	iterator;
	for(iterator iter = grid.begin<TGeomObj>(); iter != grid.end<TGeomObj>(); ++iter)
	{
		if(sh.get_subset_index(*iter) == -1){
			sel.select(*iter);
		}
	}
}

class ToolSelectUnassignedElements : public ITool
{
	public:
		void execute(LGObject* obj, QWidget* widget){
			ToolWidget* dlg = dynamic_cast<ToolWidget*>(widget);
			if(dlg){
				ug::Grid& grid = obj->get_grid();
				ug::SubsetHandler& sh = obj->get_subset_handler();
				ug::Selector& sel = obj->get_selector();

				bool selVrts = true;
				bool selEdges = true;
				bool selFaces = true;
				bool selVols = true;

				if(dlg){
					selVrts = dlg->to_bool(0);
					selEdges = dlg->to_bool(1);
					selFaces = dlg->to_bool(2);
					selVols = dlg->to_bool(3);
				}


				if(selVrts)
					SelectUnassignedElements<ug::VertexBase>(grid, sh, sel);
				if(selEdges)
					SelectUnassignedElements<ug::EdgeBase>(grid, sh, sel);
				if(selFaces)
					SelectUnassignedElements<ug::Face>(grid, sh, sel);
				if(selVols)
					SelectUnassignedElements<ug::Volume>(grid, sh, sel);

				obj->selection_changed();
			}
		}

		const char* get_name()		{return "Unassigned Elements";}
		const char* get_tooltip()	{return "Selects all elements not assigned to any subset.";}
		const char* get_group()		{return "Selection";}

		ToolWidget* get_dialog(QWidget* parent){
			ToolWidget *dlg = new ToolWidget(get_name(), parent, this,
											IDB_APPLY | IDB_OK | IDB_CLOSE);
			ToolDialog_AddElementCheckBoxes(dlg, TDUE_ALL);
			return dlg;
		}
};

class ToolInvertSelection : public ITool
{
	public:
		void execute(LGObject* obj, QWidget* widget){
			using namespace ug;
			ToolWidget* dlg = dynamic_cast<ToolWidget*>(widget);
			if(dlg){
				Grid& grid = obj->get_grid();
				Selector& sel = obj->get_selector();

				bool invVrts = true;
				bool invEdges = true;
				bool invFaces = true;
				bool invVols = true;

				if(dlg){
					invVrts = dlg->to_bool(0);
					invEdges = dlg->to_bool(1);
					invFaces = dlg->to_bool(2);
					invVols = dlg->to_bool(3);
				}

				if(invVrts)
					ug::InvertSelection(sel, grid.begin<VertexBase>(),
										grid.end<VertexBase>());

				if(invEdges)
					ug::InvertSelection(sel, grid.begin<EdgeBase>(),
										grid.end<EdgeBase>());

				if(invFaces)
					ug::InvertSelection(sel, grid.begin<Face>(),
										grid.end<Face>());

				if(invVols)
					ug::InvertSelection(sel, grid.begin<Volume>(),
										grid.end<Volume>());

				obj->selection_changed();
			}
		}

		const char* get_name()		{return "Invert Selection";}
		const char* get_tooltip()	{return "Inverts current selection.";}
		const char* get_group()		{return "Selection";}

		ToolWidget* get_dialog(QWidget* parent){
			ToolWidget *dlg = new ToolWidget(get_name(), parent, this,
											IDB_APPLY | IDB_OK | IDB_CLOSE);
			ToolDialog_AddElementCheckBoxes(dlg, TDUE_ALL);
			return dlg;
		}
};

class ToolEdgeSelectionFill : public ITool
{
	public:
		void execute(LGObject* obj, QWidget*)
		{
			ug::SelectionFill<ug::EdgeBase>(obj->get_selector());
			obj->selection_changed();
		}

		const char* get_name()		{return "Edge Selection Fill";}
		const char* get_tooltip()	{return "Selects neighbours of selected edges over non-selected vertices.";}
		const char* get_group()		{return "Selection | Edges";}
};

class ToolFaceSelectionFill : public ITool
{
	public:
		void execute(LGObject* obj, QWidget*)
		{
			ug::SelectionFill<ug::Face>(obj->get_selector());
			obj->selection_changed();
		}

		const char* get_name()		{return "Face Selection Fill";}
		const char* get_tooltip()	{return "Selects neighbours of selected faces over non-selected edges.";}
		const char* get_group()		{return "Selection | Faces";}
};

class ToolVolumeSelectionFill : public ITool
{
	public:
		void execute(LGObject* obj, QWidget*)
		{
			ug::SelectionFill<ug::Volume>(obj->get_selector());
			obj->selection_changed();
		}

		const char* get_name()		{return "Volume Selection Fill";}
		const char* get_tooltip()	{return "Selects neighbours of selected volumes over non-selected faces.";}
		const char* get_group()		{return "Selection | Volumes";}
};

class ToolSelectSelectionBoundary : public ITool
{
	public:
		void execute(LGObject* obj, QWidget*)
		{
			using namespace ug;

			Selector& sel = obj->get_selector();

			if(sel.num<Volume>() > 0)
				SelectAreaBoundary(sel, sel.begin<Volume>(), sel.end<Volume>());
			if(sel.num<Face>() > 0)
				SelectAreaBoundary(sel, sel.begin<Face>(), sel.end<Face>());

			obj->selection_changed();
		}

		const char* get_name()		{return "Selection Boundary";}
		const char* get_tooltip()	{return "Selects the boundary of the current selection.";}
		const char* get_group()		{return "Selection";}
};


class ToolSelectBentQuadrilaterals: public ITool
{
	public:
		void execute(LGObject* obj, QWidget* widget)
		{
			ToolWidget* dlg = dynamic_cast<ToolWidget*>(widget);
			using namespace ug;

		//	this is the threshold for the dot-products
			number dotThreshold = 0.95;
			if(dlg){
				dotThreshold = dlg->to_double(0);
			}

			Grid& grid = obj->get_grid();
			Selector& sel = obj->get_selector();
			Grid::AttachmentAccessor<VertexBase, APosition> aaPos(grid, aPosition);

		//	iterate over all quadrilaterals and search for bent ones
			size_t selCount = 0;
			for(QuadrilateralIterator iter = grid.begin<Quadrilateral>();
				iter != grid.end<Quadrilateral>(); ++iter)
			{
				Quadrilateral* q = *iter;

			//	we'll compare the dot-product of the normals
				vector3 n1, n2;
				CalculateTriangleNormal(n1, aaPos[q->vertex(0)],
										aaPos[q->vertex(1)], aaPos[q->vertex(2)]);
				CalculateTriangleNormal(n2, aaPos[q->vertex(2)],
										aaPos[q->vertex(3)], aaPos[q->vertex(0)]);

				number d1 = VecDot(n1, n2);

				if(d1 < dotThreshold){
					++selCount;
					sel.select(q);
				}
			}

			UG_LOG("Found " << selCount << " bent quadrilaterals\n");
			obj->selection_changed();
		}

		const char* get_name()		{return "Bent Quadrilaterals";}
		const char* get_tooltip()	{return "Selects quadrilaterals which do not lie in a plane.";}
		const char* get_group()		{return "Selection | Faces";}

		ToolWidget* get_dialog(QWidget* parent){
			ToolWidget *dlg = new ToolWidget(get_name(), parent, this,
											IDB_APPLY | IDB_OK | IDB_CLOSE);
			dlg->addSpinBox(tr("dot-threshold"), -1, 1, 0.95, 0.01, 9);
			return dlg;
		}
};

class ToolCloseSelection: public ITool
{
	public:
		void execute(LGObject* obj, QWidget*)
		{
			using namespace ug;

			Selector& sel = obj->get_selector();

			SelectAssociatedFaces(sel, sel.begin<Volume>(), sel.end<Volume>());
			SelectAssociatedEdges(sel, sel.begin<Face>(), sel.end<Face>());
			SelectAssociatedVertices(sel, sel.begin<EdgeBase>(), sel.end<EdgeBase>());


			obj->selection_changed();
		}

		const char* get_name()		{return "Close Selection";}
		const char* get_tooltip()	{return "Selects all associated elements of lower dimensions.";}
		const char* get_group()		{return "Selection";}
};


void RegisterSelectionTools(ToolManager* toolMgr)
{
	toolMgr->set_group_icon("Selection", ":images/tool_selection.png");

	toolMgr->register_tool(new ToolClearSelection);

	toolMgr->register_tool(new ToolSelectAll);
	toolMgr->register_tool(new ToolSelectAllVertices);
	toolMgr->register_tool(new ToolSelectAllEdges);
	toolMgr->register_tool(new ToolSelectAllFaces);
	toolMgr->register_tool(new ToolSelectAllVolumes);
	toolMgr->register_tool(new ToolDeselectAllVertices);
	toolMgr->register_tool(new ToolDeselectAllEdges);
	toolMgr->register_tool(new ToolDeselectAllFaces);
	toolMgr->register_tool(new ToolDeselectAllVolumes);

	toolMgr->register_tool(new ToolSelectAssociatedVertices);
	toolMgr->register_tool(new ToolSelectBoundaryVertices);
	toolMgr->register_tool(new ToolSelectInnerVertices);

	toolMgr->register_tool(new ToolSelectAssociatedEdges);
	toolMgr->register_tool(new ToolSelectSmoothEdgePath);
	toolMgr->register_tool(new ToolSelectBoundaryEdges);
	toolMgr->register_tool(new ToolSelectInnerEdges);
	toolMgr->register_tool(new ToolSelectNonManifoldEdges);

	toolMgr->register_tool(new ToolSelectShortEdges);
	toolMgr->register_tool(new ToolSelectLongEdges);

	toolMgr->register_tool(new ToolSelectAssociatedFaces);
	toolMgr->register_tool(new ToolSelectAssociatedManifoldFaces);
	toolMgr->register_tool(new ToolSelectLinkedFlatFaces);
	toolMgr->register_tool(new ToolSelectBoundaryFaces);
	toolMgr->register_tool(new ToolSelectInnerFaces);
	toolMgr->register_tool(new ToolSelectDegenerateFaces);
	toolMgr->register_tool(new ToolSelectIntersectingTriangles);
	//toolMgr->register_tool(new ToolSelectLastUnselectedFace);

	toolMgr->register_tool(new ToolSelectVertexByIndex);
	toolMgr->register_tool(new ToolSelectEdgeByIndex);
	toolMgr->register_tool(new ToolSelectFaceByIndex);
	toolMgr->register_tool(new ToolSelectVolumeByIndex);
	toolMgr->register_tool(new ToolSelectUnorientableVolumes);

	toolMgr->register_tool(new ToolExtendSelection);
	toolMgr->register_tool(new ToolSelectSubset);
	toolMgr->register_tool(new ToolSelectSubsetBoundary);
	toolMgr->register_tool(new ToolSelectUnassignedElements);
	toolMgr->register_tool(new ToolInvertSelection);

	toolMgr->register_tool(new ToolSelectVertexByCoordinate);
	toolMgr->register_tool(new ToolSelectEdgeByCoordinate);
	toolMgr->register_tool(new ToolSelectFaceByCoordinate);
	toolMgr->register_tool(new ToolSelectVolumeByCoordinate);

	toolMgr->register_tool(new ToolSelectUnconnectedVertices);

	toolMgr->register_tool(new ToolEdgeSelectionFill);
	toolMgr->register_tool(new ToolFaceSelectionFill);
	toolMgr->register_tool(new ToolVolumeSelectionFill);

	toolMgr->register_tool(new ToolSelectMarkedVertices);
	toolMgr->register_tool(new ToolSelectMarkedEdges);

	toolMgr->register_tool(new ToolSelectSelectionBoundary);

	toolMgr->register_tool(new ToolSelectBentQuadrilaterals);

	toolMgr->register_tool(new ToolCloseSelection, Qt::Key_C);
}
