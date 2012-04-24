//	created by Sebastian Reiter
//	s.b.reiter@googlemail.com
//	y09 m10 d15

#include <QtGui>
#include "clip_plane_widget.h"
#include "scene/lg_scene.h"

using namespace ug;

ClipPlaneWidget::ClipPlaneWidget(QWidget* parent) : QWidget(parent)
{
	m_scene = NULL;
	QVBoxLayout* vLayout = new QVBoxLayout;

//	set up the layouts, checkboxes and sliders for each plane
	for(int i = 0; i < 3; ++i)
	{
		m_checkBox[i] = new QCheckBox;
		m_checkBox[i]->setChecked(false);
		m_slider[i] = new QSlider(Qt::Horizontal);
		m_slider[i]->setRange(0, 102);
		m_slider[i]->setValue(51);

	//	connect to signals and slots
		connect(m_checkBox[i], SIGNAL(stateChanged(int)),
				this, SLOT(stateChanged(int)));
		connect(m_slider[i], SIGNAL(valueChanged(int)),
				this, SLOT(valueChanged(int)));

		QHBoxLayout* hLayout = new QHBoxLayout;
		hLayout->addWidget(m_checkBox[i]);
		hLayout->addWidget(m_slider[i]);

		vLayout->addLayout(hLayout);
	}

	m_checkBox[0]->setText(tr("xz"));
	m_checkBox[1]->setText(tr("xy"));
	m_checkBox[2]->setText(tr("yz"));

	setLayout(vLayout);

//TODO:	the widget shouldn't have a fixed height.
//		instead it should reszie less easy than other widgets vertically.
	setFixedHeight(sizeHint().height());

}

ClipPlaneWidget::~ClipPlaneWidget()
{
}

void ClipPlaneWidget::setClipPlane(int index, float ia)
{
//	the bounding box of the scene
	ug::vector3 vMin, vMax;
	m_scene->get_bounding_box(vMin, vMax);

//	position of the plane (moving on the diagonal)
	vector3 vPlanePos;
	VecScaleAdd(vPlanePos, 1.f - ia, vMin, ia, vMax);

	switch(index)
	{
		case 0:	//xz
			m_scene->setClipPlane(0, Plane(vPlanePos, vector3(0, -1, 0)));
			break;
		case 1:	//xy
			m_scene->setClipPlane(1, Plane(vPlanePos, vector3(0, 0, 1)));
			break;
		case 2:	//yz
			m_scene->setClipPlane(2, Plane(vPlanePos, vector3(-1, 0, 0)));
			break;
	}
}

void ClipPlaneWidget::setScene(LGScene* scene)
{
	m_scene = scene;
	updateClipPlanes();
	connect(m_scene, SIGNAL(geometry_changed()),
			this, SLOT(updateClipPlanes()));
}

void ClipPlaneWidget::updateClipPlanes()
{
	if(!m_scene)
		return;

	for(int i = 0; i < 3; ++i)
		setClipPlane(i, (float)(m_slider[i]->value()-1) / 100.f);

	bool gotOne = false;

	for(int i = 0; i < 3; ++i)
		gotOne |= m_scene->clipPlaneIsEnabled(i);

	if(gotOne)
		m_scene->update_visuals();
}

void ClipPlaneWidget::valueChanged(int newValue)
{
	if(!m_scene)
		return;

	QSlider* slider = qobject_cast<QSlider*>(sender());
//	get the slider index
	int sliderIndex = 0;
	for(; sliderIndex < 3; ++sliderIndex)
	{
		if(slider == m_slider[sliderIndex])
			break;
	}

//	the interpolation amount
	float ia = (float)(newValue-1) / 100.f;

//	set clip plane position
	setClipPlane(sliderIndex, ia);

//	update visuals
	if(m_scene->clipPlaneIsEnabled(sliderIndex))
		m_scene->update_visuals();
}

void ClipPlaneWidget::stateChanged(int newState)
{
	if(!m_scene)
		return;

	QCheckBox* checkBox = qobject_cast<QCheckBox*>(sender());
//	get the slider index
	int cbIndex = 0;
	for(; cbIndex < 3; ++cbIndex)
	{
		if(checkBox == m_checkBox[cbIndex])
			break;
	}

	if(cbIndex < 3)
	{
		switch(newState)
		{
			case 0:
				m_scene->enableClipPlane(cbIndex, false);
				break;
			case 2:
				m_scene->enableClipPlane(cbIndex, true);
				break;
		}

		m_scene->update_visuals();
	}
}

/*
LGScene*	m_scene;
QSlider*	m_slider[3];
QCheckBox*	m_checkBox[3];
*/
