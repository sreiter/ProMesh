// created by Sebastian Reiter
// s.b.reiter@gmail.com

#include "common/util/stringify.h"
#include "common/util/string_util.h"
#include "bridge/bridge.h"
#include "standard_tools.h"

using namespace std;
using namespace ug;
using namespace ug::promesh;
using namespace ug::bridge;


template <class T>
static T ToNumber(const std::string& str){
	std::istringstream istr(str.c_str());
	istr.imbue(std::locale("C"));
	T num = 0;
	istr >> num;
	return num;
}


class RegistryTool : public ITool{
	public:
		RegistryTool(std::string name, ExportedFunction* func) :
			m_name(name),
			m_func(func)
		{
			string grp = func->group();
			size_t pos = grp.find("ug4/");
			if(pos != string::npos)
				grp.erase(pos, 4);
			pos = grp.find("promesh/");
			if(pos != string::npos)
				grp.erase(pos, 8);

			m_group = ReplaceAll(grp, "/", "|");
		}

		virtual void execute(LGObject* obj,
							 QWidget* widget)
		{
			ParameterStack paramsIn, paramsOut;
			paramsIn.push(static_cast<Mesh*>(obj));
			
			ToolWidget* dlg = dynamic_cast<ToolWidget*>(widget);
			if(dlg){
				const ParameterInfo& params = m_func->params_in();
				for(int iparam = 1; iparam < params.size(); ++iparam){
					int toolParam = iparam - 1;
					switch(params.type(iparam)){
						case Variant::VT_BOOL:
							paramsIn.push(dlg->to_bool(toolParam));
							break;
						case Variant::VT_INT:
							paramsIn.push(dlg->to_int(toolParam));
							break;
						case Variant::VT_SIZE_T:
							paramsIn.push(static_cast<size_t>(dlg->to_int(toolParam)));
							break;
						case Variant::VT_FLOAT:
							paramsIn.push(static_cast<float>(dlg->to_double(toolParam)));
							break;
						case Variant::VT_DOUBLE:
							paramsIn.push(dlg->to_double(toolParam));
							break;
						case Variant::VT_CSTRING:
							paramsIn.push(dlg->to_string(toolParam).toStdString().c_str());
							break;
						case Variant::VT_STDSTRING:
							paramsIn.push(dlg->to_string(toolParam).toStdString());
							break;
						default:
							UG_THROW("Unsupported Type in RegistryTool for parameter " << toolParam);
							break;
					}
				}
			}

			m_func->execute(paramsIn, paramsOut);

			if(paramsOut.size() > 0){
				UG_LOG(m_name);
				if(paramsIn.size() > 1){
					UG_LOG(" (");
					for(int i = 1; i < paramsIn.size(); ++i){
						UG_LOG(paramsIn.get(i));
						if(i + 1 < paramsIn.size()){
							UG_LOG(", ");
						}
					}
					UG_LOG(")");
				}
				UG_LOG(": ");

				const ParameterInfo& retInfo = m_func->params_out();
				for(int i = 0; i < paramsOut.size(); ++i){
					const Variant& v = paramsOut.get(i);
					if(v.type() == Variant::VT_POINTER
						|| v.type() == Variant::VT_CONST_POINTER
						|| v.type() == Variant::VT_SMART_POINTER
						|| v.type() == Variant::VT_CONST_SMART_POINTER)
					{
						UG_LOG(retInfo.class_name(i));
						if((strcmp(retInfo.class_name(i), "Mesh") == 0)
							&& (v.type() == Variant::VT_SMART_POINTER))
						{
						//todo: add the mesh to the current scene. Use the smart-ptr!
						}
					}
					else{
						UG_LOG(paramsOut.get(i));
					}
					if(i + 1 < paramsOut.size()){
						UG_LOG(", ");
					}
				}
				UG_LOG(endl);
			}

			obj->geometry_changed();
		}

		virtual const char* get_name()	
		{
			return m_name.c_str();
		}

		virtual const char* get_tooltip()	{return m_func->tooltip().c_str();}
		virtual const char* get_group()		{return m_group.c_str();}
		virtual const char* get_shortcut()	{return "";}

		virtual bool accepts_null_object_ptr()	{return false;}

		virtual QWidget* get_dialog(QWidget* parent)
		{
			if(m_func->num_parameter() <= 1)
				return NULL;

			ToolWidget *dlg = new ToolWidget(get_name(), parent, this,
											IDB_APPLY | IDB_OK | IDB_CLOSE);

			const ParameterInfo& params = m_func->params_in();
			bool paramError = false;
			
			vector<string> tokens;
			vector<string> options;

			for(int iparam = 1; iparam < params.size(); ++iparam){
				string paramName = Stringify() << "param-" << iparam;
				if(!m_func->parameter_name(iparam).empty())
					paramName = m_func->parameter_name(iparam);

				options.clear();
				const std::vector<std::string>& paramInfos = m_func->parameter_info_vec(iparam);
				if(paramInfos.size() >= 3){
					options = TokenizeTrimString(paramInfos[2], ';');
				}

				QString qParamName = QString::fromStdString(paramName);

				switch(params.type(iparam)){
					case Variant::VT_BOOL:
					{
						bool val = false;
						if(!options.empty()){
							for(size_t iopt = 0; iopt < options.size(); ++iopt){
								tokens = TokenizeTrimString(options[iopt], '=');
								if(tokens.size() == 2){
									if(tokens[0] == "value"){
										string tmp = ToLower(tokens[1]);
										if((tmp == "true") || (tmp == "1"))
											val = true;
									}
								}
							}
						}
						dlg->addCheckBox(qParamName, val);
					}break;

					case Variant::VT_INT:
					{
						double val = 0;
						double min = -1.e32;
						double max = 1.e32;
						double step = 1;
						if(!options.empty()){
							for(size_t iopt = 0; iopt < options.size(); ++iopt){
								tokens = TokenizeTrimString(options[iopt], '=');
								if(tokens.size() == 2){
									if(tokens[0] == "min")
										min = ToNumber<double>(tokens[1]);
									else if(tokens[0] == "max")
										max = ToNumber<double>(tokens[1]);
									else if(tokens[0] == "value")
										val = ToNumber<double>(tokens[1]);
									else if(tokens[0] == "step")
										step = ToNumber<double>(tokens[1]);
								}
							}
						}
						dlg->addSpinBox(qParamName, min, max, val, step, 0);
					}break;

					case Variant::VT_SIZE_T:
					{
						double val = 0;
						double min = 0;
						double max = 1.e32;
						double step = 1;
						if(!options.empty()){
							for(size_t iopt = 0; iopt < options.size(); ++iopt){
								tokens = TokenizeTrimString(options[iopt], '=');
								if(tokens.size() == 2){
									if(tokens[0] == "min")
										min = ToNumber<double>(tokens[1]);
									else if(tokens[0] == "max")
										max = ToNumber<double>(tokens[1]);
									else if(tokens[0] == "value")
										val = ToNumber<double>(tokens[1]);
									else if(tokens[0] == "step")
										step = ToNumber<double>(tokens[1]);
								}
							}
						}
						dlg->addSpinBox(qParamName, min, max, val, step, 0);
					}break;

					case Variant::VT_FLOAT:
					case Variant::VT_DOUBLE:
					{
						double val = 0;
						double min = -1.e32;
						double max = 1.e32;
						double step = 1;
						double digits = 9;
						if(!options.empty()){
							for(size_t iopt = 0; iopt < options.size(); ++iopt){
								tokens = TokenizeTrimString(options[iopt], '=');
								if(tokens.size() == 2){
									tokens[1] = ReplaceAll(tokens[1], string("D"), string(""));
									if(tokens[0] == "min")
										min = ToNumber<double>(tokens[1]);
									else if(tokens[0] == "max")
										max = ToNumber<double>(tokens[1]);
									else if(tokens[0] == "value")
										val = ToNumber<double>(tokens[1]);
									else if(tokens[0] == "step")
										step = ToNumber<double>(tokens[1]);
									else if(tokens[0] == "digits")
										digits = ToNumber<double>(tokens[1]);
								}
							}
						}
						dlg->addSpinBox(qParamName, min, max, val, step, digits);
					}break;

					case Variant::VT_CSTRING:
					case Variant::VT_STDSTRING:
					{
						dlg->addTextBox(qParamName, tr(""));
					}break;

					default:
						paramError = true;
						break;
				}

				if(paramError){
					UG_LOG("Unsupported parameter " << iparam << " in registry tool "
						   << m_name << endl);
				}
			}

			if(paramError){
				delete dlg;
				return NULL;
			}
			else
				return dlg;
		}

	private:
		std::string			m_name;
		std::string			m_group;
		ExportedFunction* 	m_func;
};


void RegisterTool(ToolManager* toolMgr,
				  Registry& reg,
				  std::string toolName,
				  std::string funcName)
{
	for(size_t ifct = 0; ifct < reg.num_functions(); ++ifct){
		ExportedFunction& f = reg.get_function(ifct);
		if(f.name() == funcName){
		//	find an overload whose first argument is Mesh
			for(size_t iol = 0; iol < reg.num_overloads(ifct); ++iol){
				ExportedFunction& o = reg.get_overload(ifct, iol);
				if(o.num_parameter() == 0)
					continue;

				if(strcmp(o.parameter_class_name(0), "Mesh") != 0)
					continue;

			//	o has to be added
				RegistryTool* tool = new RegistryTool(toolName, &o);
				toolMgr->register_tool(tool);
			}
		}
	}
}


void RegsiterRegistryTools(ToolManager* toolMgr)
{
	Registry& reg = GetUGRegistry();
	RegisterTool(toolMgr, reg, "Measure Grid Length", "MeasureGridLength");
	RegisterTool(toolMgr, reg, "Measure Grid Area", "MeasureGridArea");
	RegisterTool(toolMgr, reg, "Measure Grid Volume", "MeasureGridVolume");
	RegisterTool(toolMgr, reg, "Measure Subset Length", "MeasureSubsetLength");
	RegisterTool(toolMgr, reg, "Measure Subset Area", "MeasureSubsetArea");
	RegisterTool(toolMgr, reg, "Measure Subset Volume", "MeasureSubsetVolume");
	RegisterTool(toolMgr, reg, "Measure Selection Length", "MeasureSelectionLength");
	RegisterTool(toolMgr, reg, "Measure Selection Area", "MeasureSelectionArea");
	RegisterTool(toolMgr, reg, "Measure Selection Volume", "MeasureSelectionVolume");

	// RegisterTool(toolMgr, reg, "Weighted Edge Smooth", "WeightedEdgeSmooth");
	// RegisterTool(toolMgr, reg, "Weighted Face Smooth", "WeightedFaceSmooth");
	// RegisterTool(toolMgr, reg, "Weighted Normal Smooth", "WeightedNormalSmooth");
	RegisterTool(toolMgr, reg, "Slope Smooth", "SlopeSmooth");

	RegisterTool(toolMgr, reg, "Mark Corners Of Marked Edges", "MarkCornersOfMarkedEdges");

	RegisterTool(toolMgr, reg, "Replace Valence 3 Vertices", "ReplaceValence3Vertices");
	RegisterTool(toolMgr, reg, "Replace Low Valence Vertices", "ReplaceLowValenceVertices");
	RegisterTool(toolMgr, reg, "Adjust Edge Length Extended", "AdjustEdgeLengthExtended");

	RegisterTool(toolMgr, reg, "Slivers", "SelectSlivers");

	RegisterTool(toolMgr, reg, "Selection Kinks", "SelectSelectionKinkVertices");
	RegisterTool(toolMgr, reg, "Subset Kinks", "SelectSubsetKinkVertices");

	RegisterTool(toolMgr, reg, "Linked Edges", "SelectLinkedEdges");
	RegisterTool(toolMgr, reg, "Linked Faces", "SelectLinkedFaces");
	RegisterTool(toolMgr, reg, "Linked Volumes", "SelectLinkedVolumes");

	RegisterTool(toolMgr, reg, "Select Short Polychains", "SelectShortPolychains");

	RegisterTool(toolMgr, reg, "Interface Vertices", "SelectInterfaceVertices");
	RegisterTool(toolMgr, reg, "Interface Edges", "SelectInterfaceEdges");
	RegisterTool(toolMgr, reg, "Interface Faces", "SelectInterfaceFaces");

	RegisterTool(toolMgr, reg, "Remove Double Faces", "RemoveDoubleFaces");
	
	RegisterTool(toolMgr, reg, "Simplify Polylines", "SimplifyPolylines");
	RegisterTool(toolMgr, reg, "Simplify Smoothed Polylines", "SimplifySmoothedPolylines");

	RegisterTool(toolMgr, reg, "Convert To Tetrahedra", "ConvertToTetrahedra");
}
