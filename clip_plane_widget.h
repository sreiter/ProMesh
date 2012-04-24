//	created by Sebastian Reiter
//	s.b.reiter@googlemail.com
//	y09 m10 d15

#ifndef CLIP_PLANE_WIDGET_H
#define CLIP_PLANE_WIDGET_H

#include <QWidget>

////////////////////////////////////////////////////////////////////////
//	predeclarations
class LGScene;
class QSlider;
class QCheckBox;

////////////////////////////////////////////////////////////////////////
//	ClipPlaneWidget
///	controls clip-planes
class ClipPlaneWidget : public QWidget
{
	Q_OBJECT

	public:
		ClipPlaneWidget(QWidget* parent);
		virtual ~ClipPlaneWidget();

		void setScene(LGScene* scene);

	protected slots:
		void updateClipPlanes();
		void valueChanged(int newValue);
		void stateChanged(int newState);

	protected:
		void setClipPlane(int index, float ia);///<	does not call update_scene()

	protected:
		LGScene*	m_scene;
		QSlider*	m_slider[3];
		QCheckBox*	m_checkBox[3];
};



#endif // CLIP_PLANE_WIDGET_H
