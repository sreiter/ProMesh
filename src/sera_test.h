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
#include "common/boost_serialization.h"
#include "common/math/ugmath_types.h"

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
		UG_LOG("SeraBase::serialize()...\n");
	}

};

// BOOST_SERIALIZATION_ASSUME_ABSTRACT(SeraBase);

class SeraTest
{
public:
	SeraTest() :
		m_id( 12345 ),
		m_name( "nameOfSeraTest" ),
		m_pos(3.1, 2.2, 1.3)
	{};

	~SeraTest()	{};
	
	int 		m_id;
	std::string	m_name;
	ug::vector3	m_pos;

private:
	friend class boost::serialization::access;

	template <class Archive>
	void serialize( Archive& ar, const unsigned int version)
	{
		UG_LOG("SeraTest::serialize()...\n");
		ar & ug::make_nvp("id", m_id);
		ar & ug::make_nvp("name", m_name);
		ar & ug::make_nvp("pos", m_pos);
	}
};


class SeraTest2 : public SeraBase
{
public:
	SeraTest2() : m_int(0)	{}

	SeraTest	m_properties;
	int			m_int;

private:
	friend class boost::serialization::access;

	template <class Archive>
	void serialize( Archive& ar, const unsigned int version)
	{
		UG_LOG("SeraTest2::serialize()...\n");
		UG_EMPTY_BASE_CLASS_SERIALIZATION(SeraTest2, SeraBase);
		// ar & ug::make_nvp("base", boost::serialization::base_object<SeraBase>(*this));
		ar & ug::make_nvp("properties", m_properties);
		ar & ug::make_nvp("int", m_int);
	}
};

BOOST_CLASS_EXPORT(SeraTest2)

#endif	//__H__UG_sera_test
