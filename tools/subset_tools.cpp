//	created by Sebastian Reiter
//	s.b.reiter@googlemail.com
//	y10 m07 d09

#include "app.h"
#include "standard_tools.h"

using namespace std;

class ToolAssignSubset : public ITool
{
	public:
		void execute(LGObject* obj, QWidget* widget){
			ToolWidget* dlg = dynamic_cast<ToolWidget*>(widget);
			int newIndex = 0;

			if(dlg){
				newIndex = dlg->to_int(0);
			}

			ug::Selector& sel = obj->get_selector();
			ug::SubsetHandler& sh = obj->get_subset_handler();

			sh.assign_subset(sel.begin<ug::VertexBase>(),
							 sel.end<ug::VertexBase>(), newIndex);
			sh.assign_subset(sel.begin<ug::EdgeBase>(),
							 sel.end<ug::EdgeBase>(), newIndex);
			sh.assign_subset(sel.begin<ug::Face>(),
							 sel.end<ug::Face>(), newIndex);
			sh.assign_subset(sel.begin<ug::Volume>(),
							 sel.end<ug::Volume>(), newIndex);

			obj->geometry_changed();
		}

		const char* get_name()	{return "Assign Subset";}
		const char* get_tooltip()	{return "Assigns the selected elements to a subset.";}
		const char* get_group()		{return "Subsets";}

		ToolWidget* get_dialog(QWidget* parent){
			ToolWidget *dlg = new ToolWidget(get_name(), parent, this,
											IDB_APPLY | IDB_OK | IDB_CLOSE);
			dlg->addSpinBox(tr("new subset index:"), -1, 1e+9, 0., 1., 0);
			return dlg;
		}
};

class ToolAssignSubsetColors : public ITool
{
	public:
		void execute(LGObject* obj, QWidget*){
			AssignSubsetColors(obj->get_subset_handler());
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
			ToolWidget* dlg = dynamic_cast<ToolWidget*>(widget);
			int newIndex = 0;

			if(dlg){
				newIndex = dlg->to_int(0);
			}

			ug::Grid& grid = obj->get_grid();
			ug::SubsetHandler& sh = obj->get_subset_handler();

			ug::SeparateSubsetsByLowerDimSubsets<ug::Face>(grid, sh);

			obj->geometry_changed();
		}

		const char* get_name()	{return "Separate Faces By Edge Subsets";}
		const char* get_tooltip()	{return "Assigns faces that are surrounded by a set of edge-subsets to a common subset.";}
		const char* get_group()		{return "Subsets | Separate";}
};

class ToolSeparateFacesBySelectedEdges : public ITool
{
	public:
		void execute(LGObject* obj, QWidget* widget){
			ToolWidget* dlg = dynamic_cast<ToolWidget*>(widget);
			int newIndex = 0;

			if(dlg){
				newIndex = dlg->to_int(0);
			}

			ug::Grid& grid = obj->get_grid();
			ug::SubsetHandler& sh = obj->get_subset_handler();
			ug::Selector& sel = obj->get_selector();

			ug::SeparateSubsetsByLowerDimSelection<ug::Face>(grid, sh, sel);

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
			ug::Grid& grid = obj->get_grid();
			ug::SubsetHandler& sh = obj->get_subset_handler();

			ug::SeparateSubsetsByLowerDimSubsets<ug::Volume>(grid, sh);

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
			ug::Grid& grid = obj->get_grid();
			ug::SubsetHandler& sh = obj->get_subset_handler();
			ug::Selector& sel = obj->get_selector();

			ug::SeparateSubsetsByLowerDimSelection<ug::Volume>(grid, sh, sel);

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
			ug::SubsetHandler& sh = obj->get_subset_handler();

			for(int i = 0; i < sh.num_subsets(); ++i){
				int firstFree = ug::GetMaxSubsetIndex<ug::Face>(sh) + 1;
				ug::SplitIrregularManifoldSubset(sh, i, firstFree);
			}

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

			if(newIndex != oldIndex){
				ug::SubsetHandler& sh = obj->get_subset_handler();
				sh.move_subset(oldIndex, newIndex);
			}

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

			ug::SubsetHandler& sh = obj->get_subset_handler();

			if(newIndex != oldIndex && newIndex < sh.num_subsets()
				&& oldIndex < sh.num_subsets())
			{
				sh.swap_subsets(oldIndex, newIndex);
			}

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

class EraseSubset : public ITool
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

			ug::Grid& grid = obj->get_grid();
			ug::SubsetHandler& sh = obj->get_subset_handler();

			if(index < sh.num_subsets())
			{
				if(eraseGeometry){
					grid.erase(sh.begin<ug::Volume>(index), sh.end<ug::Volume>(index));
					grid.erase(sh.begin<ug::Face>(index), sh.end<ug::Face>(index));
					grid.erase(sh.begin<ug::EdgeBase>(index), sh.end<ug::EdgeBase>(index));
					grid.erase(sh.begin<ug::VertexBase>(index), sh.end<ug::VertexBase>(index));
				}
				sh.erase_subset(index);
			}

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

class EraseEmptySubsets : public ITool
{
	public:
		void execute(LGObject* obj, QWidget*){
			ug::SubsetHandler& sh = obj->get_subset_handler();

			int i = 0;
			while(i < sh.num_subsets()){
				if(sh.empty(i))
					sh.erase_subset(i);
				else
					++i;
			}

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

			ug::AdjustSubsetsForLgmNg(obj->get_grid(), obj->get_subset_handler(),
									  keepIntfSubs);
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

			ug::AdjustSubsetsForSimulation(obj->get_subset_handler(),
										   preserveExistingSubsets);
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
			ug::SeparateFaceSubsetsByNormal(obj->get_grid(), obj->get_subset_handler());
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
			if(si < obj->get_subset_handler().num_subsets())
				ug::SeparateFaceSubsetsByNormal(obj->get_grid(), obj->get_subset_handler(),
												ug::aPosition, NULL, si);
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

			std::vector<number> intervals;
			intervals.push_back(0);
			for(int i = 1; i < numSecs; ++i)
				intervals.push_back((number)i / (number)numSecs);
			intervals.push_back(1.);

			ug::Grid& grid = obj->get_grid();
			ug::Selector& sel = obj->get_selector();
			ug::SubsetHandler& sh = obj->get_subset_handler();

			ug::AssignSubsetsByQuality(grid, sh, sel.begin<ug::Face>(),
										sel.end<ug::Face>(), intervals);

		//	log how many faces were assigned to the different subsets.
		//	since potentially only a subset of faces has been considered,
		//	we may not simply output the subset sizes.

			UG_LOG("Assigned faces to subsets:\n");
			for(size_t i = 0; i < intervals.size() - 1; ++i){
			//	count the number of selected faces in this section
				size_t counter = 0;
				for(FaceIterator iter = sel.begin<Face>(); iter != sel.end<Face>(); ++iter)
				{
					if(sh.get_subset_index(*iter) == (int)i)
						++counter;
				}

				UG_LOG("  quality " << intervals[i] << " - " << intervals[i+1] << ": \t" << counter << "\n");
			}

			UG_LOG(endl);

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

			number thresholdDot = cos(deg_to_rad(angle));

			Grid& g = obj->get_grid();
			SubsetHandler& sh = obj->get_subset_handler();

			Grid::AttachmentAccessor<VertexBase, APosition> aaPos(g, aPosition);

			vector<EdgeBase*> edges;
			vector<EdgeBase*> edges2;
			vector<Face*> faces;
			vector<Face*> assembledSubset;
			queue<Face*> queFaces;

		//	we'll use this vector to check whether we have to assign faces which
		//	we assembled to a subset to a new subset or whether it can stay where it
		//	is. The first assembled face-pack can always stay in its subset.
			vector<bool> vAssignNewSubset(sh.num_subsets(), false);

		//	the index at which we'll add new subsets (increases during the algorithm)
			int newSI = GetMaxSubsetIndex<Face>(sh) + 1;

		//	we use marks to mark all processed elements
			g.begin_marking();

		//	iterate over all faces and search for a degenerated boundary face.
			for(FaceIterator iter = g.begin<Face>(); iter != g.end<Face>(); ++iter){
				Face* f = *iter;
			//	the face may have been marked during subset assembly below
				if(g.is_marked(f))
					continue;
				g.mark(f);

				if(IsDegenerated(f, aaPos)){
					if(IsVolumeBoundaryFace(g, f)){
					//	the face is a candidate. Get subset index and push it to the queue
						int origSI = sh.get_subset_index(f);
						queFaces.push(f);
						assembledSubset.clear();

						while(!queFaces.empty()){
							Face* curFace = queFaces.front();
							queFaces.pop();
							assembledSubset.push_back(curFace);

						//	check all degenerated neighbor faces
							CollectAssociated(edges, g, curFace);

							vector3 dir(0, 0, 0);
							bool gotOne = false;
						//	the first non-degenerated edge defines the direction of the face
							for(size_t i_edge = 0; i_edge < edges.size(); ++i_edge){
								EdgeBase* e = edges[i_edge];
								if(VecDistanceSq(aaPos[e->vertex(0)], aaPos[e->vertex(1)]) >= SMALL*SMALL){
									VecSubtract(dir, aaPos[e->vertex(1)], aaPos[e->vertex(0)]);
									VecNormalize(dir, dir);
									gotOne = true;
									break;
								}
							}

						//	if we haven't found a non-degenerated edge, we won't continue.
							if(!gotOne)
								continue;

						//	now find associated degenerated faces
							for(size_t i_edge = 0; i_edge < edges.size(); ++i_edge){
								EdgeBase* e = edges[i_edge];

							//	we have to know whether the edge is degenerated or not.
								bool bDegEdge = (VecDistanceSq(aaPos[e->vertex(0)], aaPos[e->vertex(1)]) < SMALL*SMALL);

								CollectAssociated(faces, g, e);

								for(size_t i_face = 0; i_face < faces.size(); ++i_face){
									Face* nbrFace = faces[i_face];
									if(!g.is_marked(nbrFace) && sh.get_subset_index(nbrFace) == origSI){
										if(IsVolumeBoundaryFace(g, nbrFace)){
											if(IsDegenerated(nbrFace, aaPos)){
											//	if the edge was non-degenerated, it is automatically part of the
											//	assembled subset.
												if(!bDegEdge){
													g.mark(nbrFace);
													queFaces.push(nbrFace);
												}
												else{
												//	we have to compare the directions of the faces
													CollectAssociated(edges2, g, nbrFace);

													vector3 dir2(0, 0, 0);
													bool gotOne2 = false;
												//	the first non-degenerated edge defines the direction of the face
													for(size_t i_edge = 0; i_edge < edges2.size(); ++i_edge){
														EdgeBase* e = edges2[i_edge];
														if(VecDistanceSq(aaPos[e->vertex(0)], aaPos[e->vertex(1)]) >= SMALL*SMALL){
															VecSubtract(dir2, aaPos[e->vertex(1)], aaPos[e->vertex(0)]);
															VecNormalize(dir2, dir2);
															gotOne2 = true;
															break;
														}
													}

													if(gotOne2){
													//	we now got both directions. compare the angle.
														if(fabs(VecDot(dir, dir2)) >= thresholdDot){
														//	both are in the same subset
															g.mark(nbrFace);
															queFaces.push(nbrFace);
														}
													}
												}
											}
										}
									}
								}
							}

						}

					//	now add the assembledSubset to its new destination
						if(vAssignNewSubset[origSI]){
							sh.assign_subset(assembledSubset.begin(), assembledSubset.end(), newSI);
							++newSI;
						}
						else{
						//	if more degenerated faces are contained in this subset, they shall be
						//	assigned to another subset.
							vAssignNewSubset[origSI] = true;
						}
					}
				}
			}

			g.end_marking();

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



void RegisterSubsetTools(ToolManager* toolMgr)
{
	toolMgr->set_group_icon("Subsets", ":images/tool_subsets.png");

	toolMgr->register_tool(new ToolAssignSubset, Qt::Key_S, SMK_ALT);
	toolMgr->register_tool(new ToolAssignSubsetColors);
	toolMgr->register_tool(new ToolMoveSubset);
	toolMgr->register_tool(new ToolSwapSubsets);
	toolMgr->register_tool(new EraseSubset);
	toolMgr->register_tool(new EraseEmptySubsets);
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
}
