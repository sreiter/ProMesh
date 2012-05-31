//	created by Sebastian Reiter
//	s.b.reiter@googlemail.com
//	y10 m05 d07

#include <vector>
#include "app.h"
#include "standard_tools.h"
#include "tools_util.h"
#include "heightfields/interpolated_heightfield.h"

using namespace ug;
using namespace std;


class ToolMove : public ITool
{
public:
	void execute(LGObject* obj, QWidget* widget){
		ToolWidget* dlg = dynamic_cast<ToolWidget*>(widget);
		ug::vector3 vMove(0, 0, 0);
		bool bLocal = true;
		bool object = true;
		if(dlg){
			object = (dlg->to_int(0) == 0);
			bLocal = (dlg->to_int(1) == 0);
			vMove.x = dlg->to_double(2);
			vMove.y = dlg->to_double(3);
			vMove.z = dlg->to_double(4);
		}

		Grid& grid = obj->get_grid();
		Grid::VertexAttachmentAccessor<APosition> aaPos(grid, aPosition);
		Selector& sel = obj->get_selector();

		if(!bLocal){
			if(object)
				VecSubtract(vMove, vMove, obj->pivot());
			else{
				vector3 center = CalculateCenter(sel.begin<VertexBase>(),
									sel.end<VertexBase>(), aaPos);
				VecSubtract(vMove, vMove, center);
			}
		}

		GeometricObjectCollection goc;
		if(object)
			goc = grid.get_geometric_objects();
		else
			goc = sel.get_geometric_objects();

		for(ug::VertexBaseIterator iter = goc.vertices_begin();
			iter != goc.vertices_end(); ++iter)
		{
			ug::vector3& v = aaPos[*iter];
			v.x += vMove.x;
			v.y += vMove.y;
			v.z += vMove.z;
		}

		if(object)
			VecAdd(obj->pivot(), obj->pivot(), vMove);

		obj->geometry_changed();
	}

	const char* get_name()		{return "Move";}
	const char* get_tooltip()	{return "Moves selected vertices.";}
	const char* get_group()		{return "Coordinate Transform";}

	ToolWidget* get_dialog(QWidget* parent){
		ToolWidget *dlg = new ToolWidget(get_name(), parent, this,
								IDB_APPLY | IDB_OK | IDB_CLOSE);
		QStringList entries1;
		entries1.push_back(tr("object"));
		entries1.push_back(tr("selected"));
		dlg->addComboBox("target:", entries1, 1);

		QStringList entries2;
		entries2.push_back(tr("local"));
		entries2.push_back(tr("global"));
		dlg->addComboBox("", entries2, 0);

		dlg->addSpinBox(tr("x:"), -1.e+9, 1.e+9, 0, 1, 9);
		dlg->addSpinBox(tr("y:"), -1.e+9, 1.e+9, 0, 1, 9);
		dlg->addSpinBox(tr("z:"), -1.e+9, 1.e+9, 0, 1, 9);
		return dlg;
	}
};

class ToolScale : public ITool
{
public:
	void execute(LGObject* obj, QWidget* widget){
	ToolWidget* dlg = dynamic_cast<ToolWidget*>(widget);
		ug::vector3 vScale(1, 1, 1);
		bool object = true;
		if(dlg){
			object = (dlg->to_int(0) == 0);
			vScale.x = dlg->to_double(1);
			vScale.y = dlg->to_double(2);
			vScale.z = dlg->to_double(3);
		}

		Grid& grid = obj->get_grid();
		Grid::VertexAttachmentAccessor<APosition> aaPos(grid, aPosition);
		Selector& sel = obj->get_selector();

		GeometricObjectCollection goc;
		vector3 center;
		if(object){
			goc = grid.get_geometric_objects();
			center = obj->pivot();
		}
		else{
			goc = sel.get_geometric_objects();
			center = CalculateCenter(goc.begin<VertexBase>(),
									goc.end<VertexBase>(),
									aaPos);
		}



		for(ug::VertexBaseIterator iter = goc.begin<VertexBase>();
			iter != goc.end<VertexBase>(); ++iter)
		{
			ug::vector3& v = aaPos[*iter];
			VecSubtract(v, v, center);
			v.x *= vScale.x;
			v.y *= vScale.y;
			v.z *= vScale.z;
			VecAdd(v, v, center);
		}

		obj->geometry_changed();
	}

	const char* get_name()		{return "Scale";}
	const char* get_tooltip()	{return "Scales the coordinates of the selected vertices around their center.";}
	const char* get_group()		{return "Coordinate Transform";}

	ToolWidget* get_dialog(QWidget* parent){
		ToolWidget *dlg = new ToolWidget(get_name(), parent, this,
								IDB_APPLY | IDB_OK | IDB_CLOSE);
		QStringList entries;
		entries.push_back(tr("object"));
		entries.push_back(tr("selected"));
		dlg->addComboBox("target:", entries, 1);

		dlg->addSpinBox(tr("x:"), -1.e+9, 1.e+9, 1., 0.1, 9);
		dlg->addSpinBox(tr("y:"), -1.e+9, 1.e+9, 1., 0.1, 9);
		dlg->addSpinBox(tr("z:"), -1.e+9, 1.e+9, 1., 0.1, 9);
		return dlg;
	}
};

class ToolRotate : public ITool
{
public:
	void execute(LGObject* obj, QWidget* widget){
	ToolWidget* dlg = dynamic_cast<ToolWidget*>(widget);
		ug::vector3 vRot(0, 0, 0);
		bool object = true;

		if(dlg){
			object = (dlg->to_int(0) == 0);
		//	convert to radiants on the fly
			vRot.x = dlg->to_double(1) * (PI / 180.);
			vRot.y = dlg->to_double(2) * (PI / 180.);
			vRot.z = dlg->to_double(3) * (PI / 180.);
		}

		Grid& grid = obj->get_grid();
		Grid::VertexAttachmentAccessor<APosition> aaPos(grid, aPosition);
		Selector& sel = obj->get_selector();

		GeometricObjectCollection goc;
		vector3 center;

		if(object){
			goc = grid.get_geometric_objects();
			center = obj->pivot();
		}
		else{
			goc = sel.get_geometric_objects();
			center = CalculateCenter(goc.begin<VertexBase>(),
									goc.end<VertexBase>(),
									aaPos);
		}

	//	todo: combine rotation and transform matrix and use ugs build in methods.
	//	rotation matrix
		matrix33 matRot;
		MatRotationYawPitchRoll(matRot, vRot.x, vRot.y, vRot.z);

	//	transform all vertices
		for(ug::VertexBaseIterator iter = goc.begin<VertexBase>();
			iter != goc.end<VertexBase>(); ++iter)
		{
			ug::vector3 v = aaPos[*iter];
			VecSubtract(v, v, center);
			MatVecMult(aaPos[*iter], matRot, v);
			VecAdd(aaPos[*iter], aaPos[*iter], center);
		}

		obj->geometry_changed();
	}

	const char* get_name()		{return "Rotate";}
	const char* get_tooltip()	{return "Rotates the geometry by the given degrees around its center.";}
	const char* get_group()		{return "Coordinate Transform";}

	ToolWidget* get_dialog(QWidget* parent){
		ToolWidget *dlg = new ToolWidget(get_name(), parent, this,
								IDB_APPLY | IDB_OK | IDB_CLOSE);
		QStringList entries;
		entries.push_back(tr("object"));
		entries.push_back(tr("selected"));
		dlg->addComboBox("target:", entries, 1);

		dlg->addSpinBox(tr("x:"), -1.e+9, 1.e+9, 0, 1, 9);
		dlg->addSpinBox(tr("y:"), -1.e+9, 1.e+9, 0, 1, 9);
		dlg->addSpinBox(tr("z:"), -1.e+9, 1.e+9, 0, 1, 9);
		return dlg;
	}
};

class ToolTransform : public ITool
{
public:
	void execute(LGObject* obj, QWidget* widget){
	ToolWidget* dlg = dynamic_cast<ToolWidget*>(widget);
		bool local = true;
		bool applyToSelection = true;

		ug::matrix44 mat;
		MatDiagSet(mat, 1.);

		if(dlg){
			local = (dlg->to_int(0) == 0);
			applyToSelection = (dlg->to_int(1) == 0);
			mat = dlg->to_matrix44(2);
		}

		Grid& grid = obj->get_grid();
		Grid::VertexAttachmentAccessor<APosition> aaPos(grid, aPosition);
		Selector& sel = obj->get_selector();

		GeometricObjectCollection goc;
		vector3 center(0, 0, 0);
		if(applyToSelection){
			goc = sel.get_geometric_objects();
			if(local)
				center = CalculateCenter(goc.begin<VertexBase>(),
										goc.end<VertexBase>(),
										aaPos);
		}
		else{
			goc = grid.get_geometric_objects();
			if(local)
				center = obj->pivot();
		}

		for(ug::VertexBaseIterator iter = goc.begin<VertexBase>();
			iter != goc.end<VertexBase>(); ++iter)
		{
			ug::vector3 v = aaPos[*iter];
			VecSubtract(v, v, center);

			vector4 v4(v.x, v.y, v.z, 1.);
			ug::vector4 vtmp;
			MatVecMult(vtmp, mat, v4);
			v = vector3(vtmp.x, vtmp.y, vtmp.z);
			if(fabs(vtmp.w) > SMALL)
				VecScale(v, v, 1. / vtmp.w);
			else{
				UG_LOG("Error during Transform: 4-th component in result is 0. Aborting\n");
				obj->geometry_changed();
				return;
			}
			VecAdd(aaPos[*iter], v, center);
		}

		obj->geometry_changed();
	}

	const char* get_name()		{return "Transform";}
	const char* get_tooltip()	{return "Transforms the vertices with the given matrix";}
	const char* get_group()		{return "Coordinate Transform";}

	ToolWidget* get_dialog(QWidget* parent){
		ToolWidget *dlg = new ToolWidget(get_name(), parent, this,
								IDB_APPLY | IDB_OK | IDB_CLOSE);
		QStringList entries;
		entries.push_back(tr("local"));
		entries.push_back(tr("global"));
		dlg->addComboBox(tr("pivot:"), entries, 0);

		entries.clear();
		entries.push_back(tr("selection"));
		entries.push_back(tr("object"));
		dlg->addComboBox(tr("target:"), entries, 0);

		dlg->addMatrix(tr("transform:"), 4, 4);
		return dlg;
	}
};

class ToolConeTransform : public ITool
{
public:
	void execute(LGObject* obj, QWidget* widget){
		ToolWidget* dlg = dynamic_cast<ToolWidget*>(widget);
		using namespace ug;

		bool applyToSelection = true;
		vector3 src(0, 0, 0), dir(0, 0, 1);
		number scaleAtTip = 1.0;

		if(dlg){
			applyToSelection = (dlg->to_int(0) == 0);
			src = dlg->to_vector3(1);
			dir = dlg->to_vector3(2);
			scaleAtTip = dlg->to_double(3);
		}

		Grid& grid = obj->get_grid();
		Grid::VertexAttachmentAccessor<APosition> aaPos(grid, aPosition);
		Selector& sel = obj->get_selector();

		GeometricObjectCollection goc;
		vector3 center(0, 0, 0);
		if(applyToSelection)
			goc = sel.get_geometric_objects();
		else
			goc = grid.get_geometric_objects();

		for(ug::VertexBaseIterator iter = goc.begin<VertexBase>();
			iter != goc.end<VertexBase>(); ++iter)
		{
			vector3 v = aaPos[*iter];
			vector3 proj(0, 0, 0);

		//	project the vertex onto the ray from src along dir
			number s = ProjectPointToRay(proj, v, src, dir);

		//	s determines the scaling amount:
		//	s = 0 => scale with 1
		//	s = 1 => scale with scaleAtTip
		//	else interpolate linear
			number scaling = (1. - s) + s * scaleAtTip;

			vector3 vtmp;
			VecSubtract(vtmp, v, proj);
			VecScale(vtmp, vtmp, scaling);
			VecAdd(aaPos[*iter], proj, vtmp);
		}

		obj->geometry_changed();
	}

	const char* get_name()		{return "Cone Transform";}
	const char* get_tooltip()	{return "Transforms the vertices with the given cone transformation";}
	const char* get_group()		{return "Coordinate Transform";}

	ToolWidget* get_dialog(QWidget* parent){
		ToolWidget *dlg = new ToolWidget(get_name(), parent, this,
								IDB_APPLY | IDB_OK | IDB_CLOSE);
		QStringList entries;
		entries.push_back(tr("selection"));
		entries.push_back(tr("object"));
		dlg->addComboBox(tr("target:"), entries, 0);

		dlg->addVector(tr("base:"), 3);
		dlg->addVector(tr("axis:"), 3);
		dlg->addSpinBox(tr("scale at tip:"), 1.e-12, 1.e+12, 1., 0.1, 9);
		return dlg;
	}
};

class ToolLaplacianSmooth : public ITool
{
public:
	void execute(LGObject* obj, QWidget* widget){
	ToolWidget* dlg = dynamic_cast<ToolWidget*>(widget);
		double alpha = 0.25;
		int numIterations = 10;
		int curInd = 0;

		if(dlg){
			alpha = dlg->to_double(0);
			numIterations = dlg->to_int(1);
			curInd = dlg->to_int(2);
		}

		Grid& grid = obj->get_grid();
		Grid::VertexAttachmentAccessor<APosition> aaPos(grid, aPosition);
		Selector& sel = obj->get_selector();
		switch(curInd)
		{
		case 0: //selected
			ug::LaplacianSmooth(grid, sel.vertices_begin(), sel.vertices_end(),
								aaPos, alpha, numIterations);
			break;
		case 1:	//all
			ug::LaplacianSmooth(grid, grid.vertices_begin(), grid.vertices_end(),
								aaPos, alpha, numIterations);
			break;
		case 2:	//inner
			{
			//	collect inner vertices
				vector<VertexBase*> innerVrts;
				if(grid.num_volumes() > 0){
					for(VertexBaseIterator iter = grid.vertices_begin();
						iter != grid.vertices_end(); ++iter)
					{
						if(!IsBoundaryVertex3D(grid, *iter))
							innerVrts.push_back(*iter);
					}
				}
				else{
					for(VertexBaseIterator iter = grid.vertices_begin();
						iter != grid.vertices_end(); ++iter)
					{
						if(!IsBoundaryVertex2D(grid, *iter))
							innerVrts.push_back(*iter);
					}
				}

				ug::LaplacianSmooth(grid, innerVrts.begin(), innerVrts.end(),
									aaPos, alpha, numIterations);
			}break;
		}

		obj->geometry_changed();
	}

	const char* get_name()		{return "Laplacian Smooth";}
	const char* get_tooltip()	{return "Smoothes vertices in a grid.";}
	const char* get_group()		{return "Coordinate Transform";}

	ToolWidget* get_dialog(QWidget* parent){
		ToolWidget *dlg = new ToolWidget(get_name(), parent, this,
								IDB_APPLY | IDB_OK | IDB_CLOSE);
		dlg->addSpinBox(tr("alpha:"), 0, 1., 0.25, 0.001, 5);
		dlg->addSpinBox(tr("iterations:"), 1, 10000000, 10, 1, 0);
		QStringList entries;
		entries.push_back(tr("selected"));
		entries.push_back(tr("all"));
		entries.push_back(tr("inner"));
		dlg->addComboBox("nodes:", entries, 0);
		return dlg;
	}
};

class ToolProjectToLimitPLoop : public ITool
{
public:
	void execute(LGObject* obj, QWidget*){
		Grid& grid = obj->get_grid();
		ug::ProjectToLimitPLoop(grid, aPosition, aPosition);

		obj->geometry_changed();
	}

	const char* get_name()		{return "Project to Limit PLoop";}
	const char* get_tooltip()	{return "Projects all vertices in the grid to their limit "
										"positions as defined by the piecewise loop scheme.";}
	const char* get_group()		{return "Coordinate Transform | Loop Projection";}
};

class ToolProjectToLimitSmoothBoundary : public ITool
{
public:
	void execute(LGObject* obj, QWidget*){
		Grid& grid = obj->get_grid();
		ug::ProjectToLimitSubdivBoundary(grid, aPosition, aPosition);

		obj->geometry_changed();
	}

	const char* get_name()		{return "Project to Limit Smooth Boundary";}
	const char* get_tooltip()	{return "Projects all boundary-vertices in the grid to their limit "
										"positions as defined by the b-spline subdivision scheme.";}
	const char* get_group()		{return "Coordinate Transform | Loop Projection";}
};

class ToolSetPivot : public ITool
{
public:
	void execute(LGObject* obj, QWidget* widget){
		CoordinatesWidget* dlg = dynamic_cast<CoordinatesWidget*>(widget);
		ug::vector3 newPivot(0, 0, 0);

		if(dlg){
		//	convert to radiants on the fly
			newPivot.x = dlg->x();
			newPivot.y = dlg->y();
			newPivot.z = dlg->z();
		}

		obj->pivot() = newPivot;
	}

	const char* get_name()		{return "Set Pivot";}
	const char* get_tooltip()	{return "Sets the pivot point of the selected object.";}
	const char* get_group()		{return "Coordinate Transform | Pivot";}

	QWidget* get_dialog(QWidget* parent){
		return new CoordinatesWidget(get_name(), parent, this, false);
	}
};

class ToolSetPivotToCenter : public ITool
{
public:
	void execute(LGObject* obj, QWidget*){
		vector3 vMin, vMax;
		obj->get_bounding_box(vMin, vMax);

		VecScaleAdd(obj->pivot(), 0.5, vMin, 0.5, vMax);
	}

	const char* get_name()		{return "Set Pivot To Center";}
	const char* get_tooltip()	{return "Sets the pivot point of the selected object to the center of the bounding box.";}
	const char* get_group()		{return "Coordinate Transform | Pivot";}
};


class ToolFlattenBentQuadrilaterals : public ITool
{
public:
	void execute(LGObject* obj, QWidget* widget){
		using namespace ug;

		ToolWidget* dlg = dynamic_cast<ToolWidget*>(widget);

		int numIterations = 100;
		number stepSize = 0.01;

		if(dlg){
			numIterations = dlg->to_int(0);
			stepSize = dlg->to_double(1);
		}

		UG_LOG("Flattening bent quadrilaterals. Note that resulting faces are not necessarily flat!\n");

		Grid& grid = obj->get_grid();
		Grid::VertexAttachmentAccessor<APosition> aaPos(grid, aPosition);

		for(int i = 0; i < numIterations; ++i){
		//	iterate over all quadrilaterals
			for(QuadrilateralIterator iter = grid.begin<Quadrilateral>();
				iter != grid.end<Quadrilateral>(); ++iter)
			{
				Quadrilateral* q = *iter;

			//	find the plane that minimizes the distance to the corner points
			//	of the quadrilateral.
				vector3 p, n;
				vector3 points[4] =	{aaPos[q->vertex(0)], aaPos[q->vertex(1)],
									 aaPos[q->vertex(2)], aaPos[q->vertex(3)]};

				if(FindClosestPlane(p, n, points, 4)){
				//	find the projections of the corners and move the corner
				//	vertices a little into that direction.
					for(size_t i = 0; i < 4; ++i){
						vector3 proj;
						ProjectPointToPlane(proj, points[i], p, n);
						VecScaleAdd(aaPos[q->vertex(i)], 1. - stepSize, points[i],
									stepSize, proj);
					}
				}
			}
		}

		obj->geometry_changed();
	}

	const char* get_name()		{return "Flatten Bent Quads";}
	const char* get_tooltip()	{return "Flattens bent quadrilaterals using an iterative flattening scheme";}
	const char* get_group()		{return "Coordinate Transform";}

	QWidget* get_dialog(QWidget* parent){
		ToolWidget *dlg = new ToolWidget(get_name(), parent, this,
								IDB_APPLY | IDB_OK | IDB_CLOSE);
		dlg->addSpinBox(tr("iterations:"), 1, 1.e6, 100, 10, 0);
		dlg->addSpinBox(tr("step size:"), 1.e-9, 1, 0.01, 0.01, 9);
		return dlg;
	}
};

class ToolApplyHeightfield : public ITool
{
public:
	void execute(LGObject* obj, QWidget* widget){
		using namespace ug;

		ToolWidget* dlg = dynamic_cast<ToolWidget*>(widget);

		QString filename;
		if(dlg){
			filename = dlg->to_string(0);
		}

		if(!filename.isEmpty()){
			InterpolatedHeightfield interpHf;
			IHeightfield* hf = &interpHf;

			LGObject* obj = app::getActiveObject();
			if(!obj)
				return;

			UG_LOG("Applying heightfield: " << filename.toStdString() << "\n");

			Grid& g = obj->get_grid();
			Grid::VertexAttachmentAccessor<APosition> aaPos(g, aPosition);
			vector3 min, max;

			CalculateBoundingBox(min, max, g.vertices_begin(), g.vertices_end(), aaPos);

			if(hf->initialize(filename.toStdString().c_str(), min.x, min.y,
							max.x, max.y))
			{
			//	iterate over all nodes and adjust height.
				for(Grid::traits<VertexBase>::iterator iter = g.vertices_begin();
					iter != g.vertices_end(); ++iter)
				{
					vector3& v = aaPos[*iter];
					v.z = hf->height(v.x, v.y);
				}
			}

			obj->geometry_changed();
		}

	}

	const char* get_name()		{return "Apply Heightfield";}
	const char* get_tooltip()	{return "Calculates z-values of all nodes in terms of their x and y values.";}
	const char* get_group()		{return "Coordinate Transform | Heightfields";}

	QWidget* get_dialog(QWidget* parent){
		ToolWidget *dlg = new ToolWidget(get_name(), parent, this,
								IDB_APPLY | IDB_OK | IDB_CLOSE);
		dlg->addFileBrowser(tr("heightfield:"), FWT_OPEN, "*.*");
		return dlg;
	}
};

void RegisterCoordinateTransformTools(ToolManager* toolMgr)
{
	toolMgr->set_group_icon("Coordinate Transform", ":images/tool_transform.png");

	toolMgr->register_tool(new ToolSetPivot);
	toolMgr->register_tool(new ToolSetPivotToCenter);

	toolMgr->register_tool(new ToolMove);
	toolMgr->register_tool(new ToolScale);
	toolMgr->register_tool(new ToolRotate);
	toolMgr->register_tool(new ToolTransform);
	toolMgr->register_tool(new ToolConeTransform);
	toolMgr->register_tool(new ToolLaplacianSmooth);
	toolMgr->register_tool(new ToolProjectToLimitPLoop);
	toolMgr->register_tool(new ToolProjectToLimitSmoothBoundary);
	toolMgr->register_tool(new ToolFlattenBentQuadrilaterals);
	toolMgr->register_tool(new ToolApplyHeightfield);
}
