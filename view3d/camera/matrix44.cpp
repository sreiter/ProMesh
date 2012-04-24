//	created by Sebastian Reiter
//	s.b.reiter@googlemail.com
//	y09 m01 d26

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
