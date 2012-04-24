//	created by Sebastian Reiter
//	s.b.reiter@googlemail.com
//	y09 m09 d28

#ifndef __H__CAM__ARC_BALL__
#define __H__CAM__ARC_BALL__

#include "vec_math.h"
#include "quaternion.h"
#include "matrix44.h"

namespace cam
{

class CArcBall
{
	public:
		CArcBall();
		void set_window(int nWidth, int nHeight, float fRadius = 0.9f, int OffsetX = 0, int OffsetY = 0);

		matrix44* get_rotation_matrix();
		CQuaternion* get_rotation_quaternion();
		void set_rotation_quaternion(CQuaternion* pQuaternion);

		void begin_drag(int x, int y);
		void drag_to(int x, int y);
		void end_drag();

	private:
		vector3 get_ball_point_from_screen_coords(int nx, int ny);
		CQuaternion get_quat_from_ball_points(vector3& vFrom, vector3& vTo);

		bool		m_bDrag;

		matrix44	m_matRotation;
		CQuaternion	m_quatRotation;

		CQuaternion	m_quatDown;
		vector3		m_vDown;

		vector2		m_Screen;
		vector2		m_Offset;

		float		m_fRadius;

		int			m_iMouseLastX;
		int			m_iMouseLastY;
};

}//	end of namespace

#endif
