#include <QDir>
#include <QApplication>
#include <fstream>
#include <string>
#include <vector>
#include <cstdlib>
#include <sstream>

#include "standard_tools.h"
#include "common/error.h"
#include "common/util/string_util.h"
#include "../app.h"
#include "../plugins/experimental/lua_shell/lua_shell.h"

using namespace ug;
using namespace std;

typedef SmartPtr<ug::luashell::LuaShell> SPLuaShell;

class ScriptTool;
static vector<ScriptTool*>	g_scriptTools;

struct ScriptParamter{
	string varName;
	string argName;
	string typeName;
	string options;
};

struct ScriptDeclarations{
	string					name;
	vector<ScriptParamter>	inputs;
};

template <class T>
static T ToNumber(const std::string& str){
	std::istringstream istr(str.c_str());
	istr.imbue(std::locale("C"));
	T num = 0;
	istr >> num;
	return num;
}

ScriptDeclarations ParseScriptForDeclarations(string script)
{
	ScriptDeclarations decls;
	ifstream in(script.c_str());
	if(!in)
		return decls;

	char buf[256];
	string line;
	vector<string> tokens;
	int curLineNumber = 0;
	while(!in.eof()){
		++curLineNumber;

		in.getline(buf, 256);
		line = buf;
		while(in.rdstate() == ios::failbit){
			in.clear();
			in.getline(buf, 256);
			line.append(buf);
		}

		if(line.find("pm-declare-") == string::npos)
			continue;

		tokens = TokenizeTrimString(line, ':');
		if(tokens.size() != 2){
			UG_LOG("unsupported pm-declare syntax in line " << curLineNumber
				<< " of script " << script
				<< " to many occurances of ':'"<< endl);
			return decls;
		}

		if(tokens[0].find("pm-declare-name") != string::npos){
			decls.name = tokens[1];
		}
		else if(tokens[0].find("pm-declare-input") != string::npos){
			tokens = TokenizeTrimString(tokens[1], '|');
			ScriptParamter param;
			if(tokens.size() >= 3){
				param.varName = tokens[0];
				param.argName = tokens[1];
				param.typeName = ToLower(tokens[2]);
				if(tokens.size() > 3)
					param.options = tokens[3];
				decls.inputs.push_back(param);
			}
		}
	}
	return decls;
}

class ScriptTool : public ITool
{
	public:
		ScriptTool(string scriptName, string path, string group, SPLuaShell luaShell) :
			m_scriptName(scriptName),
			m_scriptPath(path),
			m_group(group),
			m_luaShell(luaShell)
		{
		}

		virtual void execute(LGObject* obj,
							 QWidget* widget)
		{
			ToolWidget* dlg = dynamic_cast<ToolWidget*>(widget);
			if(dlg){
				for(size_t i = 0; i < m_scriptDecls.inputs.size(); ++i){
					ScriptParamter& param = m_scriptDecls.inputs[i];
					if(param.typeName == "double"){
						m_luaShell->set(param.varName.c_str(), dlg->to_double(i));
					}
					else{
						UG_THROW("type " << param.typeName << " currently unsupported by script interpreter.");
					}
				}
			}
			m_luaShell->set("mesh", static_cast<ug::promesh::MeshObject*>(obj), "PM_MeshObject");
			m_luaShell->parse_file(m_scriptPath.c_str());
			obj->geometry_changed();
		}

		virtual const char* get_name()	
		{
			parse_script_decls(false);
			return m_scriptDecls.name.c_str();
		}

		virtual const char* get_tooltip()	{return "";}
		virtual const char* get_group()		{return m_group.c_str();}
		virtual const char* get_shortcut()	{return "";}

		virtual bool accepts_null_object_ptr()	{return false;}

		virtual QWidget* get_dialog(QWidget* parent)
		{
			parse_script_decls(false);
			
			vector<ScriptParamter>& inputs = m_scriptDecls.inputs;

			if(inputs.empty())
				return NULL;

			ToolWidget *dlg = new ToolWidget(get_name(), parent, this,
											IDB_APPLY | IDB_OK | IDB_CLOSE);

			refresh_dialog(dlg);

			MainWindow* mw = app::getMainWindow();
			connect(mw, SIGNAL(refreshToolDialogs()), dlg, SLOT(refreshContents()));

			return dlg;
		}

		virtual void refresh_dialog(QWidget* dialog)
		{
			ToolWidget* dlg = dynamic_cast<ToolWidget*>(dialog);
			if(!dlg)
				return;

			parse_script_decls(true);
			dlg->clear();
			
			vector<ScriptParamter>& inputs = m_scriptDecls.inputs;

			if(inputs.empty())
				return;

			std::vector<string> options;
			std::vector<string> tokens;
			for(size_t iinput = 0; iinput < inputs.size(); ++iinput){
				ScriptParamter& param = inputs[iinput];
				options.clear();
				if(!param.options.empty()){
					options = TokenizeTrimString(param.options, ';');
				}
				if(param.typeName == "double"){
					double val = 0;
					double min = -1.e32;
					double max = 1.e32;
					double step = 1;
					double digits = 9;
					if(!options.empty()){
						for(size_t iopt = 0; iopt < options.size(); ++iopt){
							tokens = TokenizeTrimString(options[iopt], '=');
							if(tokens.size() == 2){
								if(tokens[0] == "min")
									min = ToNumber<double>(tokens[1]);
								else if(tokens[0] == "max")
									max = ToNumber<double>(tokens[1]);
								else if(tokens[0] == "val")
									val = ToNumber<double>(tokens[1]);
								else if(tokens[0] == "step")
									step = ToNumber<double>(tokens[1]);
								else if(tokens[0] == "digits")
									digits = ToNumber<double>(tokens[1]);
							}
							else{
								UG_LOG("Invalid option '" << options[iopt] << "' in paramter '"
									   << param.argName << "' of script " << m_scriptPath << std::endl);
							}
						}
					}
					dlg->addSpinBox(QString(param.argName.c_str()), min, max, val, step, digits);
				}
			}
		}

		void parse_script_decls(bool force){
			if(force || m_scriptDecls.name.empty()){
				m_scriptDecls = ParseScriptForDeclarations(m_scriptPath);
				if(m_scriptDecls.name.empty())
					m_scriptDecls.name = m_scriptName;
			}
		}

		string path()			{return m_scriptPath;}

		SPLuaShell lua_shell()	{return m_luaShell;}

	private:
		ScriptDeclarations	m_scriptDecls;
		string m_scriptName;
		string m_scriptPath;
		string m_group;
		SPLuaShell	m_luaShell;
};



void ParseDirAndCreateTools(ToolManager* toolMgr, QDir dir, string group,
							SPLuaShell luaShell,
							bool performExistenceChecks = false)
{
//	parse subdirectories first
	QStringList subDirs = dir.entryList(QDir::Dirs | QDir::NoDotAndDotDot);
	for(int idir = 0; idir < subDirs.size(); ++idir){
		// UG_LOG(group << subDirs.at(idir).toStdString() << endl);
		
		QDir subDir = dir;
		subDir.cd(subDirs.at(idir));
		
		string subGroup = group;
		if(!subGroup.empty())
			subGroup.append(" | ");
		subGroup.append(subDirs.at(idir).toStdString());
		
		ParseDirAndCreateTools(toolMgr, subDir, subGroup, luaShell, performExistenceChecks);
	}

	QStringList scripts = dir.entryList(QDir::Files);
	for(int iscript = 0; iscript < scripts.size(); ++iscript){
		// UG_LOG(group << "* "<< scripts.at(iscript).toStdString() << endl);

	//	check whether the given tool already exists
		string absPath = dir.absoluteFilePath(scripts.at(iscript)).toStdString();
		if(performExistenceChecks){
			bool gotOne = false;
			for(size_t i = 0; i < g_scriptTools.size(); ++i){
				if(g_scriptTools[i]->path() == absPath){
					gotOne = true;
					break;
				}
			}
			if(gotOne)
				continue;
		}

		ScriptTool* tool = new ScriptTool(scripts.at(iscript).toStdString(),
							   absPath,
							   group,
							   luaShell);

		toolMgr->register_tool(tool);
		g_scriptTools.push_back(tool);
	}
}

void RegisterScriptTools(ToolManager* toolMgr)
{
	QDir appDir(QApplication::applicationDirPath());
	if(!appDir.exists("scripts")){
		UG_THROW("no scripts path found")
	}
	appDir.cd("scripts");

	ParseDirAndCreateTools(toolMgr, appDir, "Scripts", make_sp(new luashell::LuaShell()));
	toolMgr->set_group_icon("Scripts", ":images/tool_scripts.png");
}

int RefreshScriptTools(ToolManager* toolMgr)
{
	size_t lastNumScriptTools = g_scriptTools.size();
	int retVal = 0;

//	check for each tool, whether the according script still exists
	for(size_t i = 0; i < g_scriptTools.size();){
		ScriptTool* t = g_scriptTools[i];
		if(!FileExists(t->path().c_str())){
			toolMgr->remove_tool(t);
			delete t;
			g_scriptTools.erase(g_scriptTools.begin() + i);
		}
		else
			++i;
	}

	if(lastNumScriptTools != g_scriptTools.size())
		retVal = 1;

	lastNumScriptTools = g_scriptTools.size();

	SPLuaShell luaShell;
	if(!g_scriptTools.empty())
		luaShell = g_scriptTools.front()->lua_shell();
	else
		luaShell = make_sp(new luashell::LuaShell());


	QDir appDir(QApplication::applicationDirPath());
	if(!appDir.exists("scripts")){
		UG_THROW("no scripts path found")
	}
	appDir.cd("scripts");

	ParseDirAndCreateTools(toolMgr, appDir, "Scripts",
							luaShell,
							true);

	if(lastNumScriptTools != g_scriptTools.size())
		retVal = 1;

	return retVal;
}