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

#ifndef __H__PROMESH_tooldlg_oarchive
#define __H__PROMESH_tooldlg_oarchive

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
#include <boost/archive/detail/common_oarchive.hpp>
#include <boost/archive/detail/register_archive.hpp>
#include <boost/archive/impl/archive_serializer_map.ipp>

#include <QFrame>
#include <QVBoxLayout>

#include "extendible_widget.h"
#include "../tools/tool_dialog.h"

class tooldlg_oarchive :
		public boost::archive::detail::common_oarchive<tooldlg_oarchive>
{
public:
	using base_t = boost::archive::detail::common_oarchive<tooldlg_oarchive>;
	typedef boost::mpl::bool_<false> is_loading;
	typedef boost::mpl::bool_<true> is_saving;

	tooldlg_oarchive(QWidget* parent) :
		m_parent(parent),
		m_layout(NULL),
		m_curName("")
	{
		push_widget_layer();
	}

	QWidget* widget()
	{
		if(m_widgetLayers.empty())
			push_widget_layer();
		return m_widgetLayers.top().frame();
	}

	void save_binary(const void *address, std::size_t count){}


	void save_object(
        const void *x, 
        const boost::archive::detail::basic_oserializer & bos
    )
    {
    	if(m_curName){
    		// std::cout << m_prefix << "save_object: " << m_curName << std::endl;
	    	push_widget_layer(m_curName);
	    	base_t::save_object(x, bos);
	    	pop_widget_layer();
	    }
	    else{
	    	base_t::save_object(x, bos);
	    }
    }

	void save_pointer(
		const void * t, 
		const boost::archive::detail::basic_pointer_oserializer * bpos_ptr)
    {
    	if(m_curName){
    		// std::cout << m_prefix << "save_pointer: " << m_curName << std::endl;
	    	push_widget_layer(m_curName);
	    	base_t::save_pointer(t, bpos_ptr);
	    	pop_widget_layer();
	    }
	    else{
	    	base_t::save_pointer(t, bpos_ptr);
	    }
    }



private:
	friend class boost::archive::detail::interface_oarchive <tooldlg_oarchive>;
	friend class boost::archive::save_access;

	ToolWidget* tool_widget(const char* name) {
		if(m_widgetLayers.empty())
			push_widget_layer("");
		return m_widgetLayers.top().toolWidget();
	}

	template<class T>
	void save_override(const T &t, int i){
		base_t::save_override(t, i);
	}

	template<class T>
	void save_override(const boost::serialization::nvp <T> &t, int i){
		m_curName = t.name();
		base_t::save_override(t, i);
	}

	template<class T>
	void save(const T &t){
	}

	void save(int val){
		// std::cout << m_prefix << "save " << m_curName << ": " << val << std::endl;
		create_spinner<int>(-1.e9, 1.e9, val, 1, 0);
	}

	void save(float val){
		// std::cout << m_prefix << "save " << m_curName << ": " << val << std::endl;
		create_spinner<float>(-1.e9, 1.e9, val, 1, 6);
	}

	void save(double val){
		// std::cout << m_prefix << "save " << m_curName << ": " << val << std::endl;
		create_spinner<float>(-1.e9, 1.e9, val, 1, 6);
	}

	template <class T>
	void create_spinner(T min, T max, T value, T step, T digits)
	{
		tool_widget(m_curName)->addSpinBox(QString(m_curName).append(":"), min, max, value, step, digits);
	}

	void save(const std::string& val){
		// std::cout << m_prefix << "save " << m_curName << ": " << val << std::endl;
		tool_widget(m_curName)->addTextBox(QString(m_curName).append(":"), QString(val.c_str()));
	}

	#ifndef BOOST_NO_STD_WSTRING
	void save(const std::wstring &ws){
	}
	#endif

	void push_widget_layer(const char* name = 0)
	{
		QWidget* parent = m_parent;
		QLayout* layout = m_layout;
		if(!m_widgetLayers.empty()){
			parent = m_widgetLayers.top().frame();
			layout = m_widgetLayers.top().layout();
		}

		ExtendibleWidget* extWidget = NULL;
		if(name != 0 && *name != 0){
			extWidget = new ExtendibleWidget(parent);
			layout->addWidget(extWidget);
			layout->setAlignment(extWidget, Qt::AlignLeft);
			extWidget->setText(QString(name));
			extWidget->setChecked(true);
			parent = extWidget;
			layout = NULL;
		}

		m_widgetLayers.push(WidgetLayer(parent));

		if(extWidget)
			extWidget->setWidget(m_widgetLayers.top().frame());

		m_prefix.append("  ");
	}

	void pop_widget_layer()
	{
		if(!m_widgetLayers.empty()){
			m_widgetLayers.pop();
		}
		if(m_prefix.size() > 2)
			m_prefix.resize(m_prefix.size() - 2);
	}

	class WidgetLayer {
		public:
			WidgetLayer (QWidget* parent) :
				m_toolWidget(NULL)
			{
				m_frame = new QFrame(parent);
				m_layout = new QVBoxLayout(m_frame);
				m_layout->setSpacing(0);
				m_layout->setContentsMargins(0, 0, 0, 0);
				m_frame->setLayout(m_layout);
				m_frame->setLineWidth(1);
			};

			QFrame* frame ()		{return m_frame;}
			QLayout* layout ()		{return m_layout;}
			ToolWidget* toolWidget(){
				if(!m_toolWidget){
					m_toolWidget = new ToolWidget(QString("--properties--"), m_frame, NULL, 0);
					m_layout->addWidget(m_toolWidget);
					m_layout->setAlignment(m_toolWidget, Qt::AlignLeft);
				}
				return m_toolWidget;
			}

		private:
			QFrame*			m_frame;
			QLayout*		m_layout;
			ToolWidget*		m_toolWidget;
	};

	QWidget*		m_parent;
	QFrame*			m_frame;
	std::stack<WidgetLayer>	m_widgetLayers;
	QLayout*		m_layout;
	const char*		m_curName;	///< can be NULL due to intermediate objects
	std::string		m_prefix;
};

BOOST_SERIALIZATION_REGISTER_ARCHIVE(tooldlg_oarchive);

#endif	//__H__UG_tooldlg_oarchive
