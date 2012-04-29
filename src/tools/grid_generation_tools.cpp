//	created by Sebastian Reiter
//	s.b.reiter@googlemail.com
//	y10 m05 d07

#include <vector>
#include <fstream>
#include "app.h"
#include "standard_tools.h"
#include "tools_util.h"
#include "lib_grid/algorithms/remeshing/delaunay_triangulation.h"

//#include "lib_discretization/spatial_discretization/disc_util/finite_volume_output.h"

using namespace std;

class ToolCreateVertex : public ITool
{
	public:
		void execute(LGObject* obj, QWidget* widget){
			using namespace ug;
			CoordinatesWidget* dlg = dynamic_cast<CoordinatesWidget*>(widget);
			if(dlg){
				Grid& grid = obj->get_grid();
				Selector& sel = obj->get_selector();
				SubsetHandler& sh = obj->get_subset_handler();
				Grid::VertexAttachmentAccessor<APosition> aaPos(grid, aPosition);

				vector3 coord;
				coord.x = dlg->x();
				coord.y = dlg->y();
				coord.z = dlg->z();

			//	build a new vertex
				Vertex* vrt = *grid.create<Vertex>();

				if(vrt){
					aaPos[vrt] = coord;
					sel.clear();
					sel.select(vrt);
				//todo: use the currently marked subset.
					sh.assign_subset(vrt, 0);
				}
			}
			obj->geometry_changed();
		}

		const char* get_name()		{return "Create Vertex";}
		const char* get_tooltip()	{return "Creates a new vertex";}
		const char* get_group()		{return "Grid Generation";}

		QWidget* get_dialog(QWidget* parent){
			return new CoordinatesWidget(get_name(), parent, this, false);
		}
};


class ToolCreateEdge : public ITool
{
	public:
		void execute(LGObject* obj, QWidget*){
		//	build an edge or a face between selected vertices.
			ug::Selector& sel = obj->get_selector();
			ug::Grid& grid = obj->get_grid();
			ug::SubsetHandler& sh = obj->get_subset_handler();

			size_t numVrts = sel.num<ug::VertexBase>();
			vector<ug::VertexBase*> vrts;
			vrts.reserve(numVrts);
			vrts.assign(sel.begin<ug::VertexBase>(), sel.end<ug::VertexBase>());

			ug::Edge* e = NULL;
			switch(numVrts){
				case 2:{//	create edge
						if(!grid.get_edge(vrts[0], vrts[1]))
							e = *grid.create<ug::Edge>(ug::EdgeDescriptor(vrts[0], vrts[1]));
						else{
							UG_LOG("Can't create edge: Edge already exists.\n");
						}
					}break;

				default:
					UG_LOG("Can't create edge: Bad number of vertices. 2 are required.\n");
					break;
			}

		//todo: use the currently marked subset.
			if(e)
				sh.assign_subset(e, 0);

			obj->geometry_changed();
		}

		const char* get_name()		{return "Create Edge";}
		const char* get_tooltip()	{return "Creates an edge between two selected vertices.";}
		const char* get_group()		{return "Grid Generation";}
};

class ToolCreateFace : public ITool
{
	public:
		void execute(LGObject* obj, QWidget*){
		//	build an edge or a face between selected vertices.
			ug::Selector& sel = obj->get_selector();
			ug::Grid& grid = obj->get_grid();
			ug::SubsetHandler& sh = obj->get_subset_handler();

			size_t numVrts = sel.num<ug::VertexBase>();
			vector<ug::VertexBase*> vrts;
			vrts.reserve(numVrts);
			vrts.assign(sel.begin<ug::VertexBase>(), sel.end<ug::VertexBase>());

			ug::Face* f = NULL;
			switch(numVrts){
			/*
			case 2://	create edge
				if(!grid.get_edge(vrts[0], vrts[1]))
					grid.create<EdgeBase>(EdgeDescriptor(vrts[0], vrts[1]));
				else{
					UG_LOG("Can't create edge: Edge already exists.\n")
				}
				break;
			*/
			case 3:{//	create triangle
				ug::FaceDescriptor fd(3);
				for(size_t i = 0; i < 3; ++i)
					fd.set_vertex(i, vrts[i]);

				if(!grid.get_face(fd))
					f = *grid.create<ug::Triangle>(ug::TriangleDescriptor(vrts[0], vrts[1], vrts[2]));
				else{
					UG_LOG("Can't create face: Face already exists.\n");
				}
			}break;

			case 4:{//	create quadrilateral
				ug::FaceDescriptor fd(4);
				for(size_t i = 0; i < 4; ++i)
					fd.set_vertex(i, vrts[i]);

				if(!grid.get_face(fd))
					f = *grid.create<ug::Quadrilateral>(ug::QuadrilateralDescriptor(vrts[0], vrts[1],
																			vrts[2], vrts[3]));
				else{
					UG_LOG("Can't create face: Face already exists.\n");
				}
			}break;

			default:
				UG_LOG("Can't create face: Bad number of vertices. 3 or 4 are supported.\n");
				break;
			}

		//todo: use the currently marked subset.
			if(f)
				sh.assign_subset(f, 0);

			obj->geometry_changed();
		}

		const char* get_name()		{return "Create Face";}
		const char* get_tooltip()	{return "Creates a face between selected vertices.";}
		const char* get_group()		{return "Grid Generation";}
};


class ToolCreatePlane : public ITool
{
public:
	void execute(LGObject* obj, QWidget* widget){
		ToolWidget* dlg = dynamic_cast<ToolWidget*>(widget);
		using namespace ug;

		int orientation = 0;
		number width = 1.;
		number height = 1.;
		vector3 center(0, 0, 0);
		int newSI = 0;

		if(dlg){
			orientation = dlg->to_int(0);
			width = dlg->to_double(1);
			height = dlg->to_double(2);
			center.x = dlg->to_double(3);
			center.y = dlg->to_double(4);
			center.z = dlg->to_double(5);
			newSI = dlg->to_int(6);
		}

		Grid& grid = obj->get_grid();
		Grid::VertexAttachmentAccessor<APosition> aaPos(grid, aPosition);
		Selector& sel = obj->get_selector();
		SubsetHandler& sh = obj->get_subset_handler();

		sel.clear();
		bool autoselEnabled = sel.autoselection_enabled();
		sel.enable_autoselection(true);

	//	create the vertices
		VertexBase* vrts[4];
		for(size_t i = 0; i < 4; ++i)
			vrts[i] = *grid.create<Vertex>();

		vector3 dirWidth, dirHeight;

		switch(orientation){
			case 0:	// xy
			{
				dirWidth = vector3(0.5 * width, 0, 0);
				dirHeight = vector3(0, 0.5 * height, 0);
			}break;
			case 1:	// xz
			{
				dirWidth = vector3(0.5 * width, 0, 0);
				dirHeight = vector3(0, 0, 0.5 * height);
			}break;
			case 2:	// xy
			{
				dirWidth = vector3(0, 0.5 * width, 0);
				dirHeight = vector3(0, 0, 0.5 * height);
			}break;
		}//	end of switch

		VecSubtract(aaPos[vrts[0]], center, dirWidth);
		VecAdd(aaPos[vrts[0]], aaPos[vrts[0]], dirHeight);

		VecSubtract(aaPos[vrts[1]], center, dirWidth);
		VecSubtract(aaPos[vrts[1]], aaPos[vrts[1]], dirHeight);

		VecAdd(aaPos[vrts[2]], center, dirWidth);
		VecSubtract(aaPos[vrts[2]], aaPos[vrts[2]], dirHeight);

		VecAdd(aaPos[vrts[3]], center, dirWidth);
		VecAdd(aaPos[vrts[3]], aaPos[vrts[3]], dirHeight);

	//	create the plane
		grid.create<Quadrilateral>(QuadrilateralDescriptor(vrts[0], vrts[1], vrts[2], vrts[3]));

	//	assign subset
		sh.assign_subset(sel.begin<VertexBase>(), sel.end<VertexBase>(), newSI);
		sh.assign_subset(sel.begin<EdgeBase>(), sel.end<EdgeBase>(), newSI);
		sh.assign_subset(sel.begin<Face>(), sel.end<Face>(), newSI);

	//	restore selector
		sel.enable_autoselection(autoselEnabled);

		obj->geometry_changed();
	}

	const char* get_name()		{return "Create Plane";}
	const char* get_tooltip()	{return "Creates a plane.";}
	const char* get_group()		{return "Grid Generation";}

	ToolWidget* get_dialog(QWidget* parent){
		ToolWidget *dlg = new ToolWidget(get_name(), parent, this,
								IDB_APPLY | IDB_OK | IDB_CLOSE);
		QStringList entries;
		entries.push_back(tr("xy"));
		entries.push_back(tr("xz"));
		entries.push_back(tr("yz"));

		dlg->addComboBox("plane orientation", entries, 0);
		dlg->addSpinBox(tr("width:"), -1.e+9, 1.e+9, 2., 1, 9);
		dlg->addSpinBox(tr("height:"), -1.e+9, 1.e+9, 2., 1, 9);
		dlg->addSpinBox(tr("center x:"), -1.e+9, 1.e+9, 0, 1, 9);
		dlg->addSpinBox(tr("center y:"), -1.e+9, 1.e+9, 0, 1, 9);
		dlg->addSpinBox(tr("center z:"), -1.e+9, 1.e+9, 0, 1, 9);
		dlg->addSpinBox(tr("new subset index:"), -1, 1.e+9, 0, 1, 0);
		return dlg;
	}
};

class ToolCreateCircle : public ITool
{
public:
	void execute(LGObject* obj, QWidget* widget){
		ToolWidget* dlg = dynamic_cast<ToolWidget*>(widget);
		using namespace ug;

	//todo: add a 'regular' flag. This requires that the optimizer can
	//		be applied to a selected subset of the grid.
		number radius = 1.;
		vector3 center(0, 0, 0);
		int numRimVertices = 12;
		int newSI = 0;

		if(dlg){
			center = dlg->to_vector3(0);
			radius = dlg->to_double(1);
			numRimVertices = dlg->to_int(2);
			newSI = dlg->to_int(3);
		}

		Grid& grid = obj->get_grid();
		Grid::VertexAttachmentAccessor<APosition> aaPos(grid, aPosition);
		Selector& sel = obj->get_selector();
		SubsetHandler& sh = obj->get_subset_handler();

		sel.clear();
		bool autoselEnabled = sel.autoselection_enabled();
		sel.enable_autoselection(true);

	//	create the vertices
	//	create one upfront, the others in a loop
		VertexBase* centerVrt = *grid.create<Vertex>();
		aaPos[centerVrt] = center;
		VertexBase* firstVrt = *grid.create<Vertex>();
		aaPos[firstVrt] = vector3(0, radius, 0);
		VecAdd(aaPos[firstVrt], aaPos[firstVrt], center);
		VertexBase* lastVrt = firstVrt;
		for(int i = 1; i < numRimVertices; ++i){
		//	create a new vertex
			number ia = (float)i / (float)numRimVertices;
			VertexBase* vNew = *grid.create<Vertex>();
			aaPos[vNew] = vector3(sin(2. * PI * ia), cos(2. * PI * ia), 0);
			VecScale(aaPos[vNew], aaPos[vNew], radius);
			VecAdd(aaPos[vNew], aaPos[vNew], center);

		//	create a new triangle
			grid.create<Triangle>(TriangleDescriptor(centerVrt, vNew, lastVrt));

		//	prepare the next iteration
			lastVrt = vNew;
		}

	//	one triangle is still missing
		grid.create<Triangle>(TriangleDescriptor(centerVrt, firstVrt, lastVrt));

	//	assign subset
		sh.assign_subset(sel.begin<VertexBase>(), sel.end<VertexBase>(), newSI);
		sh.assign_subset(sel.begin<EdgeBase>(), sel.end<EdgeBase>(), newSI);
		sh.assign_subset(sel.begin<Face>(), sel.end<Face>(), newSI);

	//	restore selector
		sel.enable_autoselection(autoselEnabled);

		obj->geometry_changed();
	}

	const char* get_name()		{return "Create Circle";}
	const char* get_tooltip()	{return "Creates a circle.";}
	const char* get_group()		{return "Grid Generation";}

	ToolWidget* get_dialog(QWidget* parent){
		ToolWidget *dlg = new ToolWidget(get_name(), parent, this,
								IDB_APPLY | IDB_OK | IDB_CLOSE);

		dlg->addVector(tr("center:"), 3);
		dlg->addSpinBox(tr("radius:"), 0, 1.e+9, 1., 1, 9);
		dlg->addSpinBox(tr("num rim vertices:"), 3, 1.e+9, 12, 1, 0);
		dlg->addSpinBox(tr("new subset index:"), -1, 1.e+9, 0, 1, 0);
		return dlg;
	}
};

class ToolCreateBox : public ITool
{
public:
	void execute(LGObject* obj, QWidget* widget){
		ToolWidget* dlg = dynamic_cast<ToolWidget*>(widget);
		using namespace ug;

		vector3 boxMin(-1, -1, -1);
		vector3 boxMax(1, 1, 1);
		int newSI = 0;
		bool createVolume = true;

		if(dlg){
			boxMin.x = dlg->to_double(0);
			boxMin.y = dlg->to_double(1);
			boxMin.z = dlg->to_double(2);
			boxMax.x = dlg->to_double(3);
			boxMax.y = dlg->to_double(4);
			boxMax.z = dlg->to_double(5);
			newSI = dlg->to_int(6);
			createVolume = dlg->to_bool(7);
		}

		Grid& grid = obj->get_grid();
		Grid::VertexAttachmentAccessor<APosition> aaPos(grid, aPosition);
		Selector& sel = obj->get_selector();
		SubsetHandler& sh = obj->get_subset_handler();

		sel.clear();
		bool autoselEnabled = sel.autoselection_enabled();
		sel.enable_autoselection(true);

	//	create the vertices
		VertexBase* vrts[8];
		for(size_t i = 0; i < 8; ++i)
			vrts[i] = *grid.create<Vertex>();

		aaPos[vrts[0]] = vector3(boxMin.x, boxMin.y, boxMin.z);
		aaPos[vrts[1]] = vector3(boxMax.x, boxMin.y, boxMin.z);
		aaPos[vrts[2]] = vector3(boxMax.x, boxMax.y, boxMin.z);
		aaPos[vrts[3]] = vector3(boxMin.x, boxMax.y, boxMin.z);
		aaPos[vrts[4]] = vector3(boxMin.x, boxMin.y, boxMax.z);
		aaPos[vrts[5]] = vector3(boxMax.x, boxMin.y, boxMax.z);
		aaPos[vrts[6]] = vector3(boxMax.x, boxMax.y, boxMax.z);
		aaPos[vrts[7]] = vector3(boxMin.x, boxMax.y, boxMax.z);

	//	we'll use a hexahedron and let it create all faces
		Hexahedron hexa(vrts[0], vrts[1], vrts[2], vrts[3],
						vrts[4],vrts[5], vrts[6], vrts[7]);

	//	now create the faces and register them at the grid
		for(size_t i = 0; i < hexa.num_faces(); ++i)
			grid.register_element(hexa.create_face(i));

	//	if a volume shall be created, do so now
		if(createVolume)
			grid.create_by_cloning(&hexa, hexa, NULL);

	//	assign subset
		sh.assign_subset(sel.begin<VertexBase>(), sel.end<VertexBase>(), newSI);
		sh.assign_subset(sel.begin<EdgeBase>(), sel.end<EdgeBase>(), newSI);
		sh.assign_subset(sel.begin<Face>(), sel.end<Face>(), newSI);
		sh.assign_subset(sel.begin<Volume>(), sel.end<Volume>(), newSI);

	//	restore selector
		sel.enable_autoselection(autoselEnabled);

		obj->geometry_changed();
	}

	const char* get_name()		{return "Create Box";}
	const char* get_tooltip()	{return "Creates a box.";}
	const char* get_group()		{return "Grid Generation";}

	ToolWidget* get_dialog(QWidget* parent){
		ToolWidget *dlg = new ToolWidget(get_name(), parent, this,
								IDB_APPLY | IDB_OK | IDB_CLOSE);
		dlg->addSpinBox(tr("x-min:"), -1.e+9, 1.e+9, -1., 1, 9);
		dlg->addSpinBox(tr("y-min:"), -1.e+9, 1.e+9, -1., 1, 9);
		dlg->addSpinBox(tr("z-min:"), -1.e+9, 1.e+9, -1., 1, 9);
		dlg->addSpinBox(tr("x-max:"), -1.e+9, 1.e+9, 1., 1, 9);
		dlg->addSpinBox(tr("y-max:"), -1.e+9, 1.e+9, 1., 1, 9);
		dlg->addSpinBox(tr("z-max:"), -1.e+9, 1.e+9, 1., 1, 9);
		dlg->addSpinBox(tr("new subset index:"), -1, 1.e+9, 0, 1, 0);
		dlg->addCheckBox(tr("create volume:"), true);
		return dlg;
	}
};


class ToolCreateSphere : public ITool
{
public:
	void execute(LGObject* obj, QWidget* widget){
		ToolWidget* dlg = dynamic_cast<ToolWidget*>(widget);
		using namespace ug;

	//todo: add a 'regular' flag. This requires that the optimizer can
	//		be applied to a selected subset of the grid.
		number radius = 1.;
		vector3 center(0, 0, 0);
		int numRefinements = 2;
		int newSI = 0;

		if(dlg){
			center = dlg->to_vector3(0);
			radius = dlg->to_double(1);
			numRefinements = dlg->to_int(2);
			newSI = dlg->to_int(3);
		}

		Grid& grid = obj->get_grid();
		Selector& sel = obj->get_selector();
		SubsetHandler& sh = obj->get_subset_handler();

	//	create the sphere
		GenerateIcosphere(grid, center, radius, numRefinements, aPosition, &sel);

	//	assign subset
		sh.assign_subset(sel.begin<VertexBase>(), sel.end<VertexBase>(), newSI);
		sh.assign_subset(sel.begin<EdgeBase>(), sel.end<EdgeBase>(), newSI);
		sh.assign_subset(sel.begin<Face>(), sel.end<Face>(), newSI);

		obj->geometry_changed();
	}

	const char* get_name()		{return "Create Sphere";}
	const char* get_tooltip()	{return "Creates a sphere.";}
	const char* get_group()		{return "Grid Generation";}

	ToolWidget* get_dialog(QWidget* parent){
		ToolWidget *dlg = new ToolWidget(get_name(), parent, this,
								IDB_APPLY | IDB_OK | IDB_CLOSE);

		dlg->addVector(tr("center:"), 3);
		dlg->addSpinBox(tr("radius:"), 0, 1.e+9, 1., 1, 9);
		dlg->addSpinBox(tr("num refinements:"), 0, 1.e+9, 2, 1, 0);
		dlg->addSpinBox(tr("new subset index:"), -1, 1.e+9, 0, 1, 0);
		return dlg;
	}
};


class ToolCreateTetrahedron: public ITool
{
public:
	void execute(LGObject* obj, QWidget* widget){
		ToolWidget* dlg = dynamic_cast<ToolWidget*>(widget);
		using namespace ug;

		int newSI = 0;
		bool createVolume = true;

		if(dlg){
			newSI = dlg->to_int(0);
			createVolume = dlg->to_bool(1);
		}

		Grid& grid = obj->get_grid();
		Grid::VertexAttachmentAccessor<APosition> aaPos(grid, aPosition);
		Selector& sel = obj->get_selector();
		SubsetHandler& sh = obj->get_subset_handler();

		sel.clear();
		bool autoselEnabled = sel.autoselection_enabled();
		sel.enable_autoselection(true);

	//	create the vertices
		VertexBase* vrts[4];
		for(size_t i = 0; i < 4; ++i)
			vrts[i] = *grid.create<Vertex>();

		aaPos[vrts[0]] = vector3(1, 1, 1);
		aaPos[vrts[1]] = vector3(-1, -1, 1);
		aaPos[vrts[2]] = vector3(-1, 1, -1);
		aaPos[vrts[3]] = vector3(1, -1, -1);

	//	we'll use a hexahedron and let it create all faces
		Tetrahedron tet(vrts[0], vrts[1], vrts[2], vrts[3]);

	//	now create the faces and register them at the grid
		for(size_t i = 0; i < tet.num_faces(); ++i)
			grid.register_element(tet.create_face(i));

	//	if a volume shall be created, do so now
		if(createVolume)
			grid.create_by_cloning(&tet, tet, NULL);

	//	assign subset
		sh.assign_subset(sel.begin<VertexBase>(), sel.end<VertexBase>(), newSI);
		sh.assign_subset(sel.begin<EdgeBase>(), sel.end<EdgeBase>(), newSI);
		sh.assign_subset(sel.begin<Face>(), sel.end<Face>(), newSI);
		sh.assign_subset(sel.begin<Volume>(), sel.end<Volume>(), newSI);

	//	restore selector
		sel.enable_autoselection(autoselEnabled);

		obj->geometry_changed();
	}

	const char* get_name()		{return "Create Tetrahedron";}
	const char* get_tooltip()	{return "Creates a tetrahedron.";}
	const char* get_group()		{return "Grid Generation";}

	ToolWidget* get_dialog(QWidget* parent){
		ToolWidget *dlg = new ToolWidget(get_name(), parent, this,
								IDB_APPLY | IDB_OK | IDB_CLOSE);
		dlg->addSpinBox(tr("new subset index:"), -1, 1.e+9, 0, 1, 0);
		dlg->addCheckBox(tr("create volume:"), true);
		return dlg;
	}
};

class ToolCreatePyramid: public ITool
{
public:
	void execute(LGObject* obj, QWidget* widget){
		ToolWidget* dlg = dynamic_cast<ToolWidget*>(widget);
		using namespace ug;

		int newSI = 0;
		bool createVolume = true;

		if(dlg){
			newSI = dlg->to_int(0);
			createVolume = dlg->to_bool(1);
		}

		Grid& grid = obj->get_grid();
		Grid::VertexAttachmentAccessor<APosition> aaPos(grid, aPosition);
		Selector& sel = obj->get_selector();
		SubsetHandler& sh = obj->get_subset_handler();

		sel.clear();
		bool autoselEnabled = sel.autoselection_enabled();
		sel.enable_autoselection(true);

	//	create the vertices
		VertexBase* vrts[5];
		for(size_t i = 0; i < 5; ++i)
			vrts[i] = *grid.create<Vertex>();

		aaPos[vrts[0]] = vector3(-1, -1, -1);
		aaPos[vrts[1]] = vector3(1, -1, -1);
		aaPos[vrts[2]] = vector3(1, 1, -1);
		aaPos[vrts[3]] = vector3(-1, 1, -1);
		aaPos[vrts[4]] = vector3(0, 0, 1);

	//	we'll use a hexahedron and let it create all faces
		Pyramid pyra(vrts[0], vrts[1], vrts[2], vrts[3], vrts[4]);

	//	now create the faces and register them at the grid
		for(size_t i = 0; i < pyra.num_faces(); ++i)
			grid.register_element(pyra.create_face(i));

	//	if a volume shall be created, do so now
		if(createVolume)
			grid.create_by_cloning(&pyra, pyra, NULL);

	//	assign subset
		sh.assign_subset(sel.begin<VertexBase>(), sel.end<VertexBase>(), newSI);
		sh.assign_subset(sel.begin<EdgeBase>(), sel.end<EdgeBase>(), newSI);
		sh.assign_subset(sel.begin<Face>(), sel.end<Face>(), newSI);
		sh.assign_subset(sel.begin<Volume>(), sel.end<Volume>(), newSI);

	//	restore selector
		sel.enable_autoselection(autoselEnabled);

		obj->geometry_changed();
	}

	const char* get_name()		{return "Create Pyramid";}
	const char* get_tooltip()	{return "Creates a pyramid.";}
	const char* get_group()		{return "Grid Generation";}

	ToolWidget* get_dialog(QWidget* parent){
		ToolWidget *dlg = new ToolWidget(get_name(), parent, this,
								IDB_APPLY | IDB_OK | IDB_CLOSE);
		dlg->addSpinBox(tr("new subset index:"), -1, 1.e+9, 0, 1, 0);
		dlg->addCheckBox(tr("create volume:"), true);
		return dlg;
	}
};

class ToolCreatePrism: public ITool
{
public:
	void execute(LGObject* obj, QWidget* widget){
		ToolWidget* dlg = dynamic_cast<ToolWidget*>(widget);
		using namespace ug;

		int newSI = 0;
		bool createVolume = true;

		if(dlg){
			newSI = dlg->to_int(0);
			createVolume = dlg->to_bool(1);
		}

		Grid& grid = obj->get_grid();
		Grid::VertexAttachmentAccessor<APosition> aaPos(grid, aPosition);
		Selector& sel = obj->get_selector();
		SubsetHandler& sh = obj->get_subset_handler();

		sel.clear();
		bool autoselEnabled = sel.autoselection_enabled();
		sel.enable_autoselection(true);

	//	create the vertices
		VertexBase* vrts[6];
		for(size_t i = 0; i < 6; ++i)
			vrts[i] = *grid.create<Vertex>();

		aaPos[vrts[0]] = vector3(-1, -1, -1);
		aaPos[vrts[1]] = vector3(1, -1, -1);
		aaPos[vrts[2]] = vector3(1, 1, -1);
		aaPos[vrts[3]] = vector3(-1, -1, 1);
		aaPos[vrts[4]] = vector3(1, -1, 1);
		aaPos[vrts[5]] = vector3(1, 1, 1);

	//	we'll use a hexahedron and let it create all faces
		Prism prism(vrts[0], vrts[1], vrts[2], vrts[3], vrts[4], vrts[5]);

	//	now create the faces and register them at the grid
		for(size_t i = 0; i < prism.num_faces(); ++i)
			grid.register_element(prism.create_face(i));

	//	if a volume shall be created, do so now
		if(createVolume)
			grid.create_by_cloning(&prism, prism, NULL);

	//	assign subset
		sh.assign_subset(sel.begin<VertexBase>(), sel.end<VertexBase>(), newSI);
		sh.assign_subset(sel.begin<EdgeBase>(), sel.end<EdgeBase>(), newSI);
		sh.assign_subset(sel.begin<Face>(), sel.end<Face>(), newSI);
		sh.assign_subset(sel.begin<Volume>(), sel.end<Volume>(), newSI);

	//	restore selector
		sel.enable_autoselection(autoselEnabled);

		obj->geometry_changed();
	}

	const char* get_name()		{return "Create Prism";}
	const char* get_tooltip()	{return "Creates a prism.";}
	const char* get_group()		{return "Grid Generation";}

	ToolWidget* get_dialog(QWidget* parent){
		ToolWidget *dlg = new ToolWidget(get_name(), parent, this,
								IDB_APPLY | IDB_OK | IDB_CLOSE);
		dlg->addSpinBox(tr("new subset index:"), -1, 1.e+9, 0, 1, 0);
		dlg->addCheckBox(tr("create volume:"), true);
		return dlg;
	}
};

class ToolDuplicate : public ITool
{
public:
	void execute(LGObject* obj, QWidget* widget){
		ToolWidget* dlg = dynamic_cast<ToolWidget*>(widget);
		using namespace ug;

		vector3 offset(0, 0, 0);
		bool deselectOld = true;
		bool selectNew = true;
		if(dlg){
			offset.x = dlg->to_double(0);
			offset.y = dlg->to_double(1);
			offset.z = dlg->to_double(2);
			deselectOld = dlg->to_bool(3);
			selectNew = dlg->to_bool(4);
		}

		Grid& grid = obj->get_grid();
		Selector& sel = obj->get_selector();
		Duplicate(grid, sel, offset, aPosition, deselectOld, selectNew);

		obj->geometry_changed();
	}

	const char* get_name()		{return "Duplicate";}
	const char* get_tooltip()	{return "Duplicates the selected geometry.";}
	const char* get_group()		{return "Grid Generation";}

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
			ug::vector3 totalDir(0, 1., 0);
			int numSteps = 1;
			int newVolSubsetIndex = 0;

			if(dlg){
				createFaces = dlg->to_bool(0);
				createVolumes = dlg->to_bool(1);
				totalDir.x = dlg->to_double(2);
				totalDir.y = dlg->to_double(3);
				totalDir.z = dlg->to_double(4);
				numSteps = dlg->to_int(5);
				newVolSubsetIndex = dlg->to_int(6);
			}

			ug::vector3 stepDir;
			VecScale(stepDir, totalDir, 1./(float)numSteps);

			ug::Grid& grid = obj->get_grid();
			ug::Selector& sel = obj->get_selector();
			ug::SubsetHandler& sh = obj->get_subset_handler();

			vector<ug::VertexBase*> vrts;
			vrts.assign(sel.vertices_begin(), sel.vertices_end());
			vector<ug::EdgeBase*> edges;
			edges.assign(sel.edges_begin(), sel.edges_end());
			vector<ug::Face*> faces;
			faces.assign(sel.faces_begin(), sel.faces_end());

			uint extrusionOptions = 0;
			if(createFaces)
				extrusionOptions |= ug::EO_CREATE_FACES;
			if(createVolumes)
				extrusionOptions |= ug::EO_CREATE_VOLUMES;

		//	we use sel to collect the newly created volumes
			sel.clear();
			sel.enable_autoselection(true);

		//	mark all elements that were already in the selector.
			for(int i = 0; i < numSteps; ++i)
			{
				ug::Extrude(grid, &vrts, &edges, &faces, stepDir,
							extrusionOptions, ug::aPosition);
			}

			sel.enable_autoselection(false);
			if(sel.num<ug::Volume>())
				sh.assign_subset(sel.volumes_begin(), sel.volumes_end(), newVolSubsetIndex);
			else if(sel.num<ug::Face>())
				sh.assign_subset(sel.faces_begin(), sel.faces_end(), newVolSubsetIndex);
			else
				sh.assign_subset(sel.edges_begin(), sel.edges_end(), newVolSubsetIndex);


		//	select faces, edges and vertices from the new top-layer.
			sel.clear<ug::VertexBase>();
			sel.clear<ug::EdgeBase>();
			sel.clear<ug::Face>();
			sel.select(vrts.begin(), vrts.end());
			sel.select(edges.begin(), edges.end());
			sel.select(faces.begin(), faces.end());

			obj->geometry_changed();
		}

		const char* get_name()		{return "Extrude";}
		const char* get_tooltip()	{return "Extrudes selected geometry (vertices, edges, faces).";}
		const char* get_group()		{return "Grid Generation";}

		ToolWidget* get_dialog(QWidget* parent){
			ToolWidget *dlg = new ToolWidget(get_name(), parent, this,
											IDB_APPLY | IDB_OK | IDB_CANCEL);
			dlg->addCheckBox("create faces:", true);
			dlg->addCheckBox("create volumes:", true);
			dlg->addSpinBox("x-total:", -1.e+9, 1.e+9, 0., 0.1, 9);
			dlg->addSpinBox("y-total:", -1.e+9, 1.e+9, 0., 0.1, 9);
			dlg->addSpinBox("z-total:", -1.e+9, 1.e+9, 0., 0.1, 9);
			dlg->addSpinBox("num steps:", 1, 1.e+9, 1, 1, 0);
			dlg->addSpinBox("new volume subset index:", 0, 1.e+9, 0, 1, 0);
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
		bool createVolumes = false;

		if(dlg){
			height = (number)dlg->to_double(0);
			radius = (number)dlg->to_double(1);
			createVolumes = dlg->to_bool(2);
		}

		ug::Grid& g = obj->get_grid();
		ug::Selector& sel = obj->get_selector();
		ug::SubsetHandler& sh = obj->get_subset_handler();
		ug::Grid::VertexAttachmentAccessor<ug::APosition> aaPos(g, ug::aPosition);

	//	store all source-vertices in a list
		vector<ug::VertexBase*> vrts;
		vrts.assign(sel.begin<ug::VertexBase>(), sel.end<ug::VertexBase>());

	//	iterate over selected vertices
		for(vector<ug::VertexBase*>::iterator iter = vrts.begin();
			iter != vrts.end(); ++iter)
		{
			ug::VertexBase* vrt = *iter;
			ug::vector3 n;
			ug::CalculateVertexNormal(n, g, vrt, aaPos);

			int numSubs = sh.num_subsets();
			if(!ug::ExtrudeCylinder(g, sh, vrt, n, height, radius, aaPos,
									numSubs, numSubs + 1))
			{
				UG_LOG("Cylinder-Extrude failed for the vertex at " << aaPos[vrt] << "\n");
			}
		}

		obj->geometry_changed();
	}

	const char* get_name()		{return "Extrude Cylinders";}
	const char* get_tooltip()	{return "Extrudes cylinders around selected points of a 2d manifold.";}
	const char* get_group()		{return "Grid Generation";}

	ToolWidget* get_dialog(QWidget* parent){
		ToolWidget *dlg = new ToolWidget(get_name(), parent, this,
										IDB_APPLY | IDB_OK | IDB_CANCEL);
		dlg->addSpinBox("height: ", -1.e+9, 1.e+9, 1, 1, 9);
		dlg->addSpinBox("radius: ", -1.e+9, 1.e+9, 1, 1, 9);
		dlg->addCheckBox("create volumes: ", false);
		return dlg;
	}
};
/*
class ToolCreateDualGrid : public ITool
{
private:
    enum
    {
       FV1GEOMETRY = 0,
       HFV1GEOMETRY = 1
    };

public:
        void execute(LGObject* obj, QWidget* widget){
		ToolWidget* dlg = dynamic_cast<ToolWidget*>(widget);
            using namespace ug;

            int iGeom = 0;
            bool bCV, bSCV, bSCVF;

            // check dialog
            if(dlg){
                iGeom = dlg->to_int(0);
                bCV = dlg->to_bool(1);
                bSCV = dlg->to_bool(2);
                bSCVF = dlg->to_bool(3);
            }
            else
            {
                UG_LOG("No dialog\n");
                return;
            }

            // Original Grid and SubsetHandler
            Grid& grid = obj->get_grid();
            grid.enable_options(GRIDOPT_FULL_INTERCONNECTION);
            SubsetHandler& sh = obj->get_subset_handler();

            ////////////////////////////////
            // SCVF
            ////////////////////////////////
            if(bSCVF)
            {
                // Create empty dual grid
                LGObject* dualObj = app::createEmptyLGObject("DualGrid-SCVF");
                Grid& dualGrid = dualObj->get_grid();
                SubsetHandler& dualSH = dualObj->get_subset_handler();

                // Create Dual grid
                if(grid.num<Volume>() > 0)
                {
                    switch(iGeom)
                    {
                    case FV1GEOMETRY: ug::CreateGridOfSubControlVolumeFaces<FV1Geometry, APosition>(dualSH, sh, aPosition); break;
                    case HFV1GEOMETRY: ug::CreateGridOfSubControlVolumeFaces<HFV1Geometry, APosition>(dualSH, sh, aPosition); break;
                    default: UG_LOG("Geometry Type not found, although selected. Ask programmer.\n"); return;
                    }
                }
                else if (grid.num<Face>() > 0)
                {
                    // convert to 2d positions (FVGeometry depends on PositionCoordinates)
                    grid.attach_to_vertices(aPosition2);
                    dualGrid.attach_to_vertices(aPosition2);
                    ConvertMathVectorAttachmentValues<VertexBase>(grid, aPosition, aPosition2);

                    switch(iGeom)
                    {
                    case FV1GEOMETRY: ug::CreateGridOfSubControlVolumeFaces<FV1Geometry, APosition2>(dualSH, sh, aPosition2); break;
                    case HFV1GEOMETRY: ug::CreateGridOfSubControlVolumeFaces<HFV1Geometry, APosition2>(dualSH, sh, aPosition2); break;
                    default: UG_LOG("Geometry Type not found, although selected. Ask programmer.\n"); return;
                    }

                    // convert back to 3d positions (ProMesh only handles 3d)
                    ConvertMathVectorAttachmentValues<VertexBase>(dualGrid, aPosition2, aPosition);
                    grid.detach_from_vertices(aPosition2);
                    dualGrid.detach_from_vertices(aPosition2);
                }
                else if (grid.num<Edge>() > 0)
                {
                    // convert to 1d positions (FVGeometry depends on PositionCoordinates)
                    grid.attach_to_vertices(aPosition1);
                    dualGrid.attach_to_vertices(aPosition1);
                    ConvertMathVectorAttachmentValues<VertexBase>(grid, aPosition, aPosition1);

                    switch(iGeom)
                    {
                    case FV1GEOMETRY: ug::CreateGridOfSubControlVolumeFaces<FV1Geometry, APosition1>(dualSH, sh, aPosition1); break;
                    case HFV1GEOMETRY: ug::CreateGridOfSubControlVolumeFaces<HFV1Geometry, APosition1>(dualSH, sh, aPosition1); break;
                    default: UG_LOG("Geometry Type not found, although selected. Ask programmer.\n"); return;
                    }

                    // convert back to 3d positions (ProMesh only handles 3d)
                    ConvertMathVectorAttachmentValues<VertexBase>(dualGrid, aPosition1, aPosition);
                    grid.detach_from_vertices(aPosition1);
                    dualGrid.detach_from_vertices(aPosition1);
                }
                else
                {
                    UG_LOG("Grid does not contain elements, only points. Can not create Dual Grid.\n");
                    return;
                }

                // assign subset colors
                AssignSubsetColors(dualSH);

                // update view
                dualObj->geometry_changed();
            }

            ////////////////////////////////
            // SCV
            ////////////////////////////////
            if(bSCV)
            {
                // Create empty dual grid
                LGObject* dualObj = app::createEmptyLGObject("DualGrid-SCV");
                Grid& dualGrid = dualObj->get_grid();
                SubsetHandler& dualSH = dualObj->get_subset_handler();

                // Create Dual grid
                if(grid.num<Volume>() > 0)
                {
                    switch(iGeom)
                    {
                    case FV1GEOMETRY: ug::CreateGridOfSubControlVolumes<FV1Geometry, APosition>(dualSH, sh, aPosition); break;
                    case HFV1GEOMETRY: ug::CreateGridOfSubControlVolumes<HFV1Geometry, APosition>(dualSH, sh, aPosition); break;
                    default: UG_LOG("Geometry Type not found, although selected. Ask programmer.\n"); return;
                    }
                }
                else if (grid.num<Face>() > 0)
                {
                    // convert to 2d positions (FVGeometry depends on PositionCoordinates)
                    grid.attach_to_vertices(aPosition2);
                    dualGrid.attach_to_vertices(aPosition2);
                    ConvertMathVectorAttachmentValues<VertexBase>(grid, aPosition, aPosition2);

                    switch(iGeom)
                    {
                    case FV1GEOMETRY: ug::CreateGridOfSubControlVolumes<FV1Geometry, APosition2>(dualSH, sh, aPosition2); break;
                    case HFV1GEOMETRY: ug::CreateGridOfSubControlVolumes<HFV1Geometry, APosition2>(dualSH, sh, aPosition2); break;
                    default: UG_LOG("Geometry Type not found, although selected. Ask programmer.\n"); return;
                    }

                    // convert back to 3d positions (ProMesh only handles 3d)
                    ConvertMathVectorAttachmentValues<VertexBase>(dualGrid, aPosition2, aPosition);
                    grid.detach_from_vertices(aPosition2);
                    dualGrid.detach_from_vertices(aPosition2);
                }
                else if (grid.num<Edge>() > 0)
                {
                    // convert to 1d positions (FVGeometry depends on PositionCoordinates)
                    grid.attach_to_vertices(aPosition1);
                    dualGrid.attach_to_vertices(aPosition1);
                    ConvertMathVectorAttachmentValues<VertexBase>(grid, aPosition, aPosition1);

                    switch(iGeom)
                    {
                    case FV1GEOMETRY: ug::CreateGridOfSubControlVolumes<FV1Geometry, APosition1>(dualSH, sh, aPosition1); break;
                    case HFV1GEOMETRY: ug::CreateGridOfSubControlVolumes<HFV1Geometry, APosition1>(dualSH, sh, aPosition1); break;
                    default: UG_LOG("Geometry Type not found, although selected. Ask programmer.\n"); return;
                    }

                    // convert back to 3d positions (ProMesh only handles 3d)
                    ConvertMathVectorAttachmentValues<VertexBase>(dualGrid, aPosition1, aPosition);
                    grid.detach_from_vertices(aPosition1);
                    dualGrid.detach_from_vertices(aPosition1);
                }
                else
                {
                    UG_LOG("Grid does not contain elements, only points. Can not create Dual Grid.\n");
                    return;
                }

                // assign subset colors
                AssignSubsetColors(dualSH);

                // update view
                dualObj->geometry_changed();
            }

            ////////////////////////////////
            // CV
            ////////////////////////////////
            if(bCV)
            {
                // Create empty dual grid
                LGObject* dualObj = app::createEmptyLGObject("DualGrid-CV");
                Grid& dualGrid = dualObj->get_grid();
                SubsetHandler& dualSH = dualObj->get_subset_handler();

                // Create Dual grid
                if(grid.num<Volume>() > 0)
                {
                    switch(iGeom)
                    {
                    case FV1GEOMETRY: ug::CreateGridOfControlVolumes<FV1Geometry, APosition>(dualSH, sh, aPosition); break;
                    case HFV1GEOMETRY: ug::CreateGridOfControlVolumes<HFV1Geometry, APosition>(dualSH, sh, aPosition); break;
                    default: UG_LOG("Geometry Type not found, although selected. Ask programmer.\n"); return;
                    }
                }
                else if (grid.num<Face>() > 0)
                {
                    // convert to 2d positions (FVGeometry depends on PositionCoordinates)
                    grid.attach_to_vertices(aPosition2);
                    dualGrid.attach_to_vertices(aPosition2);
                    ConvertMathVectorAttachmentValues<VertexBase>(grid, aPosition, aPosition2);

                    switch(iGeom)
                    {
                    case FV1GEOMETRY: ug::CreateGridOfControlVolumes<FV1Geometry, APosition2>(dualSH, sh, aPosition2); break;
                    case HFV1GEOMETRY: ug::CreateGridOfControlVolumes<HFV1Geometry, APosition2>(dualSH, sh, aPosition2); break;
                    default: UG_LOG("Geometry Type not found, although selected. Ask programmer.\n"); return;
                    }

                    // convert back to 3d positions (ProMesh only handles 3d)
                    ConvertMathVectorAttachmentValues<VertexBase>(dualGrid, aPosition2, aPosition);
                    grid.detach_from_vertices(aPosition2);
                    dualGrid.detach_from_vertices(aPosition2);
                }
                else if (grid.num<Edge>() > 0)
                {
                    // convert to 1d positions (FVGeometry depends on PositionCoordinates)
                    grid.attach_to_vertices(aPosition1);
                    dualGrid.attach_to_vertices(aPosition1);
                    ConvertMathVectorAttachmentValues<VertexBase>(grid, aPosition, aPosition1);

                    switch(iGeom)
                    {
                    case FV1GEOMETRY: ug::CreateGridOfControlVolumes<FV1Geometry, APosition1>(dualSH, sh, aPosition1); break;
                    case HFV1GEOMETRY: ug::CreateGridOfControlVolumes<HFV1Geometry, APosition1>(dualSH, sh, aPosition1); break;
                    default: UG_LOG("Geometry Type not found, although selected. Ask programmer.\n"); return;
                    }

                    // convert back to 3d positions (ProMesh only handles 3d)
                    ConvertMathVectorAttachmentValues<VertexBase>(dualGrid, aPosition1, aPosition);
                    grid.detach_from_vertices(aPosition1);
                    dualGrid.detach_from_vertices(aPosition1);
                }
                else
                {
                    UG_LOG("Grid does not contain elements, only points. Can not create Dual Grid.\n");
                    return;
                }

                // assign subset colors
                AssignSubsetColors(dualSH);

                // update view
                dualObj->geometry_changed();
            }



        }

        const char* get_name()		{return "Create Dual Grid";}
        const char* get_tooltip()	{return "creates the dual grid consisting of"
                                                " control volumes as used in the finite volume method";}
        const char* get_group()		{return "Grid Generation";}

        ToolWidget* get_dialog(QWidget* parent){
            ToolWidget *dlg = new ToolWidget(get_name(), parent, this,
                                            IDB_OK | IDB_CANCEL);
            QStringList entries;
            entries.push_back(tr("FV1Geometry"));
            entries.push_back(tr("HFV1Geometry"));
            dlg->addComboBox("Choose Finite Volume Type", entries, 0);

            dlg->addCheckBox("Create control volumes:", true);
            dlg->addCheckBox("Create sub control volumes:", true);
            dlg->addCheckBox("Create sub control volume faces:", true);
            return dlg;
        }
 };
*/

bool CreateFractal(LGObject* obj, number scaleFac, size_t numIterations)
{
	ug::HangingNodeRefiner_Grid href(obj->get_grid());
	return ug::CreateFractal_NormalScale(obj->get_grid(), href, scaleFac, numIterations);
}

void RegisterGridGenerationTools(ToolManager* toolMgr)
{
	toolMgr->set_group_icon("Grid Generation", ":images/tool_geometry_generation.png");

	toolMgr->register_tool(new ToolCreatePlane);
	toolMgr->register_tool(new ToolCreateCircle);
	toolMgr->register_tool(new ToolCreateBox);
	toolMgr->register_tool(new ToolCreateSphere);
	toolMgr->register_tool(new ToolCreateTetrahedron);
	toolMgr->register_tool(new ToolCreatePyramid);
	toolMgr->register_tool(new ToolCreatePrism);
	toolMgr->register_tool(new ToolCreateVertex);
	toolMgr->register_tool(new ToolCreateEdge);
	toolMgr->register_tool(new ToolCreateFace);
	toolMgr->register_tool(new ToolDuplicate);
	toolMgr->register_tool(new ToolExtrude);
	toolMgr->register_tool(new ToolExtrudeCylinders);
    //toolMgr->register_tool(new ToolCreateDualGrid);

//	ug::bridge::Registry& reg = toolMgr->get_registry();
//	reg.add_function("TestFractal", &CreateFractal);
}
