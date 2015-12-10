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

#include <cmath>
#include "matrix44.h"

namespace cam
{

matrix44::matrix44( float m00, float m01, float m02, float m03,
			float m10, float m11, float m12, float m13,
			float m20, float m21, float m22, float m23,
			float m30, float m31, float m32, float m33)
{
	_00 = m00; _01 = m01; _02 = m02; _03 = m03;
	_10 = m10; _11 = m11; _12 = m12; _13 = m13;
	_20 = m20; _21 = m21; _22 = m22; _23 = m23;
	_30 = m30; _31 = m31; _32 = m32; _33 = m33;
}

void matrix44::assign(const matrix44& mat)
{
	for(int i = 0; i < 4; ++i)
	{
		for(int j = 0; j < 4; ++j)
			m[i][j] = mat[i][j];
	}
}

void MatIdentity(matrix44& mat)
{
	for(int i = 0; i < 4; ++i)
	{
		for(int j = 0; j < 4; ++j)
			mat[i][j] = 0;
		mat[i][i] = 1.f;
	}
}

void MatTranslation(matrix44& matOut, float x, float y, float z)
{
	MatIdentity(matOut);
	matOut[3][0] = x;
	matOut[3][1] = y;
	matOut[3][2] = z;
}

void MatMultiply(matrix44& matOut, matrix44& mat1, matrix44& mat2)
{
	matrix44 tMat;
	for(int i = 0; i < 4; ++i)
	{
		for(int j = 0; j < 4; ++j)
		{
			float t = 0;
			for(int k = 0; k < 4; ++k)
				t += mat1[i][k] * mat2[k][j];
			tMat[i][j] = t;
		}
	}
	matOut = tMat;
}

void MatRotateRadX(matrix44& matOut, float rad)
{
	float sinA, cosA;

	sinA = (float)sin(rad);
	cosA = (float)cos(rad);

	matOut[0][0] =  1.0F; matOut[1][0] =  0.0F; matOut[2][0] =  0.0F;
	matOut[0][1] =  0.0F; matOut[1][1] =  cosA; matOut[2][1] = -sinA;
	matOut[0][2] =  0.0F; matOut[1][2] =  sinA; matOut[2][2] =  cosA;

	matOut[0][3] = 0.0F; matOut[1][3] = 0.0F; matOut[2][3] = 0.0F;
	matOut[3][0] = 0.0F;
	matOut[3][1] = 0.0F;
	matOut[3][2] = 0.0F;
	matOut[3][3] = 1.0F;
}

void MatRotateRadY(matrix44& matOut, float rad)
{
	float sinA, cosA;

	sinA = (float)sin(rad);
	cosA = (float)cos(rad);

	matOut[0][0] =  cosA; matOut[1][0] =  0.0F; matOut[2][0] =  sinA;
    matOut[0][1] =  0.0F; matOut[1][1] =  1.0F; matOut[2][1] =  0.0F;
    matOut[0][2] = -sinA; matOut[1][2] =  0.0F; matOut[2][2] =  cosA;

	matOut[0][3] = 0.0F; matOut[1][3] = 0.0F; matOut[2][3] = 0.0F;
	matOut[3][0] = 0.0F;
	matOut[3][1] = 0.0F;
	matOut[3][2] = 0.0F;
	matOut[3][3] = 1.0F;
}

void MatRotateRadZ(matrix44& matOut, float rad)
{
	float sinA, cosA;

	sinA = (float)sin(rad);
	cosA = (float)cos(rad);

	matOut[0][0] =  cosA; matOut[1][0] = -sinA; matOut[2][0] =  0.0F;
    matOut[0][1] =  sinA; matOut[1][1] =  cosA; matOut[2][1] =  0.0F;
    matOut[0][2] =  0.0F; matOut[1][2] =  0.0F; matOut[2][2] =  1.0F;

	matOut[0][3] = 0.0F; matOut[1][3] = 0.0F; matOut[2][3] = 0.0F;
	matOut[3][0] = 0.0F;
	matOut[3][1] = 0.0F;
	matOut[3][2] = 0.0F;
	matOut[3][3] = 1.0F;
}

void MatTranspose(matrix44& matOut, matrix44& mat)
{
	for(int i = 0; i < 4; ++i)
	{
		for(int j = 0; j < i; ++j)
		{
			float tmp = mat[i][j];
			matOut[i][j] = mat[j][i];
			matOut[j][i] = tmp;
		}
	}
}

}
