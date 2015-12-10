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

#include <algorithm>
#include <QSlider>
#include <QHBoxLayout>
#include "double_slider.h"

DoubleSlider::
DoubleSlider(QWidget* parent) :
	QWidget(parent),
	m_value(0),
	m_min(0),
	m_max(1),
	m_resolution(10000),
	m_singleStep(0.1)
{
	m_slider = new QSlider(Qt::Horizontal, this);
	m_slider->setRange(0, (int)m_resolution);
	m_slider->setValue(0);
	setSingleStep(m_singleStep);

	QHBoxLayout* l = new QHBoxLayout(this);
	l->setSpacing(0);
	l->setContentsMargins(0, 0, 0, 0);
	l->addWidget(m_slider);

	connect(m_slider, SIGNAL(valueChanged(int)), this, SLOT(sliderValueChanged(int)));
}

DoubleSlider::
~DoubleSlider()	{}

void DoubleSlider::
setValue(double val)
{
	m_value = val;
	if(m_value < m_min)
		m_value = m_min;
	else if(m_value > m_max)
		m_value = m_max;

	if(m_max <= m_min)
		m_slider->setSliderPosition(0);
	else{
		m_slider->setSliderPosition((int)(m_resolution * (m_value - m_min)
													   / (m_max - m_min)));
	}
}

double DoubleSlider::
value() const
{
	return m_value;
}

void DoubleSlider::
setSingleStep(double singleStep)
{
	if(singleStep < 0)
		singleStep = 0;

	m_singleStep = singleStep;

//	calculate percentage of range and set single-step property of
//	underlying QSlider accordingly.
	double p;
	if(m_max <= m_min)
		p = 0.1;
	else
		p = m_singleStep / (m_max - m_min);
	m_slider->setSingleStep(std::max<int>((int)(p * m_resolution), 1));
}

double DoubleSlider::
singleStep() const
{
	return m_singleStep;
}


void DoubleSlider::
setRange(double min, double max)
{
	m_min = min;
	m_max = max;
	if(m_max < m_min)
		m_max = m_min;
	setSingleStep(m_singleStep);
	setValue(m_value);
}

double DoubleSlider::
minimum() const
{
	return m_min;
}

double DoubleSlider::
maximum() const
{
	return m_max;
}


void DoubleSlider::
setResolution(int resolution)
{
	if(resolution < 0)
		resolution = 0;
	m_resolution = resolution;
	setSingleStep(m_singleStep);
	setValue(m_value);
}


void DoubleSlider::
sliderValueChanged(int value)
{
	double p = (double)(value - m_slider->minimum()) /
			   (double)(m_slider->maximum() - m_slider->minimum());
	m_value = m_min + p * (m_max - m_min);
	emit valueChanged(m_value);
}
