// authors: Sebastian Reiter
// s.b.reiter@gmail.com

#ifndef __H__PROMESH_arg_tool
#define __H__PROMESH_arg_tool

#include <sstream>

class ArgTool {
public:
	ArgTool (int argc, const char** argv) :
		m_argc (argc),
		m_argv (argv)
	{}

	void new_section(const std::string& grpName)
	{
		m_help << std::endl << "  " << grpName << std::endl;
	}

	bool has_param (const std::string& name, const std::string& desc)
	{
		add_help_entry (name, desc);
		return param_index(name) >= 0;
	}

	double get_double (const std::string& name, double defVal,
	                   const std::string& desc)
	{
		add_help_entry (name, desc);
		int pi = param_index (name);
		if(pi >= 0 && (pi + 1 < m_argc)){
			return std::atof(m_argv[pi+1]);
		}
		return defVal;
	}

	std::string get_string (const std::string& name, const std::string& defVal,
	                   		const std::string& desc)
	{
		add_help_entry (name, desc);
		int pi = param_index (name);
		if(pi >= 0 && (pi + 1 < m_argc))
			return m_argv[pi+1];
		return defVal;
	}

	const std::string get_help () const
	{
		return m_help.str();
	}

private:
	int param_index (const std::string& name) const
	{
		for(int i = 0; i < m_argc; ++i){
			if(name.compare(m_argv[i]) == 0)
				return i;
		}
		return -1;
	}

	void add_help_entry (const std::string& name, const std::string& desc)
	{
		using namespace std;
		m_help << "\n  " << name;
		const size_t len = name.size () + 2;
		if (len < m_leftColWidth)
			m_help << string(m_leftColWidth - len, ' ');

		size_t cur = 0;
		size_t next = 0;
		bool multiline = false;
		while (cur < desc.size()) {
			next = desc.find("\n", cur);
			if(next != string::npos){
				size_t tmpNext = next;
				if((tmpNext > 0) && (desc[tmpNext-1] == '\r'))
					--tmpNext;
				if(multiline)
					m_help << string(m_leftColWidth, ' ');
				m_help << desc.substr(cur, tmpNext - cur + 1);
				cur = next + 1;
				if(cur >= desc.size())
					break;
			}
			else{
				if(multiline)
					m_help << string(m_leftColWidth, ' ');
				m_help << desc.substr(cur, desc.size() - cur) << endl;
				break;
			}
			multiline = true;
		}
		if(multiline)
			m_help << endl;
	}

private:
	int m_argc;
	const char** m_argv;
	std::stringstream	m_help;
	static constexpr size_t m_leftColWidth = 12;
};

#endif	//__H__PROMESH_arg_tool
