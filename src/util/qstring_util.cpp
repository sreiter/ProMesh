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

#include "qstring_util.h"
#include <iostream>

using namespace std;

QByteArray To7BitAscii (const QString& qs)
{
	QByteArray bytes = qs.toUtf8();
	QByteArray filtered;

	for(int i = 0; i < bytes.size(); ++i){
		const char c = bytes.at(i);
		if((c & char(128)) != char(128)){
		//	leading bit is zero => c is ascii character
			filtered.push_back(c);
		}
		else {
		//	unicode character
			if((c & char(192)) == char(192)){
			//	leading unicode character
				filtered.push_back(' ');
			}
		}
	}

	return filtered;
}


// QByteArray FilterOutNonExtAscii (const QString& qs)
// {
// 	QByteArray bytes = qs.toUtf8();
// 	QString latin;

// 	for(int i = 0; i < bytes.size(); ++i){
// 		cout << "<" << int(unsigned char(bytes.at(i)));
// 		const char c = bytes.at(i);
// 		char nc = c;
// 		if((c & char(192)) == char(192)){//b11000000
// 			nc = ' ';
// 			++i;
// 			cout << "," << int(unsigned char(bytes.at(i)));
// 			if((c & char(224)) == char(224)){//b11100000
// 				++i;
// 				cout << "," << int(unsigned char(bytes.at(i)));
// 				if((c & char(240)) == char(240)){//b11110000
// 					++i;
// 					cout << "," << int(unsigned char(bytes.at(i)));
// 				}
// 			}
// 			else if(unsigned char(c) <= unsigned char(195)){ //195 = b11000011 (largest leading byte for extended ascii letters)
// 				char c1 = bytes.at(i);
// 				//	use lowest 2 bits of c and lowest 6 bits of bytes c1 to form ascii byte
// 				nc = (c << 6) | (c1 & char(63));
// 			}
// 			cout << ">";
// 		}

// 		cout << " = '" << int(unsigned char(nc)) << "'" << endl;
// 		latin.push_back(nc);
// 	}

// 	cout << endl;
// 	return latin.toLocal8Bit();
// }

