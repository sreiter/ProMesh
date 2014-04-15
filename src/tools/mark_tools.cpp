//	created by Sebastian Reiter
//	s.b.reiter@googlemail.com
//	y10 m05 d07

#include "app.h"
#include "standard_tools.h"
#include "tooltips.h"

using namespace ug;

class ToolClearMarks : public ITool
{
	public:
		void execute(LGObject* obj, QWidget*){
			obj->get_crease_handler().clear();
			obj->marks_changed();
		}

		const char* get_name()		{return "Clear Marks";}
		const char* get_tooltip()	{return TOOLTIP_CLEAR_MARKS;}
		const char* get_group()		{return "Selection | Marks";}
};

class ToolMarkCreaseEdges : public ITool
{
	public:
		void execute(LGObject* obj, QWidget* widget){
			ToolWidget* dlg = dynamic_cast<ToolWidget*>(widget);
			double minAngle = 25.;
			bool clearMarks = true;
			if(dlg){
				minAngle = dlg->to_double(0);
				clearMarks = (dlg->to_int(1) != 0);
			}

			if(clearMarks)
				obj->get_crease_handler().clear();

			ug::MarkCreaseEdges(obj->get_grid(),
							obj->get_crease_handler(),
							obj->get_grid().begin<Edge>(),
							obj->get_grid().end<Edge>(),
							REM_CREASE, minAngle);
			ug::MarkFixedCreaseVertices(obj->get_grid(),
									obj->get_crease_handler(),
									ug::REM_CREASE, ug::REM_FIXED);
			obj->marks_changed();
		}

		const char* get_name()		{return "Mark Crease Edges";}
		const char* get_tooltip()	{return TOOLTIP_MARK_CREASE_EDGES;}
		const char* get_group()		{return "Selection | Marks";}

		ToolWidget* get_dialog(QWidget* parent){
			ToolWidget *dlg = new ToolWidget(get_name(), parent, this,
									IDB_APPLY | IDB_OK | IDB_CLOSE);
			dlg->addSpinBox(tr("min crease angle:"), 0, 180, 25, 1, 0);
			dlg->addCheckBox(tr("clear marks:"), true);
			return dlg;
		}
};

class ToolMarkSelection : public ITool
{
	public:
		void execute(LGObject* obj, QWidget*){
			obj->get_crease_handler().assign_subset(
				obj->get_selector().begin<Vertex>(),
				obj->get_selector().end<Vertex>(),
				ug::REM_FIXED);
			obj->get_crease_handler().assign_subset(
				obj->get_selector().begin<Edge>(),
				obj->get_selector().end<Edge>(),
				ug::REM_CREASE);
			obj->marks_changed();
		}

		const char* get_name()		{return "Mark Selection";}
		const char* get_tooltip()	{return TOOLTIP_MARK_SELECTION;}
		const char* get_group()		{return "Selection | Marks";}
};

class ToolUnmarkSelection : public ITool
{
	public:
		void execute(LGObject* obj, QWidget*)
		{
			obj->get_crease_handler().assign_subset(
				obj->get_selector().begin<Vertex>(),
				obj->get_selector().end<Vertex>(),
				ug::REM_NONE);

			obj->get_crease_handler().assign_subset(
				obj->get_selector().begin<Edge>(),
				obj->get_selector().end<Edge>(),
				ug::REM_NONE);

			obj->marks_changed();
		}

		const char* get_name()		{return "Unmark Selection";}
		const char* get_tooltip()	{return TOOLTIP_UNMARK_SELECTION;}
		const char* get_group()		{return "Selection | Marks";}
};


void RegisterMarkTools(ToolManager* toolMgr)
{
	toolMgr->register_tool(new ToolClearMarks);
	toolMgr->register_tool(new ToolMarkSelection, Qt::Key_M);
	toolMgr->register_tool(new ToolUnmarkSelection);
	toolMgr->register_tool(new ToolMarkCreaseEdges);
}
