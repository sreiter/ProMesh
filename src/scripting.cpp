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

#include <limits>
#include "scripting.h"

using namespace std;
using namespace ug;


static SPLuaShell			g_luaShell;


template <class T>
static T ToNumber(const std::string& str){
	std::istringstream istr(str.c_str());
	istr.imbue(std::locale("C"));
	T num = 0;
	istr >> num;
	return num;
}


SPLuaShell GetDefaultLuaShell ()
{
	if(!g_luaShell.valid()){
		g_luaShell = make_sp(new luashell::LuaShell());
	}
	return g_luaShell;
}


void SetScriptDefaultVariables (SPLuaShell luaShell, const char* scriptContent)
{
	ScriptDeclarations decls;
	ParseScriptDeclarations (decls, scriptContent);

	for(size_t i = 0; i < decls.inputs.size(); ++i){
		ScriptParameter& param = decls.inputs[i];

		switch(param.val.type()) {
			case Variant::VT_BOOL:
				luaShell->set (param.varName.c_str(), param.val.to_bool());
				break;
			case Variant::VT_INT:
				luaShell->set (param.varName.c_str(), param.val.to_int());
				break;
			case Variant::VT_SIZE_T:
				luaShell->set (param.varName.c_str(), param.val.to_size_t());
				break;
			case Variant::VT_FLOAT:
			case Variant::VT_DOUBLE:
				luaShell->set (param.varName.c_str(), param.val.to_double());
				break;
			case Variant::VT_STDSTRING:
			case Variant::VT_CSTRING:
				luaShell->set (param.varName.c_str(), param.val.to_c_string());
				break;
		}
	}
}


void ParseScriptDeclarations (ScriptDeclarations& declsOut,
                              const char* scriptContent)
{
	ScriptDeclarations& decls = declsOut;
	decls.name = "";
	decls.inputs.clear();
	std::stringstream in(scriptContent);

	string line;
	vector<string> tokens;
	vector<string> paramTokens;
	int curLineNumber = 0;
	

	while(!in.eof()){
		++curLineNumber;

		getline(in, line);

		if(!line.empty() && line[line.size() - 1] == '\r')
			line.resize(line.size() - 1);

		if(line.find("pm-declare-") == string::npos)
			continue;

		TokenizeTrimString(line, tokens, ':');
		if(tokens.size() != 2){
			continue;
		}

		RemoveWhitespaceFromString(tokens[0]);
		

		if(tokens[0].compare("--pm-declare-name") == 0){
			decls.name = tokens[1];
		}
		else if(tokens[0].compare("--pm-declare-input") == 0){
			TokenizeTrimString(tokens[1], paramTokens, '|');
			ScriptParameter param;
			if(paramTokens.size() >= 3){
				param.varName = paramTokens[0];
				param.argName = paramTokens[1];
				param.typeName = ToLower(paramTokens[2]);
				if(paramTokens.size() > 3)
					param.options = paramTokens[3];
			}
			decls.inputs.push_back(param);
		}
	}

//	parse options
	std::vector<string> options;
	for(size_t iinput = 0; iinput < decls.inputs.size(); ++iinput)
	{
		ScriptParameter& param = decls.inputs[iinput];

		options.clear();
		if(!param.options.empty()){
			TokenizeTrimString(param.options, options, ';');
		}
		if((param.typeName == "double") || (param.typeName == "float")){
			param.val = double(0);
			param.min = -numeric_limits<double>::max();
			param.max = numeric_limits<double>::max();
			param.step = double(1);
			param.digits = 9;
			if(!options.empty()){
				for(size_t iopt = 0; iopt < options.size(); ++iopt){
					TokenizeTrimString(options[iopt], tokens, '=');
					if(tokens.size() == 2){
						if(tokens[0] == "min")
							param.min = ToNumber<double>(tokens[1]);
						else if(tokens[0] == "max")
							param.max = ToNumber<double>(tokens[1]);
						else if(tokens[0] == "val")
							param.val = ToNumber<double>(tokens[1]);
						else if(tokens[0] == "step")
							param.step = ToNumber<double>(tokens[1]);
						else if(tokens[0] == "digits")
							param.digits = ToNumber<double>(tokens[1]);
					}
					else{
						UG_LOG("Invalid option '" << options[iopt] << "' in paramter '"
							   << param.argName << std::endl);
					}
				}
			}
		}

		else if((param.typeName == "int") || (param.typeName == "integer")){
			param.val = int(0);
			param.min = -numeric_limits<int>::max();
			param.max = numeric_limits<int>::max();
			param.step = int(1);
			if(!options.empty()){
				for(size_t iopt = 0; iopt < options.size(); ++iopt){
					TokenizeTrimString(options[iopt], tokens, '=');
					if(tokens.size() == 2){
						if(tokens[0] == "min")
							param.min = ToNumber<int>(tokens[1]);
						else if(tokens[0] == "max")
							param.max = ToNumber<int>(tokens[1]);
						else if(tokens[0] == "val")
							param.val = ToNumber<int>(tokens[1]);
						else if(tokens[0] == "step")
							param.step = ToNumber<int>(tokens[1]);
					}
					else{
						UG_LOG("Invalid option '" << options[iopt] << "' in paramter '"
							   << param.argName << std::endl);
					}
				}
			}
		}

		else if((param.typeName == "bool") || (param.typeName == "boolean")){
			param.val = false;
			if(!options.empty()){
				for(size_t iopt = 0; iopt < options.size(); ++iopt){
					TokenizeTrimString(options[iopt], tokens, '=');
					if(tokens.size() == 2){
						if(tokens[0] == "val"){
							string tmp = ToLower(tokens[1]);
							if((tmp == "true") || (tmp == "1"))
								param.val = true;
						}
					}
					else{
						UG_LOG("Invalid option '" << options[iopt] << "' in paramter '"
							   << param.argName << std::endl);
					}
				}
			}
		}

		else if(param.typeName == "string"){
			param.val = "";
			if(!options.empty()){
				for(size_t iopt = 0; iopt < options.size(); ++iopt){
					TokenizeTrimString(options[iopt], tokens, '=');
					if(tokens.size() == 2){
						if(tokens[0] == "val"){
							param.val = tokens[1];
						}
					}
					else{
						UG_LOG("Invalid option '" << options[iopt] << "' in paramter '"
							   << param.argName << std::endl);
					}
				}
			}
		}
	}
}
