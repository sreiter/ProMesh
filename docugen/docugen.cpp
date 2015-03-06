#include <iostream>
#include <fstream>
#include <streambuf>
#include <map>
#include "bridge/bridge.h"
#include "common/util/path_provider.h"
#include "common/util/plugin_util.h"
#include "common/util/string_util.h"
#include "common/util/stringify.h"
#include "common/util/file_util.h"

using namespace std;
using namespace ug;
using namespace ug::bridge;


void DefineGroup(ostream& out, const char* id, const char* name, const char* desc,
				 const char* parentGroup = NULL);
void WriteClass(ostream& out, Registry& reg, const char* name, const char* forceGroup = NULL);
void WriteClass(ostream& out, Registry& reg, const IExportedClass* cls, const char* forceGroup = NULL);
void WriteFunction(ostream& out, Registry& reg, const char* name, const char* forceGroup = NULL);
void WriteFunction(ostream& out, Registry& reg, const ExportedFunction* func, const char* forceGroup = NULL);
void WriteGroupMembers(ostream& out, Registry& reg, const char* id);
void WriteGroupID(ostream& out, const char* id);
void WriteGroupID(ostream& out, const string& id);
void WriteFunctionSignature(ostream& out, const ExportedFunctionBase* func,
							const char* prefix = "", bool isConst = false);
string ParamToString(const ParameterInfo& info, size_t i);
string NameToVarName(const string& str);
string GroupNameToID(const string& str);

void GenerateResourceFile(const string& filename, const string& contentPath);

#define mkstr(s)	(Stringify() << s).str()


map<string, bool>	groups;
map<string, string>	originalGroupNames;


void DeleteDirectory(string path){
	#ifdef UNIX
		system(mkstr("rm -r " << path).c_str());
	#elif WINDOWS
		UG_THROW("Please implement directory-remove for windows")
	#endif
}

void CopyDirectory(string from, string to){
	#ifdef UNIX
		system(mkstr("cp -r " << from << " " << to).c_str());
	#elif WINDOWS
		UG_THROW("Please implement recursive directory copy for windows")
	#endif
}

string ReadFile(string filename){
	ifstream t(filename.c_str());
	string str;

	t.seekg(0, ios::end);   
	str.reserve(t.tellg());
	t.seekg(0, ios::beg);

	str.assign((istreambuf_iterator<char>(t)),
	            istreambuf_iterator<char>());
	return str;
}



int main(){
	cout << "docugen - script documentation generator for promesh" << endl;
	cout << "promesh-root-path: " << PROMESH_ROOT_PATH << endl;
	const string pmPath = PROMESH_ROOT_PATH;

	try{
		InitBridge();
		if(!LoadPlugins(PathProvider::get_path(PLUGIN_PATH).c_str(), "", GetUGRegistry()))
		{
			UG_LOG("ERROR during initialization of plugins: LoadPlugins failed!\n");
		}
	
		UG_LOG("Generating scripting reference" << endl);

		Registry& reg = GetUGRegistry();
		ofstream out("script_reference.cpp");
		DefineGroup(out, "ugbase", "ug-base", "Classes and functions from ug4 which are provided in ProMesh scripts.");
		DefineGroup(out, "promesh", "ProMesh Tools", "All classes and tools defined by ProMesh.");
		DefineGroup(out, "promesh_Selection", "Selection", "Tools to perform and to manipulate the current selection.", "promesh");
		DefineGroup(out, "promesh_Selection_Vertices", "Vertices", "Selection tools for vertices.", "promesh_Selection");
		DefineGroup(out, "promesh_Selection_Edges", "Edges", "Selection tools for edges.", "promesh_Selection");
		DefineGroup(out, "promesh_Selection_Faces", "Faces", "Selection tools for faces.", "promesh_Selection");
		DefineGroup(out, "promesh_Selection_Volumes", "Volumes", "Selection tools for volumes.", "promesh_Selection");

		DefineGroup(out, "promesh_Coordinate_Transform", "Coordinate Transform", "Tools to transform the mesh or individual vertices.", "promesh");

		WriteClass(out, reg, "Vec2d", "ugbase");
		WriteClass(out, reg, "Vec3d", "ugbase");
		WriteClass(out, reg, "Vec4d", "ugbase");
		WriteClass(out, reg, "Grid", "ugbase");
		WriteClass(out, reg, "Selector", "ugbase");
		WriteClass(out, reg, "ISubsetHandler", "ugbase");
		WriteClass(out, reg, "SubsetHandler", "ugbase");
		WriteClass(out, reg, "Vertex", "ugbase");
		WriteClass(out, reg, "Edge", "ugbase");
		WriteClass(out, reg, "Face", "ugbase");
		WriteClass(out, reg, "Volume", "ugbase");
		WriteGroupMembers(out, reg, "promesh");

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
			out << ReadFile(mkstr(pmPath << "recent_changes.txt")) << endl;
			out << "\\endverbatim" << endl;
			out << "*/" << endl;
		}

	//	copy the docu-sources to this folder so that doxygen can find them
		if(DirectoryExists("doxysrc"))
			DeleteDirectory("./doxysrc");		
		CopyDirectory(mkstr(pmPath << "docugen/doxysrc"), "./");

	//	call doxygen to generate the source tree
		UG_LOG("Executing doxygen...\n");
		string doxyDesc = "doxysrc/DoxygenConfig.txt";
		if(system(mkstr("doxygen " << doxyDesc).c_str()) != 0){
			UG_THROW("Execution of 'doxygen' failed for doxy-file " << doxyDesc);
		}
		UG_LOG(endl);

	//	set up the docs/html folder in PROMESH_ROOT_PATH.
	//	If the html path already exists we'll delete it. We'll use some os-specific
	//	functions here...
		if(!DirectoryExists(mkstr(pmPath << "docs")))
			CreateDirectory(mkstr(pmPath << "docs"));
		if(DirectoryExists(mkstr(pmPath << "docs/html")))
			DeleteDirectory(mkstr(pmPath << "docs/html"));
		CopyDirectory("./html", mkstr(pmPath << "docs/"));

	//	now we'll create the qt-resource file containing all the help files
		UG_LOG("Generating qt resource file...\n");
		GenerateResourceFile(mkstr(pmPath << "docs/html-resources.qrc"), "html");

		UG_LOG("done\n");
	}
	catch(UGError& err){
		UG_LOG("An error occurred:\n");
		for(size_t i = 0; i < err.num_msg(); ++i){
			UG_LOG("  " << err.get_msg(i) << std::endl);
		}
	}
	return 0;
}


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


void WriteClass(ostream& out, Registry& reg, const char* name, const char* forceGroup)
{
	IExportedClass* cls = reg.get_class(name);
	UG_COND_THROW(!cls, "Fatal error in WriteClass: a class with name " << name << " was not registered.");
	WriteClass(out, reg, cls, forceGroup);
}

void WriteClass(ostream& out, Registry& reg, const IExportedClass* cls, const char* forceGroup)
{
	if(forceGroup)
		WriteGroupID(out, forceGroup);
	else if(!cls->group().empty())
		WriteGroupID(out, GroupNameToID(cls->group()));

	if(!cls->tooltip().empty())
		out << "/// " << cls->tooltip() << endl;
	out << "class " << cls->name() << "{" << endl;
	out << "public:" << endl;
	for(size_t ictor = 0; ictor < cls->num_constructors(); ++ictor){
		const ExportedConstructor& c = cls->get_constructor(ictor);
		if(!c.tooltip().empty())
			out << "/// \\brief " << c.tooltip() << endl;

		const ParameterInfo& paramInfo = c.params_in();
		out << "\t";
		out << cls->name() << "(";
		for(size_t i = 0; i < paramInfo.size(); ++i){
			out << ParamToString(paramInfo, i);
			out << " ";
			out << NameToVarName(c.parameter_name(i));
			if(i + 1 < paramInfo.size())
				out << ", ";
		}
		out << ");" << endl;
	}

	for(size_t imethod = 0; imethod < cls->num_methods(); ++imethod){
		const ExportedMethod& m = cls->get_method(imethod);
		WriteFunctionSignature(out, &m, "\t", false);
	}

	for(size_t imethod = 0; imethod < cls->num_const_methods(); ++imethod){
		const ExportedMethod& m = cls->get_const_method(imethod);
		WriteFunctionSignature(out, &m, "\t", true);
	}
	out << "};" << endl;
}


void WriteFunction(ostream& out, Registry& reg, const char* name, const char* forceGroup)
{
	const ExportedFunction* func = NULL;
	for(size_t i = 0; i < reg.num_functions(); ++i){
		const ExportedFunction& tfunc = reg.get_function(i);
		if(tfunc.name().compare(name) == 0){
			func = &tfunc;
			break;
		}
	}

	UG_COND_THROW(!func, "Fatal error in WriteFunction: a function with name " << name << " was not registered.");
	WriteFunction(out, reg, func, forceGroup);
}

void WriteFunction(ostream& out, Registry& reg, const ExportedFunction* func, const char* forceGroup)
{
	if(forceGroup)
		WriteGroupID(out, forceGroup);
	else if(!func->group().empty())
		WriteGroupID(out, GroupNameToID(func->group()));

	WriteFunctionSignature(out, func, "", false);
}


void WriteGroupMembers(ostream& out, Registry& reg, const char* id)
{
	for(size_t i = 0; i < reg.num_functions(); ++i){
		const ExportedFunction& func = reg.get_function(i);
		if(func.group().find(id) == 0){
			WriteFunction(out, reg, &func);
		}
	}

	for(size_t i = 0; i < reg.num_classes(); ++i){
		const IExportedClass& cls = reg.get_class(i);
		if(cls.group().find(id) == 0){
			WriteClass(out, reg, &cls);
		}
	}
}


void WriteGroupID(ostream& out, const string& id)
{
//	touch the groups-value to make sure that the group is somehow known
	groups[id];
	out << "/// \\ingroup " << id << endl;
}


void WriteGroupID(ostream& out, const char* id)
{
	WriteGroupID(out, string(id));
}


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
		out << ParamToString(paramInfo, i);
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


string NameToVarName(const string& str)
{
	if(str.empty())
		return "param";
	string varName = TrimString(str);
	return ReplaceAll(varName, " ", "_");
}


string GroupNameToID(const string& str)
{
	string id = ReplaceAll(ReplaceAll(ReplaceAll(str, "/", "_"), " ", "_"), ",", "_");
	string& name = originalGroupNames[id];
	if(name.empty())
		name = str;
	return id;
}


string ParamToString(const ParameterInfo& info, size_t i)
{
	string res = string("");
	bool isVector = info.is_vector(i);
	
	if(isVector) res.append("std::vector< ");
	
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
			res.append("std::string");
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


void AddPathContentToResourceFile(ofstream& out, const string& path)
{
	vector<string> files;
	GetFilesInDirectory(files, path.c_str());

	for(size_t i = 0; i < files.size(); ++i){
		out << "<file>";
		if(!path.empty())
			 out << path << "/";
		out << files[i] << "</file>" << endl;
	}

	vector<string> dirs;
	GetDirectoriesInDirectory(dirs, path.c_str());

	for(size_t i = 0; i < dirs.size(); ++i){
		if((dirs[i].compare(".") == 0) || (dirs[i].compare("..") == 0))
			continue;
		AddPathContentToResourceFile(out, mkstr(path << "/" << dirs[i]));
	}
}

void GenerateResourceFile(const string& filename, const string& contentPath)
{
	ofstream out(filename.c_str());
	out << "<RCC>" << endl;
	out << "<qresource prefix=\"docs\">" << endl;
	AddPathContentToResourceFile(out, contentPath);
	out << "</qresource>" << endl;
	out << "</RCC>" << endl;
}