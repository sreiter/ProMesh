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

#include <string>
#include "serialization.h"
#include "boost/serialization/export.hpp"

#ifndef __H__PROMESH_sera_test
#define __H__PROMESH_sera_test

class SeraBase {
public:
	virtual ~SeraBase () {}

private:
	friend class boost::serialization::access;

	template <class Archive>
	void serialize( Archive& ar, const unsigned int version)
	{
	}

};

BOOST_SERIALIZATION_ASSUME_ABSTRACT(SeraBase)

class SeraTest
{
public:
	SeraTest() :
		m_id( 12345 ),
		m_name( "nameOfSeraTest" )
	{};

	~SeraTest()	{};
	
private:
	friend class boost::serialization::access;

	template <class Archive>
	void serialize( Archive& ar, const unsigned int version)
	{
		ar & MAKE_NVP("id", m_id);
		ar & MAKE_NVP("name", m_name);
	}

	int 		m_id;
	std::string	m_name;
};


class SeraTest2 : public SeraBase
{
public:
	SeraTest2()	{}

private:
	friend class boost::serialization::access;

	template <class Archive>
	void serialize( Archive& ar, const unsigned int version)
	{
		// ar & MAKE_NVP("base", boost::serialization::base_object<SeraBase>(*this));
		ar & MAKE_NVP("properties", m_properties);
		boost::serialization::void_cast_register<SeraTest2, SeraBase>(
				static_cast<SeraTest2 *>(NULL),
				static_cast<SeraBase *>(NULL)
				);
	}

	SeraTest	m_properties;
};

BOOST_CLASS_EXPORT(SeraTest2)

#endif	//__H__UG_sera_test
