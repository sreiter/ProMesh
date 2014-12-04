// created by Sebastian Reiter
// s.b.reiter@googlemail.com
// y10 m11 d2

#include <QtGui>
#include <queue>
#include "app.h"
#include "standard_tools.h"
#include "tool_frac_to_layer.h"
#include "tooltips.h"

using namespace std;
using namespace ug;

class ToolExpandLayers2d : public ITool
{
public:
	void execute(LGObject* obj, QWidget* widget){
		using namespace ug;

		FracToLayerWidget* dlg = dynamic_cast<FracToLayerWidget*>(widget);

		if(dlg->numEntries() == 0){
			UG_LOG("No entries selected. Aborting 'Expand Layers 2d'.\n");
			return;
		}

		Grid& grid = obj->grid();
		SubsetHandler& sh = obj->subset_handler();

		ExpandFractures2d(grid, sh, dlg->entries(), dlg->degenerated_fractures(),
						  dlg->expand_outer_boundaries());

	//	done
		obj->geometry_changed();
	}

	const char* get_name()		{return "Expand Layers 2d";}
	const char* get_tooltip()	{return TOOLTIP_EXPAND_LAYERS_2D;}
	const char* get_group()		{return "Remeshing | Layers";}

	QWidget* get_dialog(QWidget* parent){
		return new FracToLayerWidget(get_name(), parent, this);
	}
};

class ToolExpandLayers3d : public ITool
{
public:
	void execute(LGObject* obj, QWidget* widget){
		using namespace ug;

		FracToLayerWidget* dlg = dynamic_cast<FracToLayerWidget*>(widget);

		if(dlg->numEntries() == 0){
			UG_LOG("No entries selected. Aborting 'Expand Layers 3d'.\n");
			return;
		}

		Grid& grid = obj->grid();
		SubsetHandler& sh = obj->subset_handler();

		//obj->selector().clear();
		ExpandFractures3d(grid, sh, dlg->entries(), dlg->degenerated_fractures(),
						dlg->expand_outer_boundaries()/*, obj->selector()*/);

	//	done
		obj->geometry_changed();
	}

	const char* get_name()		{return "Expand Layers 3d";}
	const char* get_tooltip()	{return TOOLTIP_EXPAND_LAYERS_3D;}
	const char* get_group()		{return "Remeshing | Layers";}

	QWidget* get_dialog(QWidget* parent){
		return new FracToLayerWidget(get_name(), parent, this);
	}
};


/*	WON'T LEAD TO ANYTHING, I'M AFRAID!
class ToolExpandLayers3d : public ITool
{
public:
	void execute(LGObject* obj, QWidget* widget){
		using namespace ug;

		ToolWidget* dlg = dynamic_cast<ToolWidget*>(widget);

		if(dlg){
			subset = dlg->to_int(0);
			width = dlg->to_double(1);
		}

		Grid& grid = obj->grid();
		SubsetHandler& sh = obj->subset_handler();
		Selector& sel = obj->selector();

		Grid::VertexAttachmentAccessor<APosition> aaPos(grid, aPosition);

	//todo: Make sure that the Octree can handle quadrilaterals.
		if(sel.num<Triangle>() != sel.num<Face>()){
			UG_LOG("PROBLEM in ToolExpandLayers3d: Only triangular faces are ");
			UG_LOG("currently supported in the selection. Aborting...\n");
			return;
		}

	//	Add selected faces to an octree, so that we can perform efficient projection.
		SPOctree octree = CreateOctree(grid, grid.begin<Face>(),
										grid.end<Face>(),
										10, 30, false, aPos);

		node_tree::Traverser_ProjectPoint projector;

	//	we use a vertex attachment to store the distance of each vertex to the boundary
		ANumber aDist;
		grid.attach_to_vertices(aDist);
		Grid::VertexAttachmentAccessor<ANumber> aaDist(grid, aDist);

	//	deselect all vertices, edges and volumes
		sel.deselect_all<Vertex>();
		sel.deselect_all<Edge>();
		sel.deselect_all<Volume>();

	//	select all associated vertices of selected faces
	//	all selected vertices are later on considered to be a part of the new
	//	subset.
		SelectAssociatedVertices(sel, sel.begin<Face>(), sel.end<Face>());

	//	also select associated edges.
		SelectAssociatedEdges(sel, sel.begin<Face>(), sel.end<Face>());

	//	push all selected vertices to a queue. Also assign a distance of 0.
	//	all vertices which have ever been on the queue have to be marked.
		queue<Vertex*>	queVrts;
		for(VertexIterator iter = sel.begin<Vertex>();
			iter != sel.end<Vertex>(); ++iter)
		{
			aaDist[*iter] = 0;
			queVrts.push(*iter);
		}

	//	we'll gather associated edges in this array
		vector<Edge*>	edges;

	//	The following states are used to classify the objects.
	//	UNKNOWN corresponds to unselected. is select is called without a
	//	parameter, then this automatically corresponds to INSIDE.
		const byte UNKNOWN = 0;
		const byte INSIDE = 1;
		const byte PARTLY_INSIDE = 2;
		const byte OUTSIDE = 3;

	//	the main iteration:
	//	while there are vertices in the queue, we'll check whether we can
	//	find additional adjacent edges, which can be considered as part of
	//	the new layer.
		while(!queVrts.empty()){
		//	pop a vertex from the queue
			Vertex* vrt = queVrts.front();
			queVrts.pop();

		//	collect associated edges.
			CollectAssociated(edges, grid, vrt);

		//	iterate over those edges and, if it is not selected, check whether
		//	it shall belong either
		//		- to the new subset,
		//		- partially to the new subset,
		//		- not to the new subset at all.
			for(size_t i_edge = 0; i_edge < edges.size(); ++i_edge){
				Edge* e = edges[i_edge];

			//	if the edge has already been processed, then do not process it again.
				if(sel.is_selected(e))
					continue;

			//	e = [vrt, vcon]
				Vertex* vcon = GetConnectedVertex(e, vrt);

			//	if the distance of the associated vertex to the selected faces
			//	is not yet known, determine it now.
				if(!grid.is_selected(vcon)){
					if(!projector.project(aaPos[vcon], octree)){
						UG_LOG("Projection error at vertex " << aaPos[vcon] << ". Expect strange behavior!\n");
					//	a very simple (and not very good) approximation of the distance
						aaDist[vcon] = aaDist[vrt] + VecDistance(aaPos[vrt], aaPos[vcon]);
					}
					else{
						aaDist[vcon] = VecDistance(aaPos[vcon], projector.get_closest_point());
					}
				}

			//...
			}
		}


	//	we're through with the algorithm. Perform cleanup
		grid.end_marking();
		grid.detach_from_vertices(aDist);

	//	done
		obj->geometry_changed();
	}

	const char* get_name()		{return "Expand Layers By Adaption 3d";}
	const char* get_tooltip()	{return TOOLTIP_EXPAND_LAYERS_3D;}
	const char* get_group()		{return "Layers";}

	QWidget* get_dialog(QWidget* parent){
		ToolWidget *dlg = new ToolWidget(get_name(), parent, this,
										IDB_APPLY | IDB_OK | IDB_CLOSE);
		dlg->addSpinBox(tr("width:"), 0, 1.e9, 0.1, 0.1, 9);
		dlg->addSpinBox(tr("snap threshold:"), 0, 1.e9, 0.1, 0.1, 9);
		return dlg;
	}
};
*/

void RegisterFracToLayerTools(ToolManager* toolMgr)
{
    //toolMgr->register_tool(new ToolFracToLayer);
    toolMgr->register_tool(new ToolExpandLayers2d);
    toolMgr->register_tool(new ToolExpandLayers3d);
}




//	The code below was intended to expand fractures based on frac2layer by
//	Alexander Fuchs. However, since this approach was a little cumbersome,
//	a new implementation now makes this code obsolete.
/*
static bool CollectLines(Grid& grid, const SubsetHandler& shFace, EdgeSelector& LineSel)
{
//	store associated faces in this vector
	vector<Face*> vFaces;

//	iterate through all edges in the grid and identify lines by comparing the subset-membership
//	of the associated faces
	for(EdgeIterator EIter = grid.edges_begin(); EIter != grid.edges_end(); ++EIter)
	{
		CollectFaces(vFaces, grid, *EIter);

		//if(vFaces.size() > 1)
		{
			size_t i = 0;
			Face* f1 = NULL;
		//	find the first face that is assigned to a subset
			for(; i < vFaces.size(); ++i)
			{
				if(shFace.get_subset_index(vFaces[i]) != -1){
					f1 = vFaces[i];
				//	if the iteration leaves with a break, we have to increase i manually
					++i;
					break;
				}
			}

		//	compare with others. only check the ones that are assigned to a subset.
		//	if only one face is in a subset, we'll need a line anyway
			bool gotTwoSubsetFaces = false;
			for(; i < vFaces.size(); ++i)
			{
				if(shFace.get_subset_index(vFaces[i]) == -1)
					continue;

				gotTwoSubsetFaces = true;
				if(shFace.get_subset_index(f1) != shFace.get_subset_index(vFaces[i]))
				{
					LineSel.select(*EIter);
					break;
				}
			}

			if((!gotTwoSubsetFaces) && f1){
			//	the edge is adjacent to only one subset-face.
			//	it thus has to be a line
				LineSel.select(*EIter);
			}
		}
	}

	return true;
}


class ToolFracToLayer : public ITool
{
public:
	void execute(LGObject* obj, QWidget* widget){
		using namespace ug;

		FracToLayerDialog* dlg = dynamic_cast<FracToLayerDialog*>(widget);
		int numLayers = 2;

		if(dlg->numEntries() == 0){
			UG_LOG("No entries selected. Aborting FracToLayer.\n");
			return;
		}

		Grid& grid = obj->grid();
		SubsetHandler& sh = obj->subset_handler();

	//	the application path
		QString appPath = QCoreApplication::applicationDirPath();

	//	write the .f2l file
		QString f2lFilename = appPath;
		f2lFilename.append("/.__PROMESH_TMP_F2L.f2l");
		ofstream out(f2lFilename.toAscii());
		if(!out){
			UG_LOG("ERROR in ToolFracToLayer: couldn't open temporary file ");
			UG_LOG((const char*)f2lFilename.toAscii() << endl);
			return;
		}

	//	iterate through the entries of the FracToLayerDialog and write them to file
	//	if there are fracture intersections they have to be marked.

	//	iterate through all fractures and mark their faces
		grid.begin_marking();
		
		for(size_t i = 0; i < dlg->numEntries(); ++i){
			int si = dlg->entry(i).subsetIndex;
			for(FaceIterator iter = sh.begin<Face>(si); iter != sh.end<Face>(si); ++iter)
				grid.mark(*iter);
		}
		
	//todo: avoid this
	//	put all edges into subset -1 on startup
	//	this is important since we have to mark intersections
		sh.assign_subset(grid.edges_begin(), grid.edges_end(), -1);

	//	now find all edges which are adjacent to marked faces of
	//	different subsets and which are not assigned to a subset.
		vector<Face*> faces;
		vector<Edge*> intersectionEdges;

		for(EdgeIterator iter = grid.edges_begin();
			iter != grid.edges_end(); ++iter)
		{
			Edge* e = *iter;
			if(sh.get_subset_index(e) != -1)
				continue;

			CollectFaces(faces, grid, e);
			
			int faceSI = -1;
			for(size_t fi = 0; fi < faces.size(); ++fi){
				Face* f = faces[fi];
				if(grid.is_marked(f)){
					if(faceSI == -1)
						faceSI = sh.get_subset_index(f);
					else{
						if(faceSI != sh.get_subset_index(f)){
						//	the edge lies on an intersection
							intersectionEdges.push_back(e);
							break;
						}
					}
				}
			}
		}

		bool gotIntersections = !intersectionEdges.empty();
	//todo: of course intersections should be supported.
	//		in the moment they are disabled, since they don't work
	//		for 2 layers.
		if(gotIntersections && (numLayers == 2)){
			UG_LOG("  call of frac2layer aborted. Fracture intersections are currently not supported for two layers.\n");
			return;
		}

		int intersectionSI = 1;//	counter for intersection-subsets

		for(size_t i = 0; i < dlg->numEntries(); ++i){
			const FracToLayerDialog::SubsetEntry& entry = dlg->entry(i);
			int fracInd = entry.subsetIndex;
			out << "surface " << fracInd<< endl;
			out << "unit " << entry.newSubsetIndex + 1 << endl;
			out << "width " << entry.width << endl;
			out << "layers 2" << endl;

			if(gotIntersections){
			//	store intersections as pairs (otherFrac, subset)
				vector<pair<int, int> > intersections;

			//	check for intersections with other fractures
				bool gotOne = false;
				for(size_t j = 0; j < dlg->numEntries(); ++j){
					if(j == i)
						continue;
					int otherInd = dlg->entry(j).subsetIndex;

				//	search for the first edge in intersections, which is connected
				//	with the frac
					for(size_t k = 0; k < intersectionEdges.size(); ++k){
						Edge* e = intersectionEdges[k];
						CollectFaces(faces, grid, e);
						bool gotFrac = false;
						bool gotOther = false;
						for(size_t l = 0; l < faces.size(); ++l){
							if(sh.get_subset_index(faces[l]) == fracInd)
								gotFrac = true;
							else if(sh.get_subset_index(faces[l]) == otherInd)
								gotOther = true;
						}

					//	if the edge is connected to both, then its an intersection of
					//	the fracture.
					//	if it is already assigned to a subset, then we will simply add the
					//	subset to fracIntersectionInds.
					//for now:
					//	if not we will assign it to a new subset and write an intersection
					//todo:
					//	If not, then we have to collect neighbours which are connected to
					//	exactly the same fractures as the current edge
						int si = sh.get_subset_index(e);
						if(si == -1){
							si = intersectionSI++;
							sh.assign_subset(e, si);
						}

						intersections.push_back(make_pair<int, int>(otherInd, si));
					}
				}

				if(!intersections.empty()){
				//	write:
				//intersections 1		 % Schnitte mit anderen Surfaces
				//  surface 0 line 22		 % Schnitt mit Surface 0 an Kante 22
					out << "intersections " << intersections.size() << endl;
					for(size_t j = 0; j < intersections.size(); ++j){
						out << "  surface " << intersections[j].first
							<< " line " << intersections[j].second << endl;
					}
				}
			}
			out << "$" << endl;
		}

		grid.end_marking();
		out.close();

// surface 0         			% Surface 2 wird auseinanderzogen
// unit 2                     % Neue Elemente gehoeren zu subdomain 2
// width 0				% 0 = degenerierte Element verwenden


	//	save the current grid as .net
		QString srcGridname = appPath;
		srcGridname.append("/.__PROMESH_TMP_SRCGRID.art");
		SaveGridToFile(grid, srcGridname.toAscii(), sh);

	//	the dest-grid has to be removed
		QString destGridname = appPath;
		destGridname.append("/.__PROMESH_TMP_DESTGRID.art");
		QString rmCall = "rm ";
		rmCall.append(destGridname);
		system(rmCall.toAscii());

	//	call frac2layer with the scriptfile
		QString sysCall = appPath;
		sysCall.append("/frac2layer ")
				.append(f2lFilename)
				.append(" -i ")
				.append(srcGridname)
				.append(" -o ")
				.append(destGridname);

		int ret = system(sysCall.toAscii());

		if(ret == 0){
			grid.clear_geometry();
			if(LoadGridFromFile(grid, destGridname.toAscii(), sh)){
				UG_LOG("frac2layer succeeded.\n");
			}
			else{
				UG_LOG("ERROR in ToolFracToLayer: Couldn't import grid.\n");
				UG_LOG("Most likely frac2layer failed.\n");
			}
		}
		else{
			UG_LOG("call to frac2layer returned: " << ret << endl);
		}

	//	done
		obj->geometry_changed();
	}

	const char* get_name()		{return "Frac To Layer";}
	const char* get_tooltip()	{return TOOLTIP_FRAC_TO_LAYER;}
	const char* get_group()		{return "Fractures";}

	QWidget* get_dialog(QWidget* parent){
		return new FracToLayerDialog(get_name(), parent, this);
	}
};
*/
