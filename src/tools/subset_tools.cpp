//	created by Sebastian Reiter
//	s.b.reiter@googlemail.com
//	y10 m07 d09

#include "app.h"
#include "standard_tools.h"
#include "tools/subset_tools.h"

using namespace std;
using namespace ug;

class ToolAssignSubset : public ITool
{
	public:
		void execute(LGObject* obj, QWidget* widget){
			ToolWidget* dlg = dynamic_cast<ToolWidget*>(widget);

			QString subsetName = "";
			int newIndex = 0;

			if(dlg){
				subsetName = dlg->to_string(0);
				newIndex = dlg->to_int(1);
			}

			promesh::AssignSubset(obj, newIndex);
			if(!subsetName.isEmpty())
				promesh::SetSubsetName(obj, newIndex, subsetName.toLocal8Bit().constData());

			dlg->setString(0, QString(""));

			obj->geometry_changed();
		}

		const char* get_name()		{return "Assign Subset";}
		const char* get_tooltip()	{return "Assigns the selected elements to a subset.";}
		const char* get_group()		{return "Subsets";}

		ToolWidget* get_dialog(QWidget* parent){
			ToolWidget *dlg = new ToolWidget(get_name(), parent, this,
											IDB_APPLY | IDB_OK | IDB_CLOSE);
			dlg->addTextBox(tr("new subset name:"), "");
			dlg->addSpinBox(tr("new subset index:"), -1, 1e+9, 0., 1., 0);
			return dlg;
		}
};

class ToolAssignSubsetColors : public ITool
{
	public:
		void execute(LGObject* obj, QWidget*){
			promesh::AssignSubsetColors(obj);
			obj->visuals_changed();
		}

		const char* get_name()		{return "Assign Subset Colors";}
		const char* get_tooltip()	{return "assigns subset colors by a procedural scheme.";}
		const char* get_group()		{return "Subsets";}
};


class ToolSeparateFacesByEdgeSubsets : public ITool
{
	public:
		void execute(LGObject* obj, QWidget* widget){
			promesh::SeparateFacesByEdgeSubsets(obj);
			obj->geometry_changed();
		}

		const char* get_name()	{return "Separate Faces By Edge Subsets";}
		const char* get_tooltip()	{return "Assigns faces that are surrounded by a set of edge-subsets to a common subset.";}
		const char* get_group()		{return "Subsets | Separate";}
};

class ToolSeparateFacesBySelectedEdges : public ITool
{
	public:
		void execute(LGObject* obj, QWidget*){
			promesh::SeparateFacesBySelectedEdges(obj);
			obj->geometry_changed();
		}

		const char* get_name()		{return "Separate Faces By Selected Edges";}
		const char* get_tooltip()	{return "Assigns faces that are surrounded by a set of selected edges to a common subset.";}
		const char* get_group()		{return "Subsets | Separate";}
};

class ToolSeparateVolumesByFaceSubsets : public ITool
{
	public:
		void execute(LGObject* obj, QWidget*){
			promesh::SeparateVolumesByFaceSubsets(obj);
			obj->geometry_changed();
		}

		const char* get_name()	{return "Separate Volumes By Face Subsets";}
		const char* get_tooltip()	{return "Assigns volumes that are surrounded by a set of face-subsets to a common subset.";}
		const char* get_group()		{return "Subsets | Separate";}
};

class ToolSeparateVolumesBySelectedFaces : public ITool
{
	public:
		void execute(LGObject* obj, QWidget*){
			promesh::SeparateVolumesBySelectedFaces(obj);
			obj->geometry_changed();
		}

		const char* get_name()	{return "Separate Volumes By Selected Faces";}
		const char* get_tooltip()	{return "Assigns volumes that are surrounded by a set of selected faces to a common subset.";}
		const char* get_group()		{return "Subsets | Separate";}
};

class ToolSeparateIrregularManifoldSubsets : public ITool
{
	public:
		void execute(LGObject* obj, QWidget*){
			promesh::SeparateIrregularManifoldSubsets(obj);
			obj->geometry_changed();
		}

		const char* get_name()	{return "Separate Irregular Manifold Faces";}
		const char* get_tooltip()	{return "After this algorithm all face-subsets are regular manifolds.";}
		const char* get_group()		{return "Subsets | Separate";}
};

class ToolMoveSubset : public ITool
{
	public:
		void execute(LGObject* obj, QWidget* widget){
			ToolWidget* dlg = dynamic_cast<ToolWidget*>(widget);
			int oldIndex = 0;
			int newIndex = 0;

			if(dlg){
				oldIndex = dlg->to_int(0);
				newIndex = dlg->to_int(1);
			}

			promesh::MoveSubset(obj, oldIndex, newIndex);

			obj->geometry_changed();
		}

		const char* get_name()		{return "Move Subset";}
		const char* get_tooltip()	{return "Moves a subset to another index.";}
		const char* get_group()		{return "Subsets";}

		ToolWidget* get_dialog(QWidget* parent){
			ToolWidget *dlg = new ToolWidget(get_name(), parent, this,
											IDB_APPLY | IDB_OK | IDB_CLOSE);
			dlg->addSpinBox(tr("old subset index:"), 0, 1e+9, 0., 1., 0);
			dlg->addSpinBox(tr("new subset index:"), 0, 1e+9, 0., 1., 0);
			return dlg;
		}
};

class ToolSwapSubsets : public ITool
{
	public:
		void execute(LGObject* obj, QWidget* widget){
			ToolWidget* dlg = dynamic_cast<ToolWidget*>(widget);
			int oldIndex = 0;
			int newIndex = 0;

			if(dlg){
				oldIndex = dlg->to_int(0);
				newIndex = dlg->to_int(1);
			}

			promesh::SwapSubsets(obj, oldIndex, newIndex);

			obj->geometry_changed();
		}

		const char* get_name()		{return "Swap Subsets";}
		const char* get_tooltip()	{return "Swaps two subsets";}
		const char* get_group()		{return "Subsets";}

		ToolWidget* get_dialog(QWidget* parent){
			ToolWidget *dlg = new ToolWidget(get_name(), parent, this,
											IDB_APPLY | IDB_OK | IDB_CLOSE);
			dlg->addSpinBox(tr("subset index 1:"), 0, 1e+9, 0., 1., 0);
			dlg->addSpinBox(tr("subset index 2:"), 0, 1e+9, 0., 1., 0);
			return dlg;
		}
};

class ToolJoinSubsets : public ITool
{
	public:
		void execute(LGObject* obj, QWidget* widget){
			ToolWidget* dlg = dynamic_cast<ToolWidget*>(widget);
			int target = 0;
			int s1 = 0;
			int s2 = 0;
			bool eraseUnused = true;

			if(dlg){
				target = dlg->to_int(0);
				s1 = dlg->to_int(1);
				s2 = dlg->to_int(2);
				eraseUnused = dlg->to_bool(3);
			}

			promesh::JoinSubsets(obj, target, s1, s2, eraseUnused);

			obj->geometry_changed();
		}

		const char* get_name()		{return "Join Subsets";}
		const char* get_tooltip()	{return "Joins two subsets";}
		const char* get_group()		{return "Subsets";}

		ToolWidget* get_dialog(QWidget* parent){
			ToolWidget *dlg = new ToolWidget(get_name(), parent, this,
											IDB_APPLY | IDB_OK | IDB_CLOSE);
			dlg->addSpinBox(tr("target subset"), 0, 1e+9, 0, 1, 0);
			dlg->addSpinBox(tr("subset 1:"), 0, 1e+9, 0., 1., 0);
			dlg->addSpinBox(tr("subset 2:"), 0, 1e+9, 0., 1., 0);
			dlg->addCheckBox(tr("remove old unused subsets"), true);
			return dlg;
		}
};

class ToolEraseSubset : public ITool
{
	public:
		void execute(LGObject* obj, QWidget* widget){
			ToolWidget* dlg = dynamic_cast<ToolWidget*>(widget);
			bool eraseGeometry = true;
			int index = 0;

			if(dlg){
				eraseGeometry = dlg->to_bool(0);
				index = dlg->to_int(1);
			}
			promesh::EraseSubset(obj, index, eraseGeometry);
			obj->geometry_changed();
		}

		const char* get_name()		{return "Erase Subset";}
		const char* get_tooltip()	{return "Erases a subset, but not its associated geometry.";}
		const char* get_group()		{return "Subsets";}

		ToolWidget* get_dialog(QWidget* parent){
			ToolWidget *dlg = new ToolWidget(get_name(), parent, this,
											IDB_APPLY | IDB_OK | IDB_CLOSE);
			dlg->addCheckBox(tr("erase geometry"), true);
			dlg->addSpinBox(tr("subset index:"), 0, 1e+9, 0., 1., 0);
			return dlg;
		}
};

class ToolEraseEmptySubsets : public ITool
{
	public:
		void execute(LGObject* obj, QWidget*){
			promesh::EraseEmptySubsets(obj);
			obj->geometry_changed();
		}

		const char* get_name()		{return "Erase Empty Subsets";}
		const char* get_tooltip()	{return "Erases Subsets, which do not contain any elements at all.";}
		const char* get_group()		{return "Subsets";}
};

class ToolAdjustSubsetsForUG3 : public ITool
{
	public:
		void execute(LGObject* obj, QWidget* widget){
			ToolWidget* dlg = dynamic_cast<ToolWidget*>(widget);
			bool keepIntfSubs = false;
			if(dlg){
				keepIntfSubs = dlg->to_bool(0);
			}
			promesh::AdjustSubsetsForUG3(obj, keepIntfSubs);
			obj->geometry_changed();
		}

		const char* get_name()		{return "Adjust Subsets For UG3";}
		const char* get_tooltip()	{return "Assigns face and edge indices so that the geometry can be used with ug3.";}
		const char* get_group()		{return "Subsets";}

		ToolWidget* get_dialog(QWidget* parent){
			ToolWidget *dlg = new ToolWidget(get_name(), parent, this,
											IDB_APPLY | IDB_OK | IDB_CLOSE);
			dlg->addCheckBox(tr("keep existing interface subsets"), false);
			return dlg;
		}
};

class ToolAdjustSubsetsForUG4 : public ITool
{
	public:
		void execute(LGObject* obj, QWidget* widget){
			ToolWidget* dlg = dynamic_cast<ToolWidget*>(widget);
			bool preserveExistingSubsets = false;
			if(dlg){
				preserveExistingSubsets = dlg->to_bool(0);
			}
			promesh::AdjustSubsetsForUG4(obj, preserveExistingSubsets);
			obj->geometry_changed();
		}

		const char* get_name()		{return "Adjust Subsets For UG4";}
		const char* get_tooltip()	{return "Adjusts subsets for simulation with ug4.";}
		const char* get_group()		{return "Subsets";}

		ToolWidget* get_dialog(QWidget* parent){
			ToolWidget *dlg = new ToolWidget(get_name(), parent, this,
											IDB_APPLY | IDB_OK | IDB_CLOSE);
			dlg->addCheckBox(tr("preserve existing subsets"), false);
			return dlg;
		}
};

class ToolSeparateFaceSubsetsByNormal : public ITool
{
	public:
		void execute(LGObject* obj, QWidget*){
			promesh::SeparateFaceSubsetsByNormal(obj);
			obj->geometry_changed();
		}

		const char* get_name()		{return "Separate Face Subsets By Normal";}
		const char* get_tooltip()	{return "Collects faces of each subset that have a similar normal and assigns them to new subsets.";}
		const char* get_group()		{return "Subsets | Separate";}
};

class ToolSeparateFaceSubsetByNormal : public ITool
{
	public:
		void execute(LGObject* obj, QWidget* widget){
			ToolWidget* dlg = dynamic_cast<ToolWidget*>(widget);
			int si = 0;
			if(dlg)
				si = dlg->to_int(0);
			promesh::SeparateFaceSubsetByNormal(obj, si);
			obj->geometry_changed();
		}

		const char* get_name()		{return "Separate Face Subset By Normal";}
		const char* get_tooltip()	{return "Collects faces of a given subset that have a similar normal and assigns them to new subsets.";}
		const char* get_group()		{return "Subsets | Separate";}

		ToolWidget* get_dialog(QWidget* parent){
			ToolWidget *dlg = new ToolWidget(get_name(), parent, this,
											IDB_APPLY | IDB_OK | IDB_CLOSE);
			dlg->addSpinBox(tr("subset index"), 0, 1.e9, 0, 1, 0);
			return dlg;
		}
};

class ToolAssignSubsetsByQuality : public ITool
{
	public:
		void execute(LGObject* obj, QWidget* widget){
			using namespace ug;
			ToolWidget* dlg = dynamic_cast<ToolWidget*>(widget);
			int numSecs = 5;

			if(dlg){
				numSecs = dlg->to_int(0);
			}
			promesh::AssignSubsetsByQuality(obj, numSecs);
			obj->geometry_changed();
		}

		const char* get_name()	{return "Assign Subsets by Quality";}
		const char* get_tooltip()	{return "Assigns the selected to a subset depending on their quality.";}
		const char* get_group()		{return "Subsets";}

		ToolWidget* get_dialog(QWidget* parent){
			ToolWidget *dlg = new ToolWidget(get_name(), parent, this,
											IDB_APPLY | IDB_OK | IDB_CLOSE);
			dlg->addSpinBox(tr("num sections"), 1, 1e+9, 5, 1., 0);
			return dlg;
		}
};

class ToolSeparateDegeneratedBoundaryFaceSubsets : public ITool
{
	public:
		void execute(LGObject* obj, QWidget* widget){
			using namespace ug;
			ToolWidget* dlg = dynamic_cast<ToolWidget*>(widget);
			number angle = 60;
			if(dlg)
				angle = (number)dlg->to_double(0);

			promesh::SeparateDegeneratedBoundaryFaceSubsets(obj, angle);

			obj->geometry_changed();
		}

		const char* get_name()		{return "Separate Degenerated Boundary Face Subsets";}
		const char* get_tooltip()	{return "Separates degenerated boundary face subsets at sharp creases.";}
		const char* get_group()		{return "Subsets | Separate";}

		ToolWidget* get_dialog(QWidget* parent){
			ToolWidget *dlg = new ToolWidget(get_name(), parent, this,
											IDB_APPLY | IDB_OK | IDB_CLOSE);
			dlg->addSpinBox(tr("max angle"), -1.e9, 1.e9, 60, 1, 9);
			return dlg;
		}
};

class ToolAssignSubsetsByElementType : public ITool
{
	public:
		void execute(LGObject* obj, QWidget*){
			promesh::AssignSubsetsByElementType(obj);
			obj->geometry_changed();
		}

		const char* get_name()	{return "Assign Subsets By Element Type";}
		const char* get_tooltip()	{return "Assigns elemets to subsets based on their concrete type.";}
		const char* get_group()		{return "Subsets";}
};


void RegisterSubsetTools(ToolManager* toolMgr)
{
	toolMgr->set_group_icon("Subsets", ":images/tool_subsets.png");

	toolMgr->register_tool(new ToolAssignSubset, Qt::Key_S, SMK_ALT);
	toolMgr->register_tool(new ToolMoveSubset);
	toolMgr->register_tool(new ToolSwapSubsets);
	toolMgr->register_tool(new ToolJoinSubsets);
	toolMgr->register_tool(new ToolEraseSubset);
	toolMgr->register_tool(new ToolAdjustSubsetsForUG3);
	toolMgr->register_tool(new ToolAdjustSubsetsForUG4);
	toolMgr->register_tool(new ToolAssignSubsetsByQuality);
	toolMgr->register_tool(new ToolSeparateFacesByEdgeSubsets);
	toolMgr->register_tool(new ToolSeparateFacesBySelectedEdges);
	toolMgr->register_tool(new ToolSeparateVolumesByFaceSubsets);
	toolMgr->register_tool(new ToolSeparateVolumesBySelectedFaces);
	toolMgr->register_tool(new ToolSeparateFaceSubsetByNormal);
	toolMgr->register_tool(new ToolSeparateFaceSubsetsByNormal);
	toolMgr->register_tool(new ToolSeparateIrregularManifoldSubsets);
	toolMgr->register_tool(new ToolSeparateDegeneratedBoundaryFaceSubsets);
	toolMgr->register_tool(new ToolAssignSubsetColors);
	toolMgr->register_tool(new ToolAssignSubsetsByElementType);
	toolMgr->register_tool(new ToolEraseEmptySubsets);
}
