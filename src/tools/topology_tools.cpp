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
#include "tools/topology_tools.h"

using namespace ug;

class ToolResolveEdgeIntersections : public ITool
{
	public:
		void execute(LGObject* obj, QWidget* widget){
			using namespace ug;
			ToolWidget* dlg = dynamic_cast<ToolWidget*>(widget);

			number snapThreshold = SMALL;

			if(dlg){
				snapThreshold = dlg->to_double(0);
			}

			promesh::ResolveEdgeIntersection(obj, snapThreshold);

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

			number snapThreshold = SMALL;

			if(dlg){
				snapThreshold = dlg->to_double(0);
			}

			promesh::ResolveTriangleIntersections(obj, snapThreshold);

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

			number snapThreshold = SMALL;

			if(dlg){
				snapThreshold = dlg->to_double(0);
			}

			promesh::ProjectVerticesToCloseEdges(obj, snapThreshold);

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

			number snapThreshold = SMALL;

			if(dlg){
				snapThreshold = dlg->to_double(0);
			}

			promesh::ProjectVerticesToCloseFaces(obj, snapThreshold);

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

			number snapThreshold = SMALL;

			if(dlg){
				snapThreshold = dlg->to_double(0);
			}

			promesh::IntersectCloseEdges(obj, snapThreshold);

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

			promesh::EraseSelectedElements(obj, eraseUnusedVrts, eraseUnusedEdges,
										   eraseUnusedFaces);

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

			if(obj->get_selector().num<VertexBase>() == 0){
				UG_LOG("INFO in Remove Doubles: no vertices are selected. Aborting.\n");
			}
			else{
				size_t numVrtsRemoved = promesh::RemoveDoubles(obj, threshold);
				UG_LOG("removed " << numVrtsRemoved << " vertices with threshold " << threshold << ".\n");
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
			if(obj->get_selector().num<EdgeBase>() == 0){
				UG_LOG("INFO in Remove Double Edges: no edges are selected. Aborting.\n");
			}
			else{
				size_t numEdgesRemoved = promesh::RemoveDoubleEdges(obj);
				UG_LOG("removed " << numEdgesRemoved << " edges.\n");
				obj->geometry_changed();
			}
			promesh::RemoveDoubleEdges(obj);
		}

		const char* get_name()		{return "Remove Double Edges";}
		const char* get_tooltip()	{return "Removes selected doubles of selected edges.";}
		const char* get_group()		{return "Remeshing | Remove Doubles";}
};

class ToolMergeAtFirst : public ITool
{
	public:
		void execute(LGObject* obj, QWidget*){
			if(obj->get_selector().num<VertexBase>() == 0){
				UG_LOG("INFO in 'Merge at First': no vertices are selected. Aborting.\n");
			}
			else{
				promesh::MergeAtFirst(obj);
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
			if(obj->get_selector().num<VertexBase>() == 0){
				UG_LOG("INFO in 'Merge at Center': no vertices are selected. Aborting.\n");
			}
			else{
				promesh::MergeAtCenter(obj);
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
			if(obj->get_selector().num<VertexBase>() == 0){
				UG_LOG("INFO in 'Merge at Last': no vertices are selected. Aborting.\n");
			}
			else{
				promesh::MergeAtLast(obj);
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
			promesh::CollapseEdge(obj);
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
			promesh::SplitEdge(obj);

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
			promesh::SwapEdge(obj);

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
			vector3 p(0, 0, 0);
			vector3 n(0, 0, 1);
			//double threshold = 0.0001;

			if(dlg){
				p.x() = dlg->to_double(0);
				p.y() = dlg->to_double(1);
				p.z() = dlg->to_double(2);
				n.x() = dlg->to_double(3);
				n.y() = dlg->to_double(4);
				n.z() = dlg->to_double(5);
				//threshold = dlg->to_double(6);
			}

			if(VecLengthSq(n) < SMALL){
				UG_LOG("INFO in Plane Cut: Bad normal. Aborting.\n");
			}
			else{
				promesh::PlaneCut(obj, p, n);
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
			promesh::AdjustEdgeOrientation(obj);

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
			promesh::FixFaceOrientation(obj);

		//	done
			obj->geometry_changed();
		}

		const char* get_name()		{return "Fix Face Orientation";}
		const char* get_tooltip()	{return "Tries to change orientation of selected faces so that all neighbouring faces point into the same direction. Only works correctly for manifold selections.";}
		const char* get_group()		{return "Remeshing | Orientation";}
};

class ToolFixFaceSubsetOrientations : public ITool
{
	public:
		void execute(LGObject* obj, QWidget*){
			promesh::FixFaceSubsetOrientations(obj);

		//	done
			obj->geometry_changed();
		}

		const char* get_name()		{return "Fix Face Subset Orientations";}
		const char* get_tooltip()	{return "Iterates over all subset and tries to fix face orientation for each. Only works correctly for manifold subsets.";}
		const char* get_group()		{return "Remeshing | Orientation";}
};

class ToolFixVolumeOrientation : public ITool
{
	public:
		void execute(LGObject* obj, QWidget*){
			int numFlips = promesh::FixVolumeOrientation(obj);
			UG_LOG("orientation of " << numFlips << " volumes flipped.\n");

		//	done
			obj->geometry_changed();
		}

		const char* get_name()		{return "Fix Volume Orientation";}
		const char* get_tooltip()	{return "Changes orientation of selected volumes, so that they are oriented correctly.";}
		const char* get_group()		{return "Remeshing | Orientation";}
};

class ToolInvertFaceOrientation : public ITool
{
	public:
		void execute(LGObject* obj, QWidget*){
			promesh::InvertFaceOrientation(obj);

		//	done
			obj->geometry_changed();
		}

		const char* get_name()		{return "Invert Face Orientation";}
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
	toolMgr->register_tool(new ToolAdjustEdgeOrientation);
	toolMgr->register_tool(new ToolInvertFaceOrientation);
	toolMgr->register_tool(new ToolFixFaceOrientation);
	toolMgr->register_tool(new ToolFixFaceSubsetOrientations);
	toolMgr->register_tool(new ToolFixVolumeOrientation);
	toolMgr->register_tool(new ToolProjectVerticesToCloseEdges);
	toolMgr->register_tool(new ToolProjectVerticesToCloseFaces);
	toolMgr->register_tool(new ToolIntersectCloseEdges);
	toolMgr->register_tool(new ToolResolveEdgeIntersections);
	toolMgr->register_tool(new ToolResolveTriangleIntersections);
}
