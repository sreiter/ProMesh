#include <iostream>
#include <fstream>
#include "bridge/bridge.h"
#include "common/util/path_provider.h"
#include "common/util/plugin_util.h"

using namespace std;
using namespace ug;
using namespace ug::bridge;


void AddClass(ostream& out, Registry& reg, const char* name, const char* forceGroup = NULL);
void AddFunction(ostream& out, Registry& reg, const char* name, const char* forceGroup = NULL);
void AddGroup(ostream& out, Registry& reg, const char* name);
void DefineGroup(ostream& out, const char* name, const char* desc);
bool WriteFunctionSignature(ostream& out, const ExportedFunctionBase* func,
							const char* prefix = "", bool isConst = false);
string ParamToString(const ParameterInfo& info, size_t i);
string NameToVarName(const string& str);

int main(){
	cout << "docugen - script documentation generator for promesh" << endl;

	try{
		InitBridge();
		if(!LoadPlugins(PathProvider::get_path(PLUGIN_PATH).c_str(), "ug4/", GetUGRegistry()))
		{
			UG_LOG("ERROR during initialization of plugins: LoadPlugins failed!\n");
		}
	
		Registry& reg = GetUGRegistry();
		ofstream out("promesh-script-docu-output.cpp");

		AddClass(out, reg, "Vec3d");
		AddClass(out, reg, "Grid");
		AddClass(out, reg, "Mesh");
		AddFunction(out, reg, "SlopeSmooth");
		out.close();
	}
	catch(UGError& err){
		UG_LOG("An error occurred:\n");
		for(size_t i = 0; i < err.num_msg(); ++i){
			UG_LOG("  " << err.get_msg(i) << std::endl);
		}
	}
	return 0;
}


void AddClass(ostream& out, Registry& reg, const char* name, const char* forceGroup)
{
	IExportedClass* cls = reg.get_class(name);
	UG_COND_THROW(!cls, "Fatal error in AddClass: a class with name " << name << " was not registered.");

	if(!cls->tooltip().empty())
		out << "/// " << cls->tooltip() << endl;
	out << "class " << name << "{" << endl;
	out << "public:" << endl;
	for(size_t ictor = 0; ictor < cls->num_constructors(); ++ictor){
		const ExportedConstructor& c = cls->get_constructor(ictor);
		if(!c.tooltip().empty())
			out << "///" << c.tooltip() << endl;

		const ParameterInfo& paramInfo = c.params_in();
		out << "\t";
		out << name << "(";
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


void AddFunction(ostream& out, Registry& reg, const char* name, const char* forceGroup)
{
	ExportedFunction* func = NULL;
	for(size_t i = 0; i < reg.num_functions(); ++i){
		ExportedFunction& tfunc = reg.get_function(i);
		if(tfunc.name().compare(name) == 0){
			func = &tfunc;
			break;
		}
	}

	UG_COND_THROW(!func, "Fatal error in AddFunction: a function with name " << name << " was not registered.");

	WriteFunctionSignature(out, func, "", false);
}



bool WriteFunctionSignature(ostream& out, const ExportedFunctionBase* func,
							const char* prefix, bool isConst)
{
	const ParameterInfo& retInfo = func->params_out();
	if(retInfo.size() > 1){
		UG_LOG("Ignoring '" << func->name() << "' since it has more than one return value\n");
		return false;
	}

	if(!func->tooltip().empty())
		out << prefix << "///" << func->tooltip() << endl;

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
	return true;
}


string NameToVarName(const string& str){
	if(str.empty())
		return "unnamed";
	string varName = TrimString(str);
	return ReplaceAll(varName, " ", "_");
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
			res.append(info.class_name(i)).append("*");
			break;
		case Variant::VT_CONST_POINTER:
			res.append("const ").append(info.class_name(i)).append("*");
			break;
		case Variant::VT_SMART_POINTER:
			res.append("SmartPtr<").append(info.class_name(i)).append(">");
			break;
		case Variant::VT_CONST_SMART_POINTER:
			res.append("ConstSmartPtr<").append(info.class_name(i)).append(">");
			break;
	}
	if(isVector) res.append(" >");
	return res;
}