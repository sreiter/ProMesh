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
		m_frame(NULL),
		m_curToolWidget(NULL),
		m_layout(NULL),
		m_curName(0)
	{
		m_frame = new QFrame(parent);
		m_layout = new QVBoxLayout(m_frame);
		m_layout->setSpacing(0);
		m_layout->setContentsMargins(0, 0, 0, 0);
		m_frame->setLayout(m_layout);
		m_frame->setLineWidth(1);
	}

	QWidget* widget() {return m_frame;}

//	ignore pointer polymorphism
	// template<class T>
	// void register_type(const T * = NULL){}

	// unsigned int get_library_version(){
	// 	return 0;
	// }

	void save_binary(const void *address, std::size_t count){}

	// // the << operators 
	// template<class T>
	// tooldlg_oarchive & operator<<(T const & t){
	// 	save(t, "unknown");
	// 	return * this;
	// }

	// template<class T>
	// tooldlg_oarchive & operator<<(T * const t){
	// 	if(t != NULL)
	// 		*this << *t;
	// 	return * this;
	// }

	// template<class T, int N>
	// tooldlg_oarchive & operator<<(const T (&t)[N]){
	// 	return *this << boost::serialization::make_array(
	// 		static_cast<const T *>(&t[0]),
	// 		N
	// 	);
	// }

	// template<class T>
	// tooldlg_oarchive & operator<<(const boost::serialization::nvp< T > & t){
	// 	const char* name = t.name();
	// 	const T& val = t.const_value();
	// 	save(val, name);
	// 	return *this;
	// }

	// // the & operator 
	// template<class T>
	// tooldlg_oarchive & operator&(const T & t){
	// 	return *this << t;
	// }

private:
	friend class boost::archive::detail::interface_oarchive <tooldlg_oarchive>;
	friend class boost::archive::save_access;

	ToolWidget* get_tool_widget(const char* name) {
		if(!m_curToolWidget){
			m_curToolWidget = new ToolWidget(QString(name), m_frame, NULL, 0);
			m_layout->addWidget(m_curToolWidget);
			m_layout->setAlignment(m_curToolWidget, Qt::AlignLeft);
		}

		return m_curToolWidget;
	}



	template<class Archive>
	struct save_enum_type {
		template<class T>
		static void invoke(Archive &ar, const T &t, const char* name){
			// ar.m_os << static_cast<int>(t);
		}
	};


	template<class Archive>
	struct save_primitive {
		static ToolWidget* tw(Archive& ar, const char* name){
			return ar.get_tool_widget(QString(name).append(":").toLocal8Bit().constData());
		}

		template<class T>
		static void invoke(Archive & ar, const T & t, const char* name){
			std::cout << "can't create tool entry for " << name << " with type " << typeid(t).name() << std::endl;
		}

		static void invoke(Archive & ar, int t, const char* name){
			tw(ar, name)->addSlider(QString(name), 0, 10, 1);
		}

		static void invoke(Archive & ar, const std::string& t, const char* name){
			tw(ar, name)->addTextBox(QString(name), QString(t.c_str()));
		}
	};


	template<class Archive>
	struct save_only {
		template<class T>
		static void invoke(Archive & ar, const T & t, const char* name){
			ExtendibleWidget* extWidget = new ExtendibleWidget(ar.m_frame);
			ar.m_layout->addWidget(extWidget);
			ar.m_layout->setAlignment(extWidget, Qt::AlignLeft);

			tooldlg_oarchive tar(extWidget);
			boost::serialization::serialize_adl(
				tar, 
				const_cast<T &>(t), 
				boost::serialization::version< T >::value
			);
			
			extWidget->setText(QString(name));
			extWidget->setWidget(tar.widget());
			extWidget->setChecked(true);
		}

		template<class T>
		static void invoke(Archive & ar, T * t, const char* name){
			invoke(ar, *t, name);
		}
	};



	template<class T>
	void save_override(const T &t, int i){
		// base_t::save_override (t, i);
		save(t, "unknown");
		// base_t::save_override(t, i);
	}

	template<class T>
	void save_override(const boost::serialization::nvp <T> &t, int i){
		// m_curName = t.name();
		// base_t::save_override(t.const_value(), i);
		save (t.value(), t.name());
		// std::cout << "name: " << t.name() << std::endl;
		// base_t::save_override (t, i);
	}

	// template<class T>
	// void save_override(const boost::serialization::nvp <T*> &t, int i){
	// 	base_t::save_override (t, i);
	// }

	template<class T>
	void save(const T &t){
		save (t, m_curName);
	}

	template<class T>
	void save(const T &t, const char* name){
		typedef 
			BOOST_DEDUCED_TYPENAME boost::mpl::eval_if<boost::is_enum< T >,
				boost::mpl::identity<save_enum_type<tooldlg_oarchive> >,
			//else
			BOOST_DEDUCED_TYPENAME boost::mpl::eval_if<
				// if its primitive
					boost::mpl::equal_to<
						boost::serialization::implementation_level< T >,
						boost::mpl::int_<boost::serialization::primitive_type>
					>,
					boost::mpl::identity<save_primitive<tooldlg_oarchive> >,
			// else
				boost::mpl::identity<save_only<tooldlg_oarchive> >
			> >::type typex;
		typex::invoke(*this, t, name);
	}

	// template<class T>
	// void save(T *t, const char* name){
	// 	save(*t, name);
	// }

	#ifndef BOOST_NO_STD_WSTRING
	void save(const std::wstring &ws){
	}
	#endif


	QWidget*		m_parent;
	QFrame*			m_frame;
	ToolWidget*		m_curToolWidget;
	QLayout*		m_layout;
	const char*		m_curName;
};

BOOST_SERIALIZATION_REGISTER_ARCHIVE(tooldlg_oarchive);

#endif	//__H__UG_tooldlg_oarchive
