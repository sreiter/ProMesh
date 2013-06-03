//	created by Sebastian Reiter
//	s.b.reiter@googlemail.com
//	y11 m01 d04

#include "app.h"
#include "standard_tools.h"

using namespace ug;

class ToolRefine : public ITool
{
	public:
		void execute(LGObject* obj, QWidget* widget){
			ToolWidget* dlg = dynamic_cast<ToolWidget*>(widget);

			Grid& grid = obj->get_grid();
			Selector& sel = obj->get_selector();
			SubsetHandler& sh = obj->get_subset_handler();
			bool siEnabled = sh.strict_inheritance_enabled();

			if(dlg)
				sh.enable_strict_inheritance(dlg->to_bool(0));

			Refine(grid, sel);

			sh.enable_strict_inheritance(siEnabled);

		//	done
			obj->geometry_changed();
		}

		const char* get_name()		{return "Refine";}
		const char* get_tooltip()	{return "Refines selected elements and builds a regular closure.";}
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

			Grid& grid = obj->get_grid();
			Selector& sel = obj->get_selector();
			SubsetHandler& sh = obj->get_subset_handler();
			bool siEnabled = sh.strict_inheritance_enabled();
			bool anisotropic = false;

			if(dlg){
				sh.enable_strict_inheritance(dlg->to_bool(0));
				anisotropic = dlg->to_bool(1);
			}

			HangingNodeRefiner_Grid refiner(grid);
			//refiner.enable_automark_objects_of_higher_dim(true);
			//refiner.enable_node_dependency_order_1(false);

			if(anisotropic){
				refiner.mark(sel.edges_begin(), sel.edges_end(), RM_ANISOTROPIC);
				refiner.mark(sel.faces_begin(), sel.faces_end(), RM_ANISOTROPIC);
				refiner.mark(sel.volumes_begin(), sel.volumes_end(), RM_ANISOTROPIC);
			}
			else{
				refiner.mark(sel.edges_begin(), sel.edges_end(), RM_REFINE);
				refiner.mark(sel.faces_begin(), sel.faces_end(), RM_REFINE);
				refiner.mark(sel.volumes_begin(), sel.volumes_end(), RM_REFINE);
			}

			refiner.refine();

			sh.enable_strict_inheritance(siEnabled);

		//	done
			obj->geometry_changed();
		}

		const char* get_name()		{return "Hanging Node Refine";}
		const char* get_tooltip()	{return "Refines selected elements using hanging nodes";}
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

			Grid& grid = obj->get_grid();
			Selector& sel = obj->get_selector();
			SubsetHandler& sh = obj->get_subset_handler();
			bool siEnabled = sh.strict_inheritance_enabled();

			if(dlg)
				sh.enable_strict_inheritance(dlg->to_bool(0));

		//	currently only triangles are supported in smooth refinement.
		//	convert all selected quads to triangles first.
			Grid::VertexAttachmentAccessor<APosition> aaPos(grid, aPosition);
			Triangulate(grid, sel.begin<Quadrilateral>(), sel.end<Quadrilateral>(), &aaPos);

		//	since we use a flat hierarchy, a temporary position attachment is required
			APosition aTmpPos;
			grid.attach_to_vertices(aTmpPos);

			RefinementCallbackSubdivisionLoop<APosition>
				refCallbackLoop(grid, aPosition, aTmpPos);

			Refine(grid, sel, &refCallbackLoop);

		//	copy position data of selected vertices
			CopyAttachments(grid, sel.begin<VertexBase>(),
								sel.end<VertexBase>(),
								aTmpPos, aPosition);


			grid.detach_from_vertices(aTmpPos);

			sh.enable_strict_inheritance(siEnabled);

		//	done
			obj->geometry_changed();
		}

		const char* get_name()		{return "Refine Smooth";}
		const char* get_tooltip()	{return "Refines selected elements using piecewise smooth refinement.";}
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

			Grid& grid = obj->get_grid();
			Selector& sel = obj->get_selector();
			SubsetHandler& sh = obj->get_subset_handler();
			bool siEnabled = sh.strict_inheritance_enabled();

			if(dlg)
				sh.enable_strict_inheritance(dlg->to_bool(0));

		//	since we use a flat hierarchy, a temporary position attachment is required
			APosition aTmpPos;
			grid.attach_to_vertices(aTmpPos);

			RefinementCallbackSubdivBoundary<APosition>
				refCallbackLoopBnd(grid, aPosition, aTmpPos);

			Refine(grid, sel, &refCallbackLoopBnd);

			sh.enable_strict_inheritance(siEnabled);

		//	copy position data of selected vertices
			CopyAttachments(grid, sel.begin<VertexBase>(),
								sel.end<VertexBase>(),
								aTmpPos, aPosition);

			grid.detach_from_vertices(aTmpPos);

		//	done
			obj->geometry_changed();
		}

		const char* get_name()		{return "Refine Smooth Boudary 2d";}
		const char* get_tooltip()	{return "Refines selected elements using smooth subdivision rules on the boundary edges.";}
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
		const char* get_tooltip()	{return "Refines selected elements using hanging nodes. Fractures are refined anisotropic.";}
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
		using namespace ug;

		size_t numIterations = 5;
		double scaleFac = 0.5;

		if(dlg){
			numIterations = dlg->to_int(0);
			scaleFac = dlg->to_double(1);
		}

		Grid& grid = obj->get_grid();

		//Grid::VertexAttachmentAccessor<APosition> aaPos(grid, aPosition);

	//	we'll use a hanging-node refiner
		RefinementCallbackFractal refCallback(grid, scaleFac);
		HangingNodeRefiner_Grid href(grid);
		href.set_refinement_callback(&refCallback);

	//	iterate for the specified number of times
		for(size_t i = 0; i < numIterations; ++i){
			if(grid.num_volumes() > 0){
			//	iterate over all faces and mark them for refinement, if they are boundary faces.
				for(FaceIterator iter = grid.faces_begin();
					iter != grid.faces_end(); ++iter)
				{
					if(IsVolumeBoundaryFace(grid, *iter)){
						href.mark(*iter);
					}

				}
			}
			else if(grid.num_faces() > 0){
			//	markall faces
				href.mark(grid.faces_begin(), grid.faces_end());
			}
			else{
			//	mark all edges
				href.mark(grid.edges_begin(), grid.edges_end());
			}

		//	refine them
			href.refine();

		//	change the scalefac
			refCallback.set_scale_fac(-0.5 * refCallback.get_scale_fac());
			//refCallback.set_scale_fac(-0.5 * refCallback.get_scale_fac());
			//refCallback.set_scale_fac(refCallback.get_scale_fac() * refCallback.get_scale_fac());

		}

	//	done
		obj->geometry_changed();
	}

	const char* get_name()		{return "Fractal Refine";}
	const char* get_tooltip()	{return "Refines the whole geometry using a fractal-refinement scheme-";}
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

			Grid& grid = obj->get_grid();
			Selector& sel = obj->get_selector();
			SubsetHandler& sh = obj->get_subset_handler();
			bool siEnabled = sh.strict_inheritance_enabled();

			if(dlg)
				sh.enable_strict_inheritance(dlg->to_bool(0));

		//	access position attachment
			Grid::VertexAttachmentAccessor<APosition> aaPos(grid, aPosition);

			std::vector<EdgeBase*> edges;
			std::vector<Face*> faces;
			std::vector<Volume*> vols;
			vols.assign(sel.begin<Volume>(), sel.end<Volume>());

		//todo: support insert center for all selections
			if(grid.num<Volume>()){
				if(sel.num<Face>() > 0){
					UG_LOG("InsertCenter for faces is currently not supported if"
							" volumes are present.\n");
				}
			}
			else
				faces.assign(sel.begin<Face>(), sel.end<Face>());

		//todo: support insert center for all selections
			if(grid.num<Face>() > 0){
				if(sel.num<EdgeBase>() > 0){
					UG_LOG("InsertCenter for edges is currently not supported if"
							" faces are present.\n");
				}
			}
			else
				edges.assign(sel.begin<EdgeBase>(), sel.end<EdgeBase>());

		//	insert centers
			for(size_t i = 0; i < vols.size(); ++i){
				Volume* vol = vols[i];
				Vertex* vrt = *grid.create<Vertex>(vol);
				aaPos[vrt] = CalculateCenter(vol, aaPos);
				InsertCenterVertex(grid, vol, vrt, true);
			}

		//	insert centers
			for(size_t i = 0; i < faces.size(); ++i){
				Face* f = faces[i];
				Vertex* vrt = *grid.create<Vertex>(f);
				aaPos[vrt] = CalculateCenter(f, aaPos);
				InsertCenterVertex(grid, f, vrt, true);
			}

		//	split edges
			for(size_t i = 0; i < edges.size(); ++i){
				EdgeBase* e = edges[i];
				vector3 center = CalculateCenter(e, aaPos);
				Vertex* vrt = SplitEdge<Vertex>(grid, e);
				aaPos[vrt] = center;
			}

			sh.enable_strict_inheritance(siEnabled);

		//	done
			obj->geometry_changed();
		}

		const char* get_name()		{return "Insert Center";}
		const char* get_tooltip()	{return "Inserts a central vertex in all selected elements.";}
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

