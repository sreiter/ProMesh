// created by Sebastian Reiter
// s.b.reiter@googlemail.com
// 23.04.2012 (m,d,y)
 

#include "app.h"
#include "standard_tools.h"
#include "lib_grid/algorithms/remeshing/delaunay_triangulation.h"

using namespace std;


///	Volume constraints are used during Retetrahedralization.
/**	Check whether they are attached, before using them!*/
static ug::ANumber aVolumeConstraint;


class ToolConvertToTriangles : public ITool
{
	public:
		void execute(LGObject* obj, QWidget* widget){
			using namespace ug;

			ug::Grid& grid = obj->get_grid();
			ug::Selector& sel = obj->get_selector();
			Grid::VertexAttachmentAccessor<APosition> aaPos(grid, aPosition);

			Triangulate(grid, sel.begin<Quadrilateral>(),
						sel.end<Quadrilateral>(), &aaPos);

			obj->geometry_changed();
		}

		const char* get_name()		{return "Convert To Triangles";}
		const char* get_tooltip()	{return "Converts selected quadrilaterals to triangles.";}
		const char* get_group()		{return "Remeshing | Triangulation";}
};


/*
class ToolTriangleFill : public ITool
{
public:
	void execute(LGObject* obj, QWidget* widget){
		ToolWidget* dlg = dynamic_cast<ToolWidget*>(widget);
		UG_LOG("performing triangle_fill... ");
		int newSubsetIndex = 0;
		bool triangulateInside = true;

		if(dlg){
			newSubsetIndex = dlg->to_int(0);
			triangulateInside = dlg->to_bool(1);
		}

		ug::Grid& grid = obj->get_grid();
		ug::Selector& sel = obj->get_selector();

	//	clear all faces from the selector
		sel.clear<ug::Face>();

	//	temporarily enable autoselection
		bool bAutoselectionEnabled = sel.autoselection_enabled();
		sel.enable_autoselection(true);

		//size_t numTris = grid.num<ug::Triangle>();

		if(ug::TriangleFill(grid, sel.begin<ug::EdgeBase>(),
							sel.end<ug::EdgeBase>(), triangulateInside))
		{
			UG_LOG("done. Created " << sel.num<ug::Triangle>() //grid.num<ug::Triangle>() - numTris
					<< " triangles.\n");

			obj->get_subset_handler().assign_subset(sel.begin<ug::EdgeBase>(), sel.end<ug::EdgeBase>(), newSubsetIndex);
			obj->get_subset_handler().assign_subset(sel.begin<ug::Triangle>(), sel.end<ug::Triangle>(), newSubsetIndex);

			obj->geometry_changed();
		}
		else{
			UG_LOG("failed. Make sure that only one closed poly-chain with no "
					<< "self-intersections is selected.\n");
		}

	//	restore autoselection
		sel.enable_autoselection(bAutoselectionEnabled);
	}

	const char* get_name()		{return "Triangle Fill";}
	const char* get_tooltip()	{return "Fills a closed poly-chain with triangles.";}
	const char* get_group()		{return "Grid Generation";}

	ToolWidget* get_dialog(QWidget* parent){
		ToolWidget *dlg = new ToolWidget(get_name(), parent, this,
										IDB_APPLY | IDB_OK | IDB_CANCEL);
		dlg->addSpinBox("new subset index:", -1, 1.e+9, 0, 1, 0);
		dlg->addCheckBox("triangulate inside", true);
		return dlg;
	}
};
*/
class ToolTriangleFill_SweepLine : public ITool
{
	public:
		void execute(LGObject* obj, QWidget* widget){
			using namespace ug;
			int newSubsetIndex = 0;
			bool constrainedDelaunay = true;
			number minAngle = 0;

			ToolWidget* dlg = dynamic_cast<ToolWidget*>(widget);
			if(dlg){
				newSubsetIndex = dlg->to_int(0);
				constrainedDelaunay = dlg->to_bool(1);
				minAngle = dlg->to_double(2);
			}

			Grid& grid = obj->get_grid();
			SubsetHandler& sh = obj->get_subset_handler();
			Selector& sel = obj->get_selector();

		//	if no edges are selected, nothing can be triangulated
			if(sel.num<EdgeBase>() == 0){
				UG_LOG("ERROR in TriangleFill: No edges are selected!\n");
				return;
			}

		//	before triangulating, we'll make sure that no double-edges exist
		//	in the current selection.
			RemoveDoubleEdges(grid, sel.begin<EdgeBase>(), sel.end<EdgeBase>());

			Grid::VertexAttachmentAccessor<APosition> aaPos(grid, aPosition);
			AInt aInt;
			grid.attach_to_vertices(aInt);

		//	we don't want to select new edges. This would be a problem for
		//	delaunay constraints.
			bool autoselEnabled = sel.autoselection_enabled();
			sel.enable_autoselection(false);

		//	Collect all new faces in this selector.
			FaceSelector faceSel(grid);
			faceSel.enable_autoselection(true);

			if(!TriangleFill_SweepLine(grid, sel.edges_begin(),
									sel.edges_end(), aPosition, aInt,
									&sh, newSubsetIndex))
			{
				UG_LOG("TriangleFill_SweepLine failed.\n");

			// ONLY FOR DEBUGGING - BEGIN
				static int fileCounter = 1;
				string filenamePrefix = "/Users/sreiter/Desktop/failed_sweeplines/failed_sweepline_";
				//string filenamePrefix = "C:/sweep_errors/failed_sweepline_";
				stringstream ss2d, ss3d;
				ss2d << filenamePrefix << "2d_" << fileCounter << ".obj";
				ss3d << filenamePrefix << "3d_" << fileCounter << ".obj";
				++fileCounter;
				//UG_LOG("TriangleFill_SweepLine failed!\n");
				UG_LOG("Saving failed geometries to " << ss2d.str() << " and " << ss3d.str() << endl);
				SaveGridToFile(grid, ss3d.str().c_str(), aPosition);
			//	perform transformation to 2d and save that too.
				std::vector<vector3> vrts;
				for(VertexBaseIterator iter = grid.vertices_begin();
					iter != grid.vertices_end(); ++iter)
				{
					vrts.push_back(aaPos[*iter]);
				}
				std::vector<vector2> vrts2d(vrts.size());
				TransformPointSetTo2D(&vrts2d.front(), &vrts.front(),
									  vrts.size());

				size_t counter = 0;
				for(VertexBaseIterator iter = grid.vertices_begin();
					iter != grid.vertices_end(); ++iter, ++counter)
				{
					aaPos[*iter] = vector3(vrts2d[counter].x, vrts2d[counter].y, 0);
				}

				SaveGridToFile(grid, ss2d.str().c_str(), aPosition);

				counter = 0;
				for(VertexBaseIterator iter = grid.vertices_begin();
					iter != grid.vertices_end(); ++iter, ++counter)
				{
					aaPos[*iter] = vector3(vrts[counter].x, vrts[counter].y, 0);
				}
			// ONLY FOR DEBUGGING - END

			}

			grid.detach_from_vertices(aInt);

			if(constrainedDelaunay){
				QualityGridGeneration(grid, faceSel.begin(), faceSel.end(),
							 aaPos, minAngle, IsSelected(sel));
			}

			sel.enable_autoselection(autoselEnabled);

			obj->geometry_changed();
		}

		const char* get_name()		{return "Triangle Fill";}
		const char* get_tooltip()	{return "Performs triangle fill using the sweep-line algorithm.";}
		const char* get_group()		{return "Remeshing | Triangulation";}

		ToolWidget* get_dialog(QWidget* parent){
			ToolWidget *dlg = new ToolWidget(get_name(), parent, this,
											IDB_APPLY | IDB_OK | IDB_CANCEL);
			dlg->addSpinBox("new subset index:", -1, 1.e+9, 0, 1, 0);
			dlg->addCheckBox(tr("quality grid generation"), true);
			dlg->addSpinBox(tr("min angle:"), 0, 30, 0, 1, 9);
			return dlg;
		}
};


class ToolAdjustEdgeLength : public ITool
{
	public:
		void execute(LGObject* obj, QWidget* widget){
			ToolWidget* dlg = dynamic_cast<ToolWidget*>(widget);
			double minEdgeLen = 1.;
			double maxEdgeLen = 3.;
			int numIterations = 3;
			bool adaptive = true;

			if(dlg){
				minEdgeLen = dlg->to_double(0);
				maxEdgeLen = dlg->to_double(1);
				numIterations = dlg->to_int(2);
				adaptive = dlg->to_bool(3);
			}

			ug::AdjustEdgeLength(obj->get_grid(), obj->get_crease_handler(),
								minEdgeLen, maxEdgeLen,
								numIterations, true, adaptive);

			obj->geometry_changed();
		}

		const char* get_name()		{return "Adjust Edge Length";}
		const char* get_tooltip()	{return "Remeshes the active grid so that all edges approximatly have a certain length.";}
		const char* get_group()		{return "Remeshing | Triangulation";}

		ToolWidget* get_dialog(QWidget* parent){
			ToolWidget *dlg = new ToolWidget(get_name(), parent, this,
											IDB_APPLY | IDB_OK | IDB_CLOSE);
			dlg->addSpinBox(tr("min edge length:"), 0, 1e+10, 1., 0.1, 9);
			dlg->addSpinBox(tr("max edge length:"), 0, 1e+10, 2., 0.1, 9);
			dlg->addSpinBox(tr("iterations:"), 1, 1e+10, 10, 1, 0);
			dlg->addCheckBox(tr("adaptive:"), true);
			return dlg;
		}
};


class ToolQualityGridGeneration : public ITool
{
	public:
		void execute(LGObject* obj, QWidget* widget){
			using namespace ug;
			ToolWidget* dlg = dynamic_cast<ToolWidget*>(widget);

			number minAngle = 0;
			if(dlg){
				minAngle = dlg->to_double(0);
			}

			Grid& g = obj->get_grid();
			Selector& sel = obj->get_selector();
			SubsetHandler& creases = obj->get_crease_handler();

			Grid::AttachmentAccessor<VertexBase, APosition> aaPos(g, aPosition);

			QualityGridGeneration(g, sel.begin<Triangle>(), sel.end<Triangle>(),
						 	 	  aaPos, minAngle, IsNotInSubset(creases, -1));

			obj->geometry_changed();
		}

		const char* get_name()		{return "Quality Grid Generation";}
		const char* get_tooltip()	{return "Inserts vertices as required and performs Constrained Delaunay triangulation.";}
		const char* get_group()		{return "Remeshing | Triangulation";}

		ToolWidget* get_dialog(QWidget* parent){
			ToolWidget *dlg = new ToolWidget(get_name(), parent, this,
											IDB_APPLY | IDB_OK | IDB_CLOSE);
			dlg->addSpinBox(tr("min angle:"), 0, 30, 0, 1, 9);
			return dlg;
		}
};



class ToolTetrahedralize : public ITool
{
public:
	void execute(LGObject* obj, QWidget* widget){
		ToolWidget* dlg = dynamic_cast<ToolWidget*>(widget);
		number quality = 5;
		int preserveOpt = 0;
		bool separateVolumes = true;
		if(dlg){
			quality = (number)dlg->to_double(0);
			preserveOpt = dlg->to_int(1);
			separateVolumes = dlg->to_bool(2);
		}

		bool preserveOuter = (preserveOpt >= 1);
		bool preserveAll = (preserveOpt == 2);

		UG_LOG("tetrahedralizing using 'tetgen' by Hang Si... ");
		ug::Tetrahedralize(obj->get_grid(),
							obj->get_subset_handler(),
							quality, preserveOuter, preserveAll,
							ug::aPosition);
		UG_LOG("done. Created " << obj->get_grid().num<ug::Tetrahedron>()
				<< " tetrahedrons.\n");

		if(separateVolumes){
			ug::SeparateSubsetsByLowerDimSubsets<ug::Volume>(obj->get_grid(),
															 obj->get_subset_handler());
		}

		obj->geometry_changed();
	}

	const char* get_name()		{return "Tetrahedralize";}
	const char* get_tooltip()	{return "Fills a closed surface with tetrahedrons.";}
	const char* get_group()		{return "Remeshing | Tetgen";}

	ToolWidget* get_dialog(QWidget* parent){
		ToolWidget *dlg = new ToolWidget(get_name(), parent, this,
										IDB_APPLY | IDB_OK | IDB_CANCEL);
		dlg->addSpinBox("min. dihedral angle:", 0, 18, 5, 1, 3);
		QStringList entries;
		entries.push_back(tr("nothing"));
		entries.push_back(tr("outer boundary faces"));
		entries.push_back(tr("all faces"));
		dlg->addComboBox("preserve:", entries, 0);
		dlg->addCheckBox(tr("separate volume subsets:"), true);
		return dlg;
	}
};

class ToolAssignVolumeConstraints : public ITool
{
public:
	void execute(LGObject* obj, QWidget* widget){
		using namespace ug;
		ToolWidget* dlg = dynamic_cast<ToolWidget*>(widget);

		number volConstraint = 1;
		if(dlg){
			volConstraint = (number)dlg->to_double(0);
		}

		Grid& g = obj->get_grid();
		if(!g.has_volume_attachment(aVolumeConstraint)){
			g.attach_to_volumes_dv<ANumber>(aVolumeConstraint, -1., true);
		}
		Grid::AttachmentAccessor<Volume, ANumber> aaVolCon(g, aVolumeConstraint);

		Selector& sel = obj->get_selector();
		for(Selector::traits<Volume>::iterator iter = sel.begin<Volume>();
			iter != sel.end<Volume>(); ++iter)
		{
			aaVolCon[*iter] = volConstraint;
		}

		obj->geometry_changed();
	}

	const char* get_name()		{return "Assign Volume Constraints";}
	const char* get_tooltip()	{return "Assigns volume constraints to selected tetrahedrons.";}
	const char* get_group()		{return "Remeshing | Tetgen";}

	ToolWidget* get_dialog(QWidget* parent){
		ToolWidget *dlg = new ToolWidget(get_name(), parent, this,
										IDB_APPLY | IDB_OK | IDB_CANCEL);
		dlg->addSpinBox("volume constraint:", -1.e15, 1.e15, 1, 0.1, 9);
		return dlg;
	}
};

class ToolClearVolumeConstraints : public ITool
{
public:
	void execute(LGObject* obj, QWidget*){
		using namespace ug;

		Grid& g = obj->get_grid();
		if(g.has_volume_attachment(aVolumeConstraint)){
			g.detach_from_volumes(aVolumeConstraint);
		}

		obj->geometry_changed();
	}

	const char* get_name()		{return "Clear Volume Constraints";}
	const char* get_tooltip()	{return "Clears all assigned volume constraints.";}
	const char* get_group()		{return "Remeshing | Tetgen";}
};

class ToolRetetrahedralize : public ITool
{
public:
	void execute(LGObject* obj, QWidget* widget){
		using namespace ug;
		ToolWidget* dlg = dynamic_cast<ToolWidget*>(widget);

	//	we can only retetrahedralize, if volume constraints are present.
		Grid& g = obj->get_grid();
		if(!g.has_volume_attachment(aVolumeConstraint)){
			UG_LOG("Retetrahedralization can only be applied after volume "
				   "constraints have been set. Aborting.\n");
			return;
		}

		number quality = 5;
		int preserveOpt = 0;
		if(dlg){
			quality = (number)dlg->to_double(0);
			preserveOpt = dlg->to_int(1);
		}

		bool preserveOuter = (preserveOpt >= 1);
		bool preserveAll = (preserveOpt == 2);

		UG_LOG("retetrahedralizing using 'tetgen' by Hang Si... ");
		ug::Retetrahedralize(obj->get_grid(),
							obj->get_subset_handler(),
							aVolumeConstraint,
							quality,
							preserveOuter, preserveAll,
							ug::aPosition);
		UG_LOG("done.\n");

	//	remove constraints - have to be setup anew anyways
		g.detach_from_volumes(aVolumeConstraint);

		obj->geometry_changed();
	}

	const char* get_name()		{return "Retetrahedralize";}
	const char* get_tooltip()	{return "Given a tetrahedralization and volume constraints, this method adapts the tetrahedrons.";}
	const char* get_group()		{return "Remeshing | Tetgen";}

	ToolWidget* get_dialog(QWidget* parent){
		ToolWidget *dlg = new ToolWidget(get_name(), parent, this,
										IDB_APPLY | IDB_OK | IDB_CANCEL);
		dlg->addSpinBox("min. dihedral angle:", 0, 18, 5, 1, 3);
		QStringList entries;
		entries.push_back(tr("nothing"));
		entries.push_back(tr("outer boundary faces"));
		entries.push_back(tr("all faces"));
		dlg->addComboBox("preserve:", entries, 0);
		return dlg;
	}
};


void RegisterRemeshingTools(ToolManager* toolMgr)
{
	toolMgr->set_group_icon("Remeshing", ":images/tool_remeshing.png");

	toolMgr->register_tool(new ToolConvertToTriangles);

	//toolMgr->register_tool(new ToolTriangleFill);
	toolMgr->register_tool(new ToolTriangleFill_SweepLine);
	toolMgr->register_tool(new ToolAdjustEdgeLength);
	toolMgr->register_tool(new ToolQualityGridGeneration);

	toolMgr->register_tool(new ToolTetrahedralize);
    toolMgr->register_tool(new ToolAssignVolumeConstraints);
    toolMgr->register_tool(new ToolClearVolumeConstraints);
    toolMgr->register_tool(new ToolRetetrahedralize);

}