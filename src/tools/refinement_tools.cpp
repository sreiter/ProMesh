//	created by Sebastian Reiter
//	s.b.reiter@googlemail.com
//	y11 m01 d04

#include "app.h"
#include "standard_tools.h"
#include "tools/refinement_tools.h"
#include "tooltips.h"

using namespace ug;

class ToolRefine : public ITool
{
	public:
		void execute(LGObject* obj, QWidget* widget){
			ToolWidget* dlg = dynamic_cast<ToolWidget*>(widget);
			bool strictSubsetInheritance = false;
			if(dlg)
				strictSubsetInheritance = dlg->to_bool(0);

			promesh::Refine(obj, strictSubsetInheritance);

		//	done
			obj->geometry_changed();
		}

		const char* get_name()		{return "Refine";}
		const char* get_tooltip()	{return TOOLTIP_REFINE;}
		const char* get_group()		{return "Remeshing | Refinement";}

		ToolWidget* get_dialog(QWidget* parent){
			ToolWidget *dlg = new ToolWidget(get_name(), parent, this,
											IDB_APPLY | IDB_OK | IDB_CLOSE);
			dlg->addCheckBox(tr("strict subset inheritance:"), false);
			return dlg;
		}
};

class ToolHangingNodeRefine : public ITool
{
	public:
		void execute(LGObject* obj, QWidget* widget){
			ToolWidget* dlg = dynamic_cast<ToolWidget*>(widget);

		//	display a warning, since hanging node refines may lead to problems.
			static bool bWarned = false;
			if(!bWarned){
				bWarned = true;
				QMessageBox msg(app::getMainWindow());
				QString str = tr("WARNING: ");
				str.append(tr("Hanging Node Refine introduces special geometric objects "));
				str.append(tr("which have to be treated with care.\n"));
				str.append(tr("To avoid sever problems and crashes, you shouldn't erase any elements "));
				str.append(tr("from the grid after refinement. Aditionally you shouldn't perform any "));
				str.append(tr("further refinement other than Hanging Node Refinement."));
				msg.setText(str);
				msg.exec();
			}

			bool anisotropic = false;
			bool strictSubsetInheritance = false;

			if(dlg){
				strictSubsetInheritance = dlg->to_bool(0);
				anisotropic = dlg->to_bool(1);
			}

			promesh::HangingNodeRefine(obj, strictSubsetInheritance, anisotropic);

		//	done
			obj->geometry_changed();
		}

		const char* get_name()		{return "Hanging Node Refine";}
		const char* get_tooltip()	{return TOOLTIP_HANGING_NODE_REFINE;}
		const char* get_group()		{return "Remeshing | Refinement";}

		ToolWidget* get_dialog(QWidget* parent){
			ToolWidget *dlg = new ToolWidget(get_name(), parent, this,
											IDB_APPLY | IDB_OK | IDB_CLOSE);
			dlg->addCheckBox(tr("strict subset inheritance:"), false);
			dlg->addCheckBox(tr("anisotropic:"), false);
			return dlg;
		}
};

class ToolRefineSmooth : public ITool
{
	public:
		void execute(LGObject* obj, QWidget* widget){
			ToolWidget* dlg = dynamic_cast<ToolWidget*>(widget);

			bool strictSubsetInheritance = false;
			if(dlg)
				strictSubsetInheritance = dlg->to_bool(0);

			promesh::RefineSmooth(obj, strictSubsetInheritance);

		//	done
			obj->geometry_changed();
		}

		const char* get_name()		{return "Refine Smooth";}
		const char* get_tooltip()	{return TOOLTIP_REFINE_SMOOTH;}
		const char* get_group()		{return "Remeshing | Refinement";}

		ToolWidget* get_dialog(QWidget* parent){
			ToolWidget *dlg = new ToolWidget(get_name(), parent, this,
											IDB_APPLY | IDB_OK | IDB_CLOSE);
			dlg->addCheckBox(tr("strict subset inheritance:"), false);
			return dlg;
		}
};

class ToolRefineSmoothBoundary2D : public ITool
{
	public:
		void execute(LGObject* obj, QWidget* widget){
			ToolWidget* dlg = dynamic_cast<ToolWidget*>(widget);

			bool strictSubsetInheritance = false;
			if(dlg)
				strictSubsetInheritance = dlg->to_bool(0);

			promesh::RefineSmoothBoundary2D(obj, strictSubsetInheritance);

		//	done
			obj->geometry_changed();
		}

		const char* get_name()		{return "Refine Smooth Boudary 2d";}
		const char* get_tooltip()	{return TOOLTIP_REFINE_SMOOTH_BOUNDARY_2D;}
		const char* get_group()		{return "Remeshing | Refinement";}

		ToolWidget* get_dialog(QWidget* parent){
			ToolWidget *dlg = new ToolWidget(get_name(), parent, this,
											IDB_APPLY | IDB_OK | IDB_CLOSE);
			dlg->addCheckBox(tr("strict subset inheritance:"), false);
			return dlg;
		}
};
/*
class ToolFracturedMediaRefine : public ITool
{
	public:
		void execute(LGObject* obj, QWidget* widget){
			ToolWidget* dlg = dynamic_cast<ToolWidget*>(widget);

		//	display a warning, since hanging node refines may lead to problems.
			static bool bWarned = false;
			if(!bWarned){
				bWarned = true;
				QMessageBox msg(app::getMainWindow());
				QString str = tr("WARNING: ");
				str.append(tr("Fractured Media Refine introduces special geometric objects "));
				str.append(tr("which have to be treated with care.\n"));
				str.append(tr("To avoid sever problems and crashes, you shouldn't erase any elements "));
				str.append(tr("from the grid after refinement. Aditionally you shouldn't perform any "));
				str.append(tr("further refinement other than Fractured Media Refinement."));
				msg.setText(str);
				msg.exec();
			}

			Grid& grid = obj->get_grid();
			Selector& sel = obj->get_selector();
			SubsetHandler& sh = obj->get_subset_handler();
			bool siEnabled = sh.strict_inheritance_enabled();
			number aspectRatioThreshold = SMALL;

			if(dlg){
				sh.enable_strict_inheritance(dlg->to_bool(0));
				aspectRatioThreshold = dlg->to_double(1);
			}

			FracturedMediaRefiner<Grid, APosition> refiner(grid);
			refiner.set_position_attachment(aPosition);
			refiner.set_aspect_ratio_threshold(aspectRatioThreshold);

			{
				refiner.mark(sel.edges_begin(), sel.edges_end(), RM_REGULAR);
				refiner.mark(sel.faces_begin(), sel.faces_end(), RM_REGULAR);
				refiner.mark(sel.volumes_begin(), sel.volumes_end(), RM_REGULAR);
			}

			refiner.refine();

			sh.enable_strict_inheritance(siEnabled);

		//	done
			obj->geometry_changed();
		}

		const char* get_name()		{return "Fractured Media Refine";}
		const char* get_tooltip()	{return TOOLTIP_FRACTURED_MEDIA_REFINE;}
		const char* get_group()		{return "Remeshing | Refinement";}

		ToolWidget* get_dialog(QWidget* parent){
			ToolWidget *dlg = new ToolWidget(get_name(), parent, this,
											IDB_APPLY | IDB_OK | IDB_CLOSE);
			dlg->addCheckBox(tr("strict subset inheritance:"), false);
			dlg->addSpinBox(tr("aspect ratio threshold"), 0, 1.e9, 0.001, 0.0001, 9);
			return dlg;
		}
};
*/


class ToolCreateFractal : public ITool
{
public:
	void execute(LGObject* obj, QWidget* widget){
		ToolWidget* dlg = dynamic_cast<ToolWidget*>(widget);

		size_t numIterations = 5;
		double scaleFac = 0.5;

		if(dlg){
			numIterations = dlg->to_int(0);
			scaleFac = dlg->to_double(1);
		}

		promesh::CreateFractal(obj, numIterations, scaleFac);

	//	done
		obj->geometry_changed();
	}

	const char* get_name()		{return "Fractal Refine";}
	const char* get_tooltip()	{return TOOLTIP_CREATE_FRACTAL;}
	const char* get_group()		{return "Remeshing | Refinement";}

	ToolWidget* get_dialog(QWidget* parent){
		ToolWidget *dlg = new ToolWidget(get_name(), parent, this,
								IDB_APPLY | IDB_CLOSE);
		dlg->addSpinBox(tr("iterations"), 0, 1.e+9, 5, 1, 0);
		dlg->addSpinBox(tr("scale-factor"), -1e+9, 1e+9, 0.5, 0.1, 9);
		return dlg;
	}
};

class ToolInsertCenter : public ITool
{
	public:
		void execute(LGObject* obj, QWidget* widget){
			ToolWidget* dlg = dynamic_cast<ToolWidget*>(widget);

			bool strictSubsetInheritance = false;
			if(dlg)
				strictSubsetInheritance = dlg->to_bool(0);

			promesh::InsertCenter(obj, strictSubsetInheritance);

		//	done
			obj->geometry_changed();
		}

		const char* get_name()		{return "Insert Center";}
		const char* get_tooltip()	{return TOOLTIP_INSERT_CENTER;}
		const char* get_group()		{return "Remeshing | Refinement";}

		ToolWidget* get_dialog(QWidget* parent){
			ToolWidget *dlg = new ToolWidget(get_name(), parent, this,
											IDB_APPLY | IDB_OK | IDB_CLOSE);
			dlg->addCheckBox(tr("strict subset inheritance:"), false);
			return dlg;
		}
};

void RegisterRefinementTools(ToolManager* toolMgr)
{
	toolMgr->register_tool(new ToolRefine, Qt::Key_R, SMK_ALT);
	toolMgr->register_tool(new ToolHangingNodeRefine);
	toolMgr->register_tool(new ToolRefineSmooth);
	toolMgr->register_tool(new ToolRefineSmoothBoundary2D);
	toolMgr->register_tool(new ToolInsertCenter);
	//toolMgr->register_tool(new ToolFracturedMediaRefine);
	toolMgr->register_tool(new ToolCreateFractal);
}

