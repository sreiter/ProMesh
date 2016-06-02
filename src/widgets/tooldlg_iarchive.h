/*
 * Copyright (c) 2016:  G-CSC, Goethe University Frankfurt
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

#ifndef __H__PROMESH_tooldlg_iarchive
#define __H__PROMESH_tooldlg_iarchive

#include <iostream>
#include <stack>
#include <boost/type_traits/is_enum.hpp>
#include <boost/mpl/bool.hpp>
#include <boost/mpl/eval_if.hpp>
#include <boost/mpl/int.hpp>
#include <boost/mpl/equal_to.hpp>
#include <boost/serialization/nvp.hpp>
#include <boost/serialization/array.hpp>
#include <boost/serialization/string.hpp>
#include <boost/serialization/access.hpp>
#include <boost/serialization/version.hpp>
#include <boost/serialization/serialization.hpp>
#include <boost/archive/detail/common_iarchive.hpp>
#include <boost/archive/detail/register_archive.hpp>

#include <QFrame>
#include <QVBoxLayout>

#include "common/error.h"

#include "extendible_widget.h"
#include "../tools/tool_dialog.h"

class tooldlg_iarchive :
		public boost::archive::detail::common_iarchive<tooldlg_iarchive>
{
public:
	using base_t = boost::archive::detail::common_iarchive<tooldlg_iarchive>;
	typedef boost::mpl::bool_<true> is_loading;
	typedef boost::mpl::bool_<false> is_saving;

	tooldlg_iarchive() :
		m_base(0),
		m_curName("")
	{}

	tooldlg_iarchive(QWidget* base) :
		m_base(base),
		m_curName("")
	{
		set_base_widget(base);
	}

	void set_base_widget(QWidget* base) {

		m_widgetLayers = std::stack<WidgetLayer> ();
		push_widget_layer(base);
	}

	void load_binary(const void *address, std::size_t count){}


	void load_object(
        void *x, 
        const boost::archive::detail::basic_iserializer & bis
    )
    {
    	if(m_curName){
    		// std::cout << m_prefix << "load_object: " << m_curName << std::endl;
	    	push_child_layer();
	    	base_t::load_object(x, bis);
	    	pop_widget_layer();
	    }
	    else{
	    	base_t::load_object(x, bis);
	    }
    }

    const boost::archive::detail::basic_pointer_iserializer * 
    load_pointer(
        void * & t, 
        const boost::archive::detail::basic_pointer_iserializer * bpis_ptr,
        const boost::archive::detail::basic_pointer_iserializer * (*finder)(
            const boost::serialization::extended_type_info & eti))
    {
    	if(m_curName){
    		// std::cout << m_prefix << "load_pointer: " << m_curName << std::endl;
	    	push_child_layer();
	    	const boost::archive::detail::basic_pointer_iserializer * 
	    		ret = base_t::load_pointer(t, bpis_ptr, finder);
	    	pop_widget_layer();
	    	return ret;
	    }
	    else{
	    	return base_t::load_pointer(t, bpis_ptr, finder);
	    }
    }



private:
	friend class boost::archive::detail::interface_iarchive <tooldlg_iarchive>;
	friend class boost::archive::load_access;

	class WidgetLayer {
		public:
			WidgetLayer (QWidget* base) :
				m_baseWidget(base),
				m_curItemIndex(0),
				m_curToolIndex(0)
			{
			};

			QWidget* current_widget () {
				UG_COND_THROW(!m_baseWidget, "Current layer has no base widget. Bad input widget hierarchy.");
				UG_COND_THROW(!m_baseWidget->layout(), "Current widget has no layout. "
							  "Input widget sequence does not match input class.");
				UG_COND_THROW(m_curItemIndex >= m_baseWidget->layout()->count(),
							  "Too many widgets requested from current layer. "
							  "Input widget sequence does not match input class.")
				return m_baseWidget->layout()->itemAt(m_curItemIndex)->widget();
			}

			void next_widget ()		{++m_curItemIndex;}

			int tool_index ()		{return m_curToolIndex;}
			void next_tool ()		{++m_curToolIndex;}

		private:
			QWidget*		m_baseWidget;
			int				m_curItemIndex;
			int				m_curToolIndex;
	};


	ToolWidget* tool_widget () {
		ToolWidget* twdgt = dynamic_cast<ToolWidget*>(top_layer().current_widget());
		UG_COND_THROW(!twdgt, "ToolWidget expected, but other widget received. "
					  "Input widget sequence does not match input class.");
		return twdgt;
	}

	// void load_override (boost::archive::version_type&)			{}
	// void load_override(boost::archive::object_id_type&)			{}
	// void load_override(boost::archive::class_id_type&)			{}
	// void load_override(boost::archive::class_id_optional_type&)	{}
	// void load_override(boost::archive::class_name_type&)		{}
	// void load_override(boost::archive::tracking_type&)			{}

	template<class T>
	void load_override (T &t){
		base_t::load_override(t);
	}

	template<class T>
	void load_override (const boost::serialization::nvp <T> &t){
		m_curName = t.name();
		base_t::load_override(t);
	}

	template<class T>
	void load_override (T &t, int i){
		base_t::load_override(t, i);
	}

	template<class T>
	void load_override (const boost::serialization::nvp <T> &t, int i){
		m_curName = t.name();
		base_t::load_override(t, i);
	}

	template<class T>
	void load (T &t){
		// std::cout << m_prefix << "load unknown\n";
	}

	void load (int& val){
		bool ok = false;
		val = tool_widget()->to_int(top_layer().tool_index(), &ok);
		UG_COND_THROW(!ok, "conversion to int failed. "
					  "Input widget sequence does not match input class");
		top_layer().next_tool();
		// std::cout << m_prefix << "load " << m_curName << ": " << val << std::endl;
	}

	void load (float& val){
		bool ok = false;
		val = tool_widget()->to_double(top_layer().tool_index(), &ok);
		UG_COND_THROW(!ok, "conversion to float failed. "
					  "Input widget sequence does not match input class");
		top_layer().next_tool();
		// std::cout << m_prefix << "load " << m_curName << ": " << val << std::endl;
	}

	void load (double& val){
		bool ok = false;
		val = tool_widget()->to_double(top_layer().tool_index(), &ok);
		UG_COND_THROW(!ok, "conversion to double failed. "
					  "Input widget sequence does not match input class");
		top_layer().next_tool();
		// std::cout << m_prefix << "load " << m_curName << ": " << val << std::endl;
	}

	void load (std::string& val){
		bool ok = false;
		val = tool_widget()->to_string(top_layer().tool_index(), &ok).toStdString();
		UG_COND_THROW(!ok, "conversion to string failed. "
					  "Input widget sequence does not match input class");
		top_layer().next_tool();
		// std::cout << m_prefix << "load " << m_curName << ": " << val << std::endl;
	}

	#ifndef BOOST_NO_STD_WSTRING
	void load (std::wstring &ws){
	}
	#endif

	
	void push_child_layer ()
	{
		if(dynamic_cast<ToolWidget*>(top_layer().current_widget()))
			top_layer().next_widget();

		ExtendibleWidget* ewgt = dynamic_cast<ExtendibleWidget*>(top_layer().current_widget());
		UG_COND_THROW(!ewgt, "ExtendibleWidget expected, but other widget received. "
					  "Input widget sequence does not match input class.");
		top_layer().next_widget();
		push_widget_layer (ewgt->widget());
	}

	void push_widget_layer (QWidget* base)
	{
		UG_COND_THROW (!base, "No base widget was set. Please do so through "
						"'tooldlg_iarchive::set_base_widget(...)'");
		m_prefix.append ("  ");
		m_widgetLayers.push (WidgetLayer(base));
	}

	void pop_widget_layer ()
	{
		if(!m_widgetLayers.empty()){
			m_widgetLayers.pop();
		}
		if(m_prefix.size() > 2)
			m_prefix.resize (m_prefix.size() - 2);
	}

	WidgetLayer& top_layer ()	{
		UG_COND_THROW(m_widgetLayers.empty(), "No widget layer available! "
					  "Input widget sequence does not match input class.");
		return m_widgetLayers.top();
	}


	QWidget*		m_base;
	std::stack<WidgetLayer>	m_widgetLayers;
	const char*		m_curName;	///< can be NULL due to intermediate objects
	std::string		m_prefix;
};

BOOST_SERIALIZATION_REGISTER_ARCHIVE(tooldlg_iarchive);

#endif	//__H__UG_tooldlg_iarchive
