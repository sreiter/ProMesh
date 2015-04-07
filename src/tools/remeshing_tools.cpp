// created by Sebastian Reiter
// s.b.reiter@googlemail.com
// 23.04.2012 (m,d,y)
 

#include "app.h"
#include "standard_tools.h"
#include "tools/remeshing_tools.h"
#include "tooltips.h"

using namespace ug;

class ToolConvertToTriangles : public ITool
{
	public:
		void execute(LGObject* obj, QWidget*){
			ConvertToTriangles(obj);
			obj->geometry_changed();
		}

		const char* get_name()		{return "Convert To Triangles";}
		const char* get_tooltip()	{return TOOLTIP_CONVERT_TO_TRIANGLES;}
		const char* get_group()		{return "Remeshing | Triangulation";}
};

class ToolTriangleFill : public ITool
{
	public:
		void execute(LGObject* obj, QWidget* widget){
			int newSubsetIndex = 0;
			bool constrainedDelaunay = true;
			number minAngle = 0;

			ToolWidget* dlg = dynamic_cast<ToolWidget*>(widget);
			if(dlg){
				newSubsetIndex = dlg->to_int(0);
				constrainedDelaunay = dlg->to_bool(1);
				minAngle = dlg->to_double(2);
			}

			promesh::TriangleFill(obj, constrainedDelaunay, minAngle, newSubsetIndex);

			obj->geometry_changed();
		}

		const char* get_name()		{return "Triangle Fill";}
		const char* get_tooltip()	{return TOOLTIP_TRIANGLE_FILL;}
		const char* get_group()		{return "Remeshing | Triangulation";}

		ToolWidget* get_dialog(QWidget* parent){
			ToolWidget *dlg = new ToolWidget(get_name(), parent, this,
											IDB_APPLY | IDB_OK | IDB_CANCEL);
			dlg->addSpinBox("new subset index:", -1, 1.e+9, 0, 1, 0);
			dlg->addCheckBox(tr("quality grid generation"), true);
			dlg->addSpinBox(tr("min angle:"), 0, 30, 20, 1, 9);
			return dlg;
		}
};


class ToolRetriangulate : public ITool
{
	public:
		void execute(LGObject* obj, QWidget* widget){

			ToolWidget* dlg = dynamic_cast<ToolWidget*>(widget);

			number minAngle = 0;
			if(dlg){
				minAngle = dlg->to_double(0);
			}

			promesh::Retriangulate(obj, minAngle);

			obj->geometry_changed();
		}

		const char* get_name()		{return "Retriangulate";}
		const char* get_tooltip()	{return TOOLTIP_RETRIANGULATE;}
		const char* get_group()		{return "Remeshing | Triangulation";}

		ToolWidget* get_dialog(QWidget* parent){
			ToolWidget *dlg = new ToolWidget(get_name(), parent, this,
											IDB_APPLY | IDB_OK | IDB_CLOSE);
			dlg->addSpinBox(tr("min angle:"), 0, 30, 20, 1, 9);
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
			bool automarkBoundaries = true;

			if(dlg){
				minEdgeLen = dlg->to_double(0);
				maxEdgeLen = dlg->to_double(1);
				numIterations = dlg->to_int(2);
				adaptive = dlg->to_bool(3);
				automarkBoundaries = dlg->to_bool(4);
			}
			
			promesh::AdjustEdgeLength(obj, minEdgeLen, maxEdgeLen, numIterations,
							 	 	  adaptive, automarkBoundaries);
			
			obj->geometry_changed();
		}

		const char* get_name()		{return "Adjust Edge Length";}
		const char* get_tooltip()	{return TOOLTIP_ADJUST_EDGE_LENGTH;}
		const char* get_group()		{return "Remeshing | Triangulation";}

		ToolWidget* get_dialog(QWidget* parent){
			ToolWidget *dlg = new ToolWidget(get_name(), parent, this,
											IDB_APPLY | IDB_OK | IDB_CLOSE);
			dlg->addSpinBox(tr("min edge length:"), 0, 1e+10, 1., 0.1, 9);
			dlg->addSpinBox(tr("max edge length:"), 0, 1e+10, 2., 0.1, 9);
			dlg->addSpinBox(tr("iterations:"), 1, 1e+10, 10, 1, 0);
			dlg->addCheckBox(tr("adaptive"), true);
			dlg->addCheckBox(tr("automark boundaries"), true);
			return dlg;
		}
};


class ToolAdaptSurfaceToCylinder : public ITool
{
	public:
		void execute(LGObject* obj, QWidget* widget){

			ToolWidget* dlg = dynamic_cast<ToolWidget*>(widget);
			double radius = 1.;
			double threshold = 0.1;

			if(dlg){
				radius = dlg->to_double(0);
				threshold = dlg->to_double(1);
			}

			promesh::AdaptSurfaceToCylinder(obj, radius, threshold);

			obj->geometry_changed();
		}

		const char* get_name()		{return "Adapt Surface to Cylinder";}
		const char* get_tooltip()	{return TOOLTIP_ADAPT_SURFACE_TO_CYLINDER;}
		const char* get_group()		{return "Remeshing | Triangulation";}

		ToolWidget* get_dialog(QWidget* parent){
			ToolWidget *dlg = new ToolWidget(get_name(), parent, this,
											IDB_APPLY | IDB_OK | IDB_CLOSE);
			dlg->addSpinBox(tr("radius:"), 0, 1e+10, 1., 0.1, 9);
			dlg->addSpinBox(tr("rim snap threshold:"), 0, 1e+10, 0.01, 0.01, 9);
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
		bool appendSubsetsAtEnd = true;
		int verbosity = 0;

		if(dlg){
			quality = (number)dlg->to_double(0);
			preserveOpt = dlg->to_int(1);
			separateVolumes = dlg->to_bool(2);
			appendSubsetsAtEnd = dlg->to_bool(3);
			verbosity = dlg->to_int(4);
		}

		bool preserveOuter = (preserveOpt >= 1);
		bool preserveAll = (preserveOpt == 2);

		promesh::Tetrahedralize(obj, quality, preserveOuter, preserveAll, separateVolumes,
					   	   	    appendSubsetsAtEnd, verbosity);

		obj->geometry_changed();
	}

	const char* get_name()		{return "Tetrahedralize";}
	const char* get_tooltip()	{return TOOLTIP_TETRAHEDRALIZE;}
	const char* get_group()		{return "Remeshing | Tetrahedra";}

	ToolWidget* get_dialog(QWidget* parent){
		ToolWidget *dlg = new ToolWidget(get_name(), parent, this,
										IDB_APPLY | IDB_OK | IDB_CANCEL);
		dlg->addSpinBox("min. dihedral angle:", 0, 18, 5, 1, 3);
		QStringList entries;
		entries.push_back(tr("nothing"));
		entries.push_back(tr("outer boundary faces"));
		entries.push_back(tr("all faces"));
		dlg->addComboBox("preserve:", entries, 0);
		dlg->addCheckBox(tr("separate volume subsets"), true);
		dlg->addCheckBox(tr("append subsets at end"), true);

		QStringList verbEntries;
		verbEntries.push_back(tr("none"));
		verbEntries.push_back(tr("high"));
		verbEntries.push_back(tr("higher"));
		verbEntries.push_back(tr("highest"));
		dlg->addComboBox("verbosity", verbEntries, 0);
		return dlg;
	}
};

class ToolAssignVolumeConstraints : public ITool
{
public:
	void execute(LGObject* obj, QWidget* widget){

		ToolWidget* dlg = dynamic_cast<ToolWidget*>(widget);

		number volConstraint = 1;
		if(dlg){
			volConstraint = (number)dlg->to_double(0);
		}

		promesh::AssignVolumeConstraints(obj, volConstraint);

		obj->geometry_changed();
	}

	const char* get_name()		{return "Assign Volume Constraints";}
	const char* get_tooltip()	{return TOOLTIP_ASSIGN_VOLUME_CONSTRAINTS;}
	const char* get_group()		{return "Remeshing | Tetrahedra";}

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
		promesh::ClearVolumeConstraints(obj);
		obj->geometry_changed();
	}

	const char* get_name()		{return "Clear Volume Constraints";}
	const char* get_tooltip()	{return TOOLTIP_CLEAR_VOLUME_CONSTRAINTS;}
	const char* get_group()		{return "Remeshing | Tetrahedra";}
};

class ToolRetetrahedralize : public ITool
{
public:
	void execute(LGObject* obj, QWidget* widget){

		ToolWidget* dlg = dynamic_cast<ToolWidget*>(widget);

		number quality = 5;
		int preserveOpt = 0;
		bool applyVolumeConstraint = false;
		int verbosity = 0;

		if(dlg){
			quality = (number)dlg->to_double(0);
			preserveOpt = dlg->to_int(1);
			applyVolumeConstraint = dlg->to_bool(2);
			verbosity = dlg->to_int(3);
		}

		bool preserveOuter = (preserveOpt >= 1);
		bool preserveAll = (preserveOpt == 2);

		promesh::Retetrahedralize(obj, quality, preserveOuter, preserveAll,
								  applyVolumeConstraint, verbosity);

		obj->geometry_changed();
	}

	const char* get_name()		{return "Retetrahedralize";}
	const char* get_tooltip()	{return TOOLTIP_RETETRAHEDRALIZE;}
	const char* get_group()		{return "Remeshing | Tetrahedra";}

	ToolWidget* get_dialog(QWidget* parent){
		ToolWidget *dlg = new ToolWidget(get_name(), parent, this,
										IDB_APPLY | IDB_OK | IDB_CANCEL);
		dlg->addSpinBox("min. dihedral angle:", 0, 18, 5, 1, 3);
		QStringList entries;
		entries.push_back(tr("nothing"));
		entries.push_back(tr("outer boundary faces"));
		entries.push_back(tr("all faces"));
		dlg->addComboBox("preserve:", entries, 0);
		dlg->addCheckBox(tr("apply volume constraint"), false);

		QStringList verbEntries;
		verbEntries.push_back(tr("none"));
		verbEntries.push_back(tr("high"));
		verbEntries.push_back(tr("higher"));
		verbEntries.push_back(tr("highest"));
		dlg->addComboBox("verbosity", verbEntries, 0);

		return dlg;
	}
};

class ToolDuplicate : public ITool
{
public:
	void execute(LGObject* obj, QWidget* widget){
		ToolWidget* dlg = dynamic_cast<ToolWidget*>(widget);

		vector3 offset(0, 0, 0);
		bool deselectOld = true;
		bool selectNew = true;
		if(dlg){
			offset.x() = dlg->to_double(0);
			offset.y() = dlg->to_double(1);
			offset.z() = dlg->to_double(2);
			deselectOld = dlg->to_bool(3);
			selectNew = dlg->to_bool(4);
		}

		promesh::Duplicate(obj, offset, deselectOld, selectNew);

		obj->geometry_changed();
	}

	const char* get_name()		{return "Duplicate";}
	const char* get_tooltip()	{return TOOLTIP_DUPLICATE;}
	const char* get_group()		{return "Remeshing";}

	QWidget* get_dialog(QWidget* parent){
		ToolWidget *dlg = new ToolWidget(get_name(), parent, this,
								IDB_APPLY | IDB_OK | IDB_CLOSE);
		dlg->addSpinBox(tr("offset x:"), -1.e+9, 1.e+9, 0, 1, 9);
		dlg->addSpinBox(tr("offset y:"), -1.e+9, 1.e+9, 0, 1, 9);
		dlg->addSpinBox(tr("offset z:"), -1.e+9, 1.e+9, 0, 1, 9);
		dlg->addCheckBox(tr("deselect old:"), true);
		dlg->addCheckBox(tr("select new:"), true);
		return dlg;
	}
};




class ToolExtrude : public ITool
{
	public:
		void execute(LGObject* obj, QWidget* widget){
		ToolWidget* dlg = dynamic_cast<ToolWidget*>(widget);
			bool createFaces = true;
			bool createVolumes = true;
			vector3 totalDir(0, 1., 0);
			int numSteps = 1;

			if(dlg){
				createFaces = dlg->to_bool(0);
				createVolumes = dlg->to_bool(1);
				totalDir.x() = dlg->to_double(2);
				totalDir.y() = dlg->to_double(3);
				totalDir.z() = dlg->to_double(4);
				numSteps = dlg->to_int(5);
			}

			promesh::Extrude(obj, totalDir, numSteps, createFaces, createVolumes);

			obj->geometry_changed();
		}

		const char* get_name()		{return "Extrude";}
		const char* get_tooltip()	{return TOOLTIP_EXTRUDE;}
		const char* get_group()		{return "Remeshing | Extrusion";}

		ToolWidget* get_dialog(QWidget* parent){
			ToolWidget *dlg = new ToolWidget(get_name(), parent, this,
											IDB_APPLY | IDB_OK | IDB_CANCEL);
			dlg->addCheckBox("create faces:", true);
			dlg->addCheckBox("create volumes:", true);
			dlg->addSpinBox("x-total:", -1.e+9, 1.e+9, 0., 0.1, 9);
			dlg->addSpinBox("y-total:", -1.e+9, 1.e+9, 0., 0.1, 9);
			dlg->addSpinBox("z-total:", -1.e+9, 1.e+9, 0., 0.1, 9);
			dlg->addSpinBox("num steps:", 1, 1.e+9, 1, 1, 0);
			return dlg;
		}
};

class ToolExtrudeCylinders : public ITool
{
public:
	void execute(LGObject* obj, QWidget* widget){
		ToolWidget* dlg = dynamic_cast<ToolWidget*>(widget);
		number height = 1.;
		number radius = 1.;
		number snapThreshold = 0.1;
		//bool createVolumes = false;

		if(dlg){
			height = (number)dlg->to_double(0);
			radius = (number)dlg->to_double(1);
			snapThreshold = (number)dlg->to_double(2);
			//createVolumes = dlg->to_bool(3);
		}

		promesh::ExtrudeCylinders(obj, height, radius, snapThreshold);

		obj->geometry_changed();
	}

	const char* get_name()		{return "Extrude Cylinders";}
	const char* get_tooltip()	{return TOOLTIP_EXTRUDE_CYLINDERS;}
	const char* get_group()		{return "Remeshing | Extrusion";}

	ToolWidget* get_dialog(QWidget* parent){
		ToolWidget *dlg = new ToolWidget(get_name(), parent, this,
										IDB_APPLY | IDB_OK | IDB_CANCEL);
		dlg->addSpinBox("height: ", -1.e+9, 1.e+9, 1, 1, 9);
		dlg->addSpinBox("radius: ", -1.e+9, 1.e+9, 1, 1, 9);
		dlg->addSpinBox(tr("rim snap threshold:"), 0, 1e+10, 0.01, 0.01, 9);
		//dlg->addCheckBox("create volumes: ", false);
		return dlg;
	}
};

class ToolCreateShrinkGeometry : public ITool
{
public:
	void execute(LGObject* obj, QWidget* widget){
		ToolWidget* dlg = dynamic_cast<ToolWidget*>(widget);
		number scale = 1.;

		if(dlg){
			scale = (number)dlg->to_double(0);
		}

		promesh::CreateShrinkGeometry(obj, scale);

		obj->geometry_changed();
	}

	const char* get_name()		{return "Create Shrink Geometry";}
	const char* get_tooltip()	{return TOOLTIP_CREATE_SHRINK_GEOMETRY;}
	const char* get_group()		{return "Remeshing";}

	ToolWidget* get_dialog(QWidget* parent){
		ToolWidget *dlg = new ToolWidget(get_name(), parent, this,
										IDB_APPLY | IDB_OK | IDB_CANCEL);
		dlg->addSpinBox("scale: ", -1.e+9, 1.e+9, 1, 1, 9);
		return dlg;
	}
};

void RegisterRemeshingTools(ToolManager* toolMgr)
{
	toolMgr->set_group_icon("Remeshing", ":images/tool_remeshing.png");

	toolMgr->register_tool(new ToolConvertToTriangles);

	toolMgr->register_tool(new ToolTriangleFill);
	toolMgr->register_tool(new ToolRetriangulate);
	toolMgr->register_tool(new ToolAdjustEdgeLength);
	toolMgr->register_tool(new ToolAdaptSurfaceToCylinder);

	toolMgr->register_tool(new ToolTetrahedralize);
    toolMgr->register_tool(new ToolAssignVolumeConstraints);
    toolMgr->register_tool(new ToolClearVolumeConstraints);
    toolMgr->register_tool(new ToolRetetrahedralize);

    toolMgr->register_tool(new ToolDuplicate);
	toolMgr->register_tool(new ToolExtrude);
	toolMgr->register_tool(new ToolExtrudeCylinders);

	toolMgr->register_tool(new ToolCreateShrinkGeometry);
}
