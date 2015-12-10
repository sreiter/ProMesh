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

#ifndef __H__UG__double_slider__
#define __H__UG__double_slider__

#include <QWidget>

class QSlider;

///	The DoubleSlider is similar to QSlider, but provides double values.
/**
 * - Use setRange to specify a minimum and maximum value.
 * - The Resolution (default is 10000) specifies how many separate values are
 * 		represented on the slider. This value normally doesn't have to be changed.
 * - singleStep tells how far the slider moves if one presses right on the keybard.*/
class DoubleSlider : public QWidget
{
	Q_OBJECT

	public:
		DoubleSlider(QWidget* parent = 0);
		virtual ~DoubleSlider();

		void setRange(double min, double max);
		void setResolution(int resolution);

		double value() const;

		void setSingleStep(double singleStep);
		double singleStep() const;

		double minimum() const;
		double maximum() const;

	signals:
		void valueChanged(double value);

	public slots:
		void setValue(double val);

	protected slots:
		void sliderValueChanged(int value);

	private:
		QSlider*	m_slider;
		double m_value;
		double m_min;
		double m_max;
		double m_resolution;
		double m_singleStep;
};

#endif
