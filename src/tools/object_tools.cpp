// created by Sebastian Reiter
// s.b.reiter@googlemail.com
// 28.07.2011

#include <vector>
#include "app.h"
#include "standard_tools.h"

using namespace std;
using namespace ug;

class ToolMergeObjects : public ITool
{
	public:
		void execute(LGObject* obj, QWidget* widget){
			ToolWidget* dlg = dynamic_cast<ToolWidget*>(widget);

		//todo: This method makes problems, if new objects are added while
		//		the tool-dialog is opened.

		//	get parameters
			QString mergedObjName = "mergedObj";
			vector<int> selObjInds;
			bool joinSubsets = false;

			if(dlg){
				mergedObjName = dlg->to_string(0);
				selObjInds = dlg->to_index_list(1);
				joinSubsets = dlg->to_bool(2);
			}

		//	create a new empty object and merge the selected ones into it
			LGObject* mergedObj = app::createEmptyLGObject(mergedObjName.toStdString().c_str());
			Grid& mrgGrid = mergedObj->get_grid();
			SubsetHandler& mrgSH = mergedObj->get_subset_handler();

		//	The position attachment for mrgGrid
			Grid::AttachmentAccessor<VertexBase, APosition> aaPosMRG(mrgGrid, aPosition);

		//	we'll use this attachment later on on the vertices of each source grid.
			AVertexBase aVrt;

		//	iterate through all selected objects and copy their content to mergedObj
			LGScene* scene = app::getActiveScene();

			for(size_t i_obj = 0; i_obj < selObjInds.size(); ++i_obj){
				int objInd = selObjInds[i_obj];
				if(objInd >= scene->num_objects()){
					UG_LOG("Bad selection during MergeObjects. Aborting\n");
					return;
				}

				LGObject* obj = scene->get_object(objInd);
				Grid& grid = obj->get_grid();
				SubsetHandler& sh = obj->get_subset_handler();

			//	if we're joining subsets, the subsetBaseInd is always 0. If
			//	we're not joining subsets, then subsetBaseInd has to be set
			//	to the current max subset.
				int subsetBaseInd = 0;
				if(!joinSubsets)
					subsetBaseInd = mrgSH.num_subsets();

			//	we need an attachment, which tells us the associated vertex in
			//	mrgGrid for each vertex in grid.
				Grid::AttachmentAccessor<VertexBase, AVertexBase> aaVrt(grid, aVrt, true);

			//	and we need an accessor for the position attachment
				Grid::AttachmentAccessor<VertexBase, APosition> aaPos(grid, aPosition);

			//	copy vertices
				for(VertexBaseIterator iter = grid.begin<VertexBase>();
					iter != grid.end<VertexBase>(); ++iter)
				{
					VertexBase* nvrt = *mrgGrid.create_by_cloning(*iter);
					aaPosMRG[nvrt] = aaPos[*iter];
					aaVrt[*iter] = nvrt;
					mrgSH.assign_subset(nvrt, subsetBaseInd + sh.get_subset_index(*iter));
				}

			//	copy edges
				EdgeDescriptor ed;
				for(EdgeBaseIterator iter = grid.begin<EdgeBase>();
					iter != grid.end<EdgeBase>(); ++iter)
				{
					EdgeBase* eSrc = *iter;
					ed.set_vertices(aaVrt[eSrc->vertex(0)], aaVrt[eSrc->vertex(1)]);
					EdgeBase* e = *mrgGrid.create_by_cloning(eSrc, ed);
					mrgSH.assign_subset(e, subsetBaseInd + sh.get_subset_index(eSrc));
				}

			//	copy faces
				FaceDescriptor fd;
				for(FaceIterator iter = grid.begin<Face>();
					iter != grid.end<Face>(); ++iter)
				{
					Face* fSrc = *iter;
					fd.set_num_vertices(fSrc->num_vertices());
					for(size_t i = 0; i < fd.num_vertices(); ++i)
						fd.set_vertex(i, aaVrt[fSrc->vertex(i)]);

					Face* f = *mrgGrid.create_by_cloning(fSrc, fd);
					mrgSH.assign_subset(f, subsetBaseInd + sh.get_subset_index(fSrc));
				}

			//	copy volumes
				VolumeDescriptor vd;
				for(VolumeIterator iter = grid.begin<Volume>();
					iter != grid.end<Volume>(); ++iter)
				{
					Volume* vSrc = *iter;
					vd.set_num_vertices(vSrc->num_vertices());
					for(size_t i = 0; i < vd.num_vertices(); ++i)
						vd.set_vertex(i, aaVrt[vSrc->vertex(i)]);

					Volume* v = *mrgGrid.create_by_cloning(vSrc, vd);
					mrgSH.assign_subset(v, subsetBaseInd + sh.get_subset_index(vSrc));
				}

			//	remove the temporary attachment
				grid.detach_from_vertices(aVrt);

			//	now copy the names of the subset handler
			//	we overwrite old subset-infos if the name of the new one is not
			//	empty.
				for(int i_sub = 0; i_sub < sh.num_subsets(); ++i_sub){
					mrgSH.subset_info(subsetBaseInd + i_sub) = sh.subset_info(i_sub);
				}
			}

			scene->object_changed(mergedObj);
			mergedObj->geometry_changed();
			UG_LOG("The merged grid contains:\n");
			ug::PrintGridElementNumbers(mergedObj->get_grid());
		}

		const char* get_name()		{return "Merge Objects";}
		const char* get_tooltip()	{return "Merges the selected objects into a new one.";}
		const char* get_group()		{return "Object";}

		QWidget* get_dialog(QWidget* parent){
			ToolWidget *dlg = new ToolWidget(get_name(), parent, this,
											IDB_APPLY | IDB_OK | IDB_CLOSE);

		//	The name of the new object
			dlg->addTextBox(tr("new object name:"), "mergedObj");

		//	push all names of current objects
			QStringList entries;
			LGScene* scene = app::getActiveScene();

			for(int i = 0; i < scene->num_objects(); ++i){
				entries.push_back(scene->get_scene_object(i)->name());
			}
			dlg->addListBox(tr("objects:"), entries);

		//	select whether subsets should be joined
			dlg->addCheckBox(tr("join subsets:"), false);

			return dlg;
		}
};



void RegisterObjectTools(ToolManager* toolMgr)
{
	toolMgr->set_group_icon("Object", "images/tool_unknown.png");

	toolMgr->register_tool(new ToolMergeObjects);
}
