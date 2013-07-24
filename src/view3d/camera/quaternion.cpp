//	created by Sebastian Reiter in 2006
//	parts taken from somewhere else (game programming gems 2!?!)
//	s.b.reiter@googlemail.com

#include "quaternion.h"
#include "vec_math.h"
#include "matrix44.h"

namespace cam
{

static const float DELTA = 1e-6;     // error tolerance

CQuaternion::CQuaternion()
{
}

CQuaternion::CQuaternion(float nx, float ny, float nz, float nw)
{
	x = nx;
	y = ny;
	z = nz;
	w = nw;
}

CQuaternion CQuaternion::inverse()
{
	return (CQuaternion(-x, -y, -z, w) * (1.f / length_sqr()));
}

void CQuaternion::set_values(float angle, vector3& axis)
{
	float s = (float)sin(angle / 2.f);
	float c = (float)cos(angle / 2.f);

	x = s * axis.x();
	y = s * axis.y();
	z = s * axis.z();
	w = c;
}

void CQuaternion::set_values(float nx, float ny, float nz, float angle)
{
	float tmp = (nx*nx + ny*ny + nz*nz);
	if(tmp != 0)
		tmp = 1.f / tmp;

	float s = (float) sin(angle / 2.f);
	x = s * nx * tmp;
	y = s * ny * tmp;
	z = s * nz * tmp;

	w = (float) cos(angle / 2.f);
}

void CQuaternion::set_angle_of_rotation(float angle)
{
	w = (float) cos(angle / 2.f);
}

vector3 CQuaternion::axis_of_rotation()
{
	return vector3(x, y, z);
}

float CQuaternion::angle_of_rotation()
{
	return w;
}

float CQuaternion::length()
{
	return(float) sqrt(x*x + y*y + z*z + w*w);
}

float CQuaternion::length_sqr()
{
	return(float) (x*x + y*y + z*z + w*w);
}

void CQuaternion::normalize()
{
	float m = length();
    if (m > 0.0F)
      m = 1.0F / m;
    else
      m = 0.0F;
    x *= m;
    y *= m;
    z *= m;
    w *= m;
}

CQuaternion CQuaternion::operator * (const float f)
{
	return CQuaternion(x*f, y*f, z*f, w*f);
}

CQuaternion CQuaternion::operator * (const CQuaternion& q)
{
	CQuaternion qOut;

	qOut.x = w * q.x + x * q.w + y * q.z - z * q.y;
	qOut.y = w * q.y + y * q.w + z * q.x - x * q.z;
	qOut.z = w * q.z + z * q.w + x * q.y - y * q.x;
	qOut.w = w * q.w - x * q.x - y * q.y - z * q.z;

	return qOut;
}



void matrix_from_quaternion(matrix44* matOut, CQuaternion* quat)
{

	float wx, wy, wz, xx, yy, yz, xy, xz, zz, x2, y2, z2;

	x2 = quat->x + quat->x; y2 = quat->y + quat->y; z2 = quat->z + quat->z;
	xx = quat->x * x2;   xy = quat->x * y2;   xz = quat->x * z2;
	yy = quat->y * y2;   yz = quat->y * z2;   zz = quat->z * z2;
	wx = quat->w * x2;   wy = quat->w * y2;   wz = quat->w * z2;

	(*matOut)[0][0] = 1.0 - (yy + zz);
	(*matOut)[0][1] = xy - wz;
	(*matOut)[0][2] = xz + wy;
	(*matOut)[0][3] = 0.0;

	(*matOut)[1][0] = xy + wz;
	(*matOut)[1][1] = 1.0 - (xx + zz);
	(*matOut)[1][2] = yz - wx;
	(*matOut)[1][3] = 0.0;

	(*matOut)[2][0] = xz - wy;
	(*matOut)[2][1] = yz + wx;
	(*matOut)[2][2] = 1.0 - (xx + yy);
	(*matOut)[2][3] = 0.0;

	(*matOut)[3][0] = 0;
	(*matOut)[3][1] = 0;
	(*matOut)[3][2] = 0;
	(*matOut)[3][3] = 1;

}

void quaternion_from_matrix(CQuaternion* quatOut, matrix44* mat)
{

  float  tr, s;
  float  q[4];
  int    i, j, k;

  int nxt[3] = {1, 2, 0};

  tr = (*mat)[0][0] + (*mat)[1][1] + (*mat)[2][2];

  // check the diagonal

  if (tr > 0.0)
  {
    s = sqrt (tr + 1.0);

    quatOut->w = s / 2.0;

	s = 0.5 / s;

    quatOut->x = ((*mat)[1][2] - (*mat)[2][1]) * s;
    quatOut->y = ((*mat)[2][0] - (*mat)[0][2]) * s;
    quatOut->z = ((*mat)[0][1] - (*mat)[1][0]) * s;

  } else {

	  // diagonal is negative

	  i = 0;

      if ((*mat)[1][1] > (*mat)[0][0]) i = 1;
	  if ((*mat)[2][2] > (*mat)[i][i]) i = 2;

	  j = nxt[i];
      k = nxt[j];

      s = sqrt (((*mat)[i][i] - ((*mat)[j][j] + (*mat)[k][k])) + 1.0);

	  q[i] = s * 0.5;

      if (s != 0.0) s = 0.5 / s;

	  q[3] = ((*mat)[j][k] - (*mat)[k][j]) * s;
      q[j] = ((*mat)[i][j] + (*mat)[j][i]) * s;
      q[k] = ((*mat)[i][k] + (*mat)[k][i]) * s;

	  quatOut->x = q[0];
	  quatOut->y = q[1];
	  quatOut->z = q[2];
	  quatOut->w = q[3];
  }
}

void quaternion_from_ball_points(CQuaternion* quatOut, vector3* vFrom, vector3* vTo)
{
    float fDot = Vec3Dot(*vFrom, *vTo);
    vector3 vPart;
    Vec3Cross(vPart, *vTo, *vFrom);

	*quatOut = CQuaternion(vPart.x(), vPart.y(), vPart.z(), fDot);
	quatOut->normalize();
}

void axis_from_quaternion(vector3* vXout, vector3* vYout, vector3* vZout, CQuaternion* quat)
{
	float wx, wy, wz, xx, yy, yz, xy, xz, zz, x2, y2, z2;

	x2 = quat->x + quat->x; y2 = quat->y + quat->y; z2 = quat->z + quat->z;
	xx = quat->x * x2;   xy = quat->x * y2;   xz = quat->x * z2;
	yy = quat->y * y2;   yz = quat->y * z2;   zz = quat->z * z2;
	wx = quat->w * x2;   wy = quat->w * y2;   wz = quat->w * z2;

	vXout->x() = 1.0 - (yy + zz);
	vYout->x() = xy - wz;
	vZout->z() = xz + wy;

	vXout->y() = xy + wz;
	vYout->y() = 1.0 - (xx + zz);
	vZout->y() = yz - wx;

	vXout->z() = xz - wy;
	vYout->z() = yz + wx;
	vZout->z() = 1.0 - (xx + yy);
}

void quaternion_from_axis(CQuaternion* quatOut, vector3* vX, vector3* vY, vector3* vZ)
{
  vector3* mat[3];
  float  tr, s;
  float  q[4];
  int    i, j, k;

  mat[0] = vX;
  mat[1] = vY;
  mat[2] = vZ;

  int nxt[3] = {1, 2, 0};

  tr = (*mat)[0][0] + (*mat)[1][1] + (*mat)[2][2];

  // check the diagonal

  if (tr > 0.0)
  {
    s = sqrt (tr + 1.0);

    quatOut->w = s / 2.0;

	s = 0.5 / s;

    quatOut->x = (vY->z() - vZ->y()) * s;
    quatOut->y = (vZ->x() - vX->z()) * s;
    quatOut->z = (vX->y() - vY->x()) * s;

  } else {

	  // diagonal is negative

	  i = 0;

      if (vY->y() > vX->x()) i = 1;
	  if (vZ->z() > (*mat[i])[i]) i = 2;

	  j = nxt[i];
      k = nxt[j];

      s = sqrt (((*mat[i])[i] - ((*mat[j])[j] + (*mat[k])[k])) + 1.0);

	  q[i] = s * 0.5;

      if (s != 0.0) s = 0.5 / s;

	  q[3] = ((*mat[j])[k] - (*mat[k])[j]) * s;
      q[j] = ((*mat[i])[j] + (*mat[j])[i]) * s;
      q[k] = ((*mat[i])[k] + (*mat[k])[i]) * s;

	  quatOut->x = q[0];
	  quatOut->y = q[1];
	  quatOut->z = q[2];
	  quatOut->w = q[3];
  }
}

void quaternion_slerp(CQuaternion* quatOut, CQuaternion* quat1, CQuaternion* quat2, float t)
{
        float           to1[4];
        double          omega, cosom, sinom;
        double          scale0, scale1;

        // calc cosine
        cosom = quat1->x * quat2->x + quat1->y * quat2->y + quat1->z * quat2->z
			       + quat1->w * quat2->w;

        // adjust signs (if necessary)
        if ( cosom < 0.0 )
		{
			cosom = -cosom;

			to1[0] = - quat2->x;
			to1[1] = - quat2->y;
			to1[2] = - quat2->z;
			to1[3] = - quat2->w;

        } else  {

			to1[0] = quat2->x;
			to1[1] = quat2->y;
			to1[2] = quat2->z;
			to1[3] = quat2->w;

        }

        // calculate coefficients

        if ( (1.0 - cosom) > DELTA )
		{
                // standard case (slerp)
                omega = acos(cosom);
                sinom = sin(omega);
                scale0 = sin((1.0 - t) * omega) / sinom;
                scale1 = sin(t * omega) / sinom;

        } else {
			    // "from" and "to" quaternions are very close
			    //  ... so we can do a linear interpolation

                scale0 = 1.0 - t;
                scale1 = t;
        }

		// calculate final values
		quatOut->x = scale0 * quat1->x + scale1 * to1[0];
		quatOut->y = scale0 * quat1->y + scale1 * to1[1];
		quatOut->z = scale0 * quat1->z + scale1 * to1[2];
		quatOut->w = scale0 * quat1->w + scale1 * to1[3];

}



/*SDOC***********************************************************************

  Name:		gluQuatLerp_EXT

  Action:   Linearly interpolates between two quaternion positions

  Params:   GLQUAT (first and second quaternion), GLfloat (interpolation
			parameter [0..1]), GL_QUAT (resulting quaternion; inbetween)

  Returns:  nothing

  Comments: fast but not as nearly as smooth as Slerp

***********************************************************************EDOC*/
void quaternion_lerp(CQuaternion* quatOut, CQuaternion* quat1, CQuaternion* quat2, float t)
{
        float           to1[4];
        double          cosom;
        double          scale0, scale1;

        // calc cosine
        cosom = quat1->x * quat2->x + quat1->y * quat2->y + quat1->z * quat2->z
			       + quat1->w * quat2->w;

        // adjust signs (if necessary)
        if ( cosom < 0.0 )
		{
			to1[0] = - quat2->x;
			to1[1] = - quat2->y;
			to1[2] = - quat2->z;
			to1[3] = - quat2->w;

        } else  {

			to1[0] = quat2->x;
			to1[1] = quat2->y;
			to1[2] = quat2->z;
			to1[3] = quat2->w;

        }


		// interpolate linearly
        scale0 = 1.0 - t;
        scale1 = t;

		// calculate final values
		quatOut->x = scale0 * quat1->x + scale1 * to1[0];
		quatOut->y = scale0 * quat1->y + scale1 * to1[1];
		quatOut->z = scale0 * quat1->z + scale1 * to1[2];
		quatOut->w = scale0 * quat1->w + scale1 * to1[3];

}

}

