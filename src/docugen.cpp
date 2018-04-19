/*
 * Copyright (c) 2008-2015:  G-CSC, Goethe University Frankfurt
 * Copyright (c) 2006-2008:  Steinbeis Forschungszentrum (STZ Ölbronn)
 * Copyright (c) 2006-2015:  Sebastian Reiter
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

#include <iostream>
#include <fstream>
#include <streambuf>
#include <map>
#include "bridge/bridge.h"
#include "common/util/path_provider.h"
#include "common/util/plugin_util.h"
#include "common/util/string_util.h"
#include "common/util/file_util.h"
#include "../../plugins/ProMesh/mesh.h"
#include "tools/standard_tools.h"
#include "docugen.h"

using namespace std;
using namespace ug;
using namespace ug::bridge;
using namespace ug::promesh;

static
void DefineGroup(ostream& out, const char* id, const char* name, const char* desc,
				 const char* parentGroup = NULL);
static void DefineType(ostream& out, const char* type, const char* desc, const char* group);
static void WriteClass(ostream& out, Registry& reg, const char* name, const char* forceGroup = NULL);
static void WriteClass(ostream& out, Registry& reg, const IExportedClass* cls, const char* forceGroup = NULL);
static void WriteFunction(ostream& out, Registry& reg, const char* name, const char* forceGroup = NULL);
static void WriteFunction(ostream& out, Registry& reg, const ExportedFunction* func, const char* forceGroup = NULL);
static void WriteGroupMembers(ostream& out, Registry& reg, const char* id);
static void WriteGroupID(ostream& out, const char* id);
static void WriteGroupID(ostream& out, const string& id);
static void WriteFunctionSignature(ostream& out, const ExportedFunctionBase* func,
							const char* prefix = "", bool isConst = false);
static string ParamToString(const ParameterInfo& info, int i);
static string NameToVarName(const string& str);
static string GroupNameToID(const string& str);

static void GenerateResourceFile(const string& filename, const string& searchPath, const string& targetPath);

#define mkpath(s)	(AdjustPathSeparators(mkstr(s)))

static map<string, bool>	groups;
static map<string, string>	originalGroupNames;


static
std::string AdjustPathSeparators(std::string str){
	#ifdef UNIX
		return ReplaceAll(str, "\\", "/");
	#elif WINDOWS
		return ReplaceAll(str, "/", "\\");
	#endif
}

static
void DeleteDirectory(string path){
	#ifdef UNIX
		system(mkstr("rm -rf " << path).c_str());
	#elif WINDOWS
		system(mkstr("RD /S /Q " << path).c_str());
	#endif
}

static
void CopyFile(string from, string to){
	#ifdef UNIX
		system(mkstr("cp -f " << from << " " << to).c_str());
	#elif WINDOWS
		system(mkstr("copy " << from << " " << to << "/y").c_str());
	#endif
}

static
void CopyDirectory(string from, string to){
	#ifdef UNIX
		system(mkstr("cp -r " << from << " " << to).c_str());
	#elif WINDOWS
		system(mkstr("xcopy " << from << " " << to << "/s /e /y").c_str());
	#endif
}

static
void ReplaceInFile(string filename, string searchStr, string newStr){
	#ifdef UNIX
		// string cmdline = mkstr("sed -i '/" << searchStr << "/c\\" << newStr
		// 					   << "' " << filename);
		// system(cmdline.c_str());
		UG_THROW("ReplaceInFile not yet implemented for UNIX");
	#elif WINDOWS
		string cmdline = mkstr(
				"powershell -Command \"(gc "
			<<	filename
			<< 	") -replace '"
			<<	searchStr
			<<	"', '"
			<<	newStr
			<<	"' | Out-File -encoding ASCII "
			<<	filename
			<<	" \"");
		system(cmdline.c_str());
	#endif	
}

static
string ReadFile(string filename){
	ifstream t(filename.c_str());

	UG_COND_THROW(!t, "Couldn't read file " << filename << endl);

	string str;

	t.seekg(0, ios::end);   
	str.reserve(t.tellg());
	t.seekg(0, ios::beg);

	str.assign((istreambuf_iterator<char>(t)),
	            istreambuf_iterator<char>());
	return str;
}



int RunDocugen()
{
	cout << "docugen - script documentation generator for promesh" << endl;
	cout << "Rebuild ProMesh once docugen terminated to include the new documentation." << endl;
	const string pmPath = AdjustPathSeparators(PROMESH_ROOT_PATH);
	cout << "promesh-root-path: " << pmPath << endl;

	try{
		groups				= map<string, bool>();
		originalGroupNames	= map<string, string>();

		// InitBridge();
		// if(!LoadPlugins(PathProvider::get_path(PLUGIN_PATH).c_str(), "", GetUGRegistry()))
		// {
		// 	UG_LOG("ERROR during initialization of plugins: LoadPlugins failed!\n");
		// }
	
		// RegisterTetgenTools();

		UG_LOG("Generating scripting reference" << endl);

		Registry& reg = GetUGRegistry();
		ofstream out("script_reference.cpp");
		DefineGroup(out, "nativeTypes", "native types", "Native types defined by the underlying programming language.");
		DefineGroup(out, "ugbase", "ug-base", "Classes and functions from ug4 which are provided in ProMesh scripts.");
		DefineGroup(out, "ug4_promesh", "ProMesh Tools", "All classes and tools defined by ProMesh.");
		DefineGroup(out, "ug4_promesh_Selection", "Selection", "Tools to perform and to manipulate the current selection.", "ug4_promesh");
		DefineGroup(out, "ug4_promesh_Selection_Vertices", "Vertices", "Selection tools for vertices.", "ug4_promesh_Selection");
		DefineGroup(out, "ug4_promesh_Selection_Edges", "Edges", "Selection tools for edges.", "ug4_promesh_Selection");
		DefineGroup(out, "ug4_promesh_Selection_Faces", "Faces", "Selection tools for faces.", "ug4_promesh_Selection");
		DefineGroup(out, "ug4_promesh_Selection_Volumes", "Volumes", "Selection tools for volumes.", "ug4_promesh_Selection");
		DefineGroup(out, "ug4_promesh_Selection_Coordinate_Range", "Coordinate Range", "Selection by coordinate ranges.", "ug4_promesh_Selection");
		DefineGroup(out, "ug4_promesh_Selection_Vertices_Coordinate_Based", "Coordinate Based", "Coordinate based vertex selection", "ug4_promesh_Selection_Vertices");
		DefineGroup(out, "ug4_promesh_Selection_Edges_Coordinate_Based", "Coordinate Based", "Coordinate based edge selection", "ug4_promesh_Selection_Edges");
		DefineGroup(out, "ug4_promesh_Selection_Faces_Coordinate_Based", "Coordinate Based", "Coordinate based face selection", "ug4_promesh_Selection_Faces");
		DefineGroup(out, "ug4_promesh_Selection_Volumes_Coordinate_Based", "Coordinate Based", "Coordinate based volume selection", "ug4_promesh_Selection_Volumes");
		DefineGroup(out, "ug4_promesh_Selection_Subset_Based", "Subset Based", "Selection by subsets", "ug4_promesh_Selection");
		DefineGroup(out, "ug4_promesh_Selection_Vertices_Kinks", "Kinks", "Selection by kinks", "ug4_promesh_Selection_Vertices");

		DefineGroup(out, "ug4_promesh_Coordinate_Transform", "Coordinate Transform", "Tools to transform the mesh or individual vertices.", "ug4_promesh");
		DefineGroup(out, "ug4_promesh_File_IO", "File IO", "Functions to read and write meshes from/to files.", "ug4_promesh");
		DefineGroup(out, "ug4_promesh_Grid_Generation", "Grid Generation", "Tools to create new grids from scratch.", "ug4_promesh");
		DefineGroup(out, "ug4_promesh_Grid_Generation_Basic_Elements", "Basic Elements", "Creation of basic grid elements from the current selection.", "ug4_promesh_Grid_Generation");
		DefineGroup(out, "ug4_promesh_Grid_Generation_Geometries", "Geometries", "Creation of basic geometries like cubes and spheres.", "ug4_promesh_Grid_Generation");
		DefineGroup(out, "ug4_promesh_Grid_Generation_Geometries_2D", "2D", "Creation of 2D geometries.", "ug4_promesh_Grid_Generation_Geometries");
		DefineGroup(out, "ug4_promesh_Grid_Generation_Geometries_3D", "3D", "Creation of 3D geometries.", "ug4_promesh_Grid_Generation_Geometries");
		DefineGroup(out, "ug4_promesh_Info", "Info", "Tools that provide various information on a mesh.", "ug4_promesh");
		DefineGroup(out, "ug4_promesh_Info_Measure_length__area__volume", "Measure length, area, volume", "Tools to measure some geometric properties of a mesh.", "ug4_promesh_Info");
		DefineGroup(out, "ug4_promesh_Remeshing", "Remeshing", "Tools to alter the topology of the underlying grid.", "ug4_promesh");
		DefineGroup(out, "ug4_promesh_Raster_Layers", "Raster Layers", "Tools to generate meshes for stacks of raster data.", "ug4_promesh_Remeshing");
		DefineGroup(out, "ug4_promesh_Remeshing_Edges", "Edges", "Various operations on selected edges (split, swap, collapse)", "ug4_promesh_Remeshing");
		DefineGroup(out, "ug4_promesh_Remeshing_Triangles", "Triangles", "Various operations for triangle meshes", "ug4_promesh_Remeshing");
		DefineGroup(out, "ug4_promesh_Remeshing_Quadrilaterals", "Quadrilaterals", "Various operations for quadrilateral meshes", "ug4_promesh_Remeshing");
		DefineGroup(out, "ug4_promesh_Remeshing_Extrusion", "Extrusion", "Generation of new elements through extrusion of selected lower dimensional elements into a new dimension.", "ug4_promesh_Remeshing");
		DefineGroup(out, "ug4_promesh_Remeshing_Merge_Vertices", "Merge Vertices", "Tools to merge selected elements into a single vertex.", "ug4_promesh_Remeshing");
		DefineGroup(out, "ug4_promesh_Remeshing_Orientation", "Orientation", "Tools to adjust or invert the orientation of selected elements.", "ug4_promesh_Remeshing");
		DefineGroup(out, "ug4_promesh_Remeshing_Polylines", "Polylines", "Simplification of polygonal lines.", "ug4_promesh_Remeshing");
		DefineGroup(out, "ug4_promesh_Remeshing_Refinement", "Refinement", "Generation of new elements through refinement of selected existing elements", "ug4_promesh_Remeshing");
		DefineGroup(out, "ug4_promesh_Remeshing_Remove_Doubles", "Remove Doubles", "Removal of duplicate geometry.", "ug4_promesh_Remeshing");
		DefineGroup(out, "ug4_promesh_Remeshing_Resolve_Intersections", "Resolve Intersections", "Tools to resolve geometric intersections of different elements with the goal to construct consistent meshes.", "ug4_promesh_Remeshing");
		DefineGroup(out, "ug4_promesh_Remeshing_Resolve_Intersections_Advanced", "Advanced", "Tools to resolve special geomegric intersections separately from each other.", "ug4_promesh_Remeshing_Resolve_Intersections");
		DefineGroup(out, "ug4_promesh_Remeshing_Tetrahedra", "Tetrahedra", "Tetrahedral mesh generation and remeshing of tetrahedral meshes.", "ug4_promesh_Remeshing");
		DefineGroup(out, "ug4_promesh_Remeshing_Hexahedra", "Hexahedra", "Operations for hexahedral meshes.", "ug4_promesh_Remeshing");
		DefineGroup(out, "ug4_promesh_Remeshing_Triangulation", "Triangulation", "Several algorithms to generate and to remesh triangular grids.", "ug4_promesh_Remeshing");
		DefineGroup(out, "ug4_promesh_Remeshing_Boolean_Operations", "Boolean Operations", "Algorithms to perform boolean operations on closed manifolds.", "ug4_promesh_Remeshing");

		DefineGroup(out, "ug4_promesh_Selection_Marks", "Marks", "Tools to set and remove marks on a mesh.", "ug4_promesh_Selection");
		DefineGroup(out, "ug4_promesh_Subsets", "Subsets", "Tools to group elements in subsets and to manage those subsets", "ug4_promesh");
		DefineGroup(out, "ug4_promesh_Subsets_Separate", "Separate", "Tools to generate new subsets by separating elements in given subsets by topological or geometrical properties.", "ug4_promesh");
		DefineGroup(out, "ug4_promesh_Util", "Util", "Utility classes and functions for scripting.", "ug4_promesh");

		DefineGroup(out, "ug4_promesh_Coordinate_Transform_Move", "Move", "Tools to move the current selection or the current mesh.", "ug4_promesh_Coordinate_Transform");
		DefineGroup(out, "ug4_promesh_Coordinate_Transform_Pivot", "Pivot", "Tools to set the pivot of the current mesh.", "ug4_promesh_Coordinate_Transform");
		DefineGroup(out, "ug4_promesh_Coordinate_Transform_Rotate", "Rotate", "Tools to rotate the current selection", "ug4_promesh_Coordinate_Transform");
		DefineGroup(out, "ug4_promesh_Coordinate_Transform_Scale", "Scale", "Tools to scale the current selection", "ug4_promesh_Coordinate_Transform");
		DefineGroup(out, "ug4_promesh_Coordinate_Transform_Smoothing", "Smoothing", "Tools that relocate the vertices in order to improve the smoothness of a surface.", "ug4_promesh_Coordinate_Transform");
		DefineGroup(out, "ug4_promesh_Coordinate_Transform_Subdivision_Projection", "Subdivision Projection", "Projection of vertices to their limit position according to certain subdivision schemes.", "ug4_promesh_Coordinate_Transform");

		DefineType(out, "const", "A qualifier that indicates that the declared value will not be changed by a function.", "nativeTypes");
		DefineType(out, "bool", "Boolean variable that can hold the values 'true' or 'false'", "nativeTypes");
		DefineType(out, "char", "Variable that stores characters (abcde...01234...)", "nativeTypes");
		DefineType(out, "int", "Integer variable that can hold whole numbers (-inf,+inf)", "nativeTypes");
		DefineType(out, "size_t", "Integer variable that can hold positive whole numbers [0,+inf)", "nativeTypes");
		DefineType(out, "float", "Floating point variable that can hold real numbers (-inf,+inf) at lower precision", "nativeTypes");
		DefineType(out, "double", "Floating point variable that can hold real numbers (-inf,+inf) at higher precision", "nativeTypes");
		DefineType(out, "number", "Floating point variable that can hold real numbers (-inf,+inf) at higher precision", "nativeTypes");
		DefineType(out, "string", "Holds words or longer texts. Essentially an array of char's", "nativeTypes");
		DefineType(out, "vector", "An array of values (the concrete value type depends on the template paramter)", "nativeTypes");

		WriteClass(out, reg, "Vec1d", "ugbase");
		WriteClass(out, reg, "Vec2d", "ugbase");
		WriteClass(out, reg, "Vec3d", "ugbase");
		WriteClass(out, reg, "Vec4d", "ugbase");
		WriteClass(out, reg, "Mesh", "ugbase");
		WriteClass(out, reg, "Grid", "ugbase");
		WriteClass(out, reg, "ISelector", "ugbase");
		WriteClass(out, reg, "Selector", "ugbase");
		WriteClass(out, reg, "ISubsetHandler", "ugbase");
		WriteClass(out, reg, "SubsetHandler", "ugbase");
		WriteClass(out, reg, "GridObject", "ugbase");
		WriteClass(out, reg, "Vertex", "ugbase");
		WriteClass(out, reg, "Edge", "ugbase");
		WriteClass(out, reg, "Face", "ugbase");
		WriteClass(out, reg, "Volume", "ugbase");
		// WriteClass(out, reg, "Heightfield", "ugbase");
		WriteClass(out, reg, "RasterLayers", "ug4_promesh_Raster_Layers");
		WriteClass(out, reg, "RasterLayerDesc", "ug4_promesh_Raster_Layers");
		
		WriteGroupMembers(out, reg, "ug4/promesh");

	//	iterate over all groups and log undefined ones. Add a dummy definition for
	//	each such group and associated undefined parent groups.
		bool groupsMissing = false;
		for(map<string, bool>::iterator i = groups.begin(); i != groups.end(); ++i){
			if(i->second == false){
				string fullGrpId = i->first;
				string fullGrpName = originalGroupNames[fullGrpId];
				UG_COND_THROW(fullGrpName.empty(), "No orignial group name was recorded for the group id " << fullGrpId);

				do{
					UG_LOG("UNDEFINED GROUP: " << fullGrpName
						   << " (with group-id: " << fullGrpId << ")" << endl);

					string grpName, parentGrpName;
					size_t pos = fullGrpName.find_last_of("/");
					if(pos == string::npos)
						grpName = fullGrpName;
					else{
						parentGrpName = fullGrpName.substr(0, pos);
						grpName = fullGrpName.substr(pos + 1);
					}
					
					out << "/** \\defgroup " << GroupNameToID(fullGrpName) << " " << grpName;
					if(!parentGrpName.empty())
						out << endl << " *   \\ingroup " << GroupNameToID(parentGrpName);
					out << "*/" << endl;

				//	define unknown parent groups if necessary
					fullGrpId = GroupNameToID(parentGrpName);
					if(groups.find(fullGrpId) == groups.end()){
					//	parent group is a completely unknown group and has to be defined
						fullGrpName = parentGrpName;
					}
					else
						fullGrpName = "";
				}while(!fullGrpName.empty());
			}
		}

		out.close();

	//	copy recent changes and embed it in a doxy-compatible file
		{
			ofstream out("recent_changes.cpp");
			out << "/** \\page pageRecentChanges Recent Changes" << endl;
			out << "\\verbatim" << endl;
			out << ReadFile(mkpath(pmPath << "recent_changes.txt")) << endl;
			out << "\\endverbatim" << endl;
			out << "*/" << endl;
		}

	//	copy the docu-sources to this folder so that doxygen can find them
		if(DirectoryExists(mkpath("./docugen")))
			DeleteDirectory(mkpath("./docugen"));
		CopyDirectory(mkpath(pmPath << "docugen"), mkpath("./docugen/"));

	//	adjust the doxygen script by adding the current version number
		string version = ReadFile(mkpath(pmPath << "version.txt"));
		ReplaceInFile(mkpath(mkpath("./docugen/DoxygenConfig.txt")),
						"vx.y.z", mkstr("v" << version));

	//	'clear' old documentation
		if(DirectoryExists(mkpath("./html")))
			DeleteDirectory(mkpath("./html"));

	//	call doxygen to generate the source tree
		UG_LOG("Executing doxygen...\n");
		string doxyDesc = mkpath("docugen/DoxygenConfig.txt");
		if(system(mkstr("doxygen " << doxyDesc).c_str()) != 0){
			UG_THROW("Execution of 'doxygen' failed for doxy-file " << doxyDesc);
		}
		UG_LOG(endl);

	//	set up the docs/html folder in PROMESH_ROOT_PATH.
	//	If the html path already exists we'll delete it. We'll use some os-specific
	//	functions here...
		if(DirectoryExists(mkpath(pmPath << "docs")))
			DeleteDirectory(mkpath(pmPath << "docs"));
		CopyDirectory(mkpath("./html"), mkpath(pmPath << "docs/"));

	//	copy the version information into the html folder
		CopyFile(mkpath(pmPath << "version.txt"), mkpath(pmPath << "docs/version.txt"));

	//	now we'll create the qt-resource file containing all the help files
		UG_LOG("Generating qt resource file...\n");
		GenerateResourceFile(mkpath(pmPath << "docs/html-resources.qrc"), "html", "");

		UG_LOG("done\n");
	}
	catch(UGError& err){
		UG_LOG("An error occurred:\n");
		for(size_t i = 0; i < err.num_msg(); ++i){
			UG_LOG("  " << err.get_msg(i) << std::endl);
		}
		return 1;
	}
	return 0;
}


static
void DefineGroup(ostream& out, const char* id, const char* name, const char* desc,
				 const char* parentGroup)
{
	groups[id] = true;
	out << "/** \\defgroup " << id << " " << name << endl;
	out << " *   \\brief   " << desc;
	if(parentGroup)
		out << endl << " *   \\ingroup " << parentGroup;
	out << "*/" << endl;
}

static
void DefineType(ostream& out, const char* type, const char* desc, const char* group)
{
	if(desc)
		out << "/// " << desc << endl;
	if(group)
		out << "/**  \\ingroup " << group << "*/" << endl;
	out << "class " << type << "{};" << endl;
}

static
void WriteClass(ostream& out, Registry& reg, const char* name, const char* forceGroup)
{
	IExportedClass* cls = reg.get_class(name);
	UG_COND_THROW(!cls, "Fatal error in WriteClass: a class with name " << name << " was not registered.");
	WriteClass(out, reg, cls, forceGroup);
}

static
void WriteClass(ostream& out, Registry& reg, const IExportedClass* cls, const char* forceGroup)
{
	if(forceGroup)
		WriteGroupID(out, forceGroup);
	else if(!cls->group().empty())
		WriteGroupID(out, GroupNameToID(cls->group()));

	if(!cls->tooltip().empty())
		out << "/// " << cls->tooltip() << endl;
	
	out << "class " << cls->name();
	const ClassNameNode& clsNameNode = cls->class_name_node();
	for(size_t i = 0; i < clsNameNode.num_base_classes(); ++i){
		if(i == 0)
			out << " : ";
		else
			out << ", ";
		out << "public " << clsNameNode.base_class(i).name();
	}
	out << "{" << endl;
	

	out << "public:" << endl;
	for(size_t ictor = 0; ictor < cls->num_constructors(); ++ictor){
		const ExportedConstructor& c = cls->get_constructor(ictor);
		if(!c.tooltip().empty())
			out << "/// \\brief " << c.tooltip() << endl;

		const ParameterInfo& paramInfo = c.params_in();
		out << "\t";
		out << cls->name() << "(";
		for(size_t i = 0; i < paramInfo.size(); ++i){
			out << ParamToString(paramInfo, (int)i);
			out << " ";
			out << NameToVarName(c.parameter_name(i));
			if(i + 1 < paramInfo.size())
				out << ", ";
		}
		out << ");" << endl;
	}

	for(size_t imethod = 0; imethod < cls->num_methods(); ++imethod){
		for(size_t io = 0; io < cls->num_overloads(imethod); ++io){
			WriteFunctionSignature(out, &cls->get_overload(imethod, io), "\t", false);
		}
	}

	for(size_t imethod = 0; imethod < cls->num_const_methods(); ++imethod){
		for(size_t io = 0; io < cls->num_const_overloads(imethod); ++io){
			WriteFunctionSignature(out, &cls->get_const_overload(imethod, io), "\t", false);
		}
	}
	out << "};" << endl;
}

static
void WriteFunction(ostream& out, Registry& reg, const char* name, const char* forceGroup)
{
	const ExportedFunction* func = NULL;
	size_t funcInd = 0;
	for(size_t i = 0; i < reg.num_functions(); ++i){
		const ExportedFunction& tfunc = reg.get_function(i);
		if(tfunc.name().compare(name) == 0){
			func = &tfunc;
			funcInd = i;
			break;
		}
	}

	UG_COND_THROW(!func, "Fatal error in WriteFunction: a function with name " << name << " was not registered.");
	
	for(size_t i = 0; i < reg.num_overloads(funcInd); ++i){
		WriteFunction(out, reg, &reg.get_overload(funcInd, i), forceGroup);
	}
}

static
void WriteFunction(ostream& out, Registry& reg, const ExportedFunction* func, const char* forceGroup)
{
	if(forceGroup)
		WriteGroupID(out, forceGroup);
	else if(!func->group().empty())
		WriteGroupID(out, GroupNameToID(func->group()));

	WriteFunctionSignature(out, func, "", false);
}

static
void WriteGroupMembers(ostream& out, Registry& reg, const char* id)
{
	for(size_t funcInd = 0; funcInd < reg.num_functions(); ++funcInd){
		if(reg.get_function(funcInd).group().find(id) == 0){
			for(size_t i = 0; i < reg.num_overloads(funcInd); ++i){
				WriteFunction(out, reg, &reg.get_overload(funcInd, i));
			}
		}
	}

	for(size_t i = 0; i < reg.num_classes(); ++i){
		const IExportedClass& cls = reg.get_class(i);
		if(cls.group().find(id) == 0){
			WriteClass(out, reg, &cls);
		}
	}
}

static
void WriteGroupID(ostream& out, const string& id)
{
//	touch the groups-value to make sure that the group is somehow known
	groups[id];
	out << "/// \\ingroup " << id << endl;
}

static
void WriteGroupID(ostream& out, const char* id)
{
	WriteGroupID(out, string(id));
}

static
void WriteFunctionSignature(ostream& out, const ExportedFunctionBase* func,
							const char* prefix, bool isConst)
{
	const ParameterInfo& retInfo = func->params_out();
	if(retInfo.size() > 1){
		UG_LOG("Warning: Only using first return value of '" << func->name() << " (" << retInfo.size() << " provided)\n");
	}

	if(!func->tooltip().empty())
		out << prefix << "/// \\brief " << func->tooltip() << endl;

	const ParameterInfo& paramInfo = func->params_in();

	out << prefix;
	if(retInfo.size() == 0)
		out << "void";
	else
		out << ParamToString(retInfo, 0);

	out << " " << func->name() << "(";

	for(size_t i = 0; i < paramInfo.size(); ++i){
		out << ParamToString(paramInfo, (int)i);
		out << " ";
		out << NameToVarName(func->parameter_name(i));
		if(i + 1 < paramInfo.size())
			out << ", ";
	}

	if(isConst)
		out << ") const;" << endl;
	else
		out << ");" << endl;
}

static
string NameToVarName(const string& str)
{
	if(str.empty())
		return "param";
	string varName = TrimString(str);
	return ReplaceAll(varName, " ", "_");
}

static
string GroupNameToID(const string& str)
{
	string id = ReplaceAll(ReplaceAll(ReplaceAll(str, "/", "_"), " ", "_"), ",", "_");
	string& name = originalGroupNames[id];
	if(name.empty())
		name = str;
	return id;
}

static
string ParamToString(const ParameterInfo& info, int i)
{
	string res = string("");
	bool isVector = info.is_vector(i);
	
	if(isVector) res.append("vector< ");
	
	switch(info.type(i)){
		default:
		case Variant::VT_INVALID:
			res.append("unknown");
			break;
		case Variant::VT_BOOL:
			res.append("bool");
			break;
		case Variant::VT_SIZE_T:
			res.append("size_t");
			break;
		case Variant::VT_INT:
			res.append("int");
			break;
		case Variant::VT_FLOAT:
			res.append("float");
			break;
		case Variant::VT_DOUBLE:
			res.append("double");
			break;
		case Variant::VT_CSTRING:
			res.append("const char*");
			break;
		case Variant::VT_STDSTRING:
			res.append("string");
			break;
		case Variant::VT_POINTER:
		case Variant::VT_SMART_POINTER:
			res.append(info.class_name(i)).append("*");
			break;
		case Variant::VT_CONST_POINTER:
		case Variant::VT_CONST_SMART_POINTER:
			res.append("const ").append(info.class_name(i)).append("*");
			break;
		// case Variant::VT_POINTER:
		// 	res.append(info.class_name(i)).append("*");
		// 	break;
		// case Variant::VT_CONST_POINTER:
		// 	res.append("const ").append(info.class_name(i)).append("*");
		// 	break;
		// case Variant::VT_SMART_POINTER:
		// 	res.append("SmartPtr<").append(info.class_name(i)).append(">");
		// 	break;
		// case Variant::VT_CONST_SMART_POINTER:
		// 	res.append("ConstSmartPtr<").append(info.class_name(i)).append(">");
		// 	break;
	}
	if(isVector) res.append(" >");
	return res;
}

static
void AddPathContentToResourceFile(ofstream& out, const string& searchPath, const string& targetPath)
{
	vector<string> files;
	GetFilesInDirectory(files, searchPath.c_str());

	for(size_t i = 0; i < files.size(); ++i){
		out << "<file>";
		if(!targetPath.empty())
			out << targetPath << "/";
		out << files[i] << "</file>" << endl;
	}

	vector<string> dirs;
	GetDirectoriesInDirectory(dirs, searchPath.c_str());

	for(size_t i = 0; i < dirs.size(); ++i){
		if((dirs[i].compare(".") == 0) || (dirs[i].compare("..") == 0))
			continue;

		string newSearchPath;
		if(searchPath.empty())
			newSearchPath = mkstr(dirs[i]);
		else
			newSearchPath = mkstr(searchPath << "/" << dirs[i]);

		string newTargetPath;
		if(targetPath.empty())
			newTargetPath = mkstr(dirs[i]);
		else
			newTargetPath = mkstr(targetPath << "/" << dirs[i]);

		AddPathContentToResourceFile(out, newSearchPath, newTargetPath);
	}
}

static
void GenerateResourceFile(const string& filename, const string& searchPath, const string& targetPath)
{
	ofstream out(filename.c_str());
	out << "<RCC>" << endl;
	out << "<qresource prefix=\"docs\">" << endl;
	AddPathContentToResourceFile(out, searchPath, targetPath);
	out << "</qresource>" << endl;
	out << "</RCC>" << endl;
}