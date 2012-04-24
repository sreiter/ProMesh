//	created by Sebastian Reiter (parts taken from the directx 9 sdk)
//	s.b.reiter@googlemail.com

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
