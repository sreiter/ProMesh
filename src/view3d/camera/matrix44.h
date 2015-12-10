/*
 * Copyright (c) 2008-2015:  G-CSC, Goethe University Frankfurt
 * Copyright (c) 2006-2008:  Steinbeis Forschungszentrum (STZ Ölbronn)
 * Copyright (c) 2006-2015:  Sebastian Reiter
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

#ifndef __VEC_MATH_TYPES__
#define __VEC_MATH_TYPES__

namespace cam
{

struct matrix44
{
	matrix44()	{}
	matrix44( float m00, float m01, float m02, float m03,
			float m10, float m11, float m12, float m13,
			float m20, float m21, float m22, float m23,
			float m30, float m31, float m32, float m33);

	matrix44(const matrix44& mat)			{assign(mat);}
	matrix44& operator=(const matrix44& mat){assign(mat); return *this;}

	void assign(const matrix44& mat);

    union {
        struct {
            float _00; float _01; float _02; float _03;
            float _10; float _11; float _12; float _13;
            float _20; float _21; float _22; float _23;
            float _30; float _31; float _32; float _33;

        };
        float m[4][4];
    };

    float* operator[](unsigned int index)	{return m[index];}
    const float* operator[](unsigned int index) const	{return m[index];}
};

typedef matrix44 matrix;

//	some basic methods for matrix manipulation. should be somewhere else...
void MatIdentity(matrix44& mat);
void MatTranslation(matrix44& matOut, float x, float y, float z);
void MatMultiply(matrix44& matOut, matrix44& mat1, matrix44& mat2);
void MatRotateRadX(matrix44& matOut, float rad);
void MatRotateRadY(matrix44& matOut, float rad);
void MatRotateRadZ(matrix44& matOut, float rad);
void MatTranspose(matrix44& matOut, matrix44& mat);

}

#endif
