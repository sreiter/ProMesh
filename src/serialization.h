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

#ifndef __H__PROMESH_serialization
#define __H__PROMESH_serialization

#include <boost/serialization/access.hpp>
#include <boost/serialization/nvp.hpp>

#define MAKE_NVP( name, val)	boost::serialization::make_nvp( name, val )

namespace boost{
namespace serialization{
	template <typename Archive>
	void serialize(Archive& ar, ug::vector3& v, const unsigned int version)
	{
		ar & MAKE_NVP("x", v[0]);
		ar & MAKE_NVP("y", v[1]);
		ar & MAKE_NVP("z", v[2]);
	}
}
}

#endif	//__H__UG_serialization
