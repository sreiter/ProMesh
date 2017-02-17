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

#include <set>
#include "csg_object.h"

using namespace std;
using namespace ug;

CSGObject* CreateEmptyCSGObject(const char* name)
{
    CSGObject* obj = new CSGObject;
    obj->set_name(name);
    return obj;
}

void CSGObject::
selection_changed()
{
	Selector& sel = selector();
	SubsetHandler& sh = subset_handler();

//	find all selected subsets
	set<int> selSubs;

	for(VertexIterator i = sel.begin<Vertex>();
		i != sel.end<Vertex>(); ++i)
	{
		selSubs.insert(sh.get_subset_index(*i));
	}

	for(EdgeIterator i = sel.begin<Edge>();
		i != sel.end<Edge>(); ++i)
	{
		selSubs.insert(sh.get_subset_index(*i));
	}

	for(FaceIterator i = sel.begin<Face>();
		i != sel.end<Face>(); ++i)
	{
		selSubs.insert(sh.get_subset_index(*i));
	}

	for(VolumeIterator i = sel.begin<Volume>();
		i != sel.end<Volume>(); ++i)
	{
		selSubs.insert(sh.get_subset_index(*i));
	}

	sel.clear();
	for(set<int>::iterator i = selSubs.begin(); i != selSubs.end(); ++i)
	{
		int si = *i;
		if(si < 0)
			continue;

		if(sh.num<Volume>(si) > 0)
			sel.select(sh.begin<Volume>(si), sh.end<Volume>(si));
		else if(sh.num<Face>(si) > 0)
			sel.select(sh.begin<Face>(si), sh.end<Face>(si));
		else if(sh.num<Edge>(si) > 0)
			sel.select(sh.begin<Edge>(si), sh.end<Edge>(si));
		else if(sh.num<Vertex>(si) > 0)
			sel.select(sh.begin<Vertex>(si), sh.end<Vertex>(si));
	}

	LGObject::selection_changed();
}
