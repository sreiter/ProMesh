/*
 * Copyright (c) 2017:  G-CSC, Goethe University Frankfurt
 * Author: Sebastian Reiter
 * 
 * This file is part of ProMesh.
 * 
 * ProMesh is free software: you can redistribute it and/or modify it under the
 * terms of the GNU Lesser General Public License version 3 (as published by the
 * Free Software Foundation) with the following additional attribution
 * requirements (according to LGPL/GPL v3 §7):
 * 
 * (1) The following notice must be displayed in the Appropriate Legal Notices
 * of covered and combined works: "Based on ProMesh (www.promesh3d.com)".
 * 
 * (2) The following bibliography is recommended for citation and must be
 * preserved in all covered files:
 * "Reiter, S. and Wittum, G. ProMesh -- a flexible interactive meshing software
 *   for unstructured hybrid grids in 1, 2, and 3 dimensions. In preparation."
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU Lesser General Public License for more details.
 */

#ifndef __H__PROMESH_tetgen_tools
#define __H__PROMESH_tetgen_tools

#include <QString>
#include <QProcess>
#include "promesh_plugin.h"
#include "app.h"
#include "standard_tools.h"
#include "registry/registry.h"
#include "bridge/util.h"
#include "tooltips.h"
#include "tools/file_io_tools.h"
#include "lib_grid/file_io/file_io_tetgen.h"

using namespace ug;
using namespace std;
using namespace ug::promesh;
using namespace ug::bridge;
using namespace app;


static QString BuildTetgenArguments (
        bool meshPLC,
        bool remesh,
		number maxRadiusEdgeRatio,
       	number minDihedralAngle,
		bool preserveOuter,
		bool preserveAll,
		int verbosity)
{
	QString args;
	if(verbosity == 1)
		args.append("V");
	else if(verbosity == 2)
		args.append("VV");
	else if(verbosity >= 3)
		args.append("VVV");

	if(meshPLC)
		args.append("p");

	if(remesh)
		args.append("ar");

	args.append("q").append(QString::number(maxRadiusEdgeRatio))
	    .append("/").append(QString::number(minDihedralAngle));
	if(preserveOuter || preserveAll)
		args.append("Y");
	if(preserveAll)
		args.append("Y");	// if inner bnds shall be preserved "YY" has to be passed to tetgen

	args.append("Q");
	return args;
}

static void RemoveTetgenFiles (const QString& eleFileName)
{
	QString filename = eleFileName;
	QFile::remove(filename);
	filename.replace(QString(".ele"), QString(".node"));
	QFile::remove(filename);
	filename.replace(QString(".node"), QString(".edge"));
	QFile::remove(filename);
	filename.replace(QString(".edge"), QString(".face"));
	QFile::remove(filename);
	filename.replace(QString(".face"), QString(".vol"));
	QFile::remove(filename);
}

static
void TetrahedralizeEx (	Mesh* mesh,
                        number maxRadiusEdgeRatio,
                       	number minDihedralAngle,
						bool preserveOuter,
						bool preserveAll,
						bool separateVolumes,
						bool appendSubsetsAtEnd,
						int verbosity,
						double timeOut)
{

	QString outFileName = TmpFileName("plc", ".smesh");
	// UG_LOG("Saving to file: " << outFileName.toLocal8Bit().constData() << std::endl);

	if(!SaveMesh(mesh, outFileName.toLocal8Bit().constData())){
		UG_THROW("SaveMesh failed with mesh '" << outFileName.toLocal8Bit().constData() << "' in Tetrahedral Fill\n");
	}

	QString args;
	args.append("-").append(BuildTetgenArguments(mesh->grid().num_faces() > 0,
	                            false,
	                            maxRadiusEdgeRatio,
                                minDihedralAngle,
                                preserveOuter,
                                preserveAll,
                                verbosity));

	args.append(" ").append(outFileName);


	QString call = AppDir().path()	.append(QDir::separator())
									.append("tools")
									.append(QDir::separator())
									.append("tetgen");
	// UG_LOG("Calling: '" << call.toLocal8Bit().constData() << "'\n");

	UG_LOG("Calling 'tetgen' by Hang Si (www.tetgen.org)\n");

	QProcess proc;
	proc.setProcessChannelMode(QProcess::MergedChannels);
	proc.start(call, args.split(' '));

	if(!proc.waitForFinished(timeOut * 1000)){
		if(proc.state() == QProcess::Running)
			proc.kill();
		UG_THROW("Received error during execution of tetgen: "
		         << proc.errorString().toLocal8Bit().constData());
	}
	else{
		QString output(proc.readAll());
		output.replace(QString("\r\n"), QString("\n"));
		output.replace(QString("\r"), QString("\n"));
		UG_LOG(output.toLocal8Bit().constData() << "\n");
	}

	mesh->grid().clear_geometry();
	QString inFileName(outFileName);
	inFileName.replace(QString(".smesh"), QString(".1.ele"));
	LoadMesh(mesh, inFileName.toLocal8Bit().constData());

//	remove temporary files
	QFile::remove(outFileName);
	RemoveTetgenFiles(inFileName);

	SubsetHandler& sh = mesh->subset_handler();
	Grid& grid = mesh->grid();

	int oldNumSubsets = sh.num_subsets();
	if(separateVolumes){
		SeparateSubsetsByLowerDimSubsets<Volume>(grid, sh, appendSubsetsAtEnd);
	}
	else if(appendSubsetsAtEnd){
		sh.assign_subset(grid.begin<Tetrahedron>(),
						 grid.end<Tetrahedron>(), sh.num_subsets());
	}

//	assign a subset name
	for(int i = oldNumSubsets; i < sh.num_subsets(); ++i)
		sh.subset_info(i).name = "tetrahedra";

	UG_LOG("Done\n");
}

static
void RetetrahedralizeEx (Mesh* mesh,
                       	number maxRadiusEdgeRatio,
                       	number minDihedralAngle,
						bool preserveOuter,
						bool preserveAll,
						int verbosity,
						double timeOut)
{

	QString outFileName = TmpFileName("retet", ".ele");
	// UG_LOG("Saving to file: " << outFileName.toLocal8Bit().constData() << std::endl);

	if(!SaveGridToELE(mesh->grid(), outFileName.toLocal8Bit().constData(),
	                  &mesh->subset_handler(), mesh->position_attachment(),
	                  &mesh->volume_constraint_attachment())){
		UG_THROW("SaveMesh failed with mesh '" << outFileName.toLocal8Bit().constData() << "' in Tetrahedral Fill\n");
	}
	QString args;
	args.append("-").append(BuildTetgenArguments(false, true,
                                maxRadiusEdgeRatio,
                                minDihedralAngle,
                                preserveOuter,
                                preserveAll,
                                verbosity));
	args.append(" ").append(outFileName);


	QString call = AppDir().path()	.append(QDir::separator())
									.append("tools")
									.append(QDir::separator())
									.append("tetgen");
	// UG_LOG("Calling: '" << call.toLocal8Bit().constData() << "'\n");

	UG_LOG("Calling 'tetgen' by Hang Si (www.tetgen.org)\n");

	QProcess proc;
	proc.setProcessChannelMode(QProcess::MergedChannels);
	proc.start(call, args.split(' '));

	if(!proc.waitForFinished(timeOut * 1000)){
		if(proc.state() == QProcess::Running)
			proc.kill();
		UG_THROW("Received error during execution of tetgen: "
		         << proc.errorString().toLocal8Bit().constData());
		return;
	}
	else{
		QString output(proc.readAll());
		output.replace(QString("\r\n"), QString("\n"));
		output.replace(QString("\r"), QString("\n"));
		UG_LOG(output.toLocal8Bit().constData() << "\n");
	}

	mesh->grid().clear_geometry();
	QString inFileName(outFileName);
	inFileName.replace(QString(".ele"), QString(".1.ele"));
	LoadMesh(mesh, inFileName.toLocal8Bit().constData());

//	remove temporary files
	RemoveTetgenFiles(outFileName);
	RemoveTetgenFiles(inFileName);

	UG_LOG("Done\n");
}


void RegisterTetgenTools ()
{
	ProMeshRegistry& reg = GetProMeshRegistry();

	string grp = "ug4/promesh/Remeshing/Tetrahedra";
	reg.add_function("TetrahedralFill", &TetrahedralizeEx, grp, "",
				"mesh #"
				"max radius edge ratio || value=2; min=1; step=0.1D #"
				"min dihedral angle || value=5; min=0; max=18; step=1 #"
				"preserve outer #"
				"preserve all #"
				"separate volumes || value=true #"
				"append subsets at end || value=true#"
				"verbosity || min=0; value=0; max=3; step=1#"
				"time out (s) || min= -1; value=10; step=1",
				"Fills a closed surface with tetrahedra using TetGen. "
					"Aborts if no result was computet after 'time out' elapsed.");

	reg.add_function("RemeshTetrahedra", &RetetrahedralizeEx, grp, "",
				"mesh #"
				"max radius edge ratio || value=2; min=1; step=0.1D #"
				"min dihedral angle || value=5; min=0; max=18; step=1 #"
				"preserve outer #"
				"preserve all #"
				"verbosity || min=0; value=0; max=3; step=1#"
				"time out (s) || min= -1; value=10; step=1",
				"Given a tetrahedralization and volume constraints, "
					"this method adapts the tetrahedra using TetGen. "
					"Aborts if no result was computet after 'time out' elapsed.");
}

#endif	//__H__UG_tetgen_tools
