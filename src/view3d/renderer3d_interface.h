//	created by Sebastian Reiter
//	s.b.reiter@googlemail.com
//	y09 m09 d29

#ifndef __H__RENDERER3D_INTERFACE__
#define __H__RENDERER3D_INTERFACE__

///	constants that define the draw mode of the renderer.
enum DrawMode
{
	DM_NONE = 0,
	DM_WIRE = 1,
	DM_SOLID = 1 << 1,
	DM_SOLID_WIRE = DM_SOLID | DM_WIRE
};

///	interface for classes that can draw their content using openGL.
class IRenderer3D
{
	public:
		virtual ~IRenderer3D()	{}

	///	this method is called when the renderer shall draw its content.
		virtual void draw() = 0;

	///	use this method to set the front draw mode of the renderer
		virtual void set_draw_mode_front(unsigned int drawMode) = 0;

	///	use this method to set the back draw mode of the renderer
		virtual void set_draw_mode_back(unsigned int drawMode) = 0;

	///	the camara transform
		virtual void set_transform(float* mat) = 0;
		
	///	the camera parameters
		virtual void set_camera_parameters(float fromX, float fromY, float fromZ,
										   float dirX, float dirY, float dirZ,
										   float upX, float upY, float upZ) = 0;

	///	the perspective transform
		virtual void set_perspective(float fovy, int viewWidth, int viewHeight,
									 float zNear, float zFar) = 0;

	///	returns the distance of the near and far clipping plane.
		virtual void get_clip_distance_estimate(float& nearOut, float& farOut,
												float fromX, float fromY, float fromZ,
												float toX, float toY, float toZ) = 0;
};

#endif // __H__RENDERER3D_INTERFACE__
